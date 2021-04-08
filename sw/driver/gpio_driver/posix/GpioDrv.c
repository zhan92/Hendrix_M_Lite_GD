/*
-------------------------------------------------------------------------------
TYMPHANY LTD





                  Gpio Driver
                  -------------------------

                  SW Module Document




@file        GpioDrv.c
@brief       The GPIO interfaces and implementation for stm
             Note:The GPIO_Pin parameter can be GPIO_Pin_x where x can be: (0..15) for 
             GPIOA GPIOB or GPIOC,(0..2) for GPIOD and(0..3) for GPIOF. 
@author      
@date        
@copyright (c) Tymphany Ltd. All rights reserved.

Change History:
VERSION    : 1    DRAFT      2014-03-10     Bob.Xu 
DESCRIPTION: First Draft. Generated by newclass.py
SCO/ERROR  : 
-------------------------------------------------------------------------------
*/
#include "trace.h"
#include "./GpioDrv_priv.h"

/* Private functions / variables. Declare and drivers here */

/******************************************************************************
 *
 * Start-up / shut-down functions
 *
 ******************************************************************************/
void GpioDrv_Ctor(cGpioDrv *me, const tGPIODevice *pConfig )
{
    ASSERT(me);
    ASSERT(pConfig);

    me->gpioConfig = pConfig;
    me->isCreated = TRUE;
}

/**
* Disable GPIO object
* project
* @param[in]    me              GPIO object
*/
void GpioDrv_Xtor(cGpioDrv *me)
{
    ASSERT(me);
    me->isCreated = FALSE;
}

/******************************************************************************
 *
 * Public functions
 *
 ******************************************************************************/
void GpioDrv_SetDigitalIn(cGpioDrv *me, eGPIOId gpioId)
{
    ASSERT(me);
    (void) gpioId;
}

void GpioDrv_SetDigitalOut(cGpioDrv *me, eGPIOId gpioId)
{
    ASSERT(me);
    (void) gpioId;
}

void GpioDrv_SetBit(cGpioDrv *me, eGPIOId gpioId)
{
    ASSERT(me);
    (void) gpioId;
}

int8 GpioDrv_ReadBit(cGpioDrv *me, eGPIOId gpioId)
{
    ASSERT(me);
    (void) gpioId;
    int8 rtnValue = TP_ACCESS_ERROR;
    return rtnValue;
}

void GpioDrv_ClearBit(cGpioDrv *me, eGPIOId gpioId)
{
    ASSERT(me);
    (void) gpioId;
}

void GpioDrv_DisableIntOfWakeUpKeyPin()
{
    /* Need to be implemented */
}

void GpioDrv_EnableIntOfWakeUpKeyPin()
{
    /* Need to be implemented */
}

