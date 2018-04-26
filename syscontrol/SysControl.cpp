/*
 * SysControl.cpp
 *
 *  Created on: 16.04.2018
 *      Author: HN / MG
 */

#include "SysControl.h"

using namespace std;

SysControl::SysControl(MotorControl* pMctrl, DisplayControl* pDctrl){
	m_pMotorControl = pMctrl;
	m_pDisplayControl = pDctrl;
	m_pTCPHandler_Chain = new TCPHandler_Chain(this);
	m_pTCPHandler_UI = new TCPHandler_UI(this, m_pMotorControl);
	m_pKeyboardHandler = new KeyboardHandler(this, m_pMotorControl);
	m_StateOpMode = OPMODE_INIT;
	m_StateSysState = STATE_IDLE;
	m_RequestPending = false;	
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

void SysControl::setCommand(Command cmd){
	// do something depending on received cmd
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
