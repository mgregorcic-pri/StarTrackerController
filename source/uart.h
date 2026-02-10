
/**
 ********************************************************************************
 * @file    uart.h
 * @author  
 * @date    
 * @brief   
 ********************************************************************************
 */

#ifndef _UART_H_
#define _UART_H_

#ifdef __cplusplus
extern "C" {
#endif

/************************************
 * INCLUDES
 ************************************/
#include "stdbool.h"
#include "stdint.h" 
#include "main.h"
#include "cbuffer.h"

/************************************
 * MACROS AND DEFINES
 ************************************/
#define UART_RX_BUFFER_SIZE    1024u
/************************************
 * TYPEDEFS
 ************************************/

/************************************
 * EXPORTED VARIABLES
 ************************************/

/************************************
 * GLOBAL FUNCTION PROTOTYPES
 ************************************/
void UART_Init(circular_buf_t *rxBuffer);
bool UART_Transmit(uint8_t *pData, uint16_t dataLen);
bool UART_ReadByte(uint8_t *pData);
uint16_t UART_RxBufferDataCount(void);

void UART_ByteReceived_Callback(UART_HandleTypeDef *huart);

 
#ifdef __cplusplus
}
#endif

#endif 
