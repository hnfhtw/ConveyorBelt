/*
 * DisplayControl.cpp
 *
 *  Created on: 16.04.2018
 *      Author: HN / MG
 */

#include "DisplayControl.h"
#include "OpMode.h"
#include "SysState.h"
#include "MotorState.h"
extern "C"{
	#include "hwFunc.h"
}

using namespace std;

DisplayControl::DisplayControl(){
	m_pSysControl = NULL;
	m_pMotorControl = NULL;
}

DisplayControl::~DisplayControl(){
	m_pSysControl = NULL;
	m_pMotorControl = NULL;
}

void DisplayControl::updateDisplay(){
	SysState state = m_pSysControl->getSysState();
	OpMode mode = m_pSysControl->getOpMode();
	MotorState motorState = m_pMotorControl->getMotorState();
	bool direction = m_pMotorControl->getDirection();
	int targetSpeed = m_pMotorControl->getTargetSpeed();
	int finalSpeed = m_pMotorControl->getMotorSpeedFinal();
	
	switch(mode){
		case OPMODE_INIT:			writeToDisplay (5, 5, "Operation Mode:             OPMODE_INIT  ");
									break;
		case OPMODE_CHAIN:			writeToDisplay (5, 5, "Operation Mode:             OPMODE_CHAIN ");
									break;
		case OPMODE_LOCAL:			writeToDisplay (5, 5, "Operation Mode:             OPMODE_LOCAL ");
									break;
		default:					break;
	}
	
	switch(state){
		case STATE_IDLE:			writeToDisplay (7, 5, "Chain Mode System State:    STATE_IDLE           ");
									break;
		case STATE_RECEIVEPACKET:	writeToDisplay (7, 5, "Chain Mode System State:    STATE_RECEIVEPACKET  ");
									break;
		case STATE_TRANSFERPACKET:	writeToDisplay (7, 5, "Chain Mode System State:    STATE_TRANSFERPACKET ");
									break;
		case STATE_WAITFORRCB:		writeToDisplay (7, 5, "Chain Mode System State:    STATE_WAITFORRCB     ");
									break;
		case STATE_DELIVERPACKET:	writeToDisplay (7, 5, "Chain Mode System State:    STATE_DELIVERPACKET  ");
									break;
		default:					break;
	}
	
	switch(motorState){
		case MOTOR_STOP:			writeToDisplay (9, 5, "Motor State:                MOTOR_STOP      ");
									break;
		case MOTOR_RAMPUP:			writeToDisplay (9, 5, "Motor State:                MOTOR_RAMPUP    ");
									break;
		case MOTOR_RAMPDOWN:		writeToDisplay (9, 5, "Motor State:                MOTOR_RAMPDOWN  ");
									break;
		case MOTOR_FULLSPEED:		writeToDisplay (9, 5, "Motor State:                MOTOR_FULLSPEED ");
									break;
		case MOTOR_SLOW:			writeToDisplay (9, 5, "Motor State:                MOTOR_SLOW      ");
									break;
		default:					break;
	}
	
	switch(direction){
		case true:					writeToDisplay (10, 5, "Motor Direction:            TRUE (cw)   ");
									break;
		case false:					writeToDisplay (10, 5, "Motor Direction:            FALSE (ccw) ");
									break;
		default:					break;
	}		   
	
	char speed[40];
	sprintf(speed, "Motor Actual Target Speed:  %d", targetSpeed);
	writeToDisplay (11, 5, speed);	
	sprintf(speed, "Motor Final Speed:          %d", finalSpeed);
	writeToDisplay (12, 5, speed);	
	
	if(mode == OPMODE_LOCAL){
		writeToDisplay (15, 5, "Change Direction and Final Speed if Motor is stopped:");
		writeToDisplay (16, 5, "Change Direction with 'D'");
		writeToDisplay (17, 5, "Increase Final Speed with 'F', Decrease Final Speed with 'C'");
		
		writeToDisplay(19, 5, "Start Movement with 'E'");
	}
}

void DisplayControl::setMotorControl(MotorControl* pMctrl){
	m_pMotorControl = pMctrl;
}

void DisplayControl::setSysControl(SysControl* pSctrl){
	m_pSysControl = pSctrl;
}

