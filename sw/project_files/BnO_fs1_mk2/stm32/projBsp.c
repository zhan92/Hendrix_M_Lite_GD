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
#include <stdarg.h> //va_list
#include "stm32f0xx.h"
#include "trace.h"
#include "bsp.h"
#include "projBsp.h"
#include "attachedDevices.h"
#include "IoExpanderDrv.h"
#include "object_ids.h"
#include "signals.h"
#include "PowerSrv_v2.h"
#include "controller.h"
#include "AseNgSrv.h"
#include "SettingSrv.h"


#ifdef HAS_AZOTEQ_INTEG_TOUCHKEY_DRV
#include "IQS_Commands.h"
#endif


#ifdef HAS_PROJ_PRINTF
int ProjBsp_Printf(bool toAsetk, const char *file, const int line, const char *fmt, ...)
{
    /* On FS1, when toAsetk=TRUE, this function need 960 (0x3C0) bytes on call stack
     */
    /* AseNgSrv_SendLog() also print and enter this function again, it cause infinite recursive call.
     * Thus we use "critical_section" to avoid repeated enter here
     */
    static bool critical_section= 0;
    if( critical_section ) {
        return 0;
    }
    
    critical_section= 1;    
    ASSERT(file);
    ASSERT(fmt);
    
    static char buf[256];
    va_list argus;
    va_start( argus, fmt );
    vsnprintf (buf, sizeof(buf), fmt, argus );
    va_end( argus );

    //Print to UART
#ifndef NDEBUG
    //printf(buf);
    char *p_ch= buf;
    static char pre_ch= '\n';
    uint32 time= getSysTime(); //time will overflow after 49.x days
    while(*p_ch!='\0')
    {
        if(pre_ch=='\n')
        {
            //Note the time do not include MCU sleep period (OFF mode), because on MCU do not execute time interrupt on sleep
            printf("[%dd,%02d:%02d:%02d.%03d] ", time/1000/60/60/24, (time/1000/60/60)%24, (time/1000/60)%60, (time/1000)%60, time%1000);
        }
        putchar(*p_ch);
        pre_ch= *p_ch;
        p_ch++;
    }
#endif

    //Print to ASE-TK
    if(toAsetk)
    {
        AseNgSrv_SendLog(buf, "", 0);
        //AseNgSrv_SendLog(buf, file, line);
    }
    
    critical_section= 0;
    return 0;
}
#endif



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


void ProjBsp_CyclePrintError(char* errString)
{
    /* To be implemented */
    //ProjBsp_PrintError(); refer to polkAllplayer
    while((*errString)!='\0')
    {
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        USART_SendData(USART1, (*errString));
        errString++;
    }
}

void ProjBsp_JumpToBootloader(void)
{
    // fill in your project api. 
}


#ifdef PROJECT_SPEC_ASSERT_HANDLER
void ProjBsp_AssertHandler(char const * const file, int line)
{
#ifndef NDEBUG
    tIoeLedDevice *pIoeLedConfig = (tIoeLedDevice*) getDevicebyIdAndType(LED_DEV_ID, IO_EXPANDER_DEV_TYPE, NULL);
    cIoExpanderDrv ioeDrv = {0};

    //turn off all LED and auto blink
    if(((const tDevice*)pIoeLedConfig)->deviceSubType == AW9110B)
    {
        IoExpanderDrv_Ctor_aw9110b(&ioeDrv, pIoeLedConfig);
    }

    else if(((const tDevice*)pIoeLedConfig)->deviceSubType == AW9523)
    {
        IoExpanderDrv_Ctor_aw9523(&ioeDrv, pIoeLedConfig);
    }

    else if(((const tDevice*)pIoeLedConfig)->deviceSubType == AW9120)
    {
        IoExpanderDrv_Ctor_aw9120(&ioeDrv, pIoeLedConfig);
    }

    else
    {
        ASSERT(0);
    }

    const tIoExpanderLedMap *ledMap = pIoeLedConfig->pIoExpanderLedMap;
    int i;

    for(i = 0 ; i < pIoeLedConfig->ledNum ; i++)
    {
        if(((const tDevice*)pIoeLedConfig)->deviceSubType == AW9110B)
        {
            IoExpanderDrv_SetBrightness_aw9110b(&ioeDrv, ledMap[i].port, ledMap[i].pin, 0);
        }

        else if(((const tDevice*)pIoeLedConfig)->deviceSubType == AW9523)
        {
            IoExpanderDrv_SetBrightness_aw9523(&ioeDrv, ledMap[i].port, ledMap[i].pin, 0);
        }

        else if(((const tDevice*)pIoeLedConfig)->deviceSubType == AW9120)
        {
            IoExpanderDrv_SetBrightness_aw9120(&ioeDrv, ledMap[i].pin, 0);
        }

        else
        {
            ASSERT(0);
        }
    }

    if(((const tDevice*)pIoeLedConfig)->deviceSubType == AW9110B)
    {
        IoExpanderDrv_AutoBlink_aw9110b(&ioeDrv, IOE_AUTO_PATT_ERROR);
    }

    else if(((const tDevice*)pIoeLedConfig)->deviceSubType == AW9523)
    {
        //IoExpanderDrv_AutoBlink_aw9523(&ioeDrv, IOE_AUTO_PATT_ERROR);
        //9523 doesn't support AutoBlink now
    }

    else if(((const tDevice*)pIoeLedConfig)->deviceSubType == AW9120)
    {

        IoExpanderDrv_AutoBlink_aw9120(&ioeDrv, IOE_AUTO_PATT_ERROR);
    }

    else
    {
        ASSERT(0);
    }

#endif
}
#endif


void ProjBsp_SysClkUpdate()
{
#ifdef EXTERNAL_HIGH_SPEED_CLOCK
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
  
  /* Enable HSE */    
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);
   
  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }  

  if (HSEStatus == (uint32_t)0x01)
  {
    /* Enable Prefetch Buffer and set Flash Latency */
    FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;
 
    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
    /* PCLK = HCLK/4 */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE_DIV4;

    /* PLL configuration = HSE(12MHz) * 4 = 48 MHz */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));

    /* config sysclk = 32M & PCLK=8M 
	 * it let I2C2 clock = 8M, the same as I2C1, thus I2C1/I2C2 share the same clock setting
	 */
	RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_PREDIV1 | RCC_CFGR_PLLXTPRE_PREDIV1 | RCC_CFGR_PLLMULL8);
	RCC->CFGR2 |= (uint32_t)(RCC_CFGR2_PREDIV1_DIV3);

	/* config sysclk = 48M
	 * it cause I2C2 clock = 12M, when choose this seting, I2C2 timing must be updated.
	 */
    //RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_PREDIV1 | RCC_CFGR_PLLXTPRE_PREDIV1 | RCC_CFGR_PLLMULL4);

    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }

    /* Select PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_PLL)
    {
    }
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock 
         configuration. User can add here some code to deal with this error */
  }
#endif
}



/**************************************************/
/***************  interrupt funcs *********************/
/**************************************************/
/**
  * @brief  This function handles External line 0 interrupt request
  * @param  None
  * @retval None
  */
void EXTI0_1_IRQHandler(void)
{
    TP_PRINTF("EXTI0_1_IRQHandler\r\n");
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
    if(EXTI_GetITStatus(EXTI_Line1) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}

void EXTI2_3_IRQHandler(void)
{
    TP_PRINTF("EXTI2_3_IRQHandler\r\n");
    if(EXTI_GetITStatus(EXTI_Line2) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line2);
    }
    if(EXTI_GetITStatus(EXTI_Line3) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line3);
    }
}

#define MIN_WAKY_SIG_SEND_PERIOD_MS (500)

void EXTI4_15_IRQHandler(void)
{
    static uint32 lastWakySigSentTime = 0;
    //If set other interrupt between (EXTI_Line5~EXTI_Line15), remember to clear bit

    if (EXTI_GetITStatus(EXTI_Line12))
    {
        /*
            We do not clear line interrupt here as
            it will be cleaned in following IQS_Check_EXTI();
        */
        if (Setting_IsReady(SETID_SYSTEM_SLEEP) && *(bool*)Setting_Get(SETID_SYSTEM_SLEEP))
        {
            if ((getSysTime() - lastWakySigSentTime) > MIN_WAKY_SIG_SEND_PERIOD_MS)
            {
                PowerSrvWakeUpEvent *pe = Q_NEW(PowerSrvWakeUpEvent, POWER_WAKE_UP_SIG);
                /*
                    We use MAX_WAKE_UP_TYPE to indicate that this is just s waky candidate,
                    MainApp must start debouncing in order to confirm if to wake up or not
                */
                pe->powerSrvWakeUpType = MAX_WAKE_UP_TYPE;
                QF_PUBLISH(&pe->super, NULL);

                lastWakySigSentTime = getSysTime();
            }
        }
    }

    if (EXTI_GetITStatus(EXTI_Line14))
    {
        EXTI_ClearITPendingBit(EXTI_Line14);
        if (Setting_IsReady(SETID_SYSTEM_SLEEP) && *(bool*)Setting_Get(SETID_SYSTEM_SLEEP))
        {
            if ((getSysTime() - lastWakySigSentTime) > MIN_WAKY_SIG_SEND_PERIOD_MS)
            {
                PowerSrvWakeUpEvent *pe = Q_NEW(PowerSrvWakeUpEvent, POWER_WAKE_UP_SIG);
                pe->powerSrvWakeUpType = MAX_WAKE_UP_TYPE;
                QF_PUBLISH(&pe->super, NULL);
            }
        }
    }
    if (EXTI_GetITStatus(EXTI_Line15))
    {
        EXTI_ClearITPendingBit(EXTI_Line15);
        if (Setting_IsReady(SETID_SYSTEM_SLEEP) && *(bool*)Setting_Get(SETID_SYSTEM_SLEEP))
        {
            if ((getSysTime() - lastWakySigSentTime) > MIN_WAKY_SIG_SEND_PERIOD_MS)
            {
                PowerSrvWakeUpEvent *pe = Q_NEW(PowerSrvWakeUpEvent, POWER_WAKE_UP_SIG);
                pe->powerSrvWakeUpType = MAX_WAKE_UP_TYPE;
                QF_PUBLISH(&pe->super, NULL);
            }
        }
    }

#ifdef HAS_AZOTEQ_INTEG_TOUCHKEY_DRV
    IQS_Check_EXTI();
#endif
}


void SetEnableWakeupSources()
{
#if 0
    GPIO_InitTypeDef GPIO_InitStructure;
    /* Initial GPIO for AC interrupt as it's used for ADC in running time*/
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Initial audio detect interrupt */
    /* Initial and power key to PA2 pin*/
    
    PowerDrv_SetExtInterrupt(EXTI_PortSourceGPIOA, EXTI_PinSource2,
                   EXTI_Line2, EXTI_Trigger_Rising_Falling, EXTI2_3_IRQn, ENABLE);
#endif  

}

void SetDisableWakeupSources()
{

}

/**************************************************/
/************ un-unsed interrupt funcs ****************/
/* User need to remove or fill in the specific function          */
/* if the interrupt is used                                                */
/**************************************************/

//TODO: the following IRQ is copy from STM32F1, should different with STM32F0
void WWDG_IRQHandler(void)
{
    ASSERT(0);
}

void PVD_IRQHandler(void)
{
    ASSERT(0);
}

void TAMPER_IRQHandler(void)
{
    ASSERT(0);
}

void FLASH_IRQHandler(void)
{
    ASSERT(0);
}

void RCC_IRQHandler(void)
{
    ASSERT(0);
}

void EXTI1_IRQHandler(void)
{
    ASSERT(0);
}

void EXTI3_IRQHandler(void)
{
    ASSERT(0);
}

void DMA1_Channel1_IRQHandler(void)
{
    ASSERT(0);
}

void DMA1_Channel2_IRQHandler(void)
{
    ASSERT(0);
}

void DMA1_Channel3_IRQHandler(void)
{
    ASSERT(0);
}

void DMA1_Channel4_IRQHandler(void)
{
    ASSERT(0);
}

void DMA1_Channel5_IRQHandler(void)
{
    ASSERT(0);
}

void DMA1_Channel6_IRQHandler(void)
{
    ASSERT(0);
}

void DMA1_Channel7_IRQHandler(void)
{
    ASSERT(0);
}

void ADC1_2_IRQHandler(void)
{
    ASSERT(0);
}

void CAN1_TX_IRQHandler(void)
{
    ASSERT(0);
}

void CAN1_RX0_IRQHandler(void)
{
    ASSERT(0);
}

void CAN1_RX1_IRQHandler(void)
{
    ASSERT(0);
}

void CAN1_SCE_IRQHandler(void)
{
    ASSERT(0);
}

void TIM1_BRK_IRQHandler(void)
{
    ASSERT(0);
}

void TIM1_UP_IRQHandler(void)
{
    ASSERT(0);
}

void TIM1_TRG_COM_IRQHandler(void)
{
    ASSERT(0);
}

void TIM1_CC_IRQHandler(void)
{
    ASSERT(0);
}

void TIM2_IRQHandler(void)
{
    ASSERT(0);
}

void TIM3_IRQHandler(void)
{
    ASSERT(0);
}

void TIM4_IRQHandler(void)
{
    ASSERT(0);
}

void I2C1_EV_IRQHandler(void)
{
    ASSERT(0);
}

void I2C2_ER_IRQHandler(void)
{
    ASSERT(0);
}

void SPI1_IRQHandler(void)
{
    ASSERT(0);
}

void SPI2_IRQHandler(void)
{
    ASSERT(0);
}

void USART3_IRQHandler(void)
{
    ASSERT(0);
}

void EXTI15_10_IRQHandler(void)
{
    ASSERT(0);
}

void RTCAlarm_IRQHandler(void)
{
    ASSERT(0);
}

void OTG_FS_WKUP_IRQHandler(void)
{
    ASSERT(0);
}

void TIM5_IRQHandler(void)
{
    ASSERT(0);
}


void SPI3_IRQHandler(void)
{
    ASSERT(0);
}


void UART4_IRQHandler(void)
{
    ASSERT(0);
}


void UART5_IRQHandler(void)
{
    ASSERT(0);
}


void TIM6_IRQHandler(void)
{
    ASSERT(0);
}


void TIM7_IRQHandler(void)
{
    ASSERT(0);
}


void DMA2_Channel1_IRQHandler(void)
{
    ASSERT(0);
}


void DMA2_Channel2_IRQHandler(void)
{
    ASSERT(0);
}


void DMA2_Channel3_IRQHandler(void)
{
    ASSERT(0);
}


void DMA2_Channel4_IRQHandler(void)
{
    ASSERT(0);
}


void DMA2_Channel5_IRQHandler(void)
{
    ASSERT(0);
}


void ETH_IRQHandler(void)
{
    ASSERT(0);
}

void ETH_WKUP_IRQHandler(void)
{
    ASSERT(0);
}


void CAN2_TX_IRQHandler(void)
{
    ASSERT(0);
}


void CAN2_RX0_IRQHandler(void)
{
    ASSERT(0);
}


void CAN2_RX1_IRQHandler(void)
{
    ASSERT(0);
}


void CAN2_SCE_IRQHandler(void)
{
    ASSERT(0);
}


void OTG_FS_IRQHandler(void)
{
    ASSERT(0);
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

