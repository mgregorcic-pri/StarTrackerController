/**
 ********************************************************************************
 * @file    scheduler.c
 * @author  
 * @date    
 * @brief   
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "scheduler.h"

/************************************
 * EXTERN VARIABLES
 ************************************/

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/

/************************************
 * PRIVATE TYPEDEFS
 ************************************/

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

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
void SCHEDULER_Init(scheduler_data_t *scheduler)
{
    for(uint32_t taskNum = 0; taskNum < scheduler->numOfTasks; taskNum++)
    {
        scheduler->tasks[taskNum].lastTime = *scheduler->sysTime;
    }
}
void SCHEDULER_Handler(scheduler_data_t *scheduler)
{
    for(uint32_t taskNum = 0; taskNum < scheduler->numOfTasks; taskNum++)
    {
        if((*scheduler->sysTime - scheduler->tasks[taskNum].lastTime) > scheduler->tasks[taskNum].cycleTime)
        {
            scheduler->tasks[taskNum].function();
            scheduler->tasks[taskNum].lastTime = *scheduler->sysTime;
        }
    }
}