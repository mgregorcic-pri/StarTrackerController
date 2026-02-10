
/**
 ********************************************************************************
 * @file    app_comm.h
 * @author  
 * @date    
 * @brief   
 ********************************************************************************
 */

#ifndef _APP_COMM_H_
#define _APP_COMM_H_

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
#define STX 0x02u
#define QM 0x3Fu
#define ETX 0x03u

#define MAX_PACKET_DATA_LEN 32u
/************************************
 * TYPEDEFS
 ************************************/
typedef enum
{
    ACTION_SET_ALT_AZ_VELOCITIES = 0,
    ACTION_SET_ALT_AZ_POSITION,
    ACTION_SET_FOCUSER_VELOCITY,
    ACTION_SET_FOCUSER_POSITION,
    ACTION_GET_INITIAL_POSITION,
} action_e;

typedef enum
{
    RESPONSE_SEND_STATUS = 0,
    RESPONSE_SEND_INITIAL_POSITION,
    RESPONSE_SEND_ERROR
} response_e;

typedef struct
{
    uint8_t packetCommand;
    uint8_t dataLen;
    uint8_t data[MAX_PACKET_DATA_LEN]; // Max 32 bytes
} packet_t;

/************************************
 * EXPORTED VARIABLES
 ************************************/

/************************************
 * GLOBAL FUNCTION PROTOTYPES
 ************************************/
void APP_COMM_Init(void);
void APP_COMM_Handler(void);
bool APP_COMM_GetPacket(packet_t *packet);
bool APP_COMM_SendPacket(action_e action, uint8_t *pData, uint8_t dataLen);
 
#ifdef __cplusplus
}
#endif

#endif 
