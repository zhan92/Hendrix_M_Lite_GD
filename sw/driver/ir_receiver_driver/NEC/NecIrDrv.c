/*
-------------------------------------------------------------------------------
TYMPHANY LTD





                   infared NEC format Driver
                  -------------------------

                  SW Module Document




@file        NecIrDrv.c
@brief       This file implements the infrared NEC format driver
@author      Edmond Sung, Bob.Xu
@date        2014-07-33
@copyright (c) Tymphany Ltd. All rights reserved.

Change History:
VERSION    : 1    DRAFT      2014-07-23     Bob.Xu
DESCRIPTION: First Draft. Generated by newclass.py
SCO/ERROR  : 
-------------------------------------------------------------------------------
*/

#include "stm32f0xx.h"
#include "NecIrDrv_priv.h"
#include "IR.Config"
#include "trace.h"

static void IrRxDrv_ClearState(void);
static void IrRxDrv_EdgeISR(void);
static void IrRxDrv_TimerConfig(void);
static void IrRxDrv_EXTIConfig(cIrRxDrv *me);
static void IrRxDrv_ResetReleaseCnt(void);

static tNecIrState necIrState;
static uint32 time2Tick = 0;
static eKeyState irKeyState[NUM_OF_IR_KEY] = {KEY_UP};
const tIrKeyIdMapCode * pLocalIrKeyIdMapeCode;
const tGPIOPin *pIOConfig;

static const tStmIoInfo stmIOInfo[] =
{
    {GPIOA,         RCC_AHBPeriph_GPIOA, EXTI_PortSourceGPIOA},
    {GPIOB,         RCC_AHBPeriph_GPIOB, EXTI_PortSourceGPIOB},
    {GPIOC,         RCC_AHBPeriph_GPIOC, EXTI_PortSourceGPIOC},
    {GPIOD,         RCC_AHBPeriph_GPIOD, EXTI_PortSourceGPIOD},
    {GPIO_INVALIDD, GPIO_AHPPreiph_Invalid, EXTI_PortSourceInvalid},  //port E is not available
    {GPIOF,         RCC_AHBPeriph_GPIOF, EXTI_PortSourceGPIOF},
};

void IrRxDrv_Ctor(cIrRxDrv *me, const tIrKeyIdMapCode * pIrKeyIdMapeCode, const tGPIOPin *pIoConfig)
{
    if(!me->isCreated)
    {
        pLocalIrKeyIdMapeCode = pIrKeyIdMapeCode;
        pIOConfig = pIoConfig;
        /* Enable the Externer interrup to detect the pulse generated by IR */
        IrRxDrv_EXTIConfig(me);
        /* Configure two times to mesure the pulse width */
        IrRxDrv_TimerConfig();
        me->isCreated = TRUE;
    }
}

void IrRxDrv_Xtor(cIrRxDrv *me)
{
    /* TO DO */
//    TIM_Cmd(TIM2, DISABLE);
//    TIM_Cmd(TIM3, DISABLE);
}

eKeyState IrRxDrv_ReadKeyState(cIrRxDrv *me, eKeyID keyID)
{
    uint8 i = 0;
    for( i = 0; i < NUM_OF_IR_KEY; i++)
    {
        if(keyID == pLocalIrKeyIdMapeCode[i].irKeyId)
        {
            return irKeyState[i];
        }
    }
    ASSERT(i != NUM_OF_IR_KEY);
    return KEY_INVALIDE_STATE;
}

void IrRxDrv_InterpretIrCode()
{
    if(necIrState.arrIndex >= IR_BITS_LENGTH)
    {
      /* Have got all the bits, now start decoding  */
      uint8 i = 0;
      for (i = 0;i < IR_BITS_LENGTH;i++)
      {
          /* check logic "0" */
          if((necIrState.irArr[i] <= IR_LOGIC_ZERO_UPPER_LIMIT) && \
            (necIrState.irArr[i] >= IR_LOGIC_ZERO_LOWER_LIMIT))
          {
              necIrState.command &=0xfffffffe;
          }
          /* check logic "1" */
          else if((necIrState.irArr[i] <= IR_LOGIC_ONE_UPPER_LIMIT) && \
            (necIrState.irArr[i] >= IR_LOGIC_ONE_LOWER_LIMIT))
          {
              necIrState.command |= 0X01;
          }
          else
          {
              return;
          }

          if (i+1<IR_BITS_LENGTH)
          {
              necIrState.command <<= 1;
          }
      }/* Decoding end */

      /* Decode done, if this is an IR key, a key done is sent first */
      for(i = 0; i < NUM_OF_IR_KEY; i++)
      {
         if(necIrState.command == pLocalIrKeyIdMapeCode[i].irCode)
         {
            irKeyState[i] = KEY_DOWN;
            break;
         }
         if(i == NUM_OF_IR_KEY)
         {
            ASSERT(0);
         }
      }
      necIrState.arrIndex       = 0;
      necIrState.irState = IR_STATE_IDLE;  //start to long press
    }
}

/**
  * @brief  This function handles External line 0 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI4_15_IRQHandler(void)
{
  if(EXTI_GetITStatus(1<<(pIOConfig->gpioBit)) != RESET)
  {
      /* Clear the EXTI line 0 pending bit */
      IrRxDrv_EdgeISR();
      EXTI_ClearITPendingBit(1<<(pIOConfig->gpioBit));
  }
}

void EXTI2_3_IRQHandler(void)
{
  if(EXTI_GetITStatus(1<<(pIOConfig->gpioBit)) != RESET)
  {
      /* Clear the EXTI line 0 pending bit */
      IrRxDrv_EdgeISR();
      EXTI_ClearITPendingBit(1<<(pIOConfig->gpioBit));
  }
}

/**
  * @brief  This function handles TIM2 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{
    time2Tick++;
    if (TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
    }
}

void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_CC3) != RESET)
    {
      IrRxDrv_ResetReleaseCnt();
      TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);
    }
}


static void IrRxDrv_TimerConfig()
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* TIM2 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* Enable the TIM2 gloabal Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = 800;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    /* Prescaler configuration */
    TIM_PrescalerConfig(TIM2, TIM_TimeBaseStructure.TIM_Prescaler, TIM_PSCReloadMode_Immediate);
     
    /* TIM2 Interrupts enable */
    TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
    
    /* TIM3 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    /* Enable the TIM3 gloabal Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = 16000; /* 2ms */
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    /* Prescaler configuration */
    TIM_PrescalerConfig(TIM3, TIM_TimeBaseStructure.TIM_Prescaler, TIM_PSCReloadMode_Immediate);
     
    /* TIM Interrupts enable */
    TIM_ITConfig(TIM3, TIM_IT_CC3, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}

/**
  * @brief  Configure in interrupt mode
  * @param  None
  * @retval None
  */
static void IrRxDrv_EXTIConfig(cIrRxDrv *me)
{
    EXTI_InitTypeDef   EXTI_InitStructure;
    GPIO_InitTypeDef   GPIO_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;
    uint8_t gpio_bit, nvic_channel;

    gpio_bit = pIOConfig->gpioBit;
    if( gpio_bit < 2 )
        nvic_channel = EXTI0_1_IRQn;
    else if( gpio_bit < 4 )
        nvic_channel = EXTI2_3_IRQn;
    else if( gpio_bit < 16 )
        nvic_channel = EXTI4_15_IRQn;
    else
    {
        ASSERT(0);
    }
        
    /* Enable GPIOA clock */
    RCC_AHBPeriphClockCmd(stmIOInfo[pIOConfig->gpioPort].rccAhbPeriph, ENABLE);

    /* Configure PA5 pin as input floating */
    GPIO_InitStructure.GPIO_Pin = (1<<(pIOConfig->gpioBit));
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(stmIOInfo[pIOConfig->gpioPort].gpioPort, &GPIO_InitStructure);

    /* Enable SYSCFG clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    
    SYSCFG_EXTILineConfig(stmIOInfo[pIOConfig->gpioPort].exitPortSource, gpio_bit);

    /* Configure EXTI0 line */
    EXTI_InitStructure.EXTI_Line = (1<<(pIOConfig->gpioBit));
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set EXTI0 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = nvic_channel;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief   This function should be run in every 2ms in order to check the IR key release
 * @return
 * @param
 */
static void IrRxDrv_ResetReleaseCnt()
{
    if (necIrState.releaseCountdown)
    {
        necIrState.releaseCountdown--;
        if (necIrState.releaseCountdown==0)
        {
            IrRxDrv_ClearState();
        }
    }
}

static void IrRxDrv_EdgeISR(void)
{
  necIrState.releaseCountdown = IR_KEY_RELEASE_COUNTDOWN_TIME_120MS;
  if(necIrState.irState == IR_STATE_IDLE)
    {
        time2Tick = RESET_VALUE;
        necIrState.irState  = IR_IN_PROGRESS;
        necIrState.arrIndex = 0;
        necIrState.g_TimerCount = RESET_VALUE;
    }
    else
    {
        necIrState.g_TimerCount = time2Tick;
        if(necIrState.irState == IR_IN_PROGRESS)
        {
            if((necIrState.g_TimerCount > IR_HEAD_CODE_LOWER_LIMIT && \
              necIrState.g_TimerCount < IR_HEAD_CODE_UPPER_LIMIT))
            {
              uint8 i = 0;
              for( i = 0; i < NUM_OF_IR_KEY; i++)
              {
                  irKeyState[i] = KEY_UP;
              }
              time2Tick = RESET_VALUE;
              necIrState.g_TimerCount = RESET_VALUE;
              return;
            }
            /* check IR long press, PP remote */
            else if(necIrState.g_TimerCount > IR_REPEAT_CODE_LOWER_LIMIT && \
              necIrState.g_TimerCount < IR_REPEAT_CODE_UPPER_LIMIT)
            {
                necIrState.irState = IR_STATE_IDLE;
                return;
            }
            else if(necIrState.g_TimerCount > IR_KEY_RELEASE_COUNTDOWN_TIME_120MS)
            {
                IrRxDrv_ClearState();
            }
        }
        necIrState.irArr[necIrState.arrIndex] = necIrState.g_TimerCount;
        /* save the timer counter for decoding */
        if(necIrState.arrIndex > IR_BITS_LENGTH)
        {
            necIrState.releaseCountdown = 0;
            necIrState.arrIndex       = 0;
            necIrState.irState = IR_STATE_IDLE;  //start to long press
            return;
        }
        necIrState.arrIndex++;
    }
    necIrState.g_TimerCount = RESET_VALUE;
    time2Tick = RESET_VALUE;
}

static void IrRxDrv_ClearState(void)
{
    necIrState.command        = 0;
    necIrState.irState        = IR_STATE_IDLE;
    necIrState.receivedKeyId  = INVALID_KEY;
    uint8 i = 0;
    for( i = 0; i < NUM_OF_IR_KEY; i++)
    {
        irKeyState[i] = KEY_UP;
    }
}
