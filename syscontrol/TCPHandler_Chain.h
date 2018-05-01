/*
 * TCPHandler_Chain.h
 *
 *  Created on: 16.04.2018
 *      Author: HN / MG
 */

#ifndef TCPHANDLER_CHAIN_H_
#define TCPHANDLER_CHAIN_H_

#include "SysControl.h"
#include "Command.h"
#include <string>
using namespace std;

class SysControl;

class TCPHandler_Chain{				// todo: update class diagram
private:
	SysControl*	m_pSysControl;
	int			m_serverTaskID;
	string		m_AddrMaster;
	string		m_AddrRCB;
	string		m_AddrLCB;
	bool		m_AddrRCBset;
	void startClient();
public:
	TCPHandler_Chain(SysControl* pSctrl, string addrMaster);
	~TCPHandler_Chain();
	void startServer();
	bool getAddrRCBset();
	string getAddrMaster();
	string getAddrRCB();
	void sendToRCB(Command cmd);
	void sendToLCB(Command cmd);
	void processMasterRequest(char myBuffer[80], int sFd);
	void processClientRequest(char myBuffer[80], int sFd);
};

#endif /* TCPHANDLER_CHAIN_H_ */
