/*
-------------------------------------------------------------------------------
TYMPHANY LTD





                  Gpio Driver
                  -------------------------

                  SW Module Document




@file        GpioDrv.c
@brief       The GPIO interfaces and implementation
@author      Bob.Xu 
@date        2014-03-10
@copyright (c) Tymphany Ltd. All rights reserved.

Change History:
VERSION    : 1    DRAFT      2014-03-10     Bob.Xu 
DESCRIPTION: First Draft. Generated by newclass.py
SCO/ERROR  : 
-------------------------------------------------------------------------------
*/

#include "trace.h"
#include "./GpioDrv_priv.h"
static const uint32 pic32IoPort[]=
{
    PORT_INVALID, //PORTA is invalid
    IOPORT_B,
    IOPORT_C,
    IOPORT_D,
    IOPORT_E,
    IOPORT_F,
    IOPORT_G
};
/* Private functions / variables. Declare and drivers here */
static void GpioDrv_SetPullUp(eIoPort gpioPort,eIoBit gpioBit);
static void GpioDrv_ClearPullUp(eIoPort gpioPort,eIoBit gpioBit);
static void GpioDrv_SendWakeUpKeyIntToKeySrv();
/******************************************************************************
 *
 * Start-up / shut-down functions
 *
 ******************************************************************************/
/**
* Create GPIO object
* project
* @param[in]    me              GPIO object
* @param[in]    pConfig         Config for GPIO object
*/
void GpioDrv_Ctor(cGpioDrv *me, const tGPIODevice *pConfig )
{
    uint8 i;
    ASSERT(me && pConfig);
    me->gpioConfig = pConfig;
    for(i = 0; i < pConfig->usedGPIOPinNum; i++)
    {
        switch(pConfig->pGPIOPinSet[i].gpioDirection)
        {
            case GPIO_DIGITAL_INPUT:
            {
                 PORTSetPinsDigitalIn(pic32IoPort[pConfig->pGPIOPinSet[i].gpioPort],\
                         (1<<pConfig->pGPIOPinSet[i].gpioBit));
                 if(GPIO_ACTIVE_LOW == pConfig->pGPIOPinSet[i].gpioInitAttr)
                 {
                     GpioDrv_SetPullUp(pConfig->pGPIOPinSet[i].gpioPort,\
                             pConfig->pGPIOPinSet[i].gpioBit);
                 }
                 break;
            }
            case GPIO_DIGITAL_OUTPUT:
            {
                 PORTSetPinsDigitalOut(pic32IoPort[pConfig->pGPIOPinSet[i].gpioPort],\
                         1<<pConfig->pGPIOPinSet[i].gpioBit);
                 //if(ODC_OPEN == pConfig->tGPIOPins[i].odc)
                 //{
                    /* mPORTXOpenDrainOpen */
                 //}
                 break;
            }
            case GPIO_ANALOG_INPUT:
            {
                PORTSetPinsAnalogIn(pic32IoPort[pConfig->pGPIOPinSet[i].gpioPort],\
                         1<<pConfig->pGPIOPinSet[i].gpioBit);
                break;
            }
            case GPIO_ANALOG_OUTPUT:
            {
                PORTSetPinsAnalogOut(pic32IoPort[pConfig->pGPIOPinSet[i].gpioPort],\
                         1<<pConfig->pGPIOPinSet[i].gpioBit);
                break;
            }
            case GPIO_BIDIRECTION:
            {
                /**
                 * the defualt direction is the the CPU defualt direction when
                 * MCU is power on, user need to call the public functions to
                 * set the direction if it is required.
                 */
                if(GPIO_ACTIVE_LOW == pConfig->pGPIOPinSet[i].gpioInitAttr)
                {
                    GpioDrv_SetPullUp(pConfig->pGPIOPinSet[i].gpioPort,\
                             pConfig->pGPIOPinSet[i].gpioBit);
                }
                break;
            }
            default:break;
        }
    }
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
    uint8 i;
    me->isCreated = FALSE;
    for(i = 0; i < me->gpioConfig->usedGPIOPinNum; i++)
    {
       PORTSetPinsDigitalIn(pic32IoPort[me->gpioConfig->pGPIOPinSet[i].gpioPort],\
                    1<<me->gpioConfig->pGPIOPinSet[i].gpioBit);
       GpioDrv_ClearPullUp(me->gpioConfig->pGPIOPinSet[i].gpioPort,\
                    me->gpioConfig->pGPIOPinSet[i].gpioBit);
    }
}

/******************************************************************************
 *
 * Public functions
 *
 ******************************************************************************/
/**
* This function is only available for the pins which has bidirection feature.
* If you passed a GPIO ID which doesn't have bidirection feature or the
* GPIO Id doesn't belong to the GPIO object,  this funciton will do nothing
* and return a fail
* project
* @param[in]    me              GPIO object
* @param[in]    gpioId          GPIO ID
*/
void GpioDrv_SetDigitalIn(cGpioDrv *me, eGPIOId gpioId)
{
    uint8 i;
    for(i = 0; i < me->gpioConfig->usedGPIOPinNum; i++)
    {
        /* only the gpio pin which has GPIO_BIDIRECTION feature has the access
         * to this function, otherwise, return fail */
        if (gpioId == me->gpioConfig->pGPIOPinSet[i].gpioId && \
            GPIO_BIDIRECTION == me->gpioConfig->pGPIOPinSet[i].gpioDirection)
        {
            PORTSetPinsDigitalIn(pic32IoPort[me->gpioConfig->pGPIOPinSet[i].gpioPort],\
                    1<<me->gpioConfig->pGPIOPinSet[i].gpioBit);
            return;
        }
    }
    ASSERT(0);/* gpioId doesn't belong to me,somewhere is seriously wrong */
}

/**
* This function is only available for the pins which has bidirection feature.
* If you passed a GPIO ID which doesn't have bidirection feature or the
* GPIO Id doesn't belong to the GPIO object,  this funciton will do nothing
* and return a fail
* project
* @param[in]    me              GPIO object
* @param[in]    gpioId          GPIO ID
*/
void GpioDrv_SetDigitalOut(cGpioDrv *me, eGPIOId gpioId)
{
    uint8 i;
    for(i = 0; i < me->gpioConfig->usedGPIOPinNum; i++)
    {
        if (gpioId == me->gpioConfig->pGPIOPinSet[i].gpioId &&\
                 GPIO_BIDIRECTION == me->gpioConfig->pGPIOPinSet[i].gpioDirection)
        {
            PORTSetPinsDigitalOut(pic32IoPort[me->gpioConfig->pGPIOPinSet[i].gpioPort],\
                    1<<me->gpioConfig->pGPIOPinSet[i].gpioBit);
            return ;
        }
    }
    ASSERT(0); /* gpioId doesn't belong to me,somewhere is seriously wrong */
}

/**
* This function is only available for the pins which has bidirection feature.
* If you passed a GPIO ID which doesn't have bidirection feature or the
* GPIO Id doesn't belong to the GPIO object,  this funciton will do nothing
* and return a fail
* project
* @param[in]    me              GPIO object
* @param[in]    gpioId          GPIO ID
*/
void GpioDrv_SetAnalogIn(cGpioDrv *me, eGPIOId gpioId)
{
    uint8 i;
    for(i = 0; i < me->gpioConfig->usedGPIOPinNum; i++)
    {
        if (gpioId == me->gpioConfig->pGPIOPinSet[i].gpioId &&\
                 GPIO_BIDIRECTION == me->gpioConfig->pGPIOPinSet[i].gpioDirection)
        {
            PORTSetPinsAnalogIn(pic32IoPort[me->gpioConfig->pGPIOPinSet[i].gpioPort],\
                    1<<me->gpioConfig->pGPIOPinSet[i].gpioBit);
            return;
        }
    }
    ASSERT(0); /* gpioId doesn't belong to me,somewhere is seriously wrong */
}
/**
* This function is only available for the pins which has bidirection feature.
* If you passed a GPIO ID which doesn't have bidirection feature or the
* GPIO Id doesn't belong to the GPIO object,  this funciton will do nothing
* and return a fail
* project
* @param[in]    me              GPIO object
* @param[in]    gpioId          GPIO ID
*/
void GpioDrv_SetAnalogOut(cGpioDrv *me, eGPIOId gpioId)
{
    uint8 i;
    for(i = 0; i < me->gpioConfig->usedGPIOPinNum; i++)
    {
        if (gpioId == me->gpioConfig->pGPIOPinSet[i].gpioId &&
                 GPIO_BIDIRECTION == me->gpioConfig->pGPIOPinSet[i].gpioDirection)
        {
            PORTSetPinsAnalogOut(pic32IoPort[me->gpioConfig->pGPIOPinSet[i].gpioPort],
                    1<<me->gpioConfig->pGPIOPinSet[i].gpioBit);
            return;
        }
    }
    ASSERT(0); /* gpioId doesn't belong to me,somewhere is seriously wrong */
}

/**
* If you passed a GPIO ID doesn't belong to the GPIO object,
* this funciton will do nothing and return a fail
* project
* @param[in]    me              GPIO object
* @param[in]    gpioId          GPIO ID
*/
void GpioDrv_SetBit(cGpioDrv *me, eGPIOId gpioId)
{
    uint8 i;
    for(i = 0; i < me->gpioConfig->usedGPIOPinNum; i++)
    {
        if (gpioId == me->gpioConfig->pGPIOPinSet[i].gpioId)
        {
            PORTSetBits(pic32IoPort[me->gpioConfig->pGPIOPinSet[i].gpioPort],\
                    1<<me->gpioConfig->pGPIOPinSet[i].gpioBit);
            return;
        }
    }
    ASSERT(0);/* gpioId doesn't belong to me,somewhere is seriously wrong */
}

/**
* If you passed a GPIO ID doesn't belong to the GPIO object,
* this funciton will do nothing and return an error
* project
* @param[in]    me              GPIO object
* @param[in]    gpioId          GPIO ID
*/
int8 GpioDrv_ReadBit(cGpioDrv *me, eGPIOId gpioId)
{
    int8 rtnValue  = TP_ACCESS_ERROR;
    uint8 i        = 0;

    for(i = 0; i < (me->gpioConfig->usedGPIOPinNum); i++)
    {
        if (gpioId == me->gpioConfig->pGPIOPinSet[i].gpioId)
        {
            if(PORTReadBits(pic32IoPort[me->gpioConfig->pGPIOPinSet[i].gpioPort],\
                    1<<me->gpioConfig->pGPIOPinSet[i].gpioBit))
            {
                rtnValue = 1; /* High */
            }
            else
            {
                rtnValue = 0; /* Low */
            }
            break;
        }
    }
    /* gpioId doesn't belong to me,somewhere is seriously wrong */
    ASSERT(TP_ACCESS_ERROR != rtnValue);
    return rtnValue;
}

/**
* If you passed a GPIO ID doesn't belong to the GPIO object,
* this funciton will do nothing and return a fail
* project
* @param[in]    me              GPIO object
* @param[in]    gpioId          GPIO ID
*/
void GpioDrv_ClearBit(cGpioDrv *me, eGPIOId gpioId)
{
    uint8 i;
    for(i = 0; i < me->gpioConfig->usedGPIOPinNum; i++)
    {
        if (gpioId == me->gpioConfig->pGPIOPinSet[i].gpioId)
        {
            PORTClearBits(pic32IoPort[me->gpioConfig->pGPIOPinSet[i].gpioPort],\
                    1<<me->gpioConfig->pGPIOPinSet[i].gpioBit);
            return;
        }
    }
    ASSERT(0); /* gpioId doesn't belong to me,somewhere is seriously wrong */
}



/*****************************************************************************************************************
 *
 * private functions
 *
 *****************************************************************************************************************/
/**
* Set GPIO pull up resistor
* This function is hardware dependent, should be reimplemented based on the
* project
* @param[in]    gpioPort        GPIO port
* @param[in]    gpiobit         GPIO bit
*/
static void GpioDrv_SetPullUp(eIoPort gpioPort,eIoBit gpioBit)
{
    switch(pic32IoPort[gpioPort])
    {
        case IOPORT_B:
            CNPUB |= (1<<gpioBit);
            break;
#if ((__PIC32_FEATURE_SET__ == 330) || (__PIC32_FEATURE_SET__ == 350) || (__PIC32_FEATURE_SET__ == 370) || (__PIC32_FEATURE_SET__ == 430) || (__PIC32_FEATURE_SET__ == 450) || (__PIC32_FEATURE_SET__ == 470))
        case IOPORT_C:
            CNPUC |= (1<<gpioBit);
            break;
        case IOPORT_D:
            CNPUD |= (1<<gpioBit);
            break;
        case IOPORT_E:
            CNPUE |= (1<<gpioBit);
            break;
        case IOPORT_F:
            CNPUF |= (1<<gpioBit);
            break;
        case IOPORT_G:
            CNPUG |= (1<<gpioBit);
            break;
#endif
        default:
            break;
    }
}
/**
* clear GPIO pull up resistor
* This function is hardware dependent, should be reimplemented based on the
* project
* @param[in]    gpioPort        GPIO port
* @param[in]    gpiobit         GPIO bit
*/
static void GpioDrv_ClearPullUp(eIoPort gpioPort,eIoBit gpioBit)
{
    switch(pic32IoPort[gpioPort])
    {
        case IOPORT_B:
            CNPUB &= ~gpioBit;
            break;
#if ((__PIC32_FEATURE_SET__ == 330) || (__PIC32_FEATURE_SET__ == 350) || (__PIC32_FEATURE_SET__ == 370) || (__PIC32_FEATURE_SET__ == 430) || (__PIC32_FEATURE_SET__ == 450) || (__PIC32_FEATURE_SET__ == 470))
        case IOPORT_C:
            CNPUC &= ~(1<<gpioBit);
            break;
        case IOPORT_D:
            CNPUD &= ~(1<<gpioBit);
            break;
        case IOPORT_E:
            CNPUE &= ~(1<<gpioBit);
            break;
        case IOPORT_F:
            CNPUF &= ~(1<<gpioBit);
            break;
        case IOPORT_G:
            CNPUG &= ~(1<<gpioBit);
            break;
#endif
        default:
            break;
    }
}

/*
 * Special code block for handling wake key pin when system goes to deep sleep state
 * Normal GpioDrv users should not touch any of the public interface which has
 * been listed below. This code block is hardware dependent, it should
 * be update when porting GPIO driver to other chips.
 */

static void GpioDrv_DisableIntOfWakeUpKeyPin()
{
    IFS0CLR = _IFS0_INT0IF_MASK;/*clear the interrupt source*/
    IEC0CLR = _IEC0_INT0IE_MASK;/*disable INT0 interrupt*/
}

void GpioDrv_EnableIntOfWakeUpKeyPin()
{
    INTCONbits.INT0EP = 1;/* INT0 interrupt on positive:1 /negative: 0 edge */
    IPC0bits.INT0IP = 6;/* set INT0 priority; must match IPL in testISR */
    IFS0CLR = _IFS0_INT0IF_MASK;/* clear the interrupt for INT0 */
    IEC0SET = _IEC0_INT0IE_MASK;/* enable INT0 interrupt */
}

void GpioDrv_AssertWakeUpKeyEnable()
{
    ASSERT((INTCONbits.INT0EP == 1) && (IPC0bits.INT0IP == 6) &&\
            (IFS0bits.INT0IF == 0) && (IEC0bits.INT0IE == 1));
}

/* external int0 */
void __ISR(_EXTERNAL_0_VECTOR, IPL6) ExtInt0ISR(void)
{
    IFS0CLR = _IFS0_INT0IF_MASK;              /* clear the interrupt source */
    /*
     * Normally, this interrupt is enabled and generated when systen is in deep
     * sleep mode.If this interrup is generated when systen is awake
     * It means key server has done all the preparations for system going to deep
     * sleep mode(The preparations include distroying all the keys and enabling
     * the power key interrupt,that's why this interrup is generated when the
     * systen is awake). However, Power server is still on the way to tell the
     * system to deep sleep. This unnormal interrupt should be ignored.
     */
    if(BSP_IsSystemAwake())
    {
        return;
    }
    BSP_SetSystemAwake(TRUE);
    GpioDrv_DisableIntOfWakeUpKeyPin();
    GpioDrv_SendWakeUpKeyIntToKeySrv();
}

static void GpioDrv_SendWakeUpKeyIntToKeySrv()
{
    KeyEvtFromDrv *pte = Q_NEW(KeyEvtFromDrv,WAKE_UP_KEY_INTERRUPT_SIG);
    pte->keyId = POWER_KEY;
    SendToServer(KEY_SRV_ID, (QEvt*)pte);
}
