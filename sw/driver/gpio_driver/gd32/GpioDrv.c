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

static const tStmIOInfo ioInfo[] =
{
    {GPIOA,         RCC_AHBPeriph_GPIOA},
    {GPIOB,         RCC_AHBPeriph_GPIOB},
    {GPIOC,         RCC_AHBPeriph_GPIOC},
    {GPIOD,         RCC_AHBPeriph_GPIOD},
    {GPIO_INVALIDD, GPIO_AHPPreiph_Invalid},
    {GPIOF,         RCC_AHBPeriph_GPIOF},
};

/******************************************************************************
 *
 * Start-up / shut-down functions
 *
 ******************************************************************************/
void GpioDrv_Ctor(cGpioDrv *me, const tGPIODevice *pConfig )
{
    ASSERT(me && pConfig);
    GPIO_InitTypeDef GPIOInitStr;
    uint8 i = 0;
    me->gpioConfig = pConfig;
    GPIOInitStr.GPIO_Speed = GPIO_Speed_50MHz;
    /* GPIOC Periph clock enable */
    for(i = 0; i < pConfig->usedGPIOPinNum; i++)
    {
        RCC_AHBPeriphClockCmd(ioInfo[(me->gpioConfig->pGPIOPinSet[i].gpioPort)].rccAhbPeriph, ENABLE);
        GPIOInitStr.GPIO_Pin = (1<<pConfig->pGPIOPinSet[i].gpioBit);       
        switch(pConfig->pGPIOPinSet[i].gpioDirection)
        {
            case GPIO_BIDIRECTION:
            case GPIO_DIGITAL_INPUT:
            {
                if(GPIO_DIGITAL_INPUT==pConfig->pGPIOPinSet[i].gpioDirection) {
                    ASSERT(ODC_NOT_APPLICABLE==pConfig->pGPIOPinSet[i].openDrainControl);
                }
                else { //GPIO_BIDIRECTION
                    ASSERT(ODC_NOT_APPLICABLE!=pConfig->pGPIOPinSet[i].openDrainControl);
                }
                GPIOInitStr.GPIO_Mode = GPIO_Mode_IN;
                if(GPIO_ACTIVE_LOW == pConfig->pGPIOPinSet[i].gpioInitAttr)
                {
                    GPIOInitStr.GPIO_PuPd = GPIO_PuPd_UP;
                }
                else if (GPIO_ACTIVE_HIGH == pConfig->pGPIOPinSet[i].gpioInitAttr)
                {
                    GPIOInitStr.GPIO_PuPd = GPIO_PuPd_DOWN;
                }
                else if (GPIO_ACTIVE_FLOATING == pConfig->pGPIOPinSet[i].gpioInitAttr)
                {
                    GPIOInitStr.GPIO_PuPd = GPIO_PuPd_NOPULL;
                }
                else
                {
                    ASSERT(0);
                }
                break;
            }
            case GPIO_DIGITAL_OUTPUT:
            {
                GPIOInitStr.GPIO_Mode = GPIO_Mode_OUT;

                //Set open drain mode
                GPIOInitStr.GPIO_PuPd = GPIO_PuPd_NOPULL;
                if(ODC_ENABLE == pConfig->pGPIOPinSet[i].openDrainControl)
                {
                    GPIOInitStr.GPIO_OType = GPIO_OType_OD;
                }
                else if(ODC_DISABLE == pConfig->pGPIOPinSet[i].openDrainControl)
                {
                    GPIOInitStr.GPIO_OType = GPIO_OType_PP;
                }
                else
                {
                   ASSERT(0);
                }

                //Set default value
                if (pConfig->pGPIOPinSet[i].gpioInitAttr == GPIO_DEFAULT_OUT_HIGH)
                {
                    GpioDrv_SetBit_(me, pConfig->pGPIOPinSet[i].gpioId);
                }
                else if (pConfig->pGPIOPinSet[i].gpioInitAttr == GPIO_DEFAULT_OUT_LOW)
                {
                   GpioDrv_ClearBit_(me, pConfig->pGPIOPinSet[i].gpioId);
                }
                else if (pConfig->pGPIOPinSet[i].gpioInitAttr == GPIO_DEFAULT_OUT_FLOATING)
                {
                }
                else
                {
                   ASSERT(0);
                }

                break;
            }
            case GPIO_ANALOG_INPUT:
            {
                GPIOInitStr.GPIO_Mode = GPIO_Mode_AN;
                GPIOInitStr.GPIO_PuPd = GPIO_PuPd_NOPULL;
                break;
            }
            case GPIO_ANALOG_OUTPUT:
            {
                GPIOInitStr.GPIO_Mode = GPIO_Mode_AN;
                GPIOInitStr.GPIO_PuPd = GPIO_PuPd_NOPULL;
                break;
            }
            default:break;
        }
        GPIO_Init(ioInfo[(me->gpioConfig->pGPIOPinSet[i].gpioPort)].pMCUgpioPort, &GPIOInitStr);
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
    GPIO_InitTypeDef GPIOInitStr;
    uint8 i = 0;
    me->isCreated = FALSE;
    GPIOInitStr.GPIO_Speed = GPIO_Speed_50MHz;
    for(i = 0; i < me->gpioConfig->usedGPIOPinNum; i++)
    {
        GPIOInitStr.GPIO_Pin = (1 << (me->gpioConfig->pGPIOPinSet[i].gpioBit));       
        GPIOInitStr.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIOInitStr.GPIO_Mode = GPIO_Mode_IN;
        GPIO_Init(ioInfo[(me->gpioConfig->pGPIOPinSet[i].gpioPort)].pMCUgpioPort, &GPIOInitStr);
    }
}

/******************************************************************************
 *
 * Private functions
 *
 ******************************************************************************/
static void GpioDrv_SetBit_(cGpioDrv *me, eGPIOId gpioId)
{
    uint32 gpioPinBit = 0;
    uint8 i = 0;
    for(i = 0; i < me->gpioConfig->usedGPIOPinNum; i++)
    {
        gpioPinBit = (1<<(me->gpioConfig->pGPIOPinSet[i].gpioBit));
        
        if (gpioId == me->gpioConfig->pGPIOPinSet[i].gpioId)
        {
            /* For bidiretional pin, assume it is on OUTPUT mode */          
            GPIO_SetBits(ioInfo[(me->gpioConfig->pGPIOPinSet[i].gpioPort)].pMCUgpioPort,gpioPinBit);
            return;
        }
    }
    ASSERT(0); /* gpioId doesn't belong to me,somewhere is seriously wrong */
}

static void GpioDrv_ClearBit_(cGpioDrv *me, eGPIOId gpioId)
{
    uint16 gpioPinBit;
    uint8 i = 0;
    for(i = 0; i < me->gpioConfig->usedGPIOPinNum; i++)
    {
        if (gpioId == me->gpioConfig->pGPIOPinSet[i].gpioId)
        {
            /* For bidiretional pin, assume it is on OUTPUT mode */          
            gpioPinBit = (1 << (me->gpioConfig->pGPIOPinSet[i].gpioBit));
            GPIO_WriteBit(ioInfo[(me->gpioConfig->pGPIOPinSet[i].gpioPort)].pMCUgpioPort, gpioPinBit,Bit_RESET);
            return;
        }
    }
    ASSERT(0); /* gpioId doesn't belong to me,somewhere is seriously wrong */
}

/******************************************************************************
 *
 * Public functions
 *
 ******************************************************************************/
/* Set Bidiretional pin to input */
void GpioDrv_SetDigitalIn(cGpioDrv *me, eGPIOId gpioId)
{
    ASSERT(me && me->isCreated);
    
    GPIO_InitTypeDef GPIOInitStr;
    uint8 i = 0;
    for(i = 0; i < me->gpioConfig->usedGPIOPinNum; i++)
    {
        /* only the gpio pin which has GPIO_BIDIRECTION feature has the access
         * to this function, otherwise, return fail. */
        if (gpioId == me->gpioConfig->pGPIOPinSet[i].gpioId )
        {
            ASSERT(GPIO_BIDIRECTION == me->gpioConfig->pGPIOPinSet[i].gpioDirection);
            
            GPIOInitStr.GPIO_Speed = GPIO_Speed_50MHz;
            GPIOInitStr.GPIO_Pin = (1 << (me->gpioConfig->pGPIOPinSet[i].gpioBit));
            GPIOInitStr.GPIO_Mode = GPIO_Mode_IN;
            if(GPIO_ACTIVE_LOW == me->gpioConfig->pGPIOPinSet[i].gpioInitAttr)
            {
                GPIOInitStr.GPIO_PuPd = GPIO_PuPd_UP;
            }
            else if(GPIO_ACTIVE_HIGH == me->gpioConfig->pGPIOPinSet[i].gpioInitAttr)
            {
                GPIOInitStr.GPIO_PuPd = GPIO_PuPd_DOWN;
            }
            else if (GPIO_ACTIVE_FLOATING == me->gpioConfig->pGPIOPinSet[i].gpioInitAttr)
            {
                GPIOInitStr.GPIO_PuPd = GPIO_PuPd_NOPULL;
            }
            else
            {
                ASSERT(0);
            }
            GPIO_Init(ioInfo[(me->gpioConfig->pGPIOPinSet[i].gpioPort)].pMCUgpioPort, &GPIOInitStr);
            return;
        }
    }
    ASSERT(0); /* gpioId doesn't belong to me,somewhere is seriously wrong */
}

/* Set Bidiretional pin to output */
void GpioDrv_SetDigitalOut(cGpioDrv *me, eGPIOId gpioId)
{
    ASSERT(me && me->isCreated);
    
    GPIO_InitTypeDef GPIOInitStr;
    uint8 i = 0;
    for(i = 0; i < me->gpioConfig->usedGPIOPinNum; i++)
    {
        /* only the gpio pin which has GPIO_BIDIRECTION feature has the access
         * to this function, otherwise, return fail. */
        if (gpioId == me->gpioConfig->pGPIOPinSet[i].gpioId )
        {
            ASSERT(GPIO_BIDIRECTION == me->gpioConfig->pGPIOPinSet[i].gpioDirection);
            
            GPIOInitStr.GPIO_Speed = GPIO_Speed_50MHz;
            GPIOInitStr.GPIO_Pin = (1 << (me->gpioConfig->pGPIOPinSet[i].gpioBit));
            GPIOInitStr.GPIO_Mode = GPIO_Mode_OUT;
            
            //Set open drain mode
            GPIOInitStr.GPIO_PuPd = GPIO_PuPd_NOPULL;
            if(ODC_ENABLE == me->gpioConfig->pGPIOPinSet[i].openDrainControl)
            {
                GPIOInitStr.GPIO_OType = GPIO_OType_OD;
            }
            else if(ODC_DISABLE == me->gpioConfig->pGPIOPinSet[i].openDrainControl)
            {
                GPIOInitStr.GPIO_OType = GPIO_OType_PP;
            }
            else
            {
               ASSERT(0);
            }
            
            GPIO_Init(ioInfo[(me->gpioConfig->pGPIOPinSet[i].gpioPort)].pMCUgpioPort, &GPIOInitStr);
            return;
        }
    }
    ASSERT(0); /* gpioId doesn't belong to me,somewhere is seriously wrong */
}

void GpioDrv_SetBit(cGpioDrv *me, eGPIOId gpioId)
{
    ASSERT(me && me->isCreated);
    GpioDrv_SetBit_(me, gpioId);
}

int8 GpioDrv_ReadBit(cGpioDrv *me, eGPIOId gpioId)
{
    ASSERT(me && me->isCreated);
    
    int8 rtnValue = TP_ACCESS_ERROR;
    uint8 i       = 0;
    for(i = 0; i < (me->gpioConfig->usedGPIOPinNum); i++)
    {
        if (gpioId == me->gpioConfig->pGPIOPinSet[i].gpioId)
        {
            rtnValue = GPIO_ReadInputDataBit(ioInfo[(me->gpioConfig->pGPIOPinSet[i].gpioPort)].pMCUgpioPort, \
                                  (1<<me->gpioConfig->pGPIOPinSet[i].gpioBit));
            break;
        }
    }
    /* gpioId doesn't belong to me,somewhere is seriously wrong */
    ASSERT(TP_ACCESS_ERROR != rtnValue);
    return rtnValue;
}

bool GpioDrv_isBitSet(cGpioDrv *me, eGPIOId gpioId)
{
    /* ASE-TK Server will query GPIO bit before GPIO initialization,
     * on this moment isCreated==FALSE, thus do not add ASSERT like
     *   ASSERT(me && me->isCreated);
     */
    ASSERT(me);

    bool rtnValue = FALSE;
    uint8 i       = 0;
    for(i = 0; i < (me->gpioConfig->usedGPIOPinNum); i++)
    {
        const tGPIOPin *pPin = &me->gpioConfig->pGPIOPinSet[i];
        if (gpioId == pPin->gpioId)
        {
            GPIO_TypeDef*   gpio_port   = ioInfo[pPin->gpioPort].pMCUgpioPort;
            uint16_t        gpio_bit    = 1 << pPin->gpioBit;
            rtnValue = GPIO_ReadOutputDataBit(gpio_port, gpio_bit);
            break;
        }
    }
    /* gpioId doesn't belong to me,somewhere is seriously wrong */
    ASSERT(i<me->gpioConfig->usedGPIOPinNum);
    return rtnValue;
}

void GpioDrv_ToggleBit(cGpioDrv *me, eGPIOId gpioId)
{
    if (GpioDrv_isBitSet(me, gpioId))
    {
        GpioDrv_ClearBit(me, gpioId);
    }
    else
    {
        GpioDrv_SetBit(me, gpioId);
    }
}

void GpioDrv_ClearBit(cGpioDrv *me, eGPIOId gpioId)
{
    ASSERT(me && me->isCreated);
    GpioDrv_ClearBit_(me, gpioId);
}

#ifdef HW_GPIO_DEBUG
void GpioDrv_DirectPinAccessSet(eIoPort ioPort, eIoBit ioBit, eGPIOInitAttr act)
{
    if (act == GPIO_DEFAULT_OUT_HIGH)
    {
        GPIO_SetBits(ioInfo[ioPort].pMCUgpioPort,(1 << ioBit));
    }
    else if (act == GPIO_DEFAULT_OUT_LOW)
    {
        GPIO_WriteBit(ioInfo[ioPort].pMCUgpioPort, (1 << ioBit),Bit_RESET);
    }
    else
    {
        ASSERT(0);
    }
}
uint8 GpioDrv_DirectPinAccessRead(eIoPort ioPort, eIoBit ioBit, eGPIODrection direction)
{
    uint8 value = INVALID_GPIO_READING;
    switch (direction)
    {
    case GPIO_DIGITAL_OUTPUT:
      {
         value = GPIO_ReadOutputDataBit(ioInfo[ioPort].pMCUgpioPort,(1 << ioBit));
         break;
      }
    case GPIO_DIGITAL_INPUT:
      {
         value = GPIO_ReadInputDataBit(ioInfo[ioPort].pMCUgpioPort,(1 << ioBit));
         break;
      }
        default:
      break;
    }
    return value;
}

#endif


/**
* Enable EXTI Interrupt for GPIO pin.
* On STM32F0xx, trigger the following interupt
*    void EXTI0_1_IRQHandler(void)
*    void EXTI2_3_IRQHandler(void) 
*    void EXTI4_15_IRQHandler(void)
*
* Note type is better to set a EXTITrigger_TypeDef, but GpioDrv.h should not include
* either stm32f0xx.h oro stm32f10x.h, thus set type to uint8
*
* @param[in]    me              GPIO object
* @param[in]    gpioId          GPIO ID
* @param[in]    type            Interrupt trigger type
*/
void GpioDrv_EnableExtInterrupt(cGpioDrv *me, eGPIOId gpioId, tExtiIntTrigger type)
{
    ASSERT(me && me->isCreated);

    uint8 st_type;
    if(ExtiIntTri_Rising==type) 
    {
        st_type= EXTI_Trigger_Rising;
    }
    else if(ExtiIntTri_Falling==type) 
    {
        st_type= EXTI_Trigger_Falling;
    }
    else if(ExtiIntTri_Rising_Falling==type) 
    {
        st_type= EXTI_Trigger_Rising_Falling;
    }
    else 
    {
        st_type= EXTI_Trigger_Rising;
        ASSERT(0);
    }

    uint8 i       = 0;
    for(i = 0; i < (me->gpioConfig->usedGPIOPinNum); i++)
    {          
        if (gpioId == me->gpioConfig->pGPIOPinSet[i].gpioId)
        {
            const tGPIOPin *pGpio = &me->gpioConfig->pGPIOPinSet[i];

            /* Only input GPIO can be interrupt, not sure if GPIO_BIDIRECTION also support interrupt */
            ASSERT(GPIO_DIGITAL_INPUT==pGpio->gpioDirection || GPIO_BIDIRECTION==pGpio->gpioDirection);
    
            /* Prepare paremeters */
            uint8 intPort= (pGpio->gpioPort - IO_PORT_A) + EXTI_PortSourceGPIOA;
            ASSERT(intPort<=EXTI_PortSourceGPIOF);// as EXTI_PortSourceGPIOA = 0 and intPort is unsigned integer
            
            uint8 intPinSrc=  (pGpio->gpioBit - IO_BIT_0) + EXTI_PinSource0;
            ASSERT(intPinSrc<=EXTI_PinSource15); // as EXTI_PinSource0 = 0 and intPinSrc is unsigned intgeger

            IRQn_Type irqType;
            if(intPinSrc==EXTI_PinSource0 || intPinSrc==EXTI_PinSource1) 
                irqType= EXTI0_1_IRQn;
            else if(intPinSrc==EXTI_PinSource2 || intPinSrc==EXTI_PinSource3) 
                irqType= EXTI2_3_IRQn;
            else // EXTI_PinSource4 ~ EXTI_PinSource15
                irqType= EXTI4_15_IRQn;

            uint32 intExtiLine= EXTI_Line0 << (pGpio->gpioBit - IO_BIT_0);
            ASSERT(intExtiLine>=EXTI_Line0 && intExtiLine<=EXTI_Line27);

                    
            /* Enable SYSCFG clock */
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
            
            /* Connect EXTI Line to gpio pin */
            SYSCFG_EXTILineConfig(intPort, intPinSrc);
            
            /* Configure EXTI line */
            EXTI_InitTypeDef   EXTI_InitStructure;
            EXTI_InitStructure.EXTI_Line = intExtiLine;
            EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
            EXTI_InitStructure.EXTI_Trigger = (EXTITrigger_TypeDef)st_type;
            EXTI_InitStructure.EXTI_LineCmd = ENABLE;
            EXTI_Init(&EXTI_InitStructure);

            /* Enable and set EXTI Interrupt */
            NVIC_InitTypeDef   NVIC_InitStructure;
            NVIC_InitStructure.NVIC_IRQChannel = irqType;
            //NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
            break;
        }
    }
    /* gpioId doesn't belong to me,somewhere is seriously wrong */
    ASSERT(i < me->gpioConfig->usedGPIOPinNum);
}



/**
* Disable EXTI Interrupt for GPIO pin.
* On STM32F0xx, trigger the following interupt
*    void EXTI0_1_IRQHandler(void)
*    void EXTI2_3_IRQHandler(void) 
*    void EXTI4_15_IRQHandler(void)
*
* @param[in]    me              GPIO object
* @param[in]    gpioId          GPIO ID
* @param[in]    type            Interrupt trigger type
*/
void GpioDrv_DisableExtInterrupt(cGpioDrv *me, eGPIOId gpioId)
{
    ASSERT(me && me->isCreated);

    uint8 i       = 0;
    for(i = 0; i < (me->gpioConfig->usedGPIOPinNum); i++)
    {          
        if (gpioId == me->gpioConfig->pGPIOPinSet[i].gpioId)
        {
            const tGPIOPin *pGpio = &me->gpioConfig->pGPIOPinSet[i];
    
            /* Prepare paremeters */
            uint8 intPort= (pGpio->gpioPort - IO_PORT_A) + EXTI_PortSourceGPIOA;
            ASSERT(intPort<=EXTI_PortSourceGPIOF);// as EXTI_PortSourceGPIOA = 0 and intPort is unsigned integer
            
            uint8 intPinSrc=  (pGpio->gpioBit - IO_BIT_0) + EXTI_PinSource0;
            ASSERT(intPinSrc<=EXTI_PinSource15); // as EXTI_PinSource0 = 0 and intPinSrc is unsigned intgeger

            IRQn_Type irqType;
            if(intPinSrc==EXTI_PinSource0 || intPinSrc==EXTI_PinSource1) 
                irqType= EXTI0_1_IRQn;
            else if(intPinSrc==EXTI_PinSource2 || intPinSrc==EXTI_PinSource3) 
                irqType= EXTI2_3_IRQn;
            else // EXTI_PinSource4 ~ EXTI_PinSource15
                irqType= EXTI4_15_IRQn;

            /* Enable and set EXTI Interrupt */
            NVIC_InitTypeDef   NVIC_InitStructure;
            NVIC_InitStructure.NVIC_IRQChannel = irqType;
            //NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
            NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
            NVIC_Init(&NVIC_InitStructure);
            break;
        }
    }
    /* gpioId doesn't belong to me,somewhere is seriously wrong */
    ASSERT(i < me->gpioConfig->usedGPIOPinNum);
}
    

void GpioDrv_DisableIntOfWakeUpKeyPin()
{
    /* Need to be implemented */
}

void GpioDrv_EnableIntOfWakeUpKeyPin()
{
    /* Need to be implemented */
}
