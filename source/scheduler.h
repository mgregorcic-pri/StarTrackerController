/**
 ********************************************************************************
 * @file    scheduler.h
 * @author  
 * @date    
 * @brief   
 ********************************************************************************
 */

#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#ifdef __cplusplus
extern "C" {
#endif

/************************************
 * INCLUDES
 ************************************/
#include "stdbool.h"
#include "stdint.h"

/************************************
 * MACROS AND DEFINES
 ************************************/

/************************************
 * TYPEDEFS
 ************************************/
typedef void (*task_func)(void);

typedef struct
{
    void (*function)(void);
    uint32_t cycleTime;
    uint32_t lastTime;
}task_t;

typedef struct 
{
    uint32_t *sysTime;  //ms
    task_t *tasks;
    uint32_t numOfTasks;
}scheduler_data_t;

/************************************
 * EXPORTED VARIABLES
 ************************************/

/************************************
 * GLOBAL FUNCTION PROTOTYPES
 ************************************/
void SCHEDULER_Init(scheduler_data_t *scheduler);
void SCHEDULER_Handler(scheduler_data_t *scheduler);

#ifdef __cplusplus
}
#endif

#endif 