/*
 * SysControl.h
 *
 *  Created on: 16.04.2018
 *      Author: HN / MG
 */

#ifndef SYSCONTROL_H_
#define SYSCONTROL_H_

#include "TCPHandler_Chain.h"
#include "TCPHandler_UI.h"
#include "KeyboardHandler.h"
#include "MotorControl.h"
#include "DisplayControl.h"
#include "OpMode.h"
#include "SysState.h"
#include "Command.h"

class KeyboardHandler;
class TCPHandler_Chain;
class TCPHandler_UI;

class SysControl{			// todo: update class diagram
private:
	TCPHandler_Chain*	m_pTCPHandler_Chain;
	TCPHandler_UI*		m_pTCPHandler_UI;
	
	MotorControl*		m_pMotorControl;
	DisplayControl*		m_pDisplayControl;
	OpMode				m_StateOpMode;
	SysState			m_StateSysState;
	bool				m_RequestPending;
public:
	int					cReceiveTime; // = 1000;
	SysControl(MotorControl* pMctrl, DisplayControl* pDctrl);
	~SysControl();
	void setCommand(Command cmd);
	void setOpMode(OpMode mode);
	void setSysState(SysState state);
	OpMode getOpMode();
	SysState getSysState();
	TCPHandler_Chain* getTCPHandler_Chain();
	KeyboardHandler* getKeyboardHandler();
	void setRequestPending(bool req);
	bool getRequestPending();
	//void setMotorControl(MotorControl* pMctrl);		// not needed as provided in constructor
	//void setDisplayControl(DisplayControl* pDctrl);
};

#endif /* SYSCONTROL_H_ */
