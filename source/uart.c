
/**
 ********************************************************************************
 * @file    uart.c
 * @author  
 * @date    
 * @brief   
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "uart.h"

/************************************
 * EXTERN VARIABLES
 ************************************/
extern UART_HandleTypeDef huart2;
/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/

/************************************
 * PRIVATE TYPEDEFS
 ************************************/
circular_buf_t *RxCircBuffer;

/************************************
 * STATIC VARIABLES
 ************************************/

/************************************
 * GLOBAL VARIABLES
 ************************************/

/************************************
 * STATIC FUNCTION PROTOTYPES
 ************************************/


/************************************
 * STATIC FUNCTIONS
 ************************************/
void UART_ByteReceived_Callback(UART_HandleTypeDef *huart)
{
    circular_buf_put(RxCircBuffer, USART2->RDR & 0xFF);
}

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
void UART_Init(circular_buf_t *rxBuffer)
{
    RxCircBuffer = rxBuffer;

    // Enable interrupt
    USART2->CR1 |= (1u << 5);
}

bool UART_Transmit(uint8_t *pData, uint16_t dataLen)
{
    HAL_StatusTypeDef status = HAL_UART_Transmit_DMA(&huart2, pData, dataLen);
    return (status == HAL_OK);
}

bool UART_ReadByte(uint8_t *pData)
{
    return circular_buf_get(RxCircBuffer, pData);
}

uint16_t UART_RxBufferDataCount(void)
{
    return circular_buf_dataCount(RxCircBuffer);
}
