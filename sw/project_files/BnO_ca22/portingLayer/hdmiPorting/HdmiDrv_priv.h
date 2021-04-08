/*****************************************************************************
* Model: tym_platform.qm
* File:  F:\Project_Atmos\tymphany_platform\sw/driver\include\HdmiDrv.h
*
* This code has been generated by QM tool (see state-machine.com/qm).
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*****************************************************************************/
/*${F:\Project_Atmos~::driver\include\HdmiDrv.h} ...........................*/
/**
 *  @file      hdmi_driver.h
 *  @brief     This file contains the EP91A6S HDMI driver implementation.
 *  @author    Albert
 *  @date      23-May-2016
 */
#ifndef HDMIDRV_PRIV_H
#define HDMIDRV_PRIV_H

#ifdef STM32F0XX
#include "stm32f0xx.h"
#endif
#include "HdmiDrv.h"

/* ---- Begin ---------------Register Setting of HDMI Chip */

/*
* Vendor ID
*/
#define HDMI_REG_X0 0x00    // High byte
#define HDMI_REG_X1 0x01    // Low byte

/*
* DEVICE ID
*/
#define HDMI_REG_X2 0x02    // High byte
#define HDMI_REG_X3 0x03    // Low byte

/*
* Version
*/
#define HDMI_REG_X4 0x04    // version
#define HDMI_REG_X5 0x05    // Year
#define HDMI_REG_X6 0x06    // Month

/*
* Host information
*/
#define HDMI_REG_X7 0x07

#define HDMI_REG_X8 0x08

/*
*
*/
#define HDMI_REG_X9 0x09


// [7]Power, <0>off, <1>on
// [5]Audio path, <0>TV speaker, <1>external speaker
// [2]CEC dis, <0>on, <1>off
// [1]CEC mute, <0>unmute, <1>mute
// [0]ARC en, <0>disable, <1>enable
#define HDMI_REG_X10 0x10

// [3:0]RX_Sel, <1>port1, <6>Non HDMI port
#define HDMI_REG_X11 0x11

#define HDMI_REG_X12 0x12

// [7:0]Volume, 0~100
#define HDMI_REG_X13 0x13

// [1:0]ARC support, <00>default, <10>success, <01>fail
// ## host should read this reg and write 00 to it ##
#define HDMI_REG_X14 0x14

/*
*   GC Packet
*/
#define HDMI_REG_X38 0x38

/* for HDMI_REG_X7 */

#define HDMI_BITMASK_HOST_POWER     (1 << 7)
#define HDMI_BITMASK_HOST_POWER_ON  (1 << 7)
#define HDMI_BITMASK_HOST_POWER_OFF (0)

#define HDMI_BITMASK_HOST_SOURCE        (0x0F)
#define HDMI_BITMASK_HOST_SOURCE_HDMI   (1)
#define HDMI_BITMASK_HOST_SOURCE_TV_ARC (2)
#define HDMI_BITMASK_HOST_SOURCE_OTHER  (3)

/* for HDMI_REG_X9 */
#define HDMI_BITMASK_CEC_POWER          (1 << 7)
#define HDMI_BITMASK_CEC_POWER_ON       (1 << 7)
#define HDMI_BITMASK_CEC_POWER_OFF      (0)

#define HDMI_BITMASK_I2C                (1 << 6)
#define HDMI_BITMASK_I2C_READY          (1 << 6)
#define HDMI_BITMASK_I2C_NOT_READY      (0)

/* for HDMI_REG_X10 */
#define HDMI_BITMASK_POWER      (1 << 7)
#define HDMI_BITMASK_POWER_ON   (1 << 7)
#define HDMI_BITMASK_POWER_OFF  (0)

#define HDMI_BITMASK_AUDIO_PATH     (1 << 5)
#define HDMI_BITMASK_AUDIO_PATH_AMP (1 << 5)
#define HDMI_BITMASK_AUDIO_PATH_TV  (0)

#define HDMI_BITMASK_CEC_DISABLE        (1 << 2)
#define HDMI_BITMASK_CEC_DISABLE_ON     (1 << 2)
#define HDMI_BITMASK_CEC_DISABLE_OFF    (0)

#define HDMI_BITMASK_CEC_MUTE       (1 << 1)
#define HDMI_BITMASK_CEC_MUTE_ON    (1 << 1)
#define HDMI_BITMASK_CEC_MUTE_OFF   (0)

#define HDMI_BITMASK_ARC_ENABLE         (1 << 0)
#define HDMI_BITMASK_ARC_ENABLE_ON      (1 << 0)
#define HDMI_BITMASK_ARC_ENABLE_OFF     (0)

/* for HDMI_REG_X11 */
#define HDMI_BITMASK_RX_SEL         (0x0F)
#define HDMI_BITMASK_RX_SEL_PORT1   (1)
#define HDMI_BITMASK_RX_SEL_ARC     (3)
#define HDMI_BITMASK_RX_SEL_NULL    (6)

/* for HDMI_REG_X14 */
#define HDMI_BITMASK_ARC_SUCCESS (1<<1)
#define HDMI_BITMASK_ARC_FAIL (1<<0)
#define HDMI_BITMASK_ARC_CLEAR ((1<<0)|(1<<1))

/* ---- End ---------------Register Setting of HDMI Chip */
typedef union tagREG_7 {
    uint8_t data;
    struct
    {
        uint8_t HOST_POWER:1;
        uint8_t HF_VSDB:1;
        uint8_t EDID_CTRL:1;
        uint8_t unuse:1;
        uint8_t HOST_SOURCE:4;
    }bit;
} REG_7;

typedef union tagREG_8 {
    uint8_t data;
    struct
    {
        uint8_t TV_ARC_ON  :1;
        uint8_t Rx_HDCP1x  :1;
        uint8_t Rx_HDCP2x  :1;
        uint8_t Tx_HDCP2x  :1;
        uint8_t Rx_Hot_Plug:1;
        uint8_t Rx_DDC     :1;
        uint8_t TX_HDMI    :1;
        uint8_t Tx_Hot_Plug:1;
    }bit;
} REG_8;

typedef union tagREG_9 {
    uint8_t data;
    struct
    {
        uint8_t ADO_CHF   :1;    // audio format change
        uint8_t CEC_ECF   :1;    // CEC event
        uint8_t tx_HDCP2x :1;    // Tx HDCP2.x valid/invalid
        uint8_t rx_HDCP2x :1;    // Rx HDCP2.x valid/invalid

        uint8_t I2S_SPDIF_Dis :1; // I2S/SPDIF is open/close
        uint8_t CEC_Port  :1;     // port(or channel) is change or not
        uint8_t I2C_Ready :1;     // slave i2c is ready or not, if set, i2c is ready, host can read/write register
        uint8_t CEC_Power :1;     // receive a CEC command: POWER ON/OFF
    }bit;
} REG_9;

typedef union tagREG_10 {
    uint8_t data;
    struct
    {
        uint8_t ARC_EN     :1;
        uint8_t CEC_Mute   :1;
        uint8_t CEC_DIS    :1;
        uint8_t Video_Path :1;
        uint8_t A_Reset    :1;
        uint8_t Audio_Path :1;
        uint8_t unused     :1;
        uint8_t Power      :1;
    }bit;
} REG_10;

typedef union tagREG_11 {
    uint8_t data;
    struct
    {
        uint8_t RX_Sel   :4;
        uint8_t ARP_FREQ  :4;
    }bit;
} REG_11;

typedef union tagREG_12 {
    uint8_t data;
    struct
    {
        uint8_t RX2_Sel    :4;
        uint8_t Unlock     :1;
        uint8_t unused_2   :1;
        uint8_t ARC_DIS    :1;
        uint8_t unused_1   :1;
    }bit;
} REG_12;

typedef union tagREG_14 {
    uint8_t data;
    struct
    {
        uint8_t LinkON0       :1;
        uint8_t LinkON1       :1;
        uint8_t LinkON2       :1;
        uint8_t LinkON3       :1;
        uint8_t PD_Detect     :1;
        uint8_t VMode_Select  :1;
        uint8_t Dual_In       :1;
        uint8_t VMode_Enable  :1;
    }bit;
} REG_14;


static uint8_t HdmiDrv_I2C_Read(HdmiDrv * const me, uint8_t addr, uint8_t *data);
static uint8_t HdmiDrv_I2C_Write(HdmiDrv * const me, uint8_t addr, uint8_t data);
static void HdmiDrv_InitSection0(void *p);
static void HdmiDrv_InitSection1(void *p);
static uint8_t HdmiDrv_CalcVolume(uint8_t cec_volume);
static uint8_t HdmiDrv_CalcCECVolume(uint8_t volume);
static void HdmiDrv_InterruptHandler(void);
#endif /* HDMIDRV_PRIV_H */

