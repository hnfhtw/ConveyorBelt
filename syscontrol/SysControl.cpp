/*
 * SysControl.cpp
 *
 *  Created on: 16.04.2018
 *      Author: HN / MG
 */

#include "SysControl.h"
#include "stateMachine.h"	// provided statemachine framework
#include "myFunctions.h"	// declaration of action and condition functions for statemachine framework
#include "taskLib.h"
extern "C"{
	#include "hwFunc.h"
	#include "ptpSlave.h"
}
using namespace std;

#define	RECEIVE_TIME			1000
#define	STEP_TIME		  		50
#define TRANSITION_TIME			6000
#define KEYBOARD_UPDATE_RATE	50


SysControl::SysControl(MotorControl* pMctrl, DisplayControl* pDctrl){
	m_pMotorControl = pMctrl;
	m_pDisplayControl = pDctrl;
	m_pTCPHandler_Chain = new TCPHandler_Chain(this, "91.0.0.91");		// only for debug, actual master IP is 91.0.0.91 
	m_pTCPHandler_UI = new TCPHandler_UI(this, m_pMotorControl);
	m_pKeyboardHandler = new KeyboardHandler(this, m_pMotorControl);
	m_StateOpMode = OPMODE_INIT;
	m_StateSysState = STATE_IDLE;
	m_RequestPending = false;
	m_pTCPHandler_UI->startServer();
	m_pTCPHandler_Chain->startServer();
	this->startPTP();
}

SysControl::~SysControl(){
	m_pMotorControl = NULL;
	m_pDisplayControl = NULL;
	delete m_pTCPHandler_Chain;
	delete m_pTCPHandler_UI;
	delete m_pKeyboardHandler;
	m_pTCPHandler_Chain = NULL;
	m_pTCPHandler_UI = NULL;
	m_pKeyboardHandler = NULL;
}

void SysControl::createStateMachine(){
	// Initialize table for all diagrams, event time in ms (POSIX)
	// The maximum size of the table is defined in stateTable.h:
	// MAXDIA = 9, MAXLINES = 66
	// Should these be exceeded, please correct!
	
	tab[0][0] = new TableEntry ("OPMODE_INIT","OPMODE_CHAIN","setOpMode(OPMODE_CHAIN)",0, trig00_action,trig00_condition);
	tab[0][1] = new TableEntry ("OPMODE_INIT","OPMODE_LOCAL","setOpMode(OPMODE_LOCAL)",0,trig01_action,myConditionTrue);
	tab[0][2] = new TableEntry ("OPMODE_CHAIN","OPMODE_LOCAL","setOpMode(OPMODE_LOCAL)",0,trig02_action,trig02_condition);
	tab[0][3] = new TableEntry ("OPMODE_LOCAL","OPMODE_CHAIN","setOpMode(OPMODE_CHAIN)",0,trig03_action,trig03_condition);
	
	tab[1][0] = new TableEntry ("STATE_IDLE","STATE_RECEIVEPACKET","setCommand(REQUEST)",0,trig10_action,myConditionTrue);
	tab[1][1] = new TableEntry ("STATE_RECEIVEPACKET","STATE_RECEIVEPACKET","setCommand(REQUEST)",0,trig11_action,myConditionTrue);
	tab[1][2] = new TableEntry ("STATE_RECEIVEPACKET","STATE_TRANSFERPACKET","Timer1",RECEIVE_TIME,trig12_action,myConditionTrue);
	tab[1][3] = new TableEntry ("STATE_TRANSFERPACKET","STATE_TRANSFERPACKET","setCommand(REQUEST)",0,trig13_action,myConditionTrue);
	tab[1][4] = new TableEntry ("STATE_TRANSFERPACKET","STATE_WAITFORRCB","setSysState(STATE_WAITFORRCB)",0,trig14_action,myConditionTrue);
	tab[1][5] = new TableEntry ("STATE_WAITFORRCB","STATE_WAITFORRCB","setCommand(REQUEST)",0,trig15_action,myConditionTrue);
	tab[1][6] = new TableEntry ("STATE_WAITFORRCB","STATE_WAITFORRCB","setCommand(WAIT)",0,trig16_action,myConditionTrue);
	tab[1][7] = new TableEntry ("STATE_WAITFORRCB","STATE_DELIVERPACKET","setCommand(READY)",0,trig17_action,myConditionTrue);
	tab[1][8] = new TableEntry ("STATE_DELIVERPACKET","STATE_DELIVERPACKET","setCommand(REQUEST)",0,trig18_action,myConditionTrue);
	tab[1][9] = new TableEntry ("STATE_DELIVERPACKET","STATE_IDLE","setCommand(RELEASE)",0,trig19_action,trig19_condition);
	tab[1][10] = new TableEntry ("STATE_DELIVERPACKET","STATE_RECEIVEPACKET","setCommand(RELEASE)",0,trig110_action,trig110_condition);
	
	tab[2][0] = new TableEntry ("MOTOR_STOP","MOTOR_SLOW","moveSlow()",0,trig20_action,myConditionTrue);
	tab[2][1] = new TableEntry ("MOTOR_STOP","MOTOR_RAMPUP","startRamp()",0,trig21_action,myConditionTrue);
	tab[2][2] = new TableEntry ("MOTOR_SLOW","MOTOR_STOP","stop()",0,trig22_action,myConditionTrue);
	tab[2][3] = new TableEntry ("MOTOR_SLOW","MOTOR_RAMPUP","startRamp()",0,trig23_action,myConditionTrue);
	tab[2][4] = new TableEntry ("MOTOR_RAMPUP","MOTOR_RAMPUP","Timer2",STEP_TIME,trig24_action,trig24_condition);
	tab[2][5] = new TableEntry ("MOTOR_RAMPUP","MOTOR_FULLSPEED","Timer2",STEP_TIME,trig25_action,trig25_condition);
	tab[2][6] = new TableEntry ("MOTOR_FULLSPEED","MOTOR_RAMPDOWN","Timer2",TRANSITION_TIME,trig26_action,myConditionTrue);
	tab[2][7] = new TableEntry ("MOTOR_RAMPDOWN","MOTOR_RAMPDOWN","Timer2",STEP_TIME,trig27_action,trig27_condition);
	tab[2][8] = new TableEntry ("MOTOR_RAMPDOWN","MOTOR_STOP","Timer2",STEP_TIME,trig28_action,trig28_condition);
	
	tab[3][0] = new TableEntry ("WAIT_FOR_KEY","WAIT_FOR_KEY","Timer3",KEYBOARD_UPDATE_RATE,trig30_action,myConditionTrue);
	
	// Initialize timer names for all diagrams
	// Timer names are always Timer followed by the diagram number
	timerNames[0] = "Timer0";
	timerNames[1] = "Timer1";
	timerNames[2] = "Timer2";
	timerNames[3] = "Timer3";
	
	// Initialize line numbers for all diagrams
	lines[0] = 4;
	lines[1] = 11;
	lines[2] = 9;
	lines[3] = 1;

	// Initialize first state for all diagrams
	actualState[0] = "OPMODE_INIT";
	actualState[1] = "STATE_IDLE";
	actualState[2] = "MOTOR_STOP";
	actualState[3] = "WAIT_FOR_KEY";
	
	// Set the actual number of diagrams
	diagrams = 4;
	
	// Create instance of state machine
	m_pStateMachine = new StateMachine(this);
	
	// Start timer for each diagram which needs one in the first state!
	m_pStateMachine->diaTimerTable[3]->startTimer(tab[3][0]->eventTime);
}

void SysControl::setOpMode(OpMode mode){
	m_StateOpMode = mode;
}

void SysControl::setSysState(SysState state){
	m_StateSysState = state;
}

OpMode SysControl::getOpMode(){
	return m_StateOpMode;
}

SysState SysControl::getSysState(){
	return m_StateSysState;
}

TCPHandler_Chain* SysControl::getTCPHandler_Chain(){
	return m_pTCPHandler_Chain;
}

KeyboardHandler* SysControl::getKeyboardHandler(){
	return m_pKeyboardHandler;
}

MotorControl* SysControl::getMotorControl(){
	return m_pMotorControl;
}

DisplayControl* SysControl::getDisplayControl(){
	return m_pDisplayControl;
}

StateMachine* SysControl::getStateMachine(){
	return m_pStateMachine;
}

void SysControl::setRequestPending(bool req){
	m_RequestPending = req;
}

bool SysControl::getRequestPending(){
	return m_RequestPending;
}

void SysControl::startPTP(){
	taskSpawn ("PTP", 140, 0,0x1000, (FUNCPTR) syncClk,0,0,0,0,0,0,0,0,0,0);	// set first parameter != 0 to output debug PTP debug values
}

void trig00_action(SysControl* pSysControl){
	pSysControl->setOpMode(OPMODE_CHAIN);
	pSysControl->getMotorControl()->setMotorSpeedFinal(1800);
	pSysControl->getMotorControl()->setDirection(TRUE);	
	pSysControl->getDisplayControl()->updateDisplay(false);
	return;
}

void trig01_action(SysControl* pSysControl){
	pSysControl->setOpMode(OPMODE_LOCAL);
	pSysControl->getDisplayControl()->updateDisplay(false);
	return;
}

void trig02_action(SysControl* pSysControl){
	pSysControl->setOpMode(OPMODE_LOCAL);
	pSysControl->getDisplayControl()->updateDisplay(false);
	return;
}

void trig03_action(SysControl* pSysControl){
	pSysControl->setOpMode(OPMODE_CHAIN);
	pSysControl->getMotorControl()->setMotorSpeedFinal(1800);
	pSysControl->getMotorControl()->setDirection(TRUE);	
	pSysControl->getDisplayControl()->updateDisplay(false);
	return;
}

void trig10_action(SysControl* pSysControl){
	pSysControl->setSysState(STATE_RECEIVEPACKET);
	pSysControl->getTCPHandler_Chain()->sendToLCB(READY);
	pSysControl->getDisplayControl()->updateDisplay(false);
	pSysControl->getStateMachine()->sendEvent("moveSlow()");
	return;
}

void trig11_action(SysControl* pSysControl){
	pSysControl->setRequestPending(true);
	pSysControl->getDisplayControl()->updateDisplay(false);
	return;
}

void trig12_action(SysControl* pSysControl){
	pSysControl->setSysState(STATE_TRANSFERPACKET);
	pSysControl->getTCPHandler_Chain()->sendToLCB(RELEASE);
	pSysControl->getDisplayControl()->updateDisplay(true);
	pSysControl->getStateMachine()->sendEvent("startRamp()");
	return;
}

void trig13_action(SysControl* pSysControl){
	pSysControl->setRequestPending(true);
	pSysControl->getDisplayControl()->updateDisplay(false);
	return;
}

void trig14_action(SysControl* pSysControl){
	pSysControl->setSysState(STATE_WAITFORRCB);
	pSysControl->getTCPHandler_Chain()->sendToRCB(REQUEST);
	pSysControl->getDisplayControl()->updateDisplay(false);
	return;
}

void trig15_action(SysControl* pSysControl){
	pSysControl->setRequestPending(true);
	pSysControl->getDisplayControl()->updateDisplay(false);
	return;
}

void trig16_action(SysControl* pSysControl){
	pSysControl->getDisplayControl()->updateDisplay(false);
	return;
}

void trig17_action(SysControl* pSysControl){
	pSysControl->setSysState(STATE_DELIVERPACKET);
	pSysControl->getDisplayControl()->updateDisplay(false);
	pSysControl->getStateMachine()->sendEvent("moveSlow()");
	return;
}

void trig18_action(SysControl* pSysControl){
	pSysControl->setRequestPending(true);
	pSysControl->getDisplayControl()->updateDisplay(false);
	return;
}

void trig19_action(SysControl* pSysControl){
	pSysControl->setSysState(STATE_IDLE);
	pSysControl->getDisplayControl()->updateDisplay(false);
	pSysControl->getStateMachine()->sendEvent("stop()");
	return;
}

void trig110_action(SysControl* pSysControl){
	pSysControl->setSysState(STATE_RECEIVEPACKET);
	pSysControl->setRequestPending(false);
	pSysControl->getTCPHandler_Chain()->sendToLCB(READY);
	pSysControl->getDisplayControl()->updateDisplay(false);
	pSysControl->getStateMachine()->sendEvent("moveSlow()");
	return;
}

void trig20_action(SysControl* pSysControl){
	pSysControl->getMotorControl()->setMotorState(MOTOR_SLOW);
	pSysControl->getMotorControl()->setTargetSpeed(100);
	pSysControl->getDisplayControl()->updateDisplay(false);
	return;
}

void trig21_action(SysControl* pSysControl){
	pSysControl->getMotorControl()->setMotorState(MOTOR_RAMPUP);
	pSysControl->getDisplayControl()->updateDisplay(false);
	return;
}

void trig22_action(SysControl* pSysControl){
	pSysControl->getMotorControl()->setMotorState(MOTOR_STOP);
	pSysControl->getMotorControl()->setTargetSpeed(0);
	pSysControl->getDisplayControl()->updateDisplay(false);
	return;
}

void trig23_action(SysControl* pSysControl){
	pSysControl->getMotorControl()->setMotorState(MOTOR_RAMPUP);
	pSysControl->getDisplayControl()->updateDisplay(false);
	return;
}

void trig24_action(SysControl* pSysControl){
	pSysControl->getMotorControl()->increaseSpeed();
	pSysControl->getDisplayControl()->updateDisplay(false);
	return;
}

void trig25_action(SysControl* pSysControl){
	pSysControl->getMotorControl()->setMotorState(MOTOR_FULLSPEED);
	pSysControl->getMotorControl()->setTargetSpeed(pSysControl->getMotorControl()->getMotorSpeedFinal());
	pSysControl->getDisplayControl()->updateDisplay(false);
	return;
}

void trig26_action(SysControl* pSysControl){
	pSysControl->getMotorControl()->setMotorState(MOTOR_RAMPDOWN);
	pSysControl->getDisplayControl()->updateDisplay(false);
	return;
}

void trig27_action(SysControl* pSysControl){
	pSysControl->getMotorControl()->decreaseSpeed();
	pSysControl->getDisplayControl()->updateDisplay(false);
	return;
}

void trig28_action(SysControl* pSysControl){
	pSysControl->getMotorControl()->setMotorState(MOTOR_STOP);
	pSysControl->getMotorControl()->setTargetSpeed(0);
	if(pSysControl->getOpMode() == OPMODE_CHAIN){
		pSysControl->setSysState(STATE_WAITFORRCB);
		pSysControl->getStateMachine()->sendEvent("setSysState(STATE_WAITFORRCB)");
	}
	pSysControl->getDisplayControl()->updateDisplay(false);
	return;
}

void trig30_action(SysControl* pSysControl){	
	pSysControl->getKeyboardHandler()->evaluateKey();
	return;
}

bool myConditionTrue(SysControl* pSysControl){
	return TRUE;
}

bool trig00_condition(SysControl* pSysControl){
	if (pSysControl->getTCPHandler_Chain()->getAddrRCBset() != FALSE) {
		return TRUE;
	}
	else return FALSE;
}

bool trig02_condition(SysControl* pSysControl){
	if (pSysControl->getMotorControl()->getMotorState() == MOTOR_STOP) {
		return TRUE;
	}
	else return FALSE;
}

bool trig03_condition(SysControl* pSysControl){
	if ( (pSysControl->getMotorControl()->getMotorState() == MOTOR_STOP) && (pSysControl->getTCPHandler_Chain()->getAddrRCBset() == TRUE) ) {
		return TRUE;
	}
	else return FALSE;
}

bool trig19_condition(SysControl* pSysControl){
	if (pSysControl->getRequestPending() == TRUE) {
		return FALSE;
	}
	else return TRUE;
}

bool trig110_condition(SysControl* pSysControl){
	if (pSysControl->getRequestPending() == TRUE) {
		return TRUE;
	}
	else return FALSE;
}

bool trig24_condition(SysControl* pSysControl){
	if ( pSysControl->getMotorControl()->getTargetSpeed() >= pSysControl->getMotorControl()->getMotorSpeedFinal() ) {
		return FALSE;
	}
	else return TRUE;
}

bool trig25_condition(SysControl* pSysControl){
	if ( pSysControl->getMotorControl()->getTargetSpeed() >= pSysControl->getMotorControl()->getMotorSpeedFinal() ) {
		return TRUE;
	}
	else return FALSE;
}

bool trig27_condition(SysControl* pSysControl){
	if ( pSysControl->getMotorControl()->getTargetSpeed() <= 0 ) {
		return FALSE;
	}
	else return TRUE;
}

bool trig28_condition(SysControl* pSysControl){
	if ( pSysControl->getMotorControl()->getTargetSpeed() <= 0 ) {
		return TRUE;
	}
	else return FALSE;
}
