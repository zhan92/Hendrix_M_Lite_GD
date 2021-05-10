/*
-------------------------------------------------------------------------------
TYMPHANY LTD





                  IoExpander Driver
                  -------------------------

                  SW Module Document




@file        IoExpanderDrvAw9523.c
@brief       This file implements the driver for AW9523B
@author      Wesley Lee
@date        2014-06-06
@copyright (c) Tymphany Ltd. All rights reserved.

Change History:
VERSION    : 1    DRAFT      2014-06-06     Wesley Lee
DESCRIPTION: First Draft. Generated by newclass.py
SCO/ERROR  :
-------------------------------------------------------------------------------
*/

#include "./IoExpanderDrv_priv_9523.h"
#include "product.config"
#include "GpioDrv.h"
#include "trace.h"
#include "attachedDevices.h"
#ifdef AW9523B_USE_GPIO_SETTING
typedef enum
{
    REG_ADDRESS=0,
    DATA1,
    DATA2,
    WRITE_DATA_MAX,
} eWriteData;
#endif

// default output level
#define IO_EXPANDER_DEFAULT_OUTPUT_PORT0    0x00		//initally all off
#define IO_EXPANDER_DEFAULT_OUTPUT_PORT1    0x00		//initally all off

// default LED Mode
#define IO_EXPANDER_DEFAULT_LED_MODE_PORT0  0x00    /**< 0-LED, 1-GPIO */
#define IO_EXPANDER_DEFAULT_LED_MODE_PORT1  0x00    /**< 0-LED, 1-GPIO */

// default IO Mode
#define IO_EXPANDER_DEFAULT_IO_MODE_PORT0   0x00    /**< 0-output, 1-input */
#define IO_EXPANDER_DEFAULT_IO_MODE_PORT1   0x00    /**< 0-output, 1-input */


/* Private functions / variables. Declare and drivers here */

/*****************************************************************************************************************
 *
 * Start-up / shut-down functions
 *
 *****************************************************************************************************************/
#ifdef IOEXPANDERDRV_RST_CONTROL
static void IoExpanderDrv_Rst(cIoExpanderDrv *me, tIoeLedDevice *pIoeLedConfig)
{
    /* Ctor gpio for ioexpander reset control */
    tGPIODevice *ioexpanderRstCtrl = NULL;
    ASSERT(pIoeLedConfig);
    ioexpanderRstCtrl = pIoeLedConfig->pResetGpioDevice;
#ifdef IOEXPANDERDRV_ONE_RST_FOR_MULTI_IOE
    if (!ioexpanderRstCtrl)
    {
        /* If there is only reset pin for multiple io-expander chips, then we should only reset one time before init.
        *   And the ioexpanderRstCtrl of other io-expander chips should be set to NULL in attachDevices.c
        */
        return;
    }
#else
    ASSERT(ioexpanderRstCtrl);
#endif

    GpioDrv_Ctor(&me->gpioDrv,ioexpanderRstCtrl);

    /* Reset ioexpander before init started */
    GpioDrv_ClearBit(&me->gpioDrv,pIoeLedConfig->pResetGpioDevice->pGPIOPinSet[0].gpioId);
    /* ca17 exeption handler will ctor IO-Expender, but interrupt already disable on exception,
    * to let ctor() work on both normal and exception mode, let it support two blocking delay.
    */

    BSP_BlockingDelayMs(2);//Delay 2 ms
    GpioDrv_SetBit(&me->gpioDrv,pIoeLedConfig->pResetGpioDevice->pGPIOPinSet[0].gpioId);

    BSP_BlockingDelayMs(2);//Delay 2 ms
}
#endif

/* @brief       IO Expander driver constructor
 * @param[in]   me          pointer to IO-Expander driver object
 * @param[in]   pConfig     device configuration (AW9523B is I2C device)
 */

void IoExpanderDrv_Ctor_aw9523(cIoExpanderDrv *me, tIoeLedDevice *pIoeLedConfig)
{
    me->count++;
    if(me->isReady)
    {
        return;
    }
    // // software reset
    const uint8 LED_INIT_RESET[] =
    {
        AW9523B_SW_RSTN_REG,
        pIoeLedConfig->pIoExpanderConfig->swResetValue,//AW9523B_SW_RSTN_VAULE,
    };

    // Default output level
    const uint8 LED_INIT_OUTPUT_LEVEL[] =
    {
        AW9523B_OUTPUT_PORT0_REG,
        pIoeLedConfig->pIoExpanderConfig->outPutPortA,//IO_EXPANDER_DEFAULT_OUTPUT_PORT0,
        pIoeLedConfig->pIoExpanderConfig->outPutPortB,//IO_EXPANDER_DEFAULT_OUTPUT_PORT1,
    };

    // LED mode switch
    const uint8 LED_INIT_MODE_SWITCH[] =
    {
        AW9523B_LED_MODE_PORT0_REG,
        pIoeLedConfig->pIoExpanderConfig->ledModePortA,//IO_EXPANDER_DEFAULT_LED_MODE_PORT0,
        pIoeLedConfig->pIoExpanderConfig->ledModePortB,//IO_EXPANDER_DEFAULT_LED_MODE_PORT1,
    };

#ifdef IOEXPANDERDRV_IO_DIRECTION_CTL
    const uint8 LED_INIT_IO_DIRECTION[] =
    {
        AW9523B_CONFIG_PORT0_REG,
        pIoeLedConfig->pIoExpanderConfig->ioDirectionA,
        pIoeLedConfig->pIoExpanderConfig->ioDirectionB,

    };
#endif

    // Control register
    const uint8 LED_INIT_CTRL_REG[] =
    {
        AW9523B_CTL_REG,
        pIoeLedConfig->pIoExpanderConfig->controlValue,//AW9523B_CTL_GPOMD_PUSH_PULL | AW9523B_CTL_ISEL_HALF,
    };

    if(!me->i2cDrv.isReady)
    {
#ifdef IOEXPANDERDRV_RST_CONTROL
        IoExpanderDrv_Rst(me, pIoeLedConfig);
#endif
        I2CDrv_Ctor(&me->i2cDrv,(tI2CDevice*)pIoeLedConfig->i2cDeviceConf);
        //keep sequence for specific chip to avoid blink while init;
        IoExpanderDrv_I2cWrite_aw9523(me, (uint8*)LED_INIT_RESET,          sizeof(LED_INIT_RESET));
        IoExpanderDrv_I2cWrite_aw9523(me, (uint8*)LED_INIT_CTRL_REG,       sizeof(LED_INIT_CTRL_REG));
        IoExpanderDrv_I2cWrite_aw9523(me, (uint8*)LED_INIT_MODE_SWITCH,    sizeof(LED_INIT_MODE_SWITCH));
#ifdef IOEXPANDERDRV_IO_DIRECTION_CTL
        IoExpanderDrv_I2cWrite_aw9523(me, (uint8*)LED_INIT_IO_DIRECTION,    sizeof(LED_INIT_IO_DIRECTION));
#endif
        IoExpanderDrv_I2cWrite_aw9523(me, (uint8*)LED_INIT_OUTPUT_LEVEL,   sizeof(LED_INIT_OUTPUT_LEVEL));
    }
    me->isReady = TRUE;
}
/* @brief       IO Expander driver destructor
 * @param[in]   me          pointer to IO-Expander driver object
 */
void IoExpanderDrv_Xtor_aw9523(cIoExpanderDrv *me)
{
    --me->count;
    if (me->count == 0)
    {
        // destruct only for the last object
        // Destroy output level
        const uint8 LED_DEINIT_OUTPUT_LEVEL[] =
        {
            AW9523B_OUTPUT_PORT0_REG,
            0x00,
            0x00,
        };

        // IO mode: all output to ground
        // Destroy LED mode switch
        const uint8 LED_DEINIT_MODE_SWITCH[] =
        {
            AW9523B_LED_MODE_PORT0_REG,
            0x00,
            0x00,
        };

        IoExpanderDrv_I2cWrite_aw9523(me, (uint8*)LED_DEINIT_OUTPUT_LEVEL, sizeof(LED_DEINIT_OUTPUT_LEVEL));
        IoExpanderDrv_I2cWrite_aw9523(me, (uint8*)LED_DEINIT_MODE_SWITCH,  sizeof(LED_DEINIT_MODE_SWITCH));

        I2CDrv_Xtor(&me->i2cDrv);
        me->isReady = FALSE;
    }
}
/* @brief       Set the corresponding Port-Pin brightness
 * @param[in]   me          pointer to IO-Expander driver object
 * @param[in]   port        0 or 1
 * @param[in]   pin         0 to 7
 * @param[in]   value       0 to 255 brightness, 0 means off
 */
void IoExpanderDrv_SetBrightness_aw9523(cIoExpanderDrv *me, uint8 port, uint8 pin, uint8 value)
{
    const uint8 gDimRegMap[2][8] =   /**< DIM Register mapping table */
    {
        {
            AW9523B_P0_0_DIM_REG, AW9523B_P0_1_DIM_REG,
            AW9523B_P0_2_DIM_REG, AW9523B_P0_3_DIM_REG,
            AW9523B_P0_4_DIM_REG, AW9523B_P0_5_DIM_REG,
            AW9523B_P0_6_DIM_REG, AW9523B_P0_7_DIM_REG,
        },
        {
            AW9523B_P1_0_DIM_REG, AW9523B_P1_1_DIM_REG,
            AW9523B_P1_2_DIM_REG, AW9523B_P1_3_DIM_REG,
            AW9523B_P1_4_DIM_REG, AW9523B_P1_5_DIM_REG,
            AW9523B_P1_6_DIM_REG, AW9523B_P1_7_DIM_REG,
        },
    };
    uint8 cmd[2];

    cmd[0] = gDimRegMap[port][pin];
    cmd[1] = value;
    IoExpanderDrv_I2cWrite_aw9523(me, cmd, 2);
}
/*****************************************************************************************************************
 *
 * Public functions
 *
 *****************************************************************************************************************/
/* @brief       Set the corresponding Port-Pin to High
 * @param[in]   me          pointer to IO-Expander driver object
 * @param[in]   port        0 or 1
 * @param[in]   pin         0 to 7
 */
void IoExpanderDrv_SetGpio_aw9523(cIoExpanderDrv *me, uint8 port, uint8 pin)
{
#ifdef AW9523B_USE_GPIO_SETTING
    uint8 Data[WRITE_DATA_MAX];
    IoExpanderDrv_I2cRead_aw9523(me,AW9523B_OUTPUT_PORT0_REG, &Data[DATA1]);
    Data[REG_ADDRESS] = AW9523B_OUTPUT_PORT0_REG;
    Data[port+DATA1]|= (1<<pin);
    IoExpanderDrv_I2cWrite_aw9523(me, Data, WRITE_DATA_MAX);
#else
    IoExpanderDrv_SetBrightness_aw9523(me, port, pin, AW9523B_DIM_MAX_LEVEL);
#endif
}

/* @brief       Clear the corresponding Port-Pin to Ground
 * @param[in]   me          pointer to IO-Expander driver object
 * @param[in]   port        0 or 1
 * @param[in]   pin         0 to 7
 */
void IoExpanderDrv_ClearGpio_aw9523(cIoExpanderDrv *me, uint8 port, uint8 pin)
{
#ifdef AW9523B_USE_GPIO_SETTING
    uint8 Data[WRITE_DATA_MAX];
    IoExpanderDrv_I2cRead_aw9523(me,AW9523B_OUTPUT_PORT0_REG, &Data[DATA1]);
    Data[REG_ADDRESS] = AW9523B_OUTPUT_PORT0_REG;
    Data[port+DATA1]&= ~(1<<pin);
    IoExpanderDrv_I2cWrite_aw9523(me, Data, WRITE_DATA_MAX);
#else
    IoExpanderDrv_SetBrightness_aw9523(me, port, pin, 0);
#endif
}

#ifdef AW9523B_USE_GPIO_SETTING

/* @brief       get the corresponding Port-Pin to Ground
 * @param[in]   me          pointer to IO-Expander driver object
 * @param[in]   port        0 or 1
 * @param[in]   pin         0 to 7
 */
uint8 IoExpanderDrv_ReadGpio_aw9523(cIoExpanderDrv *me, uint8 port, uint8 pin)
{
    uint8 Data[2];
    uint8 Result;
    IoExpanderDrv_I2cRead_aw9523(me,AW9523B_INPUT_PORT0_REG, Data);
    Result = (Data[port]>>pin)&0x01;
    return Result;
}
#endif

/*****************************************************************************************************************
 *
 * private functions
 *
 *****************************************************************************************************************/
/* @brief       Write data of IO Expander with I2C channel
 * @param[in]   me          pointer to IO-Expander driver object
 * @param[in]   data        pointer to the send data
 * @param[in]   length      length of the data
 */
static void IoExpanderDrv_I2cWrite_aw9523(cIoExpanderDrv *me, uint8 *data, uint8 length)
{
    uint8 tmp;
    tI2CMsg i2cMsg=
    {
        .devAddr    = (uint8)(me->i2cDrv.pConfig->devAddress),
        .regAddr    = NULL,
        .length     = length,
        .pMsg       = data,
    };

    if (TRUE == me->i2cDrv.isReady)
    {
        tmp = (uint8)I2CDrv_MasterWrite(&me->i2cDrv, &i2cMsg);
        if (TP_SUCCESS != tmp)
        {
            me->i2cDrv.isReady = 1;/*FALSE*/;
        }
    }
}

#ifdef AW9523B_USE_GPIO_SETTING
/* @brief       Read 2byte data of IO Expander with I2C channel
 * @param[in]   me          pointer to IO-Expander driver object
 * @param[in]   reg        reg number
 * @param[in]   value      pointer for data read.
 */
static void IoExpanderDrv_I2cRead_aw9523(cIoExpanderDrv *me, uint8 reg, uint8* value)
{
    cI2CDrv* pi2cObj= &me->i2cDrv;
    tI2CMsg i2cMsg=
    {
        .devAddr    = pi2cObj->pConfig->devAddress,
        .regAddr    = reg,
        .length     = 2,
        .pMsg       = value,
    };

    if (TRUE == me->i2cDrv.isReady)
    {
        if (TP_SUCCESS != I2CDrv_MasterRead(&me->i2cDrv, &i2cMsg))
        {
            me->i2cDrv.isReady = FALSE;
        }
    }
}
#endif

