
/**
 ********************************************************************************
 * @file    sdrive.c
 * @author  
 * @date    
 * @brief   
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "sdrive.h"
#include "can.h"
#include "mount_config.h"

/************************************
 * EXTERN VARIABLES
 ************************************/

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/

/************************************
 * PRIVATE TYPEDEFS
 ************************************/
typedef enum 
{
    TX_ACTION_APPLY_SETTINGS,
    TX_ACTION_READ_SETTINGS,
    TX_ACTION_SINGLE_STEP,
    TX_ACTION_MULTI_STEP,
    TX_ACTION_GO_TO_POSITION,
    TX_ACTION_VELOCITY_CONTROL,
    TX_ACTION_STOP_AND_RESET,
    TX_ACTION_CONFIGURE_CAN,
    TX_ACTION_REPORT_CAN_CONFIG,
    TX_ACTION_ADD_SUBCYCLE,
    TX_ACTION_DELETE_CYCLE_PROFILE,
    TX_ACTION_START_CYCLING,
    TX_ACTION_STOP_CYCLING,
    TX_ACTION_START_STALL_LEARN,
    TX_ACTION_REPORT_FLASHED_CYCLE,
    TX_ACTION_SAVE_CYCLE_TO_FLASH,
} tx_action_e;

typedef enum
{
    RX_ACTION_REPORT_STATUS = 0,
    RX_ACTION_REPORT_SETTINGS,
    RX_ACTION_REPORT_CAN_CONFIG,  
    RX_ACTION_REPORT_CYCLE_MEASUREMENTS,  
    RX_ACTION_REPORT_FLASHED_CYCLE,
} rx_action_e;

/************************************
 * STATIC VARIABLES
 ************************************/

/************************************
 * GLOBAL VARIABLES
 ************************************/
uint8_t NumberOfRegisteredSdrives = 0;
/************************************
 * STATIC FUNCTION PROTOTYPES
 ************************************/
sdrive_status_t* RegisterStatusPointers[2] = {NULL};
sdrive_t RegisterStatusSdrives[2] = {0};
/************************************
 * STATIC FUNCTIONS
 ************************************/
void SDRIVE_InitCan(void);

/************************************
 * GLOBAL FUNCTIONS
 ************************************/

void SDRIVE_InitCan(void)
{
    static bool isInitialized = false;
    if(isInitialized)
    {
        return;
    }
    isInitialized = true;
    CAN_Init();
}

void SDRIVE_Init(sdrive_t *sDrive, sdrive_settings_t Settings)
{
    SDRIVE_InitCan();

    uint8_t pData[16];
    pData[0] = TX_ACTION_APPLY_SETTINGS;
    pData[1] = (uint8_t)(((uint8_t)(Settings.holdTorque) & 0xF) | ((uint8_t)(Settings.holdTorque << 4) & 0xF0));
    pData[2] = (uint8_t)(( (uint16_t)(Settings.accelerationRamp) ) >> 8);
    pData[3] = (uint8_t)( (uint16_t)(Settings.accelerationRamp) );
    pData[4] = (uint8_t)(( (uint16_t)(Settings.frequency) ) >> 16);
    pData[5] = (uint8_t)(( (uint16_t)(Settings.frequency) ) >> 8);
    pData[6] = (uint8_t)(( (uint16_t)(Settings.frequency) ) >> 0);
    pData[7] = (uint8_t)(((uint8_t)(Settings.stepMode) << 4) & 0xF0);
    pData[8] = (uint8_t)
        ((Settings.leftSensor.type & 0x3) | 
        ((Settings.leftSensor.polarity << 2) & 0x4) | 
        ((Settings.rightSensor.type << 3) & 0x18) | 
        ((Settings.rightSensor.polarity << 5) & 0x20) | 
        ((Settings.temperatureSensor << 6) & 0xC0));
    pData[9] =  (uint8_t)(( ((uint32_t)Settings.leftSensor.hysteresisLvlLow  ) << 8) >> 11);
    pData[10] = (uint8_t)(( ((uint32_t)Settings.leftSensor.hysteresisLvlHigh ) << 8) >> 11);
    pData[11] = (uint8_t)(( ((uint32_t)Settings.rightSensor.hysteresisLvlLow ) << 8) >> 11);
    pData[12] = (uint8_t)(( ((uint32_t)Settings.rightSensor.hysteresisLvlHigh) << 8) >> 11);
    pData[13] = (uint8_t)(Settings.phaseCurrent >> 8);
    pData[14] = (uint8_t)(Settings.phaseCurrent >> 0);

    CAN_Transmit(sDrive->canNodeID, pData, sizeof(pData));
}

void SDRIVE_Run(sdrive_t *sDrive, uint32_t speed, step_direction_e direction)
{
    uint8_t pData[16] = {0};
    pData[0] = TX_ACTION_VELOCITY_CONTROL;

    int32_t iSpeed = (direction == DIRECTION_FORWARD) ? (int32_t)speed : -(int32_t)speed;
    pData[1] = (uint8_t)(((int32_t)(iSpeed) ) >> 24);
    pData[2] = (uint8_t)(((int32_t)(iSpeed) ) >> 16);
    pData[3] = (uint8_t)(((int32_t)(iSpeed) ) >> 8);
    pData[4] = (uint8_t)(((int32_t)(iSpeed) ) >> 0);
       
    pData[5] = (uint8_t)(0xFF);
    pData[6] = (uint8_t)(0xFF);
    pData[7] = (uint8_t)(0xFF);
    pData[8] = (uint8_t)(0xFF);

    CAN_Transmit(sDrive->canNodeID, pData, sizeof(pData));
}

void SDRIVE_SetPosition(sdrive_t *sDrive, int32_t position)
{
    uint8_t pData[12] = {0};
    pData[0] = TX_ACTION_GO_TO_POSITION;
    pData[1] = (uint8_t)(((uint32_t)(position) ) >> 24);
    pData[2] = (uint8_t)(((uint32_t)(position) ) >> 16);
    pData[3] = (uint8_t)(((uint32_t)(position) ) >> 8);
    pData[4] = (uint8_t)(((uint32_t)(position) ) >> 0);

    CAN_Transmit(sDrive->canNodeID, pData, sizeof(pData));
}

void SDRIVE_Stop(sdrive_t *sDrive)
{
    uint8_t pData[8] = {0};
    pData[0] = TX_ACTION_APPLY_SETTINGS;

    CAN_Transmit(sDrive->canNodeID, pData, sizeof(pData));
}

void SDRIVE_Handler(void)
{
    FDCAN_RxHeaderTypeDef rxHeader;
    uint8_t dataBuffer[64] = {0};
    
    if(CAN_GetNumberOfUnreadFrames() > 0)
    {
        CAN_Read(&rxHeader, dataBuffer);
        switch((rx_action_e)dataBuffer[0])
        {
            case RX_ACTION_REPORT_STATUS:
                sdrive_status_t *status = NULL;

                

                // Update registered status to app
                for(uint8_t i = 0; i < NumberOfRegisteredSdrives; i++)
                {
                    if(RegisterStatusSdrives[i].canNodeID == (rxHeader.Identifier >> 8))
                    {
                        status = RegisterStatusPointers[i];
                        
                        break;
                    }
                }

                if(status != NULL)
                {
                    // TODO Get status
                    // status->enstatus = to kar je;
                    status->position = dataBuffer[12];
                }
                else
                {
                    // TODO error?
                }
                
                
                
                break;
            default:
                // Do nothing, only status frame supported for this application
                break;
        }
    }
}

void SDRIVE_RegisterStatus(sdrive_status_t *Status, sdrive_t sDrive)
{
    RegisterStatusPointers[NumberOfRegisteredSdrives] = Status;
    RegisterStatusSdrives[NumberOfRegisteredSdrives] = sDrive;
    
    NumberOfRegisteredSdrives++;
    if(NumberOfRegisteredSdrives > NUMBER_OF_STEPPER_DRIVES)
    {
        while(1); // ERROR: To many sdrives registered!
    }
}