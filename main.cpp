/*
 * main.cpp
 *
 *  Created on: 16.04.2018
 *      Author: HN / MG
 */

#include <taskLib.h>
#include <stdio.h>
#include <intLib.h>
#include <sysLib.h>
#include <stdLib.h>
extern "C"{
	#include "hwFunc.h"
}
#include "SysControl.h"
#include "MotorControl.h"
#include "DisplayControl.h"

void setLocalIp();

int main (void) {
	// Set tick to 5 ms. This is the time resolution!
	sysClkRateSet(200);
	
	// Set local IP address according to MAC table
	setLocalIp();
	
	// Initialize target hardware
	initHardware(0);
	motorOn();	// switch on motor
	    
	// Create instances of top classes
	DisplayControl* pDisplayControl = new DisplayControl;
	MotorControl* pMotorControl = new MotorControl(pDisplayControl);
	SysControl* pSysControl = new SysControl(pMotorControl, pDisplayControl);
   
	// Create state machine
	pSysControl->createStateMachine();
	
    // Add missing links between objects
	pDisplayControl->setMotorControl(pMotorControl);
	pDisplayControl->setSysControl(pSysControl);
	pMotorControl->setSysControl(pSysControl);
	
	// Initial update of display
	pDisplayControl->updateDisplay(false);
	
	// Start PI controller for motor
	pMotorControl->startPICtrl();
		
	// Start the state machine. This method blocks, so no while(1) is needed.
	pSysControl->getStateMachine()->runToCompletion();
}
