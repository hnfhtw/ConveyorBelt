/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: piCtrl.h
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

#ifndef RTW_HEADER_piCtrl_h_
#define RTW_HEADER_piCtrl_h_
#include <stddef.h>
#include <string.h>
#ifndef piCtrl_COMMON_INCLUDES_
# define piCtrl_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* piCtrl_COMMON_INCLUDES_ */

#include "piCtrl_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

/* Block states (auto storage) for system '<Root>' */
typedef struct {
  real_T Integrator_DSTATE;            /* '<S2>/Integrator' */
} D_Work_piCtrl;

/* External inputs (root inport signals with auto storage) */
typedef struct {
  real_T In2;                          /* '<Root>/In2' */
} ExternalInputs_piCtrl;

/* External outputs (root outports fed by signals with auto storage) */
typedef struct {
  real_T u_ref;                        /* '<Root>/u_ref' */
} ExternalOutputs_piCtrl;

/* Real-time Model Data Structure */
struct tag_RTM_piCtrl {
  const char_T *errorStatus;
};

/* Block states (auto storage) */
extern D_Work_piCtrl piCtrl_DWork;

/* External inputs (root inport signals with auto storage) */
extern ExternalInputs_piCtrl piCtrl_U;

/* External outputs (root outports fed by signals with auto storage) */
extern ExternalOutputs_piCtrl piCtrl_Y;

/* Model entry point functions */
extern void piCtrl_initialize(void);
extern void piCtrl_step(void);
extern void piCtrl_terminate(void);
int_T piCtrl_main(int_T priority);

/* Real-time Model object */
extern RT_MODEL_piCtrl *const piCtrl_M;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Note that this particular code originates from a subsystem build,
 * and has its own system numbers different from the parent model.
 * Refer to the system hierarchy for this subsystem below, and use the
 * MATLAB hilite_system command to trace the generated code back
 * to the parent model.  For example,
 *
 * hilite_system('Motor/piCtrl')    - opens subsystem Motor/piCtrl
 * hilite_system('Motor/piCtrl/Kp') - opens and selects block Kp
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'Motor'
 * '<S1>'   : 'Motor/piCtrl'
 * '<S2>'   : 'Motor/piCtrl/Discrete PI Speed'
 */
#endif                                 /* RTW_HEADER_piCtrl_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
