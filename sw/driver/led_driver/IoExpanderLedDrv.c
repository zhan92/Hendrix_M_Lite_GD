/*
-------------------------------------------------------------------------------
TYMPHANY LTD





                  IoExpanderLed Driver
                  -------------------------

                  SW Module Document




@file        IoExpanderLedDrv.c
@brief       Led Drvier using the IO Expander
@author      Wesley Lee
@date        2014-07-11
@copyright (c) Tymphany Ltd. All rights reserved.

Change History:
VERSION    : 1    DRAFT      2014-07-11     Wesley Lee
DESCRIPTION: First Draft. Generated by newclass.py
SCO/ERROR  :

Change History:
VERSION    : 2               2017-09-06     Alex Li
DESCRIPTION: Add support for AW9120
SCO/ERROR  :
-------------------------------------------------------------------------------
*/

#include "./IoExpanderLedDrv_priv.h"
#include "IoExpanderLedDrv.config"
#include "trace.h"
#include "IoExpanderDrv.h"

/* Private functions / variables. Declare and drivers here */
static const tLedFunc ioeLedDrvFunc =
{
    .LedOn          = IoExpanderLedDrv_On,
    .LedOff         = IoExpanderLedDrv_Off,
    .LedSetColor    = IoExpanderLedDrv_SetColor,
    .LedXtor        = IoExpanderLedDrv_Xtor,
};

/*****************************************************************************************************************
 *
 * Start-up / shut-down functions
 *
 *****************************************************************************************************************/
/* @brief       IO Expander Led driver constructor
 * @param[in]   me          pointer to IO-Expander-Led driver object
 * @param[in]   pConfig     pointer to Led config
 * @param[in]   ioe         pointer to the attached IO-expander
 * @param[in]   index       The index of where the current object locate in the pConfig
 */
void IoExpanderLedDrv_Ctor(cIoExpanderLedDrv *me, const tDevice* pConfig, cIoExpanderDrv *ioeDrv, uint8 index)
{
    tIoeLedDevice *pIoeLedConfig = (tIoeLedDevice *)pConfig;

    me->super_.pLedFunc = &ioeLedDrvFunc;
    me->pIoeLedConfig   = &pIoeLedConfig->pIoExpanderLedMap[index];
    me->pIoExpander     = ioeDrv;
    me->pIoExpander->deviceSubType = pConfig->deviceSubType;

    if(AW9110B == pConfig->deviceSubType)
    {
        IoExpanderDrv_Ctor_aw9110b(me->pIoExpander, pIoeLedConfig);
    }

    else if(AW9523 == pConfig->deviceSubType)
    {
        IoExpanderDrv_Ctor_aw9523(me->pIoExpander, pIoeLedConfig);
    }

    else if(AW9120 == pConfig->deviceSubType)
    {
        IoExpanderDrv_Ctor_aw9120(me->pIoExpander, pIoeLedConfig);
    }

    else
    {
        ASSERT(0);
    }
}

/* @brief       IO Expander Led driver destructor
 * @param[in]   me          pointer to IO-Expander-Led driver object
 */
void IoExpanderLedDrv_Xtor(cLedDrv *me)
{
    cIoExpanderLedDrv *pDrv = (cIoExpanderLedDrv *)me;
    uint16 attached_device_index = 0;
    tIoeLedDevice * pLedDevice;

    if(AW9110B == pDrv->pIoExpander->deviceSubType)
    {
        IoExpanderDrv_Xtor_aw9110b(pDrv->pIoExpander);
    }

    else if(AW9523 == pDrv->pIoExpander->deviceSubType)
    {
        IoExpanderDrv_Xtor_aw9523(pDrv->pIoExpander);
    }

    else if(AW9120 == pDrv->pIoExpander->deviceSubType)
    {
        for(int ii = 0; ii < IO_EXPANDER_NUM; ii++)
        {
            pLedDevice = (tIoeLedDevice*) getDevicebyId(LED_DEV_ID, &attached_device_index);

            if(pLedDevice->deviceInfo.deviceSubType == AW9120)
                IoExpanderDrv_Xtor_aw9120(pDrv->pIoExpander, pLedDevice);

            attached_device_index++;
        }
    }
}

/*****************************************************************************************************************
 *
 * Public functions
 *
 *****************************************************************************************************************/
/* @brief       Turn on the IO-Expander Led with the driver pointed to
 * @param[in]   me          pointer to the super class of IO-Expander-Led driver object
 */
//TODO: To make an abstract layer for different expander types
void IoExpanderLedDrv_On(cLedDrv *me)
{
    cIoExpanderLedDrv *pDrv = (cIoExpanderLedDrv *)me;
    uint8 port  = pDrv->pIoeLedConfig->port;
    uint8 pin   = pDrv->pIoeLedConfig->pin;

    if(AW9110B == pDrv->pIoExpander->deviceSubType)
    {
        IoExpanderDrv_SetGpio_aw9110b(pDrv->pIoExpander, port, pin);
    }

    else if(AW9523 == pDrv->pIoExpander->deviceSubType)
    {
        IoExpanderDrv_SetGpio_aw9523(pDrv->pIoExpander, port, pin);
    }

    else if(AW9120 == pDrv->pIoExpander->deviceSubType)
    {
        IoExpanderDrv_TurnLedOn_aw9120(pDrv->pIoExpander, pin);
    }
}

/* @brief       Turn off the IO-Expander Led with the driver pointed to
 * @param[in]   me          pointer to the super class of IO-Expander-Led driver object
 */
void IoExpanderLedDrv_Off(cLedDrv *me)
{
    cIoExpanderLedDrv *pDrv = (cIoExpanderLedDrv *)me;
    uint8 port  = pDrv->pIoeLedConfig->port;
    uint8 pin   = pDrv->pIoeLedConfig->pin;

    if(AW9110B == pDrv->pIoExpander->deviceSubType)
    {
        IoExpanderDrv_ClearGpio_aw9110b(pDrv->pIoExpander, port, pin);
    }

    else if(AW9523 == pDrv->pIoExpander->deviceSubType)
    {
        IoExpanderDrv_ClearGpio_aw9523(pDrv->pIoExpander, port, pin);
    }

    else if(AW9120 == pDrv->pIoExpander->deviceSubType)
    {
        IoExpanderDrv_TurnLedOff_aw9120(pDrv->pIoExpander, pin);
    }
}

/* @brief       Set the brightness to the IO-Expander Led with the driver pointed to
 * @param[in]   me          pointer to the super class of IO-Expander-Led driver object
 * @param[in]   color       the color to be set, only the corresponding component will be set
 */
void IoExpanderLedDrv_SetColor(cLedDrv *me, Color color)
{
    cIoExpanderLedDrv *pDrv = (cIoExpanderLedDrv *)me;
    uint8 port  = pDrv->pIoeLedConfig->port;
    uint8 pin   = pDrv->pIoeLedConfig->pin;
    uint8 brightness = 0;

#ifdef LED_HAS_RGB

    switch(pDrv->pIoeLedConfig->colorComp)
    {
    case COLOR_COMPONENT_RED:
        brightness = GET_RED(color);
        break;

    case COLOR_COMPONENT_GREEN:
        brightness = GET_GREEN(color);
        break;

    case COLOR_COMPONENT_BLUE:
        brightness = GET_BLUE(color);
        break;

    case COLOR_COMPONENT_BRIGHTNESS:
        brightness = GET_BRIGHTNESS(color);
        break;

    default:
        ASSERT(0);
        break;
    }

#else
    uint32 pure_color_pattern = GET_PURE_COLOR(color);
    uint32 pure_color_pin = GET_PURE_COLOR(pDrv->pIoeLedConfig->pure_color);

    if(pure_color_pattern == pure_color_pin)
    {
        brightness = GET_BRIGHTNESS(color);
    }

#endif

    ASSERT(IO_EXPANDER_LED_BRIGHTNESS_RATE <= 100);
    brightness = brightness * IO_EXPANDER_LED_BRIGHTNESS_RATE / 100;

    if(AW9523 == pDrv->pIoExpander->deviceSubType)
    {
        IoExpanderDrv_SetBrightness_aw9523(pDrv->pIoExpander, port, pin, brightness);
    }

    else if(AW9110B == pDrv->pIoExpander->deviceSubType)
    {
        IoExpanderDrv_SetBrightness_aw9110b(pDrv->pIoExpander, port, pin, brightness);
    }

    else if(AW9120 == pDrv->pIoExpander->deviceSubType)
    {
        IoExpanderDrv_SetBrightness_aw9120(pDrv->pIoExpander, pin, brightness);
    }

    else
    {
        ASSERT(0);
    }
}

/*****************************************************************************************************************
 *
 * private functions
 *
 *****************************************************************************************************************/


