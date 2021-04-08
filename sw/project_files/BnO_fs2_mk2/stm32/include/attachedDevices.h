/******************************************************************************

Copyright (c) 2015, Tymphany HK Ltd. All rights reserved.

Confidential information of Tymphany HK Ltd.

The Software is provided "AS IS" and "WITH ALL FAULTS," without warranty of any
kind, including without limitation the warranties of merchantability, fitness
for a particular purpose and non-infringement. Tymphany HK LTD. makes no
warranty that the Software is free of defects or is suitable for any particular
purpose.

******************************************************************************/
/**
* @file attachedDevices.h
* @brief The devices attached to the product.
* @author Christopher Alexander
* @date 26-Nov-2013
* @copyright Tymphany Ltd.
*/

/* TODO: Auto generate this data */
#ifndef ATTACHEDDEVICES_H
#define ATTACHEDDEVICES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "deviceTypes.h"
#include "stm32f0xx_exti.h" //EXTI_LineXX


/*******************************************************************************
 *  General settings configs go here
 ******************************************************************************/

/*******************************************************************************
 *  Pin definition
 ******************************************************************************/
/* GPIO pin definition
 */

typedef enum {
    //Power
    GPIO_IN_DC_IN=GPIO_0,  //PC6, SMPS 15V DC input detection (FS1:PB15, FS2:PC6)
    GPIO_OUT_SYSPWR_ON,    //PB0, System power control (DCDC, AMP power)
    GPIO_OUT_BOOST_CAP,    //PB1
    
    //GPIO Key
    GPIO_IN_POWER_KEY,     //PC14, Power key

    //Rotary Volume
    GPIO_IN_ROTARY_VOL_A,  //Rotary volume A
    GPIO_IN_ROTARY_VOL_B,  //Rotary volume B

    //Audio
    GPIO_OUT_AMP_SDZ,      //PB13
    GPIO_IN_AMP_FAULTZ,    //PB14, High:normal operation, Low:fault condition(Over-temp, DC Detect)
    GPIO_OUT_DSP_RST_N,    //PB2
    GPIO_IN_DSP_TUNE,      //PA15, DSP get in EQ tuning mode, MCU need disable DSP IIC

    //ASE
    GPIO_OUT_BOOT_STATUS_SW, //PB8, Notify ASE-TK MCU is on Bootlaoder(high) or application(low)
    GPIO_OUT_ASE_RST_N,      //PA11, ASE-TK Reset N
    GPIO_OUT_ASE_SYS_EN,     //PA12, ASE-TK System Enable
    GPIO_IN_ASE_REQ_PDN,     //PA8,  ASE-TK require Power Down
    //GPIO_OUT_ASE_5V_EN,

    //Touch
    GPIO_OUT_TCH_360_RST,  //PC10
    GPIO_IN_TCH_360_RDY,   //PC11
    GPIO_IN_TCH_572_RDY,   //PC12

    //IO Expender
    GPIO_OUT_IOEXP_RST,    //PB12

    //AUX-IN
    GPIO_IN_AUX_IN_WAKEUP, //PC5, AUX-IN Wakeup, only for FS2

    //Touch
    GPIO_OUT_TCH_572_RST,  //PC8
    GPIO_OUT_TCH_POWER,    //PC9
}eGpioPinDef;


/* EXTI definition
 */
#define EXTI_LINE_TCH_360_RDY  EXTI_Line11  //PC11
#define EXTI_LINE_TCH_572_RDY  EXTI_Line12  //PC12



/* ADC pin definition
 *
 * Note Key order must be equal to channel order, for exmaple
 *     PC0 (ADC_IN0)  ==> assign to ADC_PIN0
 *     PA1 (ADC_IN1)  ==> assign to ADC_PIN1
 *     PC4 (ADC_IN14) ==> assign to ADC_PIN14
 *
 * The wrong example is
 *     PC0 (ADC_IN0)  ==> assign to ADC_PIN3
 *     PA1 (ADC_IN1)  ==> assign to ADC_PIN2
 *     PC4 (ADC_IN14) ==> assign to ADC_PIN1
 *  
 * The easist way is assign ADC_INx to ADC_PINx
 */
//Common
#define ADC_FS_KEY           ADC_PIN1   //PA1, ADC_IN1,  ADC key for SoftAP and Power
#define ADC_HW_VER           ADC_PIN14  //PC4, ADC_IN14, hardware version

//FS2 only
#define ADC_NTC_WF_AMP_1     ADC_PIN5   //PA5, ADC_IN5, NTC for Woofer amplifier 1 (RT2)
#define ADC_NTC_WF_AMP_2     ADC_PIN6   //PA6, ADC_IN6,  NTC for woofer amplifier 2 (RT3)
#define ADC_NTC_WF_SPK       ADC_PIN11  //PC1, ADC_IN11, NTC for Woofer speaker
#define ADC_NTC_MID_SPK_A    ADC_PIN12  //PC2, ADC_IN12, NTC for middle speaker A
#define ADC_NTC_MID_SPK_B    ADC_PIN13  //PC3, ADC_IN13, NTC for middle speaker B
#define ADC_NTC_TW_AMP       ADC_PIN10  //PC0, ADC_IN10, NTC for twetter amplifier



/*******************************************************************************
 *  ADC configs go here
 ******************************************************************************/
/* For ADC PIN */
#define NUM_OF_ADC_KEY_PIN           1  //= ArraySize(attrADCPinsForKey) of attachedDevices.c
#define NUM_OF_ADC_POWER_PIN         0 //= ArraySize(attrAdcPowerPins) of attachedDevices.c
#define NUM_OF_ADC_AUDIO_PIN         7 //= ArraySize(attrAdcPowerPins) of attachedDevices.c
#define NUM_OF_ALL_ENABLED_ADC_PIN   (NUM_OF_ADC_KEY_PIN + NUM_OF_ADC_POWER_PIN + NUM_OF_ADC_AUDIO_PIN) /* Array length of attrAdcPowerPins[] on attachedDevices.c */


/*******************************************************************************
 *  I2C config
 ******************************************************************************/
#define I2C_CLK_KHZ (400)


/*******************************************************************************
 *  Key configs go here
 ******************************************************************************/
/* Number of key event (1 adc pin may have multiple events) */
#define NUM_OF_GPIO_KEY_PIN          1  //= ArraySize(gpioKeyPins) of attachedDevices.c
#define NUM_OF_ADC_KEY               2  /* 2 key event on 1 ADC pin (Power, SoftAP), = ArraySize(adcKey) of attachedDevices.c */
#define NUM_OF_GPIO_KEY              1  /* 1 key event on 1 GPIO pin (Factory reset), = ArraySize(gpioKeyPins) of attachedDevices.c */
#define NUM_OF_AZ_INTEG_TOUCH_KEY    1  /* Although touch key have many event, config layer think it have only 1 event. = ArraySize(azIntegTOuchKeyPins) of attachedDevices.c */
#define NUM_OF_ROTATER_PAIR          1  /* 1 Rotater contain both Clock-wise and Counter-Clock-wise events */

#define NUM_OF_ALL_KEY               (NUM_OF_ADC_KEY + NUM_OF_GPIO_KEY + NUM_OF_AZ_INTEG_TOUCH_KEY + NUM_OF_ROTATER_PAIR)  // KeySrv: me->pConfig->keyboardSet[NUM_OF_ALL_KEY];

#define KEYBOARD_NUMBER              4  /* 4 keyboard include ADC + GPIO + TOUCH + ROTATER, == ArraySize(keyboardSet) */

#define IO_EXPANDER_NUM              (1)


/*******************************************************************************
 *  Led configs go here
 ******************************************************************************/
typedef enum
{
    LED_MIN = 0,
    LED1_CONN_RED = LED_MIN,
    LED2_CONN_ORG, 
    LED3_CONN_BLUE,
    LED4_CONN_WHT, 
    LED5_PROD_WHT,
    LED6_PROD_WHT, 
    LED7_PROD_ORG, 
    LED8_PROD_WHT, 
    LED9_PROD_ORG, 
    LED10_GOOGLE_YEL,
    LED11_PROD_ORG,
    LED12_PROD_WHT,
    LED13_PROD_ORG,
    LED14_PROD_WHT,
    LED15_ALWAYS_ON_WHT,
    LED16_GOOGLE_GRE,
    LED17_GOOGLE_BLUE,
    LED18_GOOGLE_RED,
    LED19_PROD_ORG,
    LED20_PROD_WHT,
    LED_MAX
}eLed;

#define NUM_OF_IOEXPANDER_LED  LED_MAX


/*******************************************************************************
 *  OTHER CONFIGS
 ******************************************************************************/
extern const tDevice * const devices[];
extern const uint8 NUM_OF_ATTACHED_DEVICES;

//#define HW_I2C_DRV_LIB

/**
* UART Driver hardware pin initialization
* @param[in]    id              UART Driver ID (ref: enum eUartDevice)
*/
void UartDrv_Init(eTpUartDevice id);

/**
* UART Driver hardware pin de-initialization
* @param[in]    id              UART Driver ID (ref: enum eUartDevice)
*/
void UartDrv_Deinit(eTpUartDevice id);

/*
* I2C GPIO configuration (calls by I2CDrv_Ctor() from I2CDrv.c)
* init SDL and SDA pins according project
*/

void I2C1_LowLevel_Init(void);
void I2C1_LowLevel_ForcePullLow(void);
void I2C2_LowLevel_Init(void);


#ifdef __cplusplus
}
#endif

#endif /* ATTACHEDDEVICES_H */