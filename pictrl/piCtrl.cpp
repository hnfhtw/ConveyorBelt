/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: piCtrl.c
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

#include "piCtrl.h"
#include "piCtrl_private.h"

/* Block states (auto storage) */
D_Work_piCtrl piCtrl_DWork;

/* External inputs (root inport signals with auto storage) */
ExternalInputs_piCtrl piCtrl_U;

/* External outputs (root outports fed by signals with auto storage) */
ExternalOutputs_piCtrl piCtrl_Y;

/* Real-time model */
RT_MODEL_piCtrl piCtrl_M_;
RT_MODEL_piCtrl *const piCtrl_M = &piCtrl_M_;

/* Model step function */
void piCtrl_step(void)
{
  real_T rtb_Sum;
  real_T rtb_Saturate;

  /* Sum: '<S2>/Sum' incorporates:
   *  DiscreteIntegrator: '<S2>/Integrator'
   *  Gain: '<S2>/Proportional Gain'
   *  Inport: '<Root>/In2'
   */
  rtb_Sum = 8.0E-6 * piCtrl_U.In2 + piCtrl_DWork.Integrator_DSTATE;
  //rtb_Sum = 0.02175 * piCtrl_U.In2 + piCtrl_DWork.Integrator_DSTATE;
  
  /* Saturate: '<S2>/Saturate' */
  if (rtb_Sum > 13.0) {
    rtb_Saturate = 13.0;
  } else if (rtb_Sum < -10.0) {
    rtb_Saturate = -10.0;
  } else {
    rtb_Saturate = rtb_Sum;
  }

  /* End of Saturate: '<S2>/Saturate' */

  /* Outport: '<Root>/u_ref' */
  piCtrl_Y.u_ref = rtb_Saturate;

  /* Update for DiscreteIntegrator: '<S2>/Integrator' incorporates:
   *  Gain: '<S2>/Integral Gain'
   *  Gain: '<S2>/Kb'
   *  Inport: '<Root>/In2'
   *  Sum: '<S2>/SumI1'
   *  Sum: '<S2>/SumI2'
   */
  piCtrl_DWork.Integrator_DSTATE += ((rtb_Saturate - rtb_Sum) * 0.015625 +
    0.015625 * piCtrl_U.In2) * 0.015625;
  /*piCtrl_DWork.Integrator_DSTATE += ((rtb_Saturate - rtb_Sum) * 0.1 +
    0.1 * piCtrl_U.In2) * 0.015625;*/
}

/* Model initialize function */
void piCtrl_initialize(void)
{
  /* Registration code */

  /* initialize error status */
  rtmSetErrorStatus(piCtrl_M, (NULL));

  /* states (dwork) */
  (void) memset((void *)&piCtrl_DWork, 0,
                sizeof(D_Work_piCtrl));

  /* external inputs */
  piCtrl_U.In2 = 0.0;

  /* external outputs */
  piCtrl_Y.u_ref = 0.0;
}

/* Model terminate function */
void piCtrl_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
