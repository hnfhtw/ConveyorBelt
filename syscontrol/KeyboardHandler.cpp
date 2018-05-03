/*
 * KeyboardHandler.cpp
 *
 *  Created on: 16.04.2018
 *      Author: HN / MG
 */

#include "KeyboardHandler.h"
#include "stateMachine.h"
extern "C"{
	#include "hwFunc.h"
}

KeyboardHandler::KeyboardHandler(SysControl* pSctrl, MotorControl* pMctrl){
	m_pMotorControl = pMctrl;
	m_pSysControl = pSctrl;
}

KeyboardHandler::~KeyboardHandler(){
	m_pMotorControl = NULL;
	m_pSysControl = NULL;
}

void KeyboardHandler::evaluateKey(){
	char ch = getKey();
	
	int finalSpeed;

	switch(ch){
		case 'A':	m_pSysControl->getStateMachine()->sendEvent("setOpMode(OPMODE_LOCAL)");
					printf("Key A pressed\n");
					break;
		case 'B':	m_pSysControl->getStateMachine()->sendEvent("setOpMode(OPMODE_CHAIN)");
					printf("Key B pressed\n");
					break;
		case 'D':	if( (m_pSysControl->getOpMode() == OPMODE_LOCAL) && (m_pMotorControl->getMotorState() == MOTOR_STOP) ){
						m_pMotorControl->setDirection(!(m_pMotorControl->getDirection()));
					}
					break;
		case 'F':	if( (m_pSysControl->getOpMode() == OPMODE_LOCAL) && (m_pMotorControl->getMotorState() == MOTOR_STOP) ){
						finalSpeed = m_pMotorControl->getMotorSpeedFinal();
						if(finalSpeed <= 2100){
							m_pMotorControl->setMotorSpeedFinal(finalSpeed + 100);
						}
					}
					break;
		case 'C':	if( (m_pSysControl->getOpMode() == OPMODE_LOCAL) && (m_pMotorControl->getMotorState() == MOTOR_STOP) ){
						finalSpeed = m_pMotorControl->getMotorSpeedFinal();
						if(finalSpeed >= 200){
							m_pMotorControl->setMotorSpeedFinal(finalSpeed - 100);
						}
					}
					break;
		case 'E':	if( (m_pSysControl->getOpMode() == OPMODE_LOCAL) && (m_pMotorControl->getMotorState() == MOTOR_STOP) ){
						m_pSysControl->getStateMachine()->sendEvent("startRamp()");
					}
					break;
		default:	break;
	}

	return;
}
