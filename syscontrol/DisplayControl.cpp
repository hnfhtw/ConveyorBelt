/*
 * DisplayControl.cpp
 *
 *  Created on: 16.04.2018
 *      Author: HN / MG
 */

#include "DisplayControl.h"

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
	// get SysState, OpMode, Speed and Direction and write data to LCD Display
}

void DisplayControl::setMotorControl(MotorControl* pMctrl){
	m_pMotorControl = pMctrl;
}

void DisplayControl::setSysControl(SysControl* pSctrl){
	m_pSysControl = pSctrl;
}

