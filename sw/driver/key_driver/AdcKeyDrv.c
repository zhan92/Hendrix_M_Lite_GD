/*
-------------------------------------------------------------------------------
TYMPHANY LTD





                  Key Driver(adc type)
                  -------------------------

                  SW Module Document




@file        AdcKeyDrv.c
@brief       The key driver(adc type) interfaces and implementation 
@author      Bob.Xu 
@date        2014-03-19
@copyright (c) Tymphany Ltd. All rights reserved.

Change History:
VERSION    : 1    DRAFT      2014-03-19     Bob.Xu 
DESCRIPTION: First Draft. Generated by newclass.py
SCO/ERROR  : 
-------------------------------------------------------------------------------
*/
#include "AdcKeyDrv.h"
#include "AdcDrv.h"
#include "trace.h"

/* local functions */
static void AdcKeyDrv_UpdateKeyStatus(cKeyDrv *me);
static int32 AdcKeyDrv_GetKeyRawValue(cKeyDrv *me);
static void AdcKeyDrv_StartScan(cKeyDrv *me);

/**
* Key Driver(adc type) object constructor
* @param[in]    me              the Key Driver(adc type) object
* @param[in]    pConfig         configuration of the Key instance
* @param[in]    keyID           the keyID
*/
void AdcKeyDrv_Ctor(cAdcKeyDrv *me, const tAdcKeyboardDevice *pAdcKeyboardConfig, eKeyID keyID)
{
    {
        /* check parameters */
        ASSERT(me && pAdcKeyboardConfig && (keyID < NORMAL_KEY_ID_MAX));
    }
        cADCDrv *pAdcDrvObj;
        pAdcDrvObj = (cADCDrv*)(pAdcKeyboardConfig->attachedDeviceObj);
        if( FALSE == (pAdcDrvObj->isCreated))
        {
            /**
             * several keys share the same adc obj, we check if adc obj
             * has been created, if yes, simply ignore this statement
             */
             ADCDrv_Ctor(pAdcDrvObj, pAdcKeyboardConfig->attachedDeviceObjConfig);
        }
        me->pAdcKeyboardConfig = pAdcKeyboardConfig;
        me->super_.KeyGetRawDataCb = AdcKeyDrv_GetKeyRawValue;
        me->super_.KeyStartScanCb = AdcKeyDrv_StartScan;
        me->super_.KeyUpdateStatusCb = AdcKeyDrv_UpdateKeyStatus;
        me->super_.keySimulationState = KEY_INVALIDE_STATE;
        me->super_.keyState = KEY_UP;
        me->super_.keyID = keyID;
        me->super_.isCreated = TRUE;
}

/**
* Key Driver object(adc type) destructor
* @param[in]    me              the Key Driver object
*/
void AdcKeyDrv_Xtor(cAdcKeyDrv *me)
{
    ASSERT(me);
    me->super_.KeyGetRawDataCb = NULL;
    me->super_.KeyStartScanCb = NULL;
    me->super_.KeyUpdateStatusCb = NULL;
    me->super_.keySimulationState = KEY_INVALIDE_STATE;
    me->super_.keyState = KEY_INVALIDE_STATE;
    me->super_.keyID = INVALID_KEY;
    me->super_.isCreated = FALSE;
    cADCDrv *pAdcDrvObj;
    pAdcDrvObj = (cADCDrv*)(me->pAdcKeyboardConfig->attachedDeviceObj);
    if(TRUE == pAdcDrvObj->isCreated)
    {
        ADCDrv_Xtor(pAdcDrvObj);
    }
    me->pAdcKeyboardConfig = NULL;
}

/**
* Read the Key(adc type) value
* @param[in]    me              the Key object
*/
static void AdcKeyDrv_StartScan(cKeyDrv *me)
{
    cAdcKeyDrv *adckeyObj;
    adckeyObj = (cAdcKeyDrv*)me;
    ADCDrv_StartScanning((cADCDrv*)(adckeyObj->pAdcKeyboardConfig->attachedDeviceObj));
}


/**
* local function to interpret the ADC value
* @param[in]    me                         the Key object
*/
static void AdcKeyDrv_UpdateKeyStatus(cKeyDrv *me)
{
    int16 adcValue;
    eAdcPin adcPin;
    uint8 i;
    /* use those temperary variable because the if condition is very long*/
    uint16 adcMin;
    uint16 adcMax;
    cADCDrv *pAdcObj;
    cAdcKeyDrv *pAdcKeyobj;
    pAdcKeyobj = (cAdcKeyDrv*)me;;
    for(i = 0; i < (pAdcKeyobj->pAdcKeyboardConfig->adcKeyboard.keyNum); i++)
    {
        if((pAdcKeyobj->super_.keyID) == (pAdcKeyobj->pAdcKeyboardConfig->pAdcKeySet[i].keyId))
        {
            adcPin = pAdcKeyobj->pAdcKeyboardConfig->pAdcKeySet[i].adcPin;
            pAdcObj = (cADCDrv*)pAdcKeyobj->pAdcKeyboardConfig->attachedDeviceObj;
            adcValue = ADCDrv_GetData(pAdcObj,adcPin);
            adcMin = pAdcKeyobj->pAdcKeyboardConfig->pAdcKeySet[i].adcMinimumValue;
            adcMax = pAdcKeyobj->pAdcKeyboardConfig->pAdcKeySet[i].adcMaxValue;
            /* tranlate adc value */
            if(ADC_DATA_IS_NOT_READY == adcValue)
            {
                /* keep the same sate since conversioin is not done */
                return ;
            }
            else if(adcValue >= adcMin && adcValue <= adcMax)
            {
                me->keyState = KEY_DOWN;
            }
            else
            {
                me->keyState = KEY_UP;
            }
            return;
        }
    }
}

/**
* local function to read the adc raw data
* @param[in]    me                         the Key object
*/
static int32 AdcKeyDrv_GetKeyRawValue(cKeyDrv *me)
{
    int16 adcValue;
    eAdcPin adcPin;
    uint8 i;
    cADCDrv *pAdcObj;
    cAdcKeyDrv * pAdcKeyObj;
    pAdcKeyObj = (cAdcKeyDrv*)me;
    pAdcObj = (cADCDrv*)( pAdcKeyObj->pAdcKeyboardConfig->attachedDeviceObj);
    for(i = 0; i < (pAdcKeyObj->pAdcKeyboardConfig->adcKeyboard.keyNum); i++)
    {
        if((pAdcKeyObj->super_.keyID) == ( pAdcKeyObj->pAdcKeyboardConfig->pAdcKeySet[i].keyId))
        {
            adcPin =  pAdcKeyObj->pAdcKeyboardConfig->pAdcKeySet[i].adcPin;
            adcValue = ADCDrv_GetData(pAdcObj,adcPin);
            return adcValue;
        }
    }
    return TP_ACCESS_ERROR;
}