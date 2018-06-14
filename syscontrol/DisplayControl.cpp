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
extern int timeOffset;

using namespace std;

DisplayControl::DisplayControl(){
	m_pSysControl = NULL;
	m_pMotorControl = NULL;
}

DisplayControl::~DisplayControl(){
	m_pSysControl = NULL;
	m_pMotorControl = NULL;
}

void DisplayControl::updateDisplay(bool setTime){
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
		
	switch(motorState){
		case MOTOR_STOP:			writeToDisplay (7, 5, "Motor State:                MOTOR_STOP      ");
									break;
		case MOTOR_RAMPUP:			writeToDisplay (7, 5, "Motor State:                MOTOR_RAMPUP    ");
									break;
		case MOTOR_RAMPDOWN:		writeToDisplay (7, 5, "Motor State:                MOTOR_RAMPDOWN  ");
									break;
		case MOTOR_FULLSPEED:		writeToDisplay (7, 5, "Motor State:                MOTOR_FULLSPEED ");
									break;
		case MOTOR_SLOW:			writeToDisplay (7, 5, "Motor State:                MOTOR_SLOW      ");
									break;
		default:					break;
	}
	
	switch(direction){
		case true:					writeToDisplay (9, 5, "Motor Direction:            TRUE (cw)   ");
									break;
		case false:					writeToDisplay (9, 5, "Motor Direction:            FALSE (ccw) ");
									break;
		default:					break;
	}		   
	
	char speed[40];
	sprintf(speed, "Motor Actual Target Speed:  %d   ", targetSpeed);
	writeToDisplay (10, 5, speed);	
	sprintf(speed, "Motor Final Speed:          %d   ", finalSpeed);
	writeToDisplay (11, 5, speed);	
	
	if(mode == OPMODE_LOCAL){
		writeToDisplay (14, 5, "Change Direction and Final Speed if Motor is stopped:");
		writeToDisplay (15, 5, "Change Direction with 'D'");
		writeToDisplay (16, 5, "Increase Final Speed with 'F', Decrease Final Speed with 'C'");

		writeToDisplay(18, 5, "Start Movement with 'E'");
		writeToDisplay(20, 5, "                                                                 ");
	}
	else if(mode == OPMODE_CHAIN){
		switch(state){
			case STATE_IDLE:			writeToDisplay (14, 5, "Chain Mode System State:    STATE_IDLE               ");
										break;
			case STATE_RECEIVEPACKET:	writeToDisplay (14, 5, "Chain Mode System State:    STATE_RECEIVEPACKET      ");
										break;
			case STATE_TRANSFERPACKET:	writeToDisplay (14, 5, "Chain Mode System State:    STATE_TRANSFERPACKET     ");
										break;
			case STATE_WAITFORRCB:		writeToDisplay (14, 5, "Chain Mode System State:    STATE_WAITFORRCB         ");
										break;
			case STATE_DELIVERPACKET:	writeToDisplay (14, 5, "Chain Mode System State:    STATE_DELIVERPACKET      ");
										break;					
			default:					break;
		}
		char adr[80];
		if(m_pSysControl->getTCPHandler_Chain()->getAddrRCBset()){
			sprintf(adr, "Address of RCB:             %s                        ", m_pSysControl->getTCPHandler_Chain()->getAddrRCB().c_str());
			writeToDisplay (16, 5, adr);
		}
		else{
			writeToDisplay (16, 5, "Address of RCB:             not set                      ");
		}
		writeToDisplay (15, 5, "                         ");	// clear display contents from local mode   
		writeToDisplay(18, 5, "                       "); 		// clear display contents from local mode
		if(setTime){
			char timeMsg[80];
			sprintf(timeMsg, "Packet received at master-time %ds                         ", (time(NULL)-timeOffset));
			writeToDisplay(20, 5, timeMsg);
			//writeToDisplay(20, 5, "Packet received at master-time %d ");
		}
	}						  
}

void DisplayControl::setMotorControl(MotorControl* pMctrl){
	m_pMotorControl = pMctrl;
}

void DisplayControl::setSysControl(SysControl* pSctrl){
	m_pSysControl = pSctrl;
}

