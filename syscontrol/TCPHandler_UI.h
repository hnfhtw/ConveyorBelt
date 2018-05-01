/*
 * TCPHandler_UI.h
 *
 *  Created on: 16.04.2018
 *      Author: HN / MG
 */

#ifndef TCPHANDLER_UI_H_
#define TCPHANDLER_UI_H_


#include "MotorControl.h"
#include "SysControl.h"

#include <string>
using namespace std;

class MotorControl;

class TCPHandler_UI{		// todo: update class diagram
private:
	MotorControl*	m_pMotorControl;	
	SysControl*		m_pSysControl;		
	string			m_AddrUser;
	void startClient();
public:
	TCPHandler_UI(SysControl* pSctrl, MotorControl* pMctrl);
	~TCPHandler_UI();
	void startServer();	
};

#endif /* TCPHANDLER_UI_H_ */
