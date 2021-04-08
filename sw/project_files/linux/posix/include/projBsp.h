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
#define BSP_TICKS_PER_SEC       1000U
#define GET_TICKS_IN_MS(x)      (x * BSP_TICKS_PER_SEC / 1000)
#define INLINE                  inline

void ProjBsp_CyclePrintError(char* errString);

#endif /* End of PROJ_BSP_H */