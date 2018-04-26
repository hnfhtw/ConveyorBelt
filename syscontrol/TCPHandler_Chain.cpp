/*
 * TCPHandler_Chain.cpp
 *
 *  Created on: 16.04.2018
 *      Author: HN / MG
 */

#include "TCPHandler_Chain.h"
#include <string>
using namespace std;

TCPHandler_Chain::TCPHandler_Chain(SysControl* pSctrl){
	m_pSysControl = pSctrl;
	m_AddrMaster = "";
	m_AddrRCB = "";
	m_AddrLCB = "";
	m_AddrRCBset = false;
}

TCPHandler_Chain::~TCPHandler_Chain(){
	m_pSysControl = NULL;
}

void TCPHandler_Chain::startServer(){
	// do something
}

void TCPHandler_Chain::startClient(){
	// do something
}

bool TCPHandler_Chain::getAddrRCBset(){
	return m_AddrRCBset;
}

void TCPHandler_Chain::sendToRCB(Command cmd){
	// do something
}

void TCPHandler_Chain::sendToLCB(Command cmd){
	// do something
}
