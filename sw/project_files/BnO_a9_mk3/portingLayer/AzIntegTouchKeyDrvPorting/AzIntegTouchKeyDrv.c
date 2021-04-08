/******************************************************************************

Copyright (c) 2015, Tymphany HK Ltd. All rights reserved.

Confidential information of Tymphany HK Ltd.

The Software is provided "AS IS" and "WITH ALL FAULTS," without warranty of any
kind, including without limitation the warranties of merchantability, fitness
for a particular purpose and non-infringement. Tymphany HK LTD. makes no
warranty that the Software is free of defects or is suitable for any particular
purpose.

******************************************************************************/
/*
-------------------------------------------------------------------------------
TYMPHANY LTD





                  Key Driver(Azeteq Intergrate Touch Type)
                  -------------------------

                  SW Module Document




@file        AzIntegTouchKeyDrv.c
@brief       The key driver(Azoteq Intergrate Touch type) interfaces and implementation 
@author      Gavin Lee 
@date        2015-12-27
@copyright (c) Tymphany Ltd. All rights reserved.

Change History:
VERSION    : 1    DRAFT      2015-12-27     Gavin Lee
DESCRIPTION: First Draft. Generated by newclass.py
SCO/ERROR  : 
-------------------------------------------------------------------------------
*/
#include "AzIntegTouchKeyDrv_priv.h"
#include "trace.h"
#include "product.config"
#include "State_Machine_priv.h"
#include "IQS333.H"


/*******************************************
 * Global Variable                         *
 *******************************************/
cAzIntegTouchKeyDrv *pTouchKeyDrv= NULL;


/*******************************************
 * Private Function Prototype              *
 *******************************************/
static void AzIntegTouchKeyDrv_StartScan(cKeyDrv *me);
static void AzIntegTouchKeyDrv_UpdateKeyStatus(cKeyDrv *me);
static int32 AzIntegTouchKeyDrv_GetKeyRawValue(cKeyDrv *me);


/*******************************************
 * Function Implemenation                  *
 *******************************************/
/**
* Key Driver(touch type) object constructor
* @param[in]    me              the Key Driver(touch type) object
* @param[in]    pKeyConfig      configuration of the Key instance
*/
void AzIntegTouchKeyDrv_Ctor(cAzIntegTouchKeyDrv *me, const tAzIntegTouchKeyboardDevice *pKeyConfig)
{
    /* check parameters */
    ASSERT(me);
    pTouchKeyDrv= me;
    
    //Not used for Touch key driver
    me->super_.KeyGetRawDataCb = &AzIntegTouchKeyDrv_GetKeyRawValue;
    me->super_.keyState = KEY_UP;
    me->super_.keyID = INVALID_KEY; 
    me->pKeyboardConfig = (const tAzIntegTouchKeyboardDevice*)pKeyConfig;

    //Other parameter
    //me->super_.KeyCallback = AzIntegTouchKeyDrv_InterruptOperation;
    me->super_.KeyStartScanCb = &AzIntegTouchKeyDrv_StartScan;
    me->super_.KeyUpdateStatusCb = &AzIntegTouchKeyDrv_UpdateKeyStatus;
    me->super_.keySimulationState = KEY_INVALIDE_STATE;

    //Initialize IQS333 I2C pins
    me->i2c333Drv.pConfig = (tI2CDevice *)pKeyConfig->i2cConfig333;
    I2CDrv_Ctor(&me->i2c333Drv, me->i2c333Drv.pConfig);
    
    //Initialize IQS263 I2C pins
    me->i2c263Drv.pConfig = (tI2CDevice *)pKeyConfig->i2cConfig263;
    I2CDrv_Ctor(&me->i2c263Drv, me->i2c263Drv.pConfig);
    
    //Initialize IQS572 Bootloader I2C pins
    //me->i2c572BlDrv.pConfig = (tI2CDevice *)pKeyConfig->i2cConfig572Bl;
    //I2CDrv_Ctor(&me->i2c572BlDrv, me->i2c572BlDrv.pConfig);

    //Initialize GPIO pins
    me->gpioDrv.gpioConfig = pKeyConfig->gpioConfig;
    GpioDrv_Ctor(&me->gpioDrv, me->gpioDrv.gpioConfig);
    ASSERT(me->gpioDrv.isCreated);


    /* Gave Power to the touch (572 + 360), and IO-Expendor. 
     * Both two drivers initialize this pin.
     */
    //GpioDrv_SetBit(&me->gpioDrv, GPIO_OUT_TCH_POWER);
    
    //Disable IQS360
    //GpioDrv_ClearBit(&me->gpioDrv, GPIO_OUT_TCH_360_RST);


    //Initialize for Azoteq Touch   
    state_machine_init(me);  /* Setup State Machine and Create IQS Devices */
    
    //Initialize EXTI Interrupt
    GpioDrv_EnableExtInterrupt(&me->gpioDrv, GPIO_IN_TCH_333_RDY, ExtiIntTri_Rising_Falling);
    GpioDrv_EnableExtInterrupt(&me->gpioDrv, GPIO_IN_TCH_263_RDY, ExtiIntTri_Rising_Falling);    
 
    me->super_.isCreated = TRUE;
}

/**
* Key Driver object(adc type) destructor
* @param[in]    me              the Key Driver object
*/
void AzIntegTouchKeyDrv_Xtor(cAzIntegTouchKeyDrv *me)
{
    ASSERT(me);
    /* 
     * any destruction will distroy the object, so the firt action is to disable the object
     * in case that the object is still used, for example by interrupt.
     */
    me->super_.isCreated = FALSE;
    //state_machine_power_off(me);    

    //Initialize EXTI Interrupt
    GpioDrv_DisableExtInterrupt(&me->gpioDrv, GPIO_IN_TCH_333_RDY);
    GpioDrv_DisableExtInterrupt(&me->gpioDrv, GPIO_IN_TCH_263_RDY);
    
    //De-initialize I2C    
    I2CDrv_Xtor(&me->i2c333Drv);
    I2CDrv_Xtor(&me->i2c263Drv);

    //De-initialize touch
    me->super_.KeyGetRawDataCb = NULL;
    me->super_.KeyStartScanCb = NULL;
    me->super_.KeyUpdateStatusCb = NULL;
    me->super_.keySimulationState = KEY_INVALIDE_STATE;
    me->super_.keyState = KEY_INVALIDE_STATE;
    me->super_.keyID = INVALID_KEY;
}


/**
* Read the Key(adc type) value
* @param[in]    me              the Key object
*/
static void AzIntegTouchKeyDrv_StartScan(cKeyDrv *me)
{
    return;
}


/**
* local function to interpret the ADC value
* @param[in]    me                         the Key object
*/
static void AzIntegTouchKeyDrv_UpdateKeyStatus(cKeyDrv *me)
{
    uint8 ret;
    cAzIntegTouchKeyDrv *pAzIntegTouchKeyobj;
    pAzIntegTouchKeyobj = (cAzIntegTouchKeyDrv*)me;
    
    if(!pAzIntegTouchKeyobj->super_.isCreated)
      return;

    state_machine_process(pAzIntegTouchKeyobj);
    return;
}


/**
* local function to read the adc raw data
* @param[in]    me                         the Key object
*/
static int32 AzIntegTouchKeyDrv_GetKeyRawValue(cKeyDrv *me)
{
    return 0;
}
