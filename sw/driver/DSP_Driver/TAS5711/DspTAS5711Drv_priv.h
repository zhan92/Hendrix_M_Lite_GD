/**
*  @file      DspTAS5711Drv_priv.h
*  @brief     This file declares the driver for the TAS5711.
*  @modified  Donald Leung/Edmond Sung
*  @date      04-2013
*  @copyright Tymphany Ltd.
*/

#ifndef DSPTAS5711DRV_PRIV_H
#define DSPTAS5711DRV_PRIV_H
#include "SettingSrv.h"
#include "TAS5711_EQ.c"

#define TAS5711_ADDRESS_TW	(0x34)
#define TAS5711_ADDRESS_WF	(0x36)

#define TW_BASE_GAIN	    (25)   
#define WF_BASE_GAIN	    (25) 

#define CLOCK_CONTROL_REG				0x00
#define DEVICE_ID_REG					0x01
#define ERROR_STATUS_REG				0x02
#define SYSTEM_CONTROL_REG1			    0x03
#define SERIAL_DATA_INTERFACE_REG       0x04
#define SYSTEM_CONTROL_REG2			    0x05
#define SOFT_MUTE_REG					0x06
#define MASTER_VOLUME_REG				0x07
#define CHANNEL_1_REG					0x08
#define CHANNEL_2_REG					0x09
#define CHANNEL_3_REG					0x0A
#define VOLUME_CONFIGURATION_REG        0x0E
#define MODULATION_LIMIT_REG            0x10
#define PWM_SHUTDOWN_GROUP_REG	        0x19
#define START_STOP_PERIOD_REG           0x1A
#define OSC_TRIM_REG					0x1B
#define BKND_ERR_REG                    0x1C
#define INPUT_MULTIPLEXER_REG           0x20
#define CHANNEL4_SOURCE_SELECT_REG	    0x21
#define PWM_OUTPUT_MIX_REG			    0x25
#define DRC_CONTROL_REG                 0x46
#define BANK_SWITCH_AND_EQ_CONTROL_REG  0x50

const uint8 TAS5711_0[]         = {0x00, 0x6c};
const uint8 TAS5711_5[]         = {0x05, 0x80};
const uint8 TAS5711_1B[]        = {0x1B, 0x00};
const uint8 TAS5711_20[]        = {0x20, 0x00, 0x89, 0x77, 0x72};
const uint8 TAS5711_46[]        = {0x46, 0x00, 0x00, 0x00, 0x03};
const uint8 TAS5711_50_EQ_OFF[] = {0x50, 0x00, 0x00, 0x00, 0x80};
const uint8 TAS5711_50_EQ_ON[]  = {0x50, 0x00, 0x00, 0x00, 0x00};
const uint8 TAS5711_51[]        = {0x51, 0,0x80,0,0,0,0x80,0,0,0,00,0,0};
const uint8 TAS5711_60[]        = {0x60, 0,0,0,0,0,0,0,0};
const uint8 TAS5711_61[]        = {0x61, 0,0,0,0,0,0,0,0};
const uint8 TAS5711_19__BTL[]   = {0x19,0x30};
const uint8 TAS5711_19_PBTL[]   = {0x19,0x3A};
const uint8 TAS5711_25__BTL[]   = {0x25, 0x01, 0x02, 0x13, 0x45};
const uint8 TAS5711_25_PBTL[]   = {0x25, 0x01, 0x10, 0x32, 0x45};

uint8 TAS5711_GAIN_WF[] = {0x08, WF_BASE_GAIN, WF_BASE_GAIN, 0xFF};
uint8 TAS5711_GAIN_TW[] = {0x08, TW_BASE_GAIN, TW_BASE_GAIN, 0xFF};

uint8 TAS5711_HARD_MUTE[] = {0x05, 0x00};
uint8 TAS5711_SOFT_MUTE[] = {0x06, 0x80};

uint8 TAS5711_MASTER_VOL[] = {MASTER_VOLUME_REG,0xFF};

const uint8  VOLUME_TABLE[] = {
    254,//-103 dB  0 step
    154, // -53  dB  1 step
    136, // -43  dB  2 step
    122, // -36  dB  3 step
    108, // -29  dB  4 step
    98, // -24  dB  5 step
    88, // -19  dB  6 step
    82, // -16  dB  7 step
    76, // -13  dB  8 step
    74, // -12  dB  9 step
    72, // -11  dB  10 step
    70, // -10  dB  11 step
    68, // -9   dB  12 step
    66, // -8   dB  13 step
    64, // -7.5 dB  14 step
    63, // -7   dB  15 step
    62, // -6.5 dB  16 step
    61, // -6   dB  17 step
    59, // -5.5 dB  18 step
    58, // -5   dB  19 step
    57, // -4.5 dB  20 step
    56, // -4   dB  21 step
    55, // -3.5 dB  22 step
    54, // -3   dB  23 step
    53, // -2.5 dB  24 step
    52, // -2   dB  25 step
    51, // -1.5 dB  26 step
    50, // -1   dB  27 step
    49, // -0.5 dB  28 step
    48, //  0   dB  29 step
};


const uint8 EQ_HIGH_29_TREBLE[9][21] = 
{
    {0x29,0x00,0x46,0x87,0xC0,0x0F,0xA0,0x47,0x93,0x00,0x24,0x90,0xD8,0x00,0xC4,0xA5,0xF2,0x0F,0xAF,0xF9,0xE3},//HPF4000Hz -6,-4.5,-3,-1.5,0,1.5,3,4.5,6dB
    {0x29,0x00,0x51,0xD7,0xA5,0x0F,0x8E,0xC4,0xC3,0x00,0x2B,0xA0,0x44,0x00,0xC2,0x3F,0x21,0x0F,0xB1,0x84,0x32},
    {0x29,0x00,0x5E,0xFE,0x7A,0x0F,0x7A,0x40,0x98,0x00,0x33,0xF4,0x02,0x00,0xBF,0xA5,0x72,0x0F,0xB3,0x27,0x7A},
    {0x29,0x00,0x6E,0x44,0x58,0x0F,0x62,0x26,0x7A,0x00,0x3D,0xD0,0x68,0x00,0xBC,0xF2,0x30,0x0F,0xB4,0xD2,0x95},
    {0x29,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x29,0x00,0x94,0x95,0xB7,0x0F,0x24,0xAA,0xF8,0x00,0x57,0x44,0x72,0x00,0xB7,0x3C,0x25,0x0F,0xB8,0x3E,0xBB},
    {0x29,0x00,0xAC,0x79,0x55,0x0E,0xFD,0xC4,0x04,0x00,0x67,0x8B,0xCC,0x00,0xB4,0x37,0xFC,0x0F,0xB9,0xFE,0xDF},
    {0x29,0x00,0xC8,0x30,0x90,0x0E,0xD0,0x33,0xAA,0x00,0x7A,0xBF,0x39,0x00,0xB1,0x17,0x85,0x0F,0xBB,0xC5,0x08},
    {0x29,0x00,0xE8,0x4C,0x21,0x0E,0x9B,0x1E,0x44,0x00,0x91,0x3A,0xAE,0x00,0xAD,0xB7,0x34,0x0F,0xBD,0xA3,0xB9},
};

const uint8 EQ_HIGH_30_TREBLE[9][21] = 
{
    {0x30,0x00,0x46,0x87,0xC0,0x0F,0xA0,0x47,0x93,0x00,0x24,0x90,0xD8,0x00,0xC4,0xA5,0xF2,0x0F,0xAF,0xF9,0xE3},//HPF4000Hz -6,-4.5,-3,-1.5,0,1.5,3,4.5,6dB
    {0x30,0x00,0x51,0xD7,0xA5,0x0F,0x8E,0xC4,0xC3,0x00,0x2B,0xA0,0x44,0x00,0xC2,0x3F,0x21,0x0F,0xB1,0x84,0x32},
    {0x30,0x00,0x5E,0xFE,0x7A,0x0F,0x7A,0x40,0x98,0x00,0x33,0xF4,0x02,0x00,0xBF,0xA5,0x72,0x0F,0xB3,0x27,0x7A},
    {0x30,0x00,0x6E,0x44,0x58,0x0F,0x62,0x26,0x7A,0x00,0x3D,0xD0,0x68,0x00,0xBC,0xF2,0x30,0x0F,0xB4,0xD2,0x95},
    {0x30,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x30,0x00,0x94,0x95,0xB7,0x0F,0x24,0xAA,0xF8,0x00,0x57,0x44,0x72,0x00,0xB7,0x3C,0x25,0x0F,0xB8,0x3E,0xBB},
    {0x30,0x00,0xAC,0x79,0x55,0x0E,0xFD,0xC4,0x04,0x00,0x67,0x8B,0xCC,0x00,0xB4,0x37,0xFC,0x0F,0xB9,0xFE,0xDF},
    {0x30,0x00,0xC8,0x30,0x90,0x0E,0xD0,0x33,0xAA,0x00,0x7A,0xBF,0x39,0x00,0xB1,0x17,0x85,0x0F,0xBB,0xC5,0x08},
    {0x30,0x00,0xE8,0x4C,0x21,0x0E,0x9B,0x1E,0x44,0x00,0x91,0x3A,0xAE,0x00,0xAD,0xB7,0x34,0x0F,0xBD,0xA3,0xB9},
};

const uint8 EQ_LOW_29_BASS[9][21] = 
{
    {0x29,0x00,0x7F,0x2E,0x84,0x0F,0x05,0x97,0x5B,0x00,0x7B,0x49,0xA1,0x00,0xFA,0x60,0xEE,0x0F,0x85,0x80,0x24},//LPF200Hz -6,-4.5,-3,-1.5,0,1.5,3,4.5,6dB
    {0x29,0x00,0x7F,0x62,0xD2,0x0F,0x05,0x5E,0xFD,0x00,0x7B,0x4F,0x2A,0x00,0xFA,0x9B,0x40,0x0F,0x85,0x48,0x41},
    {0x29,0x00,0x7F,0x97,0x4A,0x0F,0x05,0x26,0x9C,0x00,0x7B,0x54,0x9F,0x00,0xFA,0xD5,0x92,0x0F,0x85,0x10,0x44},
    {0x29,0x00,0x7F,0xCB,0xA7,0x0F,0x04,0xF0,0xAA,0x00,0x7B,0x57,0xE4,0x00,0xFB,0x0D,0x6E,0x0F,0x84,0xDA,0x8D},
    {0x29,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x29,0x00,0x80,0x34,0x6E,0x0F,0x04,0x8B,0xBC,0x00,0x7B,0x57,0xE4,0x00,0xFB,0x76,0x2C,0x0F,0x84,0x75,0x96},
    {0x29,0x00,0x80,0x69,0x0A,0x0F,0x04,0x5C,0x95,0x00,0x7B,0x54,0x9F,0x00,0xFB,0xA7,0x40,0x0F,0x84,0x46,0x2B},
    {0x29,0x00,0x80,0x9D,0xEF,0x0F,0x04,0x2F,0x88,0x00,0x7B,0x4F,0x2A,0x00,0xFB,0xD6,0x42,0x0F,0x84,0x18,0xB0},
    {0x29,0x00,0x80,0xD2,0xD4,0x0F,0x04,0x02,0xAA,0x00,0x7B,0x49,0xA1,0x00,0xFC,0x05,0x19,0x0F,0x83,0xEB,0x4D},
};

const uint8 EQ_LOW_30_BASS[9][21] = 
{  
    {0x30,0x00,0x7F,0x2E,0x84,0x0F,0x05,0x97,0x5B,0x00,0x7B,0x49,0xA1,0x00,0xFA,0x60,0xEE,0x0F,0x85,0x80,0x24},//LPF200Hz -6,-4.5,-3,-1.5,0,1.5,3,4.5,6dB
    {0x30,0x00,0x7F,0x62,0xD2,0x0F,0x05,0x5E,0xFD,0x00,0x7B,0x4F,0x2A,0x00,0xFA,0x9B,0x40,0x0F,0x85,0x48,0x41},
    {0x30,0x00,0x7F,0x97,0x4A,0x0F,0x05,0x26,0x9C,0x00,0x7B,0x54,0x9F,0x00,0xFA,0xD5,0x92,0x0F,0x85,0x10,0x44},
    {0x30,0x00,0x7F,0xCB,0xA7,0x0F,0x04,0xF0,0xAA,0x00,0x7B,0x57,0xE4,0x00,0xFB,0x0D,0x6E,0x0F,0x84,0xDA,0x8D},
    {0x30,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x30,0x00,0x80,0x34,0x6E,0x0F,0x04,0x8B,0xBC,0x00,0x7B,0x57,0xE4,0x00,0xFB,0x76,0x2C,0x0F,0x84,0x75,0x96},
    {0x30,0x00,0x80,0x69,0x0A,0x0F,0x04,0x5C,0x95,0x00,0x7B,0x54,0x9F,0x00,0xFB,0xA7,0x40,0x0F,0x84,0x46,0x2B},
    {0x30,0x00,0x80,0x9D,0xEF,0x0F,0x04,0x2F,0x88,0x00,0x7B,0x4F,0x2A,0x00,0xFB,0xD6,0x42,0x0F,0x84,0x18,0xB0},
    {0x30,0x00,0x80,0xD2,0xD4,0x0F,0x04,0x02,0xAA,0x00,0x7B,0x49,0xA1,0x00,0xFC,0x05,0x19,0x0F,0x83,0xEB,0x4D},
};

typedef struct tCtrIdEQIdMap
{
    eDspSettId dspSettid;
    eSettingId settingId;
}tCtrIdEQIdMap;

void TAS5711_SoftMute(BOOL on_off);
void TAS5711_HardMute(BOOL mute_mode);
void TAS5711_EQSwitch(BOOL bypass_mode);
void TAS5711_Bass(int8 bass_level);
void TAS5711_Treble(int8 treble_level);
void TAS5711_Volume(uint8 volume);
void TAS5711_SetGainTwitters(uint8 gain);
void TAS5711_SetGainWoofer(uint8 gain);
void TAS5711_Load_EQ();
void TAS5711_Init();
#endif 
