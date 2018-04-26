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
	bool			m_Direction;
	int				m_MotorSpeedFinal;
	int				m_TargetSpeed;
	void decreaseSpeed();
	void increaseSpeed();
public:
	int				cMotorSpeedSlow; //= 100;
	int				cTimeTransition; //= 6000;
	int				cTimeStep; // = 50;
	MotorControl(DisplayControl* pDctrl);
	~MotorControl();
	void moveSlow();
	void startRamp();
	void stop();
	MotorState getMotorState();
	void setMotorSpeedFinal(int speed);
	void setDirection(bool dir);
	//void setDisplayControl(DisplayControl* pDctrl);	// pointer provided in constructor already
	void setSysControl(SysControl* pSysctrl);
};

#endif /* MOTORCONTROL_H_ */
