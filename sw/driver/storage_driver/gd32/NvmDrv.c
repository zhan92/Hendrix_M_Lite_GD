/*
-------------------------------------------------------------------------------
TYMPHANY LTD





                  Nvm Driver
                  -------------------------

                  SW Module Document




@file        NvmDrv.c
@brief       It's the driver to read/write Non-Volatile Memory (NVM) of STM32F0
@author      Wesley Lee
@date        2014-07-01
@copyright (c) Tymphany Ltd. All rights reserved.

Change History:
VERSION    : 1    DRAFT      2014-07-01     Wesley Lee

DESCRIPTION: First Draft. Generated by newclass.py.
SCO/ERROR  :
-------------------------------------------------------------------------------
*/

#include "stm32f0xx_flash.h"
#include "trace.h"
#include "crc16.h"
#include "NvmDrv_priv.h"

/* Private functions / variables. Declare and drivers here */

/*****************************************************************************************************************
 *
 * Start-up / shut-down functions
 *
 *****************************************************************************************************************/
#ifdef HAS_CBUFFER
#include "NvmDrv.config"

static tCyclicBufferPrivConfig cycBufPrivConf[CBUF_BUFFERS_NUM];

void NvmDrv_InitCBuffer(cStorageDrv *me, uint8 cbufId);
bool NvmDrv_GetCurrCBuffSlot(cStorageDrv *me, uint8 cbufId, int32 * currentSlot);
void NvmDrv_CheckCBuffers(cNvmDrv *me);

void NvmDrv_InitCBuffer(cStorageDrv *me, uint8 cbufId)
{
    uint32 magicWord = CBUF_MAGIC_WORD;

    NvmDrv_ErasePage((cStorageDrv*)me, cycBufPrivConf[cbufId].pageAddress);
    NvmDrv_WriteWords((cStorageDrv*)me, cycBufPrivConf[cbufId].pageAddress, (uint8*)&magicWord, CBUF_MAGIC_WORD_LEN);
    cycBufPrivConf[cbufId].currentSlot = CBUF_INVALID_SLOT;
}

bool NvmDrv_GetCurrCBuffSlot(cStorageDrv *me, uint8 cbufId, int32 * currentSlot)
{
    uint32 status0 = 0;
    uint32 status1 = 1;
    uint8 ii = 0;

    for (ii = 1; ii < cycBufPrivConf[cbufId].slotNumber; ii++)
    {
        NvmDrv_ReadWords((cStorageDrv*)me, (cycBufPrivConf[cbufId].pageAddress + CBUF_MAGIC_WORD_LEN + (ii * CBUF_STAT_SLOT_LEN)), (uint8*)&status1, 4);

        if (1 != (status1 - status0))
        {
            *currentSlot = ii - 1;
            return TRUE;
        }
        status0 = status1;
    }
    return FALSE;
}

void NvmDrv_CheckCBuffers(cNvmDrv *me)
{
    uint8 ii = 0;
    uint32 magicWord = 0;

    for (ii = 0; ii < CBUF_BUFFERS_NUM; ii++)
    {
        cycBufPrivConf[ii].pageAddress  = cycBufPubConf[ii].pageAddress;
        cycBufPrivConf[ii].dataLength   = cycBufPubConf[ii].dataLength;
        cycBufPrivConf[ii].currentSlot  = CBUF_INVALID_SLOT;
        cycBufPrivConf[ii].slotNumber   = ((CBUF_PAGE_SIZE - CBUF_MAGIC_WORD_LEN) / (CBUF_MAGIC_WORD_LEN + CBUF_CRC16_SLOT_LEN + CBUF_DATA_SLOT_LEN_0));
        cycBufPrivConf[ii].headerLength = (CBUF_MAGIC_WORD_LEN + CBUF_STAT_SLOT_LEN * ((CBUF_PAGE_SIZE - CBUF_MAGIC_WORD_LEN) / (CBUF_MAGIC_WORD_LEN + CBUF_CRC16_SLOT_LEN + CBUF_DATA_SLOT_LEN_0)));

        NvmDrv_ReadWords ((cStorageDrv*)me, cycBufPrivConf[ii].pageAddress, (uint8*) &magicWord, CBUF_MAGIC_WORD_LEN);
        if (CBUF_MAGIC_WORD != magicWord)
        {
            NvmDrv_InitCBuffer((cStorageDrv*)me, ii);
        }
        else
        {
            if (FALSE == NvmDrv_GetCurrCBuffSlot((cStorageDrv*)me, ii, &cycBufPrivConf[ii].currentSlot))
            {
                NvmDrv_InitCBuffer((cStorageDrv*)me, ii);
            }
        }
    }
}

static bool NvmDrv_WriteBuffer(cStorageDrv *me, uint8 cbufId, uint8* pBuf)
{
    uint32 CRC16 = 0;

    if (cycBufPrivConf[cbufId].currentSlot >= (cycBufPrivConf[cbufId].slotNumber - 1))
    {
        NvmDrv_InitCBuffer(me, cbufId);
    }
    cycBufPrivConf[cbufId].currentSlot++;

    CRC16 = crc16(pBuf, cycBufPrivConf[cbufId].dataLength);

    NvmDrv_WriteWords(me, /*write data*/
       /*address->*/ (cycBufPrivConf[cbufId].pageAddress + cycBufPrivConf[cbufId].headerLength + cycBufPrivConf[cbufId].currentSlot * (cycBufPrivConf[cbufId].dataLength + CBUF_CRC16_SLOT_LEN)),
          /*data->*/ (pBuf),
        /*length->*/ (cycBufPrivConf[cbufId].dataLength));

    NvmDrv_WriteWords(me, /*write data crc16*/
       /*address->*/ (cycBufPrivConf[cbufId].pageAddress + cycBufPrivConf[cbufId].headerLength + cycBufPrivConf[cbufId].currentSlot * (cycBufPrivConf[cbufId].dataLength + CBUF_CRC16_SLOT_LEN) + cycBufPrivConf[cbufId].dataLength),
          /*data->*/ ((uint8*)&CRC16),
        /*length->*/ (CBUF_CRC16_SLOT_LEN));

    NvmDrv_WriteWords(me, /*write status slot*/
       /*address->*/ (cycBufPrivConf[cbufId].pageAddress + CBUF_MAGIC_WORD_LEN + cycBufPrivConf[cbufId].currentSlot * CBUF_STAT_SLOT_LEN),
          /*data->*/ ((uint8*)&cycBufPrivConf[cbufId].currentSlot),
        /*length->*/ (CBUF_STAT_SLOT_LEN));

    return NvmDrv_IsError();
}

static bool NvmDrv_ReadBuffer(cStorageDrv *me, uint8 cbufId, uint8* pBuf)
{
    uint32 CRC16 = 0;

    if (CBUF_INVALID_SLOT != cycBufPrivConf[cbufId].currentSlot
     && (cycBufPrivConf[cbufId].currentSlot < (cycBufPrivConf[cbufId].slotNumber)))
    {
        NvmDrv_ReadWords(me,
          /*address->*/ (cycBufPrivConf[cbufId].pageAddress + cycBufPrivConf[cbufId].headerLength + cycBufPrivConf[cbufId].currentSlot * (cycBufPrivConf[cbufId].dataLength + CBUF_CRC16_SLOT_LEN)),
             /*data->*/ (pBuf),
           /*length->*/ (cycBufPrivConf[cbufId].dataLength));

        NvmDrv_ReadWords(me,
          /*address->*/ (cycBufPrivConf[cbufId].pageAddress + cycBufPrivConf[cbufId].headerLength + cycBufPrivConf[cbufId].currentSlot * (cycBufPrivConf[cbufId].dataLength + CBUF_CRC16_SLOT_LEN) + cycBufPrivConf[cbufId].dataLength),
             /*data->*/ ((uint8*)&CRC16),
           /*length->*/ (CBUF_CRC16_SLOT_LEN));

        if (CRC16 == crc16(pBuf, cycBufPrivConf[cbufId].dataLength))
        {
            return TRUE;
        }
        else
        {
            NvmDrv_InitCBuffer(me, cbufId);
        }
    }
    return FALSE;
}

#endif
void NvmDrv_Ctor(cNvmDrv *me)
{
    ASSERT(me);
    
	/* Clear pending flags (if any) */
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);

    me->super_.pStorageConfig = (tStorageDevice*) getDevicebyId(INT_FLASH_DEV_ID, NULL);
    /* set up the function */
    me->super_.SetValue = NvmDrv_WriteWords;
    me->super_.GetValue = NvmDrv_ReadWords;
    me->super_.ErasePage    = NvmDrv_ErasePage;

#ifdef HAS_CBUFFER
    me->super_.SetValueCBuffer = NvmDrv_WriteBuffer;
    me->super_.GetValueCBuffer = NvmDrv_ReadBuffer;

    NvmDrv_CheckCBuffers(me);
#endif

}

void NvmDrv_Xtor(cNvmDrv *me)
{
    ASSERT(me);
}

/*****************************************************************************************************************
 *
 * private functions
 *
 *****************************************************************************************************************/
/* @brief           Write the NVM with the given buffer
 * @param   me      Storage Driver object
 * @param   addr    Address start to write
 * @param   data    pointer to data to write
 * @param   sizeInBytes
 * @return
 * @warning data stream should be in size of multiple of 4 (i.e. word)
 * @warning STM32F030 is little-endian
 * @warning The content of address to be written must be 0xFF
 */
static bool NvmDrv_WriteWords(cStorageDrv *me, uint32 addr, uint8* pBuf, uint32 sizeInBytes)
{
    ASSERT(sizeInBytes % 4 == 0);   // Words should be in size of multiple of 4
	uint32 i = 0;
    FLASH_Status s;

	FLASH_Unlock();
	for ( ; i<sizeInBytes; i+=4)
	{
        uint8   *p  = pBuf + i;
        uint32  d   = *p | *(p+1)<<8 | *(p+2)<<16 | *(p+3)<<24;
        s = FLASH_ProgramWord(addr + i, d);
        if (s != FLASH_COMPLETE)
        {
            break;
        }
	}
	FLASH_Lock();

    return NvmDrv_IsError();
}

/* @brief           Read the NVM content
 * @param   me      Storage Driver object
 * @param   addr    Address start to write
 * @param   data    pointer to data to write
 * @param   sizeInBytes
 * @return
 * @warning STM32F030 is little-endian
 */
static bool NvmDrv_ReadWords(cStorageDrv *me, uint32 addr, uint8* pBuf, uint32 sizeInBytes)
{
	uint8* p = (uint8*)addr;

    memcpy((void *)pBuf, p, sizeInBytes);
    return NvmDrv_IsError();
}

/* @brief   Erase Page
 * @param   me      Storage Drive object
 * @param   addr    Address to Erase
 */
static bool NvmDrv_ErasePage(cStorageDrv *me, uint32 addr)
{
    FLASH_Unlock();
    FLASH_Status s = FLASH_ErasePage(addr);
    FLASH_Lock();
    return NvmDrv_IsError();
}

/* @brief   Check if write was done successuffly
 * @return  TRUE - complete, FALSE - not done.
 */
static bool NvmDrv_IsError(void)
{
    return (FLASH_GetStatus() == FLASH_COMPLETE);
}


/***************************************************************************
 * Single word read/write, just keep them for bootloader usage for now
 **************************************************************************/
bool NvmDrv_WriteWord(uint32 addr, uint32 wData)
{
    return FALSE;
}

//read a single word
bool NvmDrv_ReadWord(uint32 addr, uint32* pReadData)
{
	uint32* p = (uint32*)addr;
    *pReadData = *p;
    return NvmDrv_IsError();
}


/***************************************************************************
 * Below functions is not used, but keep it here for now
 **************************************************************************/
//erase the whole NVM storage NVM_DATA
BOOL NvmDrv_EraseAll(cNvmDrv *me)
{
    return NvmDrv_IsError();
}
