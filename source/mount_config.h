/**
 ********************************************************************************
 * @file    app.h
 * @author  
 * @date    
 * @brief   
 ********************************************************************************
 */

#ifndef _MOUNT_CONFIG_H_
#define _MOUNT_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/************************************
 * INCLUDES
 ************************************/
#include "stdbool.h"
#include "stdint.h" 
#include "main.h"

/************************************
 * MACROS AND DEFINES
 ************************************/
// --- Macros for gear ratios to velocity calculation ---
#define AZIMUTH_GEAR_N_TEETH    1000.0f
#define AZIMUTH_MOTOR_N_TEETH   20.0f
#define AZIMUTH_GEAR_RATIO (float)(AZIMUTH_GEAR_N_TEETH / AZIMUTH_MOTOR_N_TEETH)
#define AZIMUTH_STEPS_PER_MOTOR_REVOLUTION 400.0
#define AZIMUTH_DEGREES_TO_STEPS (float)(AZIMUTH_STEPS_PER_MOTOR_REVOLUTION  * AZIMUTH_GEAR_RATIO / 360.0)

#define ALTITUDE_GEAR_N_TEETH   1000.0f
#define ALTITUDE_MOTOR_N_TEETH  20.0f
#define ALTITUDE_GEAR_RATIO (float)(ALTITUDE_GEAR_N_TEETH / ALTITUDE_MOTOR_N_TEETH)
#define ALTITUDE_STEPS_PER_MOTOR_REVOLUTION 400.0
#define ALTITUDE_DEGREES_TO_STEPS (float)(ALTITUDE_STEPS_PER_MOTOR_REVOLUTION  * ALTITUDE_GEAR_RATIO / 360.0)


#define NUMBER_OF_STEPPER_DRIVES 2
/************************************
 * TYPEDEFS
 ************************************/

/************************************
 * EXPORTED VARIABLES
 ************************************/

/************************************
 * GLOBAL FUNCTION PROTOTYPES
 ************************************/
 
#ifdef __cplusplus
}
#endif

#endif 
