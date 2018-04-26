
#include "systemManager.h"
#include "stateMachine.h"
#include "keyboard.h"
#include "myFunctions.h"
#include "SysControl.h"
#include "MotorControl.h"
#include "DisplayControl.h"
#include "stateMachine.h"
extern "C"{
	#include "hwFunc.h"
}


#define	RECEIVE_TIME			1000
#define	STEP_TIME		  		50
#define TRANSITION_TIME			6000
#define KEYBOARD_UPDATE_RATE	50

int n, m;
StateMachine* pStateMachine;
KeyboardHandler* pKeyboardHandler;
SysControl* pSysControl;
MotorControl* pMotorControl;
DisplayControl* pDisplayControl;


SystemManager :: SystemManager() {
	// Initialize table for all diagrams, event time in ms (POSIX)
	// The maximum size of the table is defined in stateTable.h:
	// MAXDIA = 9, MAXLINES = 66
	// Should these be exceeded, please correct!
	
	tab[0][0] = new TableEntry ("OPMODE_INIT","OPMODE_CHAIN","setOpMode(OPMODE_CHAIN)",0,trig00_action,trig00_condition);
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
	
	// Create instance of my Keyboard
	//pKeyboard = new Keyboard;
	
	// Create instances of other classes
    pDisplayControl = new DisplayControl;
    pMotorControl = new MotorControl(pDisplayControl);
    pSysControl = new SysControl(pMotorControl, pDisplayControl);
    
    // add missing links between objects
    pDisplayControl->setMotorControl(pMotorControl);
    pDisplayControl->setSysControl(pSysControl);
    pMotorControl->setSysControl(pSysControl);

   // pKeyboardHandler = pSysControl->getKeyboardHandler();
    
	// Create instance of state machine
	pStateMachine = new StateMachine;

	// Start timer for each diagram which needs one in the first state!
	// In my case these are diagram 0 and 2
	//pStateMachine->diaTimerTable[0]->startTimer(tab[0][0]->eventTime);
	pStateMachine->diaTimerTable[3]->startTimer(tab[3][0]->eventTime);

	// Initial actions can be done here, if needed!
	//updateDisplay();
	//n = 0;
	//m = 0;

	return;
}

SystemManager :: ~SystemManager() {
	return;
}

void trig00_action(){
	pSysControl->setOpMode(OPMODE_CHAIN);
	pMotorControl->setMotorSpeedFinal(1800);
	pMotorControl->setDirection(TRUE);	
	pDisplayControl->updateDisplay();
	printf("OPMODE_CHAIN entered\n");
	//writeToDisplay (10, 10, "OPMODE_CHAIN entered");
	return;
}

void trig01_action(){
	pSysControl->setOpMode(OPMODE_LOCAL);
	pDisplayControl->updateDisplay();
	printf("OPMODE_LOCAL entered\n");
	//writeToDisplay (10, 10, "OPMODE_LOCAL entered");
	return;
}

void trig02_action(){
	pSysControl->setOpMode(OPMODE_LOCAL);
	pDisplayControl->updateDisplay();
	printf("OPMODE_LOCAL entered\n");
	//writeToDisplay (10, 10, "OPMODE_LOCAL entered");
	return;
}

void trig03_action(){
	pSysControl->setOpMode(OPMODE_CHAIN);
	pMotorControl->setMotorSpeedFinal(1800);
	pMotorControl->setDirection(TRUE);	
	pDisplayControl->updateDisplay();
	printf("OPMODE_CHAIN entered\n");
	//writeToDisplay (10, 10, "OPMODE_CHAIN entered");
	return;
}

void trig10_action(){
	pSysControl->setSysState(STATE_RECEIVEPACKET);
	pSysControl->getTCPHandler_Chain()->sendToLCB(READY);
	pMotorControl->moveSlow();
	pDisplayControl->updateDisplay();
		pStateMachine->sendEvent("moveSlow()");
	return;
}

void trig11_action(){
	pSysControl->setRequestPending(true);
	pDisplayControl->updateDisplay();
	return;
}

void trig12_action(){
	pSysControl->setSysState(STATE_TRANSFERPACKET);
	pSysControl->getTCPHandler_Chain()->sendToLCB(RELEASE);
	pMotorControl->startRamp();
	pDisplayControl->updateDisplay();
		pStateMachine->sendEvent("startRamp()");
	return;
}

void trig13_action(){
	pSysControl->setRequestPending(true);
	pDisplayControl->updateDisplay();
	return;
}

void trig14_action(){
	pSysControl->setSysState(STATE_WAITFORRCB);
	pSysControl->getTCPHandler_Chain()->sendToRCB(REQUEST);
	pDisplayControl->updateDisplay();
	return;
}

void trig15_action(){
	pSysControl->setRequestPending(true);
	pDisplayControl->updateDisplay();
	return;
}

void trig16_action(){
	pDisplayControl->updateDisplay();
	return;
}

void trig17_action(){
	pSysControl->setSysState(STATE_DELIVERPACKET);
	pMotorControl->moveSlow();
	pDisplayControl->updateDisplay();
		pStateMachine->sendEvent("moveSlow()");
	return;
}

void trig18_action(){
	pSysControl->setRequestPending(true);
	pDisplayControl->updateDisplay();
	return;
}

void trig19_action(){
	pSysControl->setSysState(STATE_IDLE);
	pMotorControl->stop();
	pDisplayControl->updateDisplay();
		pStateMachine->sendEvent("stop()");
	return;
}

void trig110_action(){
	pSysControl->setSysState(STATE_RECEIVEPACKET);
	pMotorControl->stop();
	pSysControl->setRequestPending(false);
	pSysControl->getTCPHandler_Chain()->sendToLCB(READY);
	pMotorControl->moveSlow();
	pDisplayControl->updateDisplay();
		pStateMachine->sendEvent("moveSlow()");
	return;
}

void trig20_action(){
	pMotorControl->setMotorState(MOTOR_SLOW);
	pMotorControl->setTargetSpeed(pMotorControl->cMotorSpeedSlow);
	pDisplayControl->updateDisplay();
	return;
}

void trig21_action(){
	pMotorControl->setMotorState(MOTOR_RAMPUP);
	pDisplayControl->updateDisplay();
	return;
}

void trig22_action(){
	pMotorControl->setMotorState(MOTOR_STOP);
	pMotorControl->setTargetSpeed(0);
	pDisplayControl->updateDisplay();
	return;
}

void trig23_action(){
	pMotorControl->setMotorState(MOTOR_RAMPUP);
	pDisplayControl->updateDisplay();
	return;
}

void trig24_action(){
	pMotorControl->increaseSpeed();
	pDisplayControl->updateDisplay();
	return;
}

void trig25_action(){
	pMotorControl->setMotorState(MOTOR_FULLSPEED);
	pMotorControl->setTargetSpeed(pMotorControl->getMotorSpeedFinal());
	pDisplayControl->updateDisplay();
	return;
}

void trig26_action(){
	pMotorControl->setMotorState(MOTOR_RAMPDOWN);
	pDisplayControl->updateDisplay();
	return;
}

void trig27_action(){
	pMotorControl->decreaseSpeed();
	pDisplayControl->updateDisplay();
	return;
}

void trig28_action(){
	pMotorControl->setMotorState(MOTOR_STOP);
	pMotorControl->setTargetSpeed(0);
	pSysControl->setSysState(STATE_WAITFORRCB);
	pDisplayControl->updateDisplay();
		pStateMachine->sendEvent("setSysState(STATE_WAITFORRCB)");
	return;
}

void trig30_action(){
	
	/*char ch = pKeyboard->getPressedKey();
	if(ch == 'A'){
		pStateMachine->sendEvent("setOpMode(OPMODE_LOCAL)");
		printf("Key A pressed\n");
	}
	else if(ch == 'B'){
		pStateMachine->sendEvent("setOpMode(OPMODE_CHAIN)");
		printf("Key B pressed\n");
	}*/
	//(pSysControl->getKeyboardHandler())   ->evaluateKey();
	//KeyboardHandler* pHandler = pSysControl->getKeyboardHandler();
	//pHandler->evaluateKey();
	
	
	//pKeyboardHandler->evaluateKey();							// ORIGINAL - Member Funktion vom Keyboard Handler aufrufen -> Programm stürzt ab
	
	
	char ch = getKey();											// VERSUCH - Code aus KeyboardHandler Funktion rauskopiert -> stürzt auch ab
		
		int finalSpeed;

		switch(ch){
			case 'A':	pStateMachine->sendEvent("setOpMode(OPMODE_LOCAL)");
						printf("Key A pressed\n");
						break;
			case 'B':	pStateMachine->sendEvent("setOpMode(OPMODE_CHAIN)");
						printf("Key B pressed\n");
						break;
			case 'D':	if( (pSysControl->getOpMode() == OPMODE_LOCAL) && (pMotorControl->getMotorState() == MOTOR_STOP) ){
							pMotorControl->setDirection(!(pMotorControl->getDirection()));
						}
						break;
			case 'F':	if( (pSysControl->getOpMode() == OPMODE_LOCAL) && (pMotorControl->getMotorState() == MOTOR_STOP) ){
							finalSpeed = pMotorControl->getMotorSpeedFinal();
							if(finalSpeed <= 1700){
								pMotorControl->setMotorSpeedFinal(finalSpeed + 100);
							}
						}
						break;
			case 'C':	if( (pSysControl->getOpMode() == OPMODE_LOCAL) && (pMotorControl->getMotorState() == MOTOR_STOP) ){
							finalSpeed = pMotorControl->getMotorSpeedFinal();
							if(finalSpeed >= 200){
								pMotorControl->setMotorSpeedFinal(finalSpeed - 100);
							}
						}
						break;
			case 'E':	pStateMachine->sendEvent("startRamp()");
						break;
			default:	break;
		}
		
		
	return;
}

// pStateMachine->sendEvent("Trigg0");



bool myConditionTrue(){
	return TRUE;
}

bool trig00_condition(){
	/*if (getAddrRCBset() == TRUE) {
		return TRUE;
	}
	else return FALSE;
	*/
	
	return TRUE;
}

bool trig02_condition(){
	/*if (getMotorState() == MOTOR_STOP) {
		return TRUE;
	}
	else return FALSE;
	*/
	
	return TRUE;
}

bool trig03_condition(){
	/*if ( (getMotorState() == MOTOR_STOP) && (getAddrRCBset() == TRUE) ) {
		return TRUE;
	}
	else return FALSE;
	*/
	
	return TRUE;
}

bool trig19_condition(){
	/*if ( m_RequestPending == TRUE ) {
		return FALSE;
	}
	else return TRUE;
	*/
	
	return TRUE;
}

bool trig110_condition(){
	/*if ( m_RequestPending == TRUE ) {
		return TRUE;
	}
	else return FALSE;
	*/
	
	return TRUE;
}

bool trig24_condition(){
	/*if ( m_TargetSpeed >= m_MotorSpeedFinal == TRUE ) {
		return FALSE;
	}
	else return TRUE;
	*/
	
	return TRUE;
}

bool trig25_condition(){
	/*if ( m_TargetSpeed >= m_MotorSpeedFinal == TRUE ) {
		return TRUE;
	}
	else return FALSE;
	*/
	
	return TRUE;
}

bool trig27_condition(){
	/*if ( m_TargetSpeed <= 0 ) {
		return FALSE;
	}
	else return TRUE;
	*/
	
	return TRUE;
}

bool trig28_condition(){
	/*if ( m_TargetSpeed <= 0 ) {
		return TRUE;
	}
	else return FALSE;
	*/
	
	return TRUE;
}

