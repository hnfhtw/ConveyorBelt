/*
 * KeyboardHandler.h
 *
 *  Created on: 16.04.2018
 *      Author: HN / MG
 */

#ifndef KEYBOARDHANDLER_H_
#define KEYBOARDHANDLER_H_

#include "SysControl.h"
#include "MotorControl.h"

class MotorControl;
class SysControl;

class KeyboardHandler{
private:
	MotorControl*	m_pMotorControl;
	SysControl*		m_pSysControl;
public:
	KeyboardHandler(SysControl* pSctrl, MotorControl* pMctrl);
	~KeyboardHandler();
	void evaluateKey();	
};

#endif /* KEYBOARDHANDLER_H_ */
