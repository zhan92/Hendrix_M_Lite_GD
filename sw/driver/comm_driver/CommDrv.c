/*
-------------------------------------------------------------------------------
TYMPHANY LTD





                  Comm Driver
                  -------------------------

                  SW Module Document




@file        CommDrv.c
@brief       It's the driver to communicate with SoC
@author      Eason Huang 
@date        2016-06-02
@@Copyright (c) <2015> Tymphany HK Ltd. All rights reserved.

DESCRIPTION: Generated by newclass.py
SCO/ERROR  : 
-------------------------------------------------------------------------------
*/

#include "./CommDrv_priv.h"
#include "trace.h"

#include "UartDrv.h"
#include "ringbuf.h"
#include "GPIODrv.h"
#include "object_ids.h"
#include "bsp.h"

#define COMM_UART_TX_BUF_SIZE     (0x100 * 4)
#define COMM_UART_RX_BUF_SIZE     (0x100)

#define SWITCH2UART(x)          GpioDrv_SetBit(&(x),	GPIO_OUT_MTK_USB_UART_SEL)
#define SWITCH2USB(x)           GpioDrv_ClearBit(&(x),	GPIO_OUT_MTK_USB_UART_SEL)

#define SOCPWR_ENABLE(x)		do{                                             \
									GpioDrv_SetBit(&(x), GPIO_OUT_SYSPWR_ON);   \
									BSP_BlockingDelayMs(3000);                  \
									GpioDrv_SetBit(&(x), GPIO_OUT_1V5_POWER);   \
									GpioDrv_SetBit(&(x), GPIO_OUT_1V2_POWER);   \
									GpioDrv_SetBit(&(x), GPIO_OUT_WIFI_POWER);  \
								}while(0)

#define SOCPWR_DISABLE(x)		do{                                             \
									GpioDrv_ClearBit(&(x), GPIO_OUT_WIFI_POWER);\
									GpioDrv_ClearBit(&(x), GPIO_OUT_1V2_POWER); \
									GpioDrv_ClearBit(&(x), GPIO_OUT_1V5_POWER); \
									GpioDrv_ClearBit(&(x), GPIO_OUT_SYSPWR_ON); \
								}while(0)

/* Private functions / variables. Declare and drivers here */
static uint8        uartTxBuf[COMM_UART_TX_BUF_SIZE];
static uint8        uartRxBuf[COMM_UART_RX_BUF_SIZE];
static cRingBuf     txBuf;
static cRingBuf     rxBuf;
cUartDrv            uartComm;
static cGpioDrv     gpioComm;

/*****************************************************************************************************************
 *
 * Start-up / shut-down functions
 *
 *****************************************************************************************************************/
void CommDrv_Ctor(cCommDrv *me)
{
    //Initialize Comm GPIO pins
    gpioComm.gpioConfig= (tGPIODevice*)getDevicebyIdAndType(COMM_DEV_ID, GPIO_DEV_TYPE, NULL);
    ASSERT(gpioComm.gpioConfig);
    GpioDrv_Ctor(&gpioComm, gpioComm.gpioConfig);

    //Initialize Comm UART
    RingBuf_Ctor(&txBuf, uartTxBuf, ArraySize(uartTxBuf));
    RingBuf_Ctor(&rxBuf, uartRxBuf, ArraySize(uartRxBuf));
    UartDrv_Ctor(&uartComm, (tUARTDevice*) getDevicebyIdAndType(COMM_DEV_ID, UART_DEV_TYPE, NULL), &txBuf, &rxBuf);
    UartDrv_RegisterRxCallback(&uartComm, CommDrv_OnReceive);
}

void CommDrv_Xtor(cCommDrv *me)
{
    UartDrv_Xtor(&uartComm);
    RingBuf_Xtor(&txBuf);
    RingBuf_Xtor(&rxBuf);
}

/*****************************************************************************************************************
 *
 * Public functions
 *
 *****************************************************************************************************************/
void CommDrv_WriteCommand(uint8 *cmd)
{
    uint8 sendBuf[MAX_COMM_CMD_LEN+1];
    uint8 cmdLen, totalLen;

    memset(sendBuf, '\0', MAX_COMM_CMD_LEN+1);
    sendBuf[0] = MCU_CMD_START_CODE;
    cmdLen = strlen(cmd);
    memcpy(sendBuf+1, cmd, cmdLen);
    sendBuf[cmdLen+1] = MCU_CMD_END_CODE;
    totalLen = cmdLen + 2;
    TP_PRINTF("%s: 0x%x\n\r", __func__, sendBuf[1]);
    UartDrv_Write(&uartComm, sendBuf, totalLen);
}

void CommDrv_SetSOCPower(socPower flag)
{
    if (flag == ON)
    {
        SOCPWR_ENABLE(gpioComm);
    }
    else
    {
        SOCPWR_DISABLE(gpioComm);
    }
    TP_PRINTF("%s: %d\n\r", __func__, flag);
}

void CommDrv_SelSOCUsbFunc(usbFunc func)
{
    if (func == USB2USB)
    {
        SWITCH2USB(gpioComm);
    }
    else
    {
        SWITCH2UART(gpioComm);
    }
    TP_PRINTF("%s: %d\n\r", __func__, func);
}

/*****************************************************************************************************************
 *
 * private functions
 *
 *****************************************************************************************************************/
/* Call back function receiving message from SoC */
/* After decode the commands, this function will send the message to CommSrv to handle the command */
static void CommDrv_OnReceive(void* p)
{
    static bool cmdStart = FALSE, cmdEnd = FALSE;
    uint8 buff[MAX_COMM_CMD_LEN+1];
    uint32 bytesRead = 0;
    tUartRxData *uartData = (tUartRxData *)p;
    data d = RingBuf_TopData(&rxBuf);

    if (d == SOC_CMD_START_CODE)
    {
        cmdStart = TRUE;
    }
    else if (d == SOC_CMD_END_CODE)
    {
        cmdEnd = TRUE;
    }

    if (cmdEnd == TRUE)
    {
        bytesRead = RingBuf_GetUsedSize(&rxBuf);
        if (bytesRead > MAX_COMM_CMD_LEN)
        {
            RingBuf_Reset(&rxBuf);
        }
        else
        {
            RingBuf_Pop(&rxBuf, buff, bytesRead);
            buff[bytesRead] = '\0';
            CommCmdEvt* evt = Q_NEW(CommCmdEvt, COMM_RECEIVE_COMMAND);
            evt->dir = SOC2MCU;
            memcpy(&evt->cmd, buff, bytesRead+1);
            SendToServer(COMM_SRV_ID,(QEvt*)evt);
            CommDrv_ACK();
        }
        cmdStart = cmdEnd = FALSE;
    }
}

static void CommDrv_ACK(void)
{
    uint32 size = 0;
    uint8 dest_buff[MAX_COMM_CMD_LEN+1];

    memset(dest_buff, '\0', MAX_COMM_CMD_LEN+1);
    dest_buff[0] = MCU_CMD_ACK;
    size = strlen(dest_buff);
    UartDrv_Write(&uartComm, (uint8* )dest_buff, size);
}