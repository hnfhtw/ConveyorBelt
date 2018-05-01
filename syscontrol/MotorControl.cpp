/*
 * MotorControl.cpp
 *
 *  Created on: 16.04.2018
 *      Author: HN / MG
 */

#include "MotorControl.h"

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

void MotorControl::moveSlow(){
	// do something
}

void MotorControl::startRamp(){
	// do something
}

void MotorControl::stop(){
	// do something
}

void MotorControl::setMotorState(MotorState state){
	 m_MotorState = state;
}

MotorState MotorControl::getMotorState(){
	return m_MotorState;
}

void MotorControl::setMotorSpeedFinal(int speed){
	m_MotorSpeedFinal = speed;	// todo: maybe add error handling
	m_pDisplayControl->updateDisplay();
}

int MotorControl::getMotorSpeedFinal(){
	return m_MotorSpeedFinal;
}

void MotorControl::setDirection(bool dir){
	m_Direction = dir;
	m_pDisplayControl->updateDisplay();
}

bool MotorControl::getDirection(){
	return m_Direction;
}

void MotorControl::setSysControl(SysControl* pSysctrl){
	m_pSysControl = pSysctrl;
}

void MotorControl::setTargetSpeed(int speed){
	m_TargetSpeed = speed;	// todo: maybe add error handling
}

int MotorControl::getTargetSpeed(){
	return m_TargetSpeed;
}

void MotorControl::increaseSpeed(){
	int stepSize = m_MotorSpeedFinal/20;
	m_TargetSpeed += stepSize;
}

void MotorControl::decreaseSpeed(){
	int stepSize = m_MotorSpeedFinal/20;
	m_TargetSpeed -= stepSize;	
}


