/**
* @file attachedDevices.c
* @brief The devices attached to the product.
* @author Christopher Alexander, Wesley Lee
* @date 26-Nov-2013
* @copyright Tymphany Ltd.
*/

#include "bsp.h"
#include "attachedDevices.h"
#include "UartDrv.h"
#include "AdcDrv.h"
#include "LedDrv.h"
#include "GpioDrv.h"
#include "DspDrv.h"
#include "trace.h"

#include "product.config"

/* TODO: Fill this shit in */

static const tPwmLedMap pwmLedMap[] =
{
                /* GPIO Num, Output Compare Reg Num, port num & bit num */
    {LED_RED,   COLOR_COMPONENT_RED,    {49, 2, 1, OCREG_1}},
    {LED_GREEN, COLOR_COMPONENT_GREEN,  {52, 2, 4, OCREG_5}},
    {LED_BLUE,  COLOR_COMPONENT_BLUE,   {53, 2, 5, OCREG_2}},
};

static const tPwmLedDevice ledConfig = {
    .deviceInfo.deviceID    = LED_DEV_ID,
    .deviceInfo.deviceType  = PWM_DEV_TYPE,
    .ledNum     = NUM_OF_PWM_LED,
    .pPwmLedMap = pwmLedMap,
};

/* Do not define it as const since the callback function will be set on the fly */
tI2CDevice polk_allplay_DSP =
{
    .deviceInfo.deviceID = DSP_DEV_ID,
    .deviceInfo.deviceType = I2C_DEV_TYPE,
    .i2cMode = I2C_MASTER_MODE,
    .channel = I2C_CHANNEL_ONE,
    .pvSlaveCallback = NULL,
    .address = 0x30, /* device address */
    .regAddrLen = REG_LEN_8BITS,
    .baudRate = 45,
};

static const tUARTDevice UartAllPlayConfig = {
    .deviceInfo.deviceID    = ALLPLAY_DEV_ID,
    .deviceInfo.deviceType  = UART_DEV_TYPE,
    .baudrate   = 115200,
    .uartId     = TP_UART_DEV_1,
    .byteSize   = TP_UART_BYTE_SIZE_8_BITS,
    .parity     = TP_UART_PARITY_NONE,
    .stopBits   = TP_UART_STOP_BIT_1,
    .dmaEnable  = FALSE,
    .interrupt.priority    = INT_PRIORITY_LEVEL_5,
    .interrupt.subPriority = INT_SUB_PRIORITY_LEVEL_1,
    .bBatchProcessInIRQ    = 1
};

#if NUM_OF_UART >= 2
static const tUARTDevice UartDebugConfig = {
    .deviceInfo.deviceID    = DEBUG_DEV_ID,
    .deviceInfo.deviceType  = UART_DEV_TYPE,
    .baudrate   = 115200,
    .uartId     = TP_UART_DEV_2,
    .byteSize   = TP_UART_BYTE_SIZE_8_BITS,
    .parity     = TP_UART_PARITY_NONE,
    .stopBits   = TP_UART_STOP_BIT_1,
    .dmaEnable  = FALSE,
    .interrupt.priority    = INT_PRIORITY_LEVEL_5,
    .interrupt.subPriority = INT_SUB_PRIORITY_LEVEL_0,
    .bBatchProcessInIRQ    = 0
};
#endif  // NUM_OF_UART >= 2

static const tGPIOPin gpioPins[] =
{
  {GPIO_0,   IO_PORT_D,    IO_BIT_0,    GPIO_DIGITAL_INPUT,    GPIO_ACTIVE_HIGH,  ODC_NOT_APPLICABLE},
  {GPIO_1,   IO_PORT_B,    IO_BIT_7,    GPIO_DIGITAL_INPUT,    GPIO_ACTIVE_LOW,   ODC_NOT_APPLICABLE},
  {GPIO_2,   IO_PORT_G,    IO_BIT_8,    GPIO_DIGITAL_INPUT,    GPIO_ACTIVE_LOW,   ODC_NOT_APPLICABLE}
};

static const tGPIODevice GPIOConfigForKey =
{
  .usedGPIOPinNum =  NUM_OF_GPIO_KEY_PIN,
  .pGPIOPinSet = gpioPins
};

static const tAdcPinIoAttr attrOfKeyAdcPins[] =
{
    {.adcPin = ADC_PIN25},
    {.adcPin = ADC_PIN26}
};
static const tADCDevice ADCConfigForKey =
{
    .ADCEnabledPinNum = NUM_OF_ADC_KEY_PIN,
    .pAdcPinIoAttr = attrOfKeyAdcPins
};

static const tAdcPinIoAttr attrOfPowerAdcPins[] =
{
    {.adcPin = ADC_PIN2},
    {.adcPin = ADC_PIN3}
};
static const tADCDevice ADCConfigForPower =
{
    .deviceInfo.deviceID    = POWER_DEV_ID,
    .deviceInfo.deviceType  = ADC_DEV_TYPE,
    .ADCEnabledPinNum = NUM_OF_ADC_POWER_PIN,
    .pAdcPinIoAttr = attrOfPowerAdcPins
};

/**
 * keyboard object setting
 * The ADC threshold defined in our document is not correct(theoretical value),
 * It has too large difference with the real value. Below is the theoretical
 * adc threshold table and the real threshold  table
 * Theoretical threshold table (defined in our project specification):
        key_id       key_ADC_channel   adc_max_value  adc_max_value
        {VOLUME_DOWN_KEY,   ADC_CHANNEL0,   {(512-15),  (512+15)}},
        {VOLUME_UP_KEY,     ADC_CHANNEL0,   {(410-13),  (410+13)}},
        {VOL_UP_DOWN_KEY,   ADC_CHANNEL0,   {(640-18),  (640+18)}},
        {NEXT_PREV_KEY,     ADC_CHANNEL1,   {(600-17),  (600+17)}},
        {NEXT_KEY,          ADC_CHANNEL1,   {(300-10),  (300+10)}},
        {PLAY_PAUSE_KEY,    ADC_CHANNEL1,   {(410-13),  (408+13)}},
        {PREV_KEY,          ADC_CHANNEL1,   {(512-15),  (512+15)}}
 * The real threshold table is based on Ruben's idea, the adc range is enlarged
 * to handle special case such as hardware bugs. Apparently, the range is
 * continuous, this enables the software to, at least, "do something" if there
 * is hardware defeat which lead to an uncommon adc value. It is difficult
 * to detect the bug if the software just ignore this uncommon adc value.
 */
#define ADC_THRESHOLD_LEVEL_1   248
#define ADC_THRESHOLD_LEVEL_2   351
#define ADC_THRESHOLD_LEVEL_3   465
#define ADC_THRESHOLD_LEVEL_4   562
#define ADC_THRESHOLD_LEVEL_5   688
#define ADC_THRESHOLD_RANGE_1   ADC_THRESHOLD_LEVEL_1,ADC_THRESHOLD_LEVEL_2
#define ADC_THRESHOLD_RANGE_2   ADC_THRESHOLD_LEVEL_2,ADC_THRESHOLD_LEVEL_3
#define ADC_THRESHOLD_RANGE_3   ADC_THRESHOLD_LEVEL_3,ADC_THRESHOLD_LEVEL_4
#define ADC_THRESHOLD_RANGE_4   ADC_THRESHOLD_LEVEL_4,ADC_THRESHOLD_LEVEL_5\

static const tAdcKey adcKeys[]=
{   /* key_id         key_ADC_channel   adc threshold */
    {VOLUME_DOWN_KEY,   ADC_PIN25,   ADC_THRESHOLD_RANGE_3},
    {VOLUME_UP_KEY,     ADC_PIN25,   ADC_THRESHOLD_RANGE_2},
    {VOL_UP_DOWN_KEY,   ADC_PIN25,   ADC_THRESHOLD_RANGE_4},
    {NEXT_PREV_KEY,     ADC_PIN26,   ADC_THRESHOLD_RANGE_4},
    {NEXT_KEY,          ADC_PIN26,   ADC_THRESHOLD_RANGE_1},
    {PLAY_PAUSE_KEY,    ADC_PIN26,   ADC_THRESHOLD_RANGE_2},
    {PREV_KEY,          ADC_PIN26,   ADC_THRESHOLD_RANGE_3}
};
static tAdcKeyboardDevice adcKeyboardConfig =
{
    .adcKeyboard.deviceInfo.deviceID = KEYBOARD_DEV_ID,
    .adcKeyboard.deviceInfo.deviceType = ADC_DEV_TYPE,
    .adcKeyboard.keyNum = NUM_OF_ADC_KEY,
    .attachedDeviceObjConfig = &ADCConfigForKey,
    .pAdcKeySet = adcKeys
};


static const tGpioKey gpioKeys[] =
{   /* key_id           */
    {POWER_KEY,         GPIO_0},
    {DIRECT_MODE_KEY,   GPIO_1},
    {NET_RESET_KEY,     GPIO_2}
};
static tGpioKeyboardDevice gpioKeyboardConfig =
{
    .gpioKeyboard.deviceInfo.deviceID = KEYBOARD_DEV_ID,
    .gpioKeyboard.deviceInfo.deviceType = GPIO_DEV_TYPE,
    .gpioKeyboard.keyNum = NUM_OF_GPIO_KEY,
    .attachedDeviceObjConfig = &GPIOConfigForKey,
    .pGpioKeySet = gpioKeys
};
/* end of keyboard attached devices setting */


const tDevice * const devices[] =
{
    (tDevice*)&polk_allplay_DSP,
    (tDevice*)&ledConfig,
    (tDevice*)&UartAllPlayConfig,
#if NUM_OF_UART >= 2
    (tDevice*)&UartDebugConfig,
#endif // NUM_OF_UART >= 2
    (tDevice*)&adcKeyboardConfig,
    (tDevice*)&gpioKeyboardConfig,
    (tDevice*)&ADCConfigForPower,
};

const uint16 NUM_OF_ATTACHED_DEVICES = ArraySize(devices);

//======================================================================

void UartDrv_Init(eTpUartDevice id)
{
    switch(id)
    {
    case TP_UART_DEV_1:
        mPORTBClearBits( BIT_9 );
        mPORTBSetPinsDigitalIn( BIT_9 );
        TYM_SET_BIT(CNPUB, CNB9_PULLUP_ENABLE);

        mPORTFClearBits( BIT_5 );
        mPORTFSetPinsDigitalOut( BIT_5 );
        TYM_SET_BIT(CNPUF, CNF5_PULLUP_ENABLE);

        /* select pin as non-analog */
        ANSELBbits.ANSB9=0;
        //ANSELFbits.ANSF5=0; //no ANSEL reg. for port F.

        PPSUnLock;
        /* for groups see pps.h */
        PPSInput( 1, U1RX, RPB9 );
        PPSOutput( 2, RPF5, U1TX );
        PPSLock;
        break;
#if NUM_OF_UART >= 2
    case TP_UART_DEV_2:
        mPORTBClearBits( BIT_10 );
        mPORTBSetPinsDigitalIn( BIT_10 );
        TYM_SET_BIT(CNPUB, CNB10_PULLUP_ENABLE);

        mPORTDClearBits( BIT_11 );
        mPORTDSetPinsDigitalOut( BIT_11 );
        TYM_SET_BIT(CNPUD, CND11_PULLUP_ENABLE);

        /* select pin as non-analog */
        ANSELBbits.ANSB10=0;
        //ANSELDbits.ANSD11=0; // no analog function for D11

        PPSUnLock;
        /* for groups see pps.h */
        PPSInput( 1, U2RX, RPB10 );
        PPSOutput( 2, RPD11, U2TX );
        PPSLock;
        break;
#endif  // NUM_OF_UART >= 2
    default:
        ASSERT(0);
        break;
    }
}

void UartDrv_Deinit(eTpUartDevice id)
{
    switch(id)
    {
    case TP_UART_DEV_1:
        RPF5R   = 0x00;     // RPF5 No Connected
        U1RXR   = 0x0E;     // Reserved
        mPORTBClearBits( BIT_9 );
        mPORTBSetPinsDigitalIn( BIT_9 );
        TYM_CLR_BIT(CNPUB, CNB9_PULLUP_ENABLE);

        mPORTFClearBits( BIT_5 );
        mPORTFSetPinsDigitalIn( BIT_5 );
        TYM_CLR_BIT(CNPUF, CNF5_PULLUP_ENABLE);
        break;
#if NUM_OF_UART >= 2
    case TP_UART_DEV_2:
        RPD11R  = 0x00;     // RPD11 No Connected
        U2RXR   = 0x0E;     // Reserved
        mPORTBClearBits( BIT_10 );
        mPORTBSetPinsDigitalIn( BIT_10 );
        TYM_CLR_BIT(CNPUB, CNB10_PULLUP_ENABLE);

        mPORTDClearBits( BIT_11 );
        mPORTDSetPinsDigitalIn( BIT_11 );
        TYM_CLR_BIT(CNPUD, CND11_PULLUP_ENABLE);
        break;
#endif  // NUM_OF_UART >= 2
    default:
        ASSERT(0);
        break;
    }
}

static void powerKeyIntEnable()
{
    INTCONbits.INT0EP = 1;/* INT0 interrupt on positive:1 /negative: 0 edge */
    IPC0bits.INT0IP = 6;/* set INT0 priority; must match IPL in testISR */
    IFS0CLR = _IFS0_INT0IF_MASK;/* clear the interrupt for INT0 */
    IEC0SET = _IEC0_INT0IE_MASK;/* enable INT0 interrupt */
}
static void I2cDrv_Init(void)
{
    TRISDbits.TRISD10 =0;    //set RD10 output    => SCL1_PIN
    CNPUDbits.CNPUD10=0;
    PORTDbits.RD10 = 0;
    CNPDDbits.CNPDD10=0;

    TRISDbits.TRISD9 =0;    //set RD9 output    => SDA1_PIN
    CNPUDbits.CNPUD9=0;
    PORTDbits.RD9 = 0;
    CNPDDbits.CNPDD9=0;
}

static void LedDrv_Init(void)
{
    IEC0bits.T3IE = 0;

    // Red LED
    RPD1Rbits.RPD1R     = 0x0C;                     //set RD1 as the OC1
    OC1CON              = 0;                        //turn off the OC
    OC1CONbits.OCTSEL   = 1;                        //use the timer3
    OC1RS               = MAX_BRT;                  //initial the brightness to be 100
    OC1CONbits.OCM      = OC_PWM_FAULT_PIN_DISABLE; //set the OC to use the PWM mode
    IEC0bits.OC1IE      = 0;                        //disable OC interrupt

    // Green LED
    RPD4Rbits.RPD4R     = 0x0B;                     // set RD4 as the OC5
    OC5CON              = 0;
    OC5CONbits.OCTSEL   = 1;
    OC5RS               = MAX_BRT;
    OC5CONbits.OCM      = OC_PWM_FAULT_PIN_DISABLE;
    IEC0bits.OC5IE      = 0;

    // Blue LED
    RPD5Rbits.RPD5R     = 0x0B;                     // set RD5 as the OC2
    OC2CON              = 0;
    OC2CONbits.OCTSEL   = 1;
    OC2RS               = MAX_BRT;
    OC2CONbits.OCM      = OC_PWM_FAULT_PIN_DISABLE;
    IEC0bits.OC2IE      = 0;
}

void Hardware_Init(void)
{
    INTDisableInterrupts();
    
    //Set default voltage of output pin
    SAM_FACTORY_RESET_HIGH;     // High (not reset)
    RESET_PIN_LOW;
    DC3V3_EN_PIN_LOW;
    SDZ_PIN_OFF;
    AMP_MUTE_HIGH;

    //Config as output pin
    SAM_FACTORY_RESET_IO = 0;       // Output
    RESET_PIN_IO = 0;               // Output
    DC3V3_EN_PIN_IO = 0;            // Output
    SAM_RESET_PIN_IO= 0;

    INTEnableInterrupts();

    I2cDrv_Init();
    LedDrv_Init();
    powerKeyIntEnable();
}