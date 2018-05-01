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
#include "ifLib.h"			 // check if needed
#include "systemManager.h"
#include "stateMachine.h"

SystemManager * pSystemManager;
StateMachine * pStateMachine;

void setLocalIp();

int main (void) {
	// Set tick to 5 ms. This is the time resolution!
	sysClkRateSet(200);
	
	// Set local IP address according to MAC table
	setLocalIp();
	
  // Initialize target hardware
	initHardware(0);
	     
	// Create instance of top class
  pSystemManager = new SystemManager;

	// Start the state machine. This method blocks, so no while(1) is needed.
  pStateMachine->runToCompletion();
}
