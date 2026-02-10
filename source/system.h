
/**
 ********************************************************************************
 * @file    systttttttttttttttttttttttttttttttttttttttttttt.h
 * @author  
 * @date    
 * @brief   
 ********************************************************************************
 */

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

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

/************************************
 * EXPORTED VARIABLES
 ************************************/

/************************************
 * GLOBAL FUNCTION PROTOTYPES
 ************************************/

void SYSTEM_Init(void);
volatile uint32_t* SYSTEM_GetPtrToTick(void);
volatile uint32_t SYSTEM_GetSysTick(void);
uint32_t HAL_GetTick(void);
 
#ifdef __cplusplus
}
#endif

#endif 
