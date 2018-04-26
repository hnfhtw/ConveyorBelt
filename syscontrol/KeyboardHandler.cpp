/*
 * KeyboardHandler.cpp
 *
 *  Created on: 16.04.2018
 *      Author: HN / MG
 */

#include "KeyboardHandler.h"

using namespace std;

KeyboardHandler::KeyboardHandler(SysControl* pSctrl, MotorControl* pMctrl){
	m_pMotorControl = pMctrl;
	m_pSysControl = pSctrl;
	m_BufferSpeed = 0;
	m_Direction = false;
	m_Buffer = 0;
}

KeyboardHandler::~KeyboardHandler(){
	m_pMotorControl = NULL;
	m_pSysControl = NULL;
}

void KeyboardHandler::evaluateKey(){
	// char return value is not necessary
	// todo: check if key of keyboard is pressed
	//		 if yes: inform SysControl or MotorControl depending on the pressed key
}
