
/**
 ********************************************************************************
 * @file    app.h
 * @author  
 * @date    
 * @brief   
 ********************************************************************************
 */

#ifndef _APP_H_
#define _APP_H_

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
typedef struct 
{
    float altitude;
    float azimuth;
    float altitudeVelocity;
    float azimuthVelocity;
    bool altitudeEndswitchLow;
    bool altitudeEndswitchHigh;
    int16_t temperature;
} status_t;

typedef struct
{
    status_t status;
} app_data_t;

/************************************
 * EXPORTED VARIABLES
 ************************************/

/************************************
 * GLOBAL FUNCTION PROTOTYPES
 ************************************/
void APP_Init(void);
void APP_Handler(void);
 
#ifdef __cplusplus
}
#endif

#endif 
