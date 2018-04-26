/*
 * main.cpp
 *
 *  Created on: 16.04.2018
 *      Author: HN / MG
 */
/*
#include <stdlib.h>
#include <stdio.h>
#include <intLib.h>
#include <taskLib.h>
#include <sysLib.h>
#include <time.h>
#include "SysControl.h"
#include "MotorControl.h"
#include "DisplayControl.h"
#include "stateMachine.h"

SysControl* pSysControl;
MotorControl* pMotorControl;
DisplayControl* pDisplayControl;
StateMachine* pStateMachine;

int main (void)
{
    //initHardware(0);  // check if needed here
    // add code for systick resolution
  
    // create objects
    pDisplayControl = new DisplayControl;
    pMotorControl = new MotorControl(pDisplayControl);
    pSysControl = new SysControl(pMotorControl, pDisplayControl);
    
    // add missing links between objects
    pDisplayControl->setMotorControl(pMotorControl);
    pDisplayControl->setSysControl(pSysControl);
    pMotorControl->setSysControl(pSysControl);
    
    // start state machine
    pStateMachine->sendEvent("InitSystem");
    pStateMachine->runToCompletion();
    
    return 0;
}*/
