// cppcheck-suppress-file misra-c2012-2.5; Because this is a general purpose driver, not all macros are used in every project

/**
  ******************************************************************************
  * @file           : cbuffer.h
  * @brief          : Header for dbus_sg.c file.
  *                   This file contains the common defines of the circular buffer module.
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CBUFFER_H_
#define CBUFFER_H_

/******************************************************************************* 
**                      Include Section                                       ** 
*******************************************************************************/ 
#include "stdbool.h"
#include "stdint.h" 
#include "main.h"

/******************************************************************************* 
**                      Version Information                                   ** 
*******************************************************************************/ 
#define CBUFF_H_MAJOR_VERSION             0
#define CBUFF_H_MINOR_VERSION             2
#define CBUFF_H_PATCH_VERSION             0
/******************************************************************************* 
**                      Global Symbols                                        ** 
*******************************************************************************/ 
typedef enum{
  CBUFF_ERROR_FULL,
}circular_buf_error_e;

/******************************************************************************* 
**                      Global Data Types                                     ** 
*******************************************************************************/ 
/* --- Module IDs --- */
typedef struct  {
	uint8_t * buffer;
	uint16_t head;
	uint16_t tail;
	uint16_t max; /*max size of the buffer*/
}circular_buf_t;

typedef struct  {
	bool success;
	uint16_t count;
}circular_buf_status_t;

typedef void (*circular_buf_ExternalFunction_t)(circular_buf_t* cbuf, circular_buf_error_e error);

/******************************************************************************* 
**                      Global Data                                           ** 
*******************************************************************************/ 

/******************************************************************************* 
**                      Global Function Prototypes                            ** 
*******************************************************************************/ 
void circular_buf_reset(circular_buf_t* cbuf);
bool circular_buf_put(circular_buf_t* cbuf, uint8_t data);
bool circular_buf_get(circular_buf_t* cbuf, uint8_t * data);
circular_buf_status_t circular_buf_put_multi(circular_buf_t* cbuf, uint8_t * data, uint16_t length); 
circular_buf_status_t circular_buf_get_multi(circular_buf_t* cbuf, uint8_t * data, uint16_t length); 
bool circular_buf_empty(circular_buf_t* cbuf);
bool circular_buf_full(circular_buf_t* cbuf);
uint16_t circular_buf_dataCount(circular_buf_t* cbuf);
void circular_buf_RegisterErrorCallback(circular_buf_ExternalFunction_t pExternalError);


/******************************************************************************* 
**                      Private Function Prototypes                           ** 
*******************************************************************************/ 

#endif /* CBUFFER_H_ */

/************************ (C) COPYRIGHT Iskra Mehanizmi *****END OF FILE****/
