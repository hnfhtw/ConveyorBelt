/*
 * MotorControl.h
 *
 *  Created on: 16.04.2018
 *      Author: HN / MG
 */

#ifndef MOTORCONTROL_H_
#define MOTORCONTROL_H_

#include "SysControl.h"
#include "DisplayControl.h"
#include "MotorState.h"

class DisplayControl;

class MotorControl{		// todo: update class diagram
private:
	DisplayControl*	m_pDisplayControl;
	SysControl*		m_pSysControl;
	MotorState		m_MotorState;
	bool			m_Direction;				// TRUE = clockwise, FALSE = counter clockwise
	int				m_MotorSpeedFinal;
	int				m_TargetSpeed;
public:
	MotorControl(DisplayControl* pDctrl);
	~MotorControl();
	void setMotorState(MotorState state);
	MotorState getMotorState();
	void setMotorSpeedFinal(int speed);
	int getMotorSpeedFinal();
	void setDirection(bool dir);
	bool getDirection();
	//void setDisplayControl(DisplayControl* pDctrl);	// pointer provided in constructor already
	void setSysControl(SysControl* pSysctrl);
	void setTargetSpeed(int speed);
	int getTargetSpeed();
	void decreaseSpeed();
	void increaseSpeed();
};

#endif /* MOTORCONTROL_H_ */
