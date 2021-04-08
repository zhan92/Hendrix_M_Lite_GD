/*
-------------------------------------------------------------------------------
TYMPHANY LTD





                  Power Driver
                  -------------------------

                  SW Module Document




@file        PowerDrv.c
@brief       It's the power driver for STM32F0xx, used in iHome
@author      Johnny Fan 
@date        2014-06-06
@copyright (c) Tymphany Ltd. All rights reserved.

Change History:
VERSION    : 1    DRAFT      2014-06-06     Johnny Fan 
DESCRIPTION: First Draft. Generated by newclass.py
SCO/ERROR  : 
-------------------------------------------------------------------------------
*/

#include "PowerDrv_priv.h"
#include "trace.h"
#include "BluetoothDrv.h"

/*GPIO object*/
static cGpioDrv gpioDrv;

/* Private functions / variables. Declare and drivers here */
static tpowerUpSequence powerUpSequence[] =
{
    {&PowerDrv_PowerUpStage1, GET_TICKS_IN_MS(100)},
    {&PowerDrv_PowerUpStage2, GET_TICKS_IN_MS(100)},
    {&PowerDrv_PowerUpStage3, GET_TICKS_IN_MS(100)},
    {&PowerDrv_PowerUpStage4, GET_TICKS_IN_MS(100)},
};

static QActive* pIntEvtRequestor;
static bool isIntEvtRequest;

/* The ADC config is not in devices[], so can't use getDeviceId for now, use extern for quick test*/
cADCDrv adcDrv;

/*****************************************************************************************************************
 *
 * Start-up / shut-down functions
 *
 *****************************************************************************************************************/
void PowerDrv_Ctor(cPowerDrv *me)
{
    /* Fill me in! */
    me->step = INITIAL_STEP;
 #ifdef EXTERNAL_BATTERY
    me->ejectBatteryStep = INITIAL_STEP;
#endif
    /* ADC for reading battery and AC input status*/
    uint16 attached_device_index = 0;
    tADCDevice *pPowerAdcConf;
    pPowerAdcConf = (tADCDevice*)getDevicebyId(POWER_DEV_ID,&attached_device_index);
    tGPIODevice *pPowerGPIOConf;
    attached_device_index++;
    pPowerGPIOConf = (tGPIODevice*)getDevicebyId(POWER_DEV_ID,&attached_device_index);
    ADCDrv_Ctor(&adcDrv, pPowerAdcConf);
    ADCDrv_StartScanning(&adcDrv);
    GpioDrv_Ctor(&gpioDrv,pPowerGPIOConf);
}

void PowerDrv_Xtor(cPowerDrv *me)
{
    /* Fill me in! */
    me->step = INITIAL_STEP;
#ifdef EXTERNAL_BATTERY
    me->ejectBatteryStep = INITIAL_STEP;
#endif       
    ADCDrv_Xtor(&adcDrv);
    PowerDrv_DeinitialPower(me);

}
/*****************************************************************************************************************
 *
 * Public interrupt handle function
 *
 *****************************************************************************************************************/
 #ifdef HAS_BATTERY
void  PowerDrv_HandleDcDetInterrupt()
{ /* send the event to power server */  
    PowerDrv_PushIntEvtToServer(AC_IN_INTERRUPT_SIG);
    /* disable the interrupt request, unless power server turn it on again*/
    PowerDrv_UnRegisterIntEvent();   
    TP_PRINTF("interrupt happen in driver layer\r\n");
}
#ifdef EXTERNAL_BATTERY
void  PowerDrv_HandleExtBattDetInterrupt()
{ /* send the event to power server */  
    PowerDrv_PushIntEvtToServer(EXT_BATTERY_IN_INTERRUPT_SIG);
    /* disable the interrupt request, unless power server turn it on again*/
    PowerDrv_UnRegisterIntEvent();   
    TP_PRINTF("interrupt happen in driver layer\r\n");
}
#endif
#endif

void EXTI0_1_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line1) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line1);
#ifdef EXTERNAL_BATTERY
        PowerDrv_HandleExtBattDetInterrupt();
#endif
    }
}
/*****************************************************************************************************************
 *
 * Public functions
 *
 *****************************************************************************************************************/
uint16  PowerDrv_InitialPower(cPowerDrv *me)
{
    /*need to change uint16 to int32, but keep it for now as it's used in other project*/
    uint16 delayTime;
    if(me->step < Q_DIM(powerUpSequence))
    {
        /* call the power up function according to step*/
        powerUpSequence[me->step].powerUpFunction(me); 
        /* get delay time*/
        delayTime = powerUpSequence[me->step].delaytime;
        me->step ++;
        
    }
    else
    { /* if reset is completed, return 0 */
        delayTime = 0;
        me->step = 0;
    } 
    return delayTime;
}
 
void  PowerDrv_DeinitialPower(cPowerDrv *me)
{ 
    /* Reset DSP, BT and IO EXPANDER*/
    BT_DSP_EXPANDER_RST_ON(gpioDrv);   
    /*turn off system pwr*/
    SYS_PWR_DISABLE(gpioDrv);          
    /* Turn off soleniod*/
    SOL_OFF(gpioDrv);
    SOL_5V_DISABLE(gpioDrv);
    /*Turn off power bank, amplifier power*/ 
    EXT_5V_DISABLE(gpioDrv);
    AMP_9V_DISABLE(gpioDrv);       
#ifdef HAS_BATTERY
    /* disable input source*/
    PowerDrv_SetInputSource(me, SET_DC_INPUT);    
    /* use internal batter for testing now*/
    PowerDrv_SetBatterySource(me, SET_INT_BATTERY);
#endif
}

#ifdef HAS_BATTERY
#ifdef EXTERNAL_BATTERY
int32 PowerDrv_EjectExtBattery(cPowerDrv *me)
{
    int32 delayTime;
    if(me->ejectBatteryStep == INITIAL_STEP)
    { /* 1st step, turn on power for Solenoid*/      
        SOL_5V_ENABLE(gpioDrv);   
        delayTime = EJECT_BATTERY_STEP1_TIME_MS;  
        me->ejectBatteryStep ++;
             
    }
    else if (me->ejectBatteryStep == 1)
    { /* 2nd step, turn on the FET to enable power to Solenoid*/  
        SOL_ON(gpioDrv);
        delayTime = EJECT_BATTERY_STEP2_TIME_MS;  
        me->ejectBatteryStep ++;
    }
    else
    { /* eject process is completed, turn off Solenoid and return 0 time*/     
        SOL_5V_DISABLE(gpioDrv);
        SOL_OFF(gpioDrv);
        delayTime = 0;
        me->ejectBatteryStep = INITIAL_STEP;
    } 
    return delayTime;
}
#endif //EXTERNAL_BATTERY

void  PowerDrv_RegisterIntEvent(QActive* pRequestor)
{    
    pIntEvtRequestor = pRequestor;
    isIntEvtRequest = TRUE;
}
void  PowerDrv_UnRegisterIntEvent()
{    
    pIntEvtRequestor = NULL;
    isIntEvtRequest = FALSE;
}

void  PowerDrv_GetInputSourceState(cPowerDrv *me, tInputSourceState* inputSourceState)
{
    if (adcDrv.isCreated)
    {
        int16 acRawValue = (int16)ADCDrv_GetData(&adcDrv, adcDrv.ADCConfig->pAdcPinIoAttr[1].adcPin);
        if(ADC_DATA_IS_NOT_READY!=acRawValue)
        {
            inputSourceState->dcPlugInVoltage = DC_ADC_TO_mVOLT(acRawValue);
            if( (inputSourceState->dcPlugInVoltage < CHARGER_MAX_mVOLT) &&
                (inputSourceState->dcPlugInVoltage > CHARGER_MIN_mVOLT))
            {
                inputSourceState->isDcPlugIn = TRUE;
            }
            else
            {
                inputSourceState->isDcPlugIn = FALSE;
            }
        }
        ADCDrv_StartScanning(&adcDrv);
    }
    inputSourceState->isUsbPlugIn = IS_MINI_U_IN(gpioDrv);
    inputSourceState->isChargingDone = IS_CHAR_DONE(gpioDrv);   
    inputSourceState->isDcPlugInDetectByGPIO = IS_DC_IN(gpioDrv);
    inputSourceState->isExtBattDetectByGPIO = IS_EXT_BAT_IN(gpioDrv);
}

bool  PowerDrv_GetBatteryVol(cPowerDrv *me, tBatteryVol* batteryVol)
{
    bool ret = FALSE;
    int16 intBattAdcValue1 = 0;
    int16 extBattAdcValue1 = 0;
    if (adcDrv.isCreated)
    {    
        intBattAdcValue1 = (int16)ADCDrv_GetData(&adcDrv, adcDrv.ADCConfig->pAdcPinIoAttr[0].adcPin);        
        extBattAdcValue1 = (int16)ADCDrv_GetData(&adcDrv, adcDrv.ADCConfig->pAdcPinIoAttr[2].adcPin);        
        if((ADC_DATA_IS_NOT_READY != intBattAdcValue1)&&(ADC_DATA_IS_NOT_READY != extBattAdcValue1))
        {
            batteryVol->intBatteryVol = BATT_ADC_TO_mVOLT(intBattAdcValue1);
            batteryVol->extBatteryVol = BATT_ADC_TO_mVOLT(extBattAdcValue1);
            ret = TRUE;
        }
        ADCDrv_StartScanning(&adcDrv);
    }  
    return ret;
}

void  PowerDrv_SetInputSource(cPowerDrv *me, eInputSource inputSource)
{
    switch(inputSource)
    {
        case SET_DC_INPUT:
        {
            SEL_DC_INPUT(gpioDrv);
            USB_5V_DISABLE(gpioDrv);
            break;
        }        
        case SET_USB_INPUT:
        {   /* Micro USB charges internal battery: DC9V/Mic5V_CTL=Low, Mini_5V_CTL=Low */
            USB_5V_ENABLE(gpioDrv);
            SEL_USB_INPUT(gpioDrv);  
            break;
        }        
        case SET_USB_NONE_INPUT:
        {   /* Micro USB charges external battery: DC9V/Mic5V_CTL=Low, Mini_5V_CTL=High */
            USB_5V_DISABLE(gpioDrv);
            SEL_USB_INPUT(gpioDrv);
            break;
        }  
        case SET_NONE_INPUT:
        {
            SEL_DC_INPUT(gpioDrv);
            USB_5V_DISABLE(gpioDrv);
            break;
        }
        default:
        {
            ASSERT(0);
        }
    }    
}

void  PowerDrv_SetBatterySource(cPowerDrv *me, eBatterySource batterySource)
{
    switch(batterySource)
    {
        case SET_INT_BATTERY:
        {
            INT_BAT_ENABLE(gpioDrv);
            EXT_BAT_DISABLE(gpioDrv);
            break;
        }        
        case SET_EXT_BATTERY:
        {
            EXT_BAT_ENABLE(gpioDrv);
            INT_BAT_DISABLE(gpioDrv);
            break;
        }        
        case SET_NO_BATTERY:
        {
            INT_BAT_DISABLE(gpioDrv);
            EXT_BAT_DISABLE(gpioDrv);
            break;
        }
        default:
        {
            ASSERT(0);
        }
    }
    
}

void  PowerDrv_SetExtraCommand(cPowerDrv *me, eExtraCommand extraCommand)
{
    switch(extraCommand)
    {
        case ENABLE_TPS54328_CMD:
        {
            EXT_5V_ENABLE(gpioDrv);
            break;
        }        
        case DISABLE_TPS54328_CMD:
        {
            EXT_5V_DISABLE(gpioDrv);
            break;
        }        
        case ENABLE_SOL_CMD:
        { // be care that it will affect the external battery ejection            
            SOL_5V_ENABLE(gpioDrv);  
            break;
        }
        case DISABLE_SOL_CMD:
        { // be care that it will affect the external battery ejection            
            SOL_5V_DISABLE(gpioDrv);
            break;
        }
        case SET_A_5V_CTL_CMD:
        {
            SET_A_5V_CTL_PIN(gpioDrv);
            break;
        }
        case CLR_A_5V_CTL_CMD:
        {
            CLR_A_5V_CTL_PIN(gpioDrv);
            break;
        }
        default:
        {
            ASSERT(0);
        }
    }
}


void PowerDrv_PowerSaveSleep()
{    
   /**
     * Reset ADC conversion status for the case when adc conversion is on the fly
     * while system goes to deep sleep mode.
     */
    TP_PRINTF("int enable < sleep\r\n");
    ADCDrv_ResetAdcConversionStatus(&adcDrv);
    /* set the IO to low to save power*/
    PowerDrv_SetBatterySource(NULL, SET_INT_BATTERY);
    PowerDrv_SetInputSource(NULL, SET_USB_INPUT);
    PowerDrv_SetExtraCommand(NULL, DISABLE_TPS54328_CMD);
    CLR_A_5V_CTL_PIN(gpioDrv);
    PowerDrv_DisableSystemTimerInt();
    PowerDrv_EnablelWakeUpSources();
    if(IS_DC_IN(gpioDrv))
    { // before entering sleep, if AC already in, asser for debug purpose
        ASSERT(0);
    }
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
    PowerDrv_EnableSystemTimerInt();    
}
#endif // HAS_BATTERY
/*****************************************************************************************************************
 *
 * private functions
 *
 *****************************************************************************************************************/
static void PowerDrv_PowerUpStage1(cPowerDrv *me)
{ 
    
    /*reset BT, DSP, IO Expander*/
    BT_DSP_EXPANDER_RST_ON(gpioDrv);    
    SYS_PWR_DISABLE(gpioDrv); 
    /* need to check the input source status, and decide which source to use*/   
    /* but for now, just enable internal battery for testing*/     
#ifdef HAS_BATTERY
    PowerDrv_SetInputSource(me, SET_DC_INPUT);    
    PowerDrv_SetBatterySource(me, SET_INT_BATTERY);
#endif
}

 static void PowerDrv_PowerUpStage2(cPowerDrv *me)
{   
    /*enable system power*/
    SYS_PWR_ENABLE(gpioDrv); 
           
}

static void PowerDrv_PowerUpStage3(cPowerDrv *me)
{  
    /*enable amp power*/
    AMP_9V_ENABLE(gpioDrv);
    /*set A_5V_CTL low*/
    CLR_A_5V_CTL_PIN(gpioDrv);
    
}

static void PowerDrv_PowerUpStage4(cPowerDrv *me)
{
    /*release the reset pin*/
    BT_DSP_EXPANDER_RST_OFF(gpioDrv);    
}

#ifdef HAS_BATTERY
static void PowerDrv_PushIntEvtToServer(eSignal signal)
{
    if(isIntEvtRequest != TRUE)
    {
        return;
    }
    if(pIntEvtRequestor!= NULL)
    {
        QEvt* pEvt = Q_NEW(QEvt, signal);
        QACTIVE_POST(pIntEvtRequestor,(QEvt*)pEvt,0);
    }
}

static void PowerDrv_EnableExtInterrupt(uint8 port, uint8 pin, uint32 extiLine, 
                                                  EXTITrigger_TypeDef type, IRQn_Type irq)
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
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set EXTI Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = irq;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void PowerDrv_EnablelWakeUpSources()
{
    PowerDrv_DisableEXTI_Config();
    
    GPIO_InitTypeDef GPIO_InitStructure;
    /* Initial GPIO for AC interrupt as it's used for ADC in running time*/
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Initial and Enable power Key interrupt */
    PowerDrv_EnableExtInterrupt(EXTI_PortSourceGPIOA, EXTI_PinSource2, 
               EXTI_Line2, EXTI_Trigger_Rising_Falling, EXTI2_3_IRQn);
    
    /* Initial and Enable Eject Key interrupt */
    PowerDrv_EnableExtInterrupt(EXTI_PortSourceGPIOA, EXTI_PinSource3, 
               EXTI_Line3, EXTI_Trigger_Rising_Falling, EXTI2_3_IRQn);

    /* Initial and Enable NFC tag interrupt */    
    PowerDrv_EnableExtInterrupt(EXTI_PortSourceGPIOA, EXTI_PinSource7, 
                   EXTI_Line7, EXTI_Trigger_Rising_Falling, EXTI4_15_IRQn);

    /* Initial and Enable AC interrupt to PA6 pin*/
    PowerDrv_EnableExtInterrupt(EXTI_PortSourceGPIOA, EXTI_PinSource6, 
                   EXTI_Line6, EXTI_Trigger_Rising, EXTI4_15_IRQn);
    
    /* Initial and Enable Micro USB interrupt, Connect EXTI 15 Line to PA15 pin */
    PowerDrv_EnableExtInterrupt(EXTI_PortSourceGPIOA, EXTI_PinSource15, 
                   EXTI_Line15, EXTI_Trigger_Rising, EXTI4_15_IRQn);    
  
    /* Initial and Enable EXT_BAT_INT interrupt, Connect EXTI 1 Line to PB1 pin */
     PowerDrv_EnableExtInterrupt(EXTI_PortSourceGPIOB, EXTI_PinSource1, 
                   EXTI_Line1, EXTI_Trigger_Rising, EXTI0_1_IRQn);    

}

static void PowerDrv_DisableEXTI_Config()
{
  EXTI_InitTypeDef   EXTI_InitStructure;
 
  EXTI_InitStructure.EXTI_Line = EXTI_Line1 | EXTI_Line2 | EXTI_Line3 | EXTI_Line4 |
                                 EXTI_Line5 | EXTI_Line6 | EXTI_Line7 | EXTI_Line8 |
                                 EXTI_Line9 | EXTI_Line10 | EXTI_Line11 | EXTI_Line12 |
                                 EXTI_Line13 | EXTI_Line14 | EXTI_Line15 | EXTI_Line16 |  
                                 EXTI_Line17 | EXTI_Line19 | EXTI_Line21 | EXTI_Line22 |  
                                 EXTI_Line23 | EXTI_Line27 ;       
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
  EXTI_Init(&EXTI_InitStructure); 
}

static void PowerDrv_DisableSystemTimerInt()
{
    uint32 tmp = SysTick->CTRL;
    tmp = tmp & ~SysTick_CTRL_TICKINT_Msk;
    tmp = tmp & ~SysTick_CTRL_ENABLE_Msk;
    SysTick->CTRL  = tmp;  
}

static void PowerDrv_EnableSystemTimerInt()
{
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Msk   |
                   SysTick_CTRL_ENABLE_Msk;                    /* Enable SysTick IRQ and SysTick Timer */
}
#endif //HAS_BATTERY


