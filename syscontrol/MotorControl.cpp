/*
 * MotorControl.cpp
 *
 *  Created on: 16.04.2018
 *      Author: HN / MG
 */

extern "C"{
	#include "hwFunc.h"
}
#include "MotorControl.h"
#include "taskLib.h"
#include "piCtrl.h"
using namespace std;

MotorControl::MotorControl(DisplayControl* pDctrl){
	m_pDisplayControl = pDctrl;
	m_pSysControl = NULL;
	m_MotorState = MOTOR_STOP;
	m_Direction = true;			// TRUE = clockwise, FALSE = counter clockwise
	m_MotorSpeedFinal = 1800;
	m_TargetSpeed = 0;
}

MotorControl::~MotorControl(){
	m_pDisplayControl = NULL;
	m_pSysControl = NULL;
}

void MotorControl::startPICtrl(){
	taskSpawn ("piCtrl", 111, 0,0x1000, (FUNCPTR) piCtrl_main,110,(int) this,0,0,0,0,0,0,0,0);
}

void MotorControl::setMotorState(MotorState state){
	 m_MotorState = state;
}

MotorState MotorControl::getMotorState(){
	return m_MotorState;
}

void MotorControl::setMotorSpeedFinal(int speed){
	m_MotorSpeedFinal = speed;
	m_pDisplayControl->updateDisplay(false);
}

int MotorControl::getMotorSpeedFinal(){
	return m_MotorSpeedFinal;
}

void MotorControl::setDirection(bool dir){
	m_Direction = dir;
	m_pDisplayControl->updateDisplay(false);
}

bool MotorControl::getDirection(){
	return m_Direction;
}

void MotorControl::setSysControl(SysControl* pSysctrl){
	m_pSysControl = pSysctrl;
}

void MotorControl::setTargetSpeed(int speed){
	m_TargetSpeed = speed;
}

int MotorControl::getTargetSpeed(){
	return m_TargetSpeed;
}

void MotorControl::increaseSpeed(){
	int stepSize = m_MotorSpeedFinal/20;
	setTargetSpeed(m_TargetSpeed + stepSize);
}

void MotorControl::decreaseSpeed(){
	int stepSize = m_MotorSpeedFinal/20;
	setTargetSpeed(m_TargetSpeed - stepSize);
}


