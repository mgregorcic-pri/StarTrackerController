
/**
 ********************************************************************************
 * @file    can.c
 * @author  
 * @date    
 * @brief   
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "can.h"

/************************************
 * EXTERN VARIABLES
 ************************************/
extern FDCAN_HandleTypeDef hfdcan1;

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/
#define CAN_RX_ID              0x100
#define CAN_BROADCAST_ID       0x200
#define NODE_ID                0x01
/************************************
 * PRIVATE TYPEDEFS
 ************************************/

/************************************
 * STATIC VARIABLES
 ************************************/
FDCAN_TxHeaderTypeDef TxHeader;
FDCAN_RxHeaderTypeDef RxHeader;
uint8_t TxData[8];

/************************************
 * GLOBAL VARIABLES
 ************************************/

/************************************
 * STATIC FUNCTION PROTOTYPES
 ************************************/
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs);

/************************************
 * STATIC FUNCTIONS
 ************************************/

/************************************
 * GLOBAL FUNCTIONS
 ************************************/

 void CAN_Init(void) 
{
    FDCAN_FilterTypeDef sFilterConfig;

    /* Configure Rx filter */
    sFilterConfig.IdType = FDCAN_EXTENDED_ID;
    sFilterConfig.FilterIndex = 0;
    sFilterConfig.FilterType = FDCAN_FILTER_RANGE;
    sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    sFilterConfig.FilterID1 = 0;
    sFilterConfig.FilterID2 = 0x1FFFFFFF;
    if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK)
    {
        Error_Handler();
    }

    /* Start the FDCAN module */
    if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
    {
        Error_Handler();
    }

    /* Prepare Tx Header */
    TxHeader.Identifier = 0;
    TxHeader.IdType = FDCAN_EXTENDED_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.DataLength = FDCAN_DLC_BYTES_2;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_PASSIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_ON;
    TxHeader.FDFormat = FDCAN_FD_CAN;
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;
}

bool CAN_Transmit(uint32_t id, uint8_t *pData, uint8_t length) 
{
    TxHeader.DataLength = FDCAN_DLC_BYTES_16;
    TxHeader.Identifier = id;
    TxHeader.IdType = FDCAN_EXTENDED_ID;
    HAL_StatusTypeDef status = HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, pData);

    return (status == HAL_OK) ? true : false;
}

uint32_t CAN_GetNumberOfUnreadFrames(void)
{
    return HAL_FDCAN_GetRxFifoFillLevel(&hfdcan1, FDCAN_RX_FIFO0);
}

void CAN_Read(FDCAN_RxHeaderTypeDef *RxFrame, uint8_t *pData)
{
    HAL_StatusTypeDef status = HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, RxFrame, pData);
}

void CAN_Handler(void)
{
    while(1)
    {
        if(CAN_GetNumberOfUnreadFrames() > 0)
        {
            uint8_t pData[64] = {0};
            CAN_Read(&RxHeader,pData);
            
        }
    }
}