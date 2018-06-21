/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: ert_main.c
 *
 * Code generated for Simulink model 'piCtrl'.
 *
 * Model version                  : 1.59
 * Simulink Coder version         : 8.13 (R2017b) 24-Jul-2017
 * C/C++ source code generated on : Thu May 17 16:13:04 2018
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: 32-bit Generic
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include <stddef.h>
extern "C"{
	#include "rtwtypes.h"
	#include "hwFunc.h"
}
#include "MotorControl.h"
#include "piCtrl.h"                    /* Model's header file */

/* ANSI C headers */
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* VxWorks headers */
#include <vxWorks.h>
#include <taskLib.h>
#include <sysLib.h>
#include <semLib.h>
#include <rebootLib.h>
#include <logLib.h>

/* this sets the standard stack size for spawned tasks used by the model.
 * this can be changed by compiling with '-DSTACK_SIZE=nnn' where nnn is
 * the stack size desired.
 */
#ifndef STACK_SIZE
#define STACK_SIZE                     16384
#endif

static int_T tBaseRate(SEM_ID sem, SEM_ID startStopSem, MotorControl* pMotorControl)
{
  int_T i;
  int speedRpm;
  double motorVoltage;
  int voltageDig;
  double pulseCnt;
  int targetSpeed;
  
  while (1) {
    if (rtmGetErrorStatus(piCtrl_M) != (NULL)) {
      fprintf(stderr,"Error status: %s \n", rtmGetErrorStatus(piCtrl_M));
      semGive(startStopSem);
      return(1);
    }

    if (semTake(sem,NO_WAIT) != ERROR) {
      logMsg("Rate for SingleRate task too fast.\n",0,0,0,0,0,0);
    } else {
      semTake(sem, WAIT_FOREVER);
    }

    /* Set model inputs here */
    pulseCnt = (double) getEncoderPulsesZeroCorrected();
    speedRpm = (int) (getRotationDirection(0) * pulseCnt * 60.0 / (0.015625 * 64)); 
    targetSpeed = pMotorControl->getTargetSpeed();
    if(!pMotorControl->getDirection()){
    	targetSpeed *= -1;
    }
    
    piCtrl_U.In2 = targetSpeed - speedRpm;		// set target speed - current speed
    
    /* Step the model */
    piCtrl_step();

    /* Get model outputs here */
    motorVoltage = piCtrl_Y.u_ref;
    //printf("%d;%d\n",  targetSpeed, speedRpm);		// HN-DEBUG - to get ramp and step response
    voltageDig = 2048 - (int)(motorVoltage * 220.0);	// Depending on the setup used 0rpm is at approximately 2600, not at 2048
    //printf("%d;%d;%d\n",  targetSpeed, speedRpm, voltageDig);		// HN-DEBUG - to get ramp and step response
    if (voltageDig > 4095) {
      voltageDig = 4095;
    }
    else if (voltageDig < 0) {
      voltageDig = 0;
    }
    writeAnalog(0,voltageDig);
  }

  return(1);
}

/*
 * Spawn piCtrl_main as an independent VxWorks task from your
 * application code, specifying its O/S priority
 */
int_T piCtrl_main(int_T priority, MotorControl* pMotorControl)
{
  const char *status;
  real_T requestedSR, actualSR;
  int_T VxWorksTIDs[1];
  SEM_ID rtTaskSemaphoreList[1];
  SEM_ID rtClockSem, startStopSem;
  printf("Warning: The simulation will run forever. "
         "To change this behavior select the 'MAT-file logging' option.\n");
  //fflush((NULL));
  if (priority <= 0 || priority > 255-(1)+1) {
    priority = 30;
  }

  rtClockSem = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);
  startStopSem = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);

  /* Initialize model */
  piCtrl_initialize();
  requestedSR = 1.0 / 0.015625;
  sysAuxClkDisable();
  sysAuxClkRateSet((int_T)(requestedSR + 0.5));
  actualSR = (real_T) sysAuxClkRateGet();
  printf("Actual sample rate in Hertz: %f\n",actualSR);
  VxWorksTIDs[0] = taskSpawn("tBaseRate",
    priority, VX_FP_TASK, STACK_SIZE, (FUNCPTR)tBaseRate, (int_T) rtClockSem,
    (int_T) startStopSem, (int) pMotorControl, 0, 0, 0, 0, 0, 0, 0);
  if (sysAuxClkConnect((FUNCPTR) semGive, (int_T) rtClockSem) == OK) {
    rebootHookAdd((FUNCPTR) sysAuxClkDisable);
    printf("\nSimulation starting\n");
    sysAuxClkEnable();
  }

  semTake(startStopSem, WAIT_FOREVER);
  sysAuxClkDisable();
  taskDelete(VxWorksTIDs[0]);
  semDelete(rtClockSem);
  semDelete(startStopSem);

  /* Disable rt_OneStep() here */

  /* Terminate model */
  piCtrl_terminate();
  return(EXIT_SUCCESS);
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
