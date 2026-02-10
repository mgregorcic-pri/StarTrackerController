
/**
 ********************************************************************************
 * @file    sdrive.h
 * @author  
 * @date    
 * @brief   
 ********************************************************************************
 */

#ifndef _SDRIVE_H_
#define _SDRIVE_H_

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

/************************************
 * TYPEDEFS
 ************************************/
typedef enum
{
  ERROR_OK = 0,
  ERROR_OVERCURRENT,
  ERROR_UNDERVOLTAGE,
  ERROR_OPEN_LOAD,
  ERROR_OVERTEMPERATURE,
  ERROR_DRIVER,
  ERROR_STALL,
} stepper_error_e;

typedef enum
{
    CPU = 0,
    NTC,
    TC,
} temperature_sensor_type_t;

typedef enum
{
    DIRECTION_FORWARD = 0,
    DIRECTION_BACKWARD
} step_direction_e;

typedef enum
{
    TRQ_DAC_100 = 0,
    TRQ_DAC_93 = 1,
    TRQ_DAC_87 = 2,
    TRQ_DAC_81 = 3,
    TRQ_DAC_75 = 4,
    TRQ_DAC_68 = 5,
    TRQ_DAC_62 = 6,
    TRQ_DAC_56 = 7,
    TRQ_DAC_50 = 8,
    TRQ_DAC_43 = 9,
    TRQ_DAC_37 = 10,
    TRQ_DAC_31 = 11,
    TRQ_DAC_25 = 12,
    TRQ_DAC_18 = 13,
    TRQ_DAC_12 = 14,
    TRQ_DAC_6 = 15,
    TRQ_DAC_0 = 16
} torque_control_e;

typedef enum
{
    MICROSTEP_FULL_100 = 0,
    MICROSTEP_FULL_71 = 1,
    MICROSTEP_NON_CIRC_1_2 = 2,
    MICROSTEP_1_2 = 3,
    MICROSTEP_1_4 = 4,
    MICROSTEP_1_8 = 5,
    MICROSTEP_1_16 = 6,
    MICROSTEP_1_32 = 7,
    MICROSTEP_1_64 = 8,
    MICROSTEP_1_128 = 9,
    MICROSTEP_1_256 = 10
} step_mode_e;

typedef enum{
    SENSOR_NONE = 0,
    SENSOR_STOP,
    SENSOR_EM_STOP,
} end_sensor_type_e;

typedef enum{
    SENSOR_POL_LOW = 0,
    SENSOR_POL_HIGH = 1,
} end_sensor_polarity_e;

typedef struct{
    end_sensor_type_e type;
    end_sensor_polarity_e polarity;
    bool currentState;
    uint16_t adcValue;
    uint16_t hysteresisLvlHigh;
    uint16_t hysteresisLvlLow;
} end_sensor_t;

typedef enum
{
    CONTROL_POSITION,
    CONTROL_VELOCITY,
} control_mode_t;

typedef struct 
{
    torque_control_e holdTorque;
    torque_control_e runTorque;
    uint16_t phaseCurrent;
    step_mode_e stepMode;
    end_sensor_t rightSensor;
    end_sensor_t leftSensor;
    uint16_t accelerationRamp;
    uint16_t decelerationRamp;
    uint16_t frequency;
    int32_t requestedPosition;
    control_mode_t controlMode;
    temperature_sensor_type_t temperatureSensor;
} sdrive_settings_t;

typedef struct
{
    uint32_t canNodeID;
} sdrive_t;

typedef struct 
{
    int32_t position;
    float velocity;
    int16_t temperature;
    stepper_error_e error;
} sdrive_status_t;


/************************************
 * EXPORTED VARIABLES
 ************************************/

/************************************
 * GLOBAL FUNCTION PROTOTYPES
 ************************************/
void SDRIVE_Init(sdrive_t *sDrive, sdrive_settings_t Settings);
void SDRIVE_SetPosition(sdrive_t *sDrive, int32_t position);
void SDRIVE_Run(sdrive_t *sDrive, uint32_t speed, step_direction_e direction);
void SDRIVE_Stop(sdrive_t *sDrive); 
void SDRIVE_Handler(void);
void SDRIVE_RegisterStatus(sdrive_status_t *Status, sdrive_t sDrive);

#ifdef __cplusplus
}
#endif

#endif 
