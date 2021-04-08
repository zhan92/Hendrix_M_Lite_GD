/*
-------------------------------------------------------------------------------
TYMPHANY LTD





                  Public project Bsp interfaces
                  -------------------------

                  SW Module Document




@file        projBsp.h
@brief       This file declares the public project bsp interfaces, user should
             implement these interfaces based on the requiremnets and
             the implementation should be located at projBsp.c
@author      Bob.Xu
@date        2014-06-12
@copyright (c) Tymphany Ltd. All rights reserved.

Change History:
VERSION    : 1    DRAFT      2014-06-12     Bob.Xu
DESCRIPTION: First Draft. Generated by newclass.py
SCO/ERROR  :
-------------------------------------------------------------------------------
*/
#ifndef PROJ_BSP_H
#define PROJ_BSP_H

#include "product.config"

/* Microchip XC32 compiler always ignore "inline".
 * We should use " __attribute__((always_inline))"
 */
#define INLINE inline __attribute__((always_inline))

#define BSP_TICKS_PER_SEC       1000U
#define GET_TICKS_IN_MS(x)      (x * BSP_TICKS_PER_SEC / 1000)

#ifdef __DEBUG //connect to debugger and run the program
    #define DEBUGGER_PAUSE() __asm volatile ("sdbbp 0")
    #define DEBUGGER_NOP()   __asm volatile ("nop")
#else
    #define DEBUGGER_PAUSE()
    #define DEBUGGER_NOP()
#endif

/* Note core timer will overflow every 214.748 seconds. (under clock frequency 40MHz)
*/
#define GetSystemTimeUs()       ( ReadCoreTimer()/(CPU_OSC_FREQ_HZ/2/1000000) )
#define GetSystemTimeMs()       ( ReadCoreTimer()/(CPU_OSC_FREQ_HZ/2/1000   ) )

void ProjBsp_CyclePrintError(char* errString);


void ProjBsp_JumpToBootloader();

void ProjBsp_SoftReboot();
#endif /* End of PROJ_BSP_H */