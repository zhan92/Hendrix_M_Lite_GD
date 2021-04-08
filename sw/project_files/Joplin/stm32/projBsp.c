/*
-------------------------------------------------------------------------------
TYMPHANY LTD





                  Public project Bsp interfaces
                  -------------------------

                  SW Module Document




@file        projBsp.h
@brief       This file implements the public project bsp interfaces which declared
             at projBsp.h
@author      Bob.Xu
@date        2014-07-14
@copyright (c) Tymphany Ltd. All rights reserved.

Change History:
VERSION    : 1    DRAFT      2014-06-12     Bob.Xu
DESCRIPTION: First Draft. Generated by newclass.py
SCO/ERROR  :
-------------------------------------------------------------------------------
*/
#include "stm32f0xx.h"

#include "bsp.h"
#include "projBsp.h"
#include "controller.h"
#include "attachedDevices.h"
#include "signals.h"
#ifdef HAS_BLUETOOTH
#include "BluetoothDrv.h"
#endif

#define MIN_SEND_INTERVAL_MS (50)

static void SetExtInterrupt(uint8 port, uint8 pin, uint32 extiLine,
                                                  EXTITrigger_TypeDef type, IRQn_Type irq, FunctionalState state);

/*****************************************************************************************************************
 *
 *  Interrupt functions
 *
 *****************************************************************************************************************/
/**
  * @brief  This function handles External line 0 interrupt request
  * @param  None
  * @retval None
  */
void EXTI4_15_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line14) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line14);
    }
    if(EXTI_GetITStatus(EXTI_Line15) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line15);
    }
}

void EXTI2_3_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line2) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line2);
        /* Todo: send msg out? */
     }
}

void EXTI0_1_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line0);
        /* Todo: send msg out? */
    }
}


void ProjBsp_SysClkUpdate()
{
#ifdef HAS_MCO
		MCO_Init();
#endif
}

void SetDisableWakeupSources()
{
    SetExtInterrupt(EXTI_PortSourceGPIOA, EXTI_PinSource2,
               EXTI_Line2, EXTI_Trigger_Rising_Falling, EXTI2_3_IRQn, DISABLE);

    SetExtInterrupt(EXTI_PortSourceGPIOB, EXTI_PinSource0,
                   EXTI_Line0, EXTI_Trigger_Rising, EXTI0_1_IRQn, DISABLE);
}

static void SetExtInterrupt(uint8 port, uint8 pin, uint32 extiLine,
                                                  EXTITrigger_TypeDef type, IRQn_Type irq, FunctionalState state)
{
    /* Enable the interrupts */
    EXTI_InitTypeDef   EXTI_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;

    /* Connect EXTI Line to gpio pin */
    SYSCFG_EXTILineConfig(port, pin);
    /* Configure EXTI line */
    EXTI_InitStructure.EXTI_Line = extiLine;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = type;
    EXTI_InitStructure.EXTI_LineCmd = state;
    EXTI_Init(&EXTI_InitStructure);
    /* Enable and set EXTI Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = irq;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = state;
    NVIC_Init(&NVIC_InitStructure);
}

void SetEnableWakeupSources(void)
{
    //disable it for now
    GPIO_InitTypeDef GPIO_InitStructure;
    /* Initial GPIO for AC interrupt as it's used for ADC in running time*/
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Power Key input */
    /* Initial and power key to PA2 pin*/
    SetExtInterrupt(EXTI_PortSourceGPIOA, EXTI_PinSource2,
                   EXTI_Line2, EXTI_Trigger_Rising_Falling, EXTI2_3_IRQn, ENABLE);


    /* Initial GPIO for AC interrupt as it's used for ADC in running time*/
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* DC input */
    /* Initial and power key to PB0 pin*/
    SetExtInterrupt(EXTI_PortSourceGPIOB, EXTI_PinSource0,
                   EXTI_Line0, EXTI_Trigger_Rising, EXTI0_1_IRQn, ENABLE);

}

void RTC_IRQHandler()
{
    RTC_TimeTypeDef zeroTime = {0};

    if (RTC_GetITStatus(RTC_IT_ALRA) != RESET)
    {
        RTC_ClearITPendingBit(RTC_IT_ALRA);

        RTC_TimeStructInit(&zeroTime);
        RTC_SetTime(RTC_Format_BIN, &zeroTime);

        QEvt* pEvt = Q_NEW(QEvt, TIMER_FEED_WD_SIG);
        SendToServer(MAIN_APP_ID, (QEvt*)pEvt);
    }
    if(EXTI_GetITStatus(EXTI_Line17) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line17);
    }
}

void RTC_SetUpWakeUpAlarm(uint8 seconds)
{
    RTC_TimeTypeDef  alarmTime = {0};
    RTC_AlarmTypeDef alarm     = {0};
    RTC_TimeTypeDef  zeroTime  = {0};

    RTC_TimeStructInit(&alarmTime);
    RTC_AlarmStructInit(&alarm);
    RTC_TimeStructInit(&zeroTime);

    alarmTime.RTC_Seconds = seconds;
    RTC_SetTime(RTC_Format_BIN, &zeroTime);

    alarm.RTC_AlarmTime = alarmTime;
    alarm.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay | RTC_AlarmMask_Hours | RTC_AlarmMask_Minutes;

    RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &alarm);

    RTC_OutputConfig(RTC_Output_Disable, RTC_OutputPolarity_Low);
    RTC_ITConfig(RTC_IT_ALRA, ENABLE);
    RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
    RTC_ClearFlag(RTC_FLAG_ALRAF);
}

void RTC_Initialize()
{
    RTC_InitTypeDef rtc = {0};
    EXTI_InitTypeDef exti = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    PWR_BackupAccessCmd(ENABLE);

    RCC_BackupResetCmd(ENABLE);
    RCC_BackupResetCmd(DISABLE);

    RCC_LSICmd(ENABLE);
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
    RCC_RTCCLKCmd(ENABLE);

    RTC_StructInit(&rtc);

    /*
        ck_spre = RTCCLK/((PREDIV_A+1) * (PREDIV_B+1))
        40kHz/(99+1)*(399+1) = 1
    */
    rtc.RTC_AsynchPrediv = 0x63;
    rtc.RTC_SynchPrediv  = 0x18F;

    RTC_Init(&rtc);

    EXTI_ClearITPendingBit(EXTI_Line17);

    EXTI_StructInit(&exti);
    exti.EXTI_Line = EXTI_Line17;
    exti.EXTI_Mode = EXTI_Mode_Interrupt;
    exti.EXTI_Trigger = EXTI_Trigger_Rising;
    exti.EXTI_LineCmd = ENABLE;
    EXTI_Init(&exti);

    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/* On exception handler, interrupt is disabled, we can not use BSP_BlockingDelayMs()
 * But this delay is not accurary
 */
void BSP_ExpBlockingDelayMs(uint32 ms)
{
#if defined(INTERNAL_HIGH_SPEED_CLOCK)
    uint32 us= 1000 * ms; //8MHz CPU
#elif defined(EXTERNAL_HIGH_SPEED_CLOCK)
    uint32 us= 6000 * ms; //48MHz CPU
#else
    error wrong clock setting
#endif

    while(us--)
    {
        asm("nop");
        asm("nop");
    }
}


