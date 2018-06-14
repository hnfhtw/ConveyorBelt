/*
 * DisplayControl.h
 *
 *  Created on: 16.04.2018
 *      Author: HN / MG
 */

#ifndef DISPLAYCONTROL_H_
#define DISPLAYCONTROL_H_

#include "SysControl.h"
#include "MotorControl.h"

class MotorControl;
class SysControl;

class DisplayControl{
private:
	SysControl*		m_pSysControl;
	MotorControl*	m_pMotorControl;
public:
	DisplayControl();
	~DisplayControl();
	void updateDisplay(bool setTime);
	void setMotorControl(MotorControl* pMctrl);
	void setSysControl(SysControl* pSctrl);
};

#endif /* DISPLAYCONTROL_H_ */
