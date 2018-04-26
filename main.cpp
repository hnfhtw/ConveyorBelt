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
#include "ifLib.h"			 // check
#include "systemManager.h"	// check
#include "stateMachine.h"	// check

SystemManager * pSystemManager;
StateMachine * pStateMachine;

void setLocalIp();

int main (void) {

	// Set tick to 5 ms. This is the time resolution!
	sysClkRateSet(200);
	
	// Set local IP address according to MAC table
	setLocalIp();
	
    // create objects
    /*pDisplayControl = new DisplayControl;
    pMotorControl = new MotorControl(pDisplayControl);
    pSysControl = new SysControl(pMotorControl, pDisplayControl);
    
    // add missing links between objects
    pDisplayControl->setMotorControl(pMotorControl);
    pDisplayControl->setSysControl(pSysControl);
    pMotorControl->setSysControl(pSysControl);*/
    
    // start state machine
    //pStateMachine->sendEvent("InitSystem");
    //pStateMachine->runToCompletion();
    
    
	// Create instance of top class
    pSystemManager = new SystemManager;

	// Start the state machine. This method blocks, so no while(1) is needed.
    pStateMachine->runToCompletion();
	
}
