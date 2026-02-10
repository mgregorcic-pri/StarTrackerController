/**
  ******************************************************************************
  * @file           : cbuffer.c
  * @brief          : Source for the circular buffer module.
  ******************************************************************************
  * @attention
  *
  *
  *****/

/******************************************************************************* 
**                      REVISION HISTORY                                      ** 
**  <VERSION>    <DATE>    <AUTHOR>    <REVISION LOG>                         **
**  V1.0.0       05032020  LLeban      Initial version                        **
**                                                                            **
*******************************************************************************/

/******************************************************************************* 
**                      MISRA Rule Violation                                  ** 
*******************************************************************************/

/******************************************************************************* 
**                      Include Section                                       ** 
*******************************************************************************/
#include "cbuffer.h"

/******************************************************************************* 
**                      Version Information                                   ** 
*******************************************************************************/
#define CBUFF_C_MAJOR_VERSION 0
#define CBUFF_C_MINOR_VERSION 2
#define CBUFF_C_PATCH_VERSION 0
/******************************************************************************* 
**                      Version Check                                         ** 
*******************************************************************************/
#if ((CBUFF_C_MAJOR_VERSION != CBUFF_H_MAJOR_VERSION) || \
     (CBUFF_C_MINOR_VERSION != CBUFF_H_MINOR_VERSION) || \
     (CBUFF_C_PATCH_VERSION != CBUFF_H_PATCH_VERSION))
#error "Software Version Numbers of cbuffer.c and cbuffer.h are different"
#endif
/******************************************************************************* 
**                      Macros                                                ** 
*******************************************************************************/

/******************************************************************************* 
**                      Type Definitions                                      ** 
*******************************************************************************/


/******************************************************************************* 
**                      Internal Variables                                    ** 
*******************************************************************************/
circular_buf_ExternalFunction_t ExternalErrorCalbackFunction = NULL;
/******************************************************************************* 
**                      Private Function Prototypes		                        ** 
*******************************************************************************/

/******************************************************************************* 
**                      Function Implementation                               ** 
*******************************************************************************/

void circular_buf_reset(circular_buf_t* cbuf)
{
    cbuf->head = 0u;
    cbuf->tail = 0u;
}

bool circular_buf_full(circular_buf_t* cbuf)
{
  bool retval = false;
  /* We determine "full" case by head being one position behind the tail*/
  if(((cbuf->head + 1u) % cbuf->max) == cbuf->tail)
  {
    retval = true;
  }

  return retval;
}

bool circular_buf_empty(circular_buf_t* cbuf)
{
  bool retval = false;
  /* We define empty as head == tail*/
  if(cbuf->head == cbuf->tail)
  {
    retval = true;
  }
	
    return retval;
}

bool circular_buf_get(circular_buf_t * cbuf, uint8_t * data)
{
    bool retval = false;

    if(!circular_buf_empty(cbuf))
    {
        *data = cbuf->buffer[cbuf->tail];
        cbuf->tail = (cbuf->tail + 1u) % cbuf->max;

        retval = true;
    }

    return retval;
}

bool circular_buf_put(circular_buf_t * cbuf, uint8_t data)
{
    bool retval = false;

    if(!circular_buf_full(cbuf))
    {
        cbuf->buffer[cbuf->head] = data;
        cbuf->head = (cbuf->head + 1u) % cbuf->max;

        retval = true;
    }
    else
    {
      if(ExternalErrorCalbackFunction != NULL)
      {
        ExternalErrorCalbackFunction(cbuf, CBUFF_ERROR_FULL);
      }
    }

    return retval;
}

circular_buf_status_t circular_buf_put_multi(circular_buf_t* cbuf, uint8_t * data, uint16_t length)
{
  circular_buf_status_t status = {.success = true, .count = 0u};

  for (uint8_t i = 0u; i < length; i++)
  {
    if(circular_buf_put(cbuf, data[i]))
    {
      status.count++;
    }
    else
    {
      status.success = false;
      break;
    }
  }
  
  return status;
}


circular_buf_status_t circular_buf_get_multi(circular_buf_t* cbuf, uint8_t * data, uint16_t length)
{
    circular_buf_status_t status = {.success = true, .count=0};

    for (uint8_t i = 0u; i < length; i++)
    {
      if(circular_buf_get(cbuf, &data[i]))
      {
        status.count++;
      }
      else
      {
        status.success = false;
        break;
      }
    }
  
  return status;
}

uint16_t circular_buf_dataCount(circular_buf_t* cbuf)
{
  if(cbuf->tail <= cbuf->head)
  {
    return (cbuf->head - cbuf->tail);
  }
  else
  {
    return(cbuf->max - (cbuf->tail - cbuf->head));
  }
}

void circular_buf_RegisterErrorCallback(circular_buf_ExternalFunction_t pExternalError)
{
  ExternalErrorCalbackFunction = pExternalError;
}
/******************************************************************************* 
**                      Private Function Prototypes                           ** 
*******************************************************************************/



