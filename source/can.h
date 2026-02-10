
/**
 ********************************************************************************
 * @file    can.h
 * @author  
 * @date    
 * @brief   
 ********************************************************************************
 */

#ifndef _CAN_H_
#define _CAN_H_

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

void CAN_Init(void);
bool CAN_Transmit(uint32_t id, uint8_t *pData, uint8_t length);
uint32_t CAN_GetNumberOfUnreadFrames(void);
void CAN_Read(FDCAN_RxHeaderTypeDef *RxFrame, uint8_t *pData);
void CAN_Handler(void);
 
#ifdef __cplusplus
}
#endif

#endif 
