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
	m_Direction = false;			// todo: define what direction true/false means
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

MotorState MotorControl::getMotorState(){
	return m_MotorState;
}

void MotorControl::setMotorSpeedFinal(int speed){
	m_MotorSpeedFinal = speed;	// todo: maybe add error handling
}

void MotorControl::setDirection(bool dir){
	m_Direction = dir;
}

void MotorControl::setSysControl(SysControl* pSysctrl){
	m_pSysControl = pSysctrl;
}

