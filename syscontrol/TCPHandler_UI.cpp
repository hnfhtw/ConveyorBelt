/*
 * TCPHandler_UI.cpp
 *
 *  Created on: 16.04.2018
 *      Author: HN / MG
 */

#include "TCPHandler_UI.h"

using namespace std;

TCPHandler_UI::TCPHandler_UI(SysControl* pSctrl, MotorControl* pMctrl){
	m_pSysControl = pSctrl;
	m_pMotorControl = pMctrl;
	m_AddrUser = "";
}

TCPHandler_UI::~TCPHandler_UI(){
	m_pSysControl = NULL;
	m_pMotorControl = NULL;
}

void TCPHandler_UI::startServer(){
	// do something
}

void TCPHandler_UI::startClient(){
	// do something
}
