#ifndef DSPDRV_V2_H
#define DSPDRV_V2_H
#include "cplus.h"
#include "commonTypes.h"
#include "deviceTypes.h"
#include "I2CDrv.h"

typedef struct
{
    void (*initSectionFunc)(void* me);         // function point to a specific initialization section
    uint16 delaytime;   // time duration to next intialization section in ms
}tDspInitSection;

CLASS(cDSPDrv1452)
    bool            isCreated:1;
    tDspInitSection const *pInitTable;
    uint8           sectionSize;
    uint8           initPhase;
    uint16          max_vol;
    uint16          default_vol;
    cI2CDrv         *i2cObj;  /* A i2c obj should be ctored before ctor dspDrv*/
    uint8           deviceAddr;
    bool            i2cEnable; /* When DSP cable insert, disable I2C */
METHODS

/**
 * Construct the DSP driver instance.
 * @param      me - instance of the driver
 */
void DSPDrv1452_Ctor(cDSPDrv1452* me, cI2CDrv *pI2cObj);

/**
 * Destruct the DSP driver instance.
 * @param      me - instance of the driver
 */
void DSPDrv1452_Xtor(cDSPDrv1452* me);


/**
 * Set DSP initialization function
 * @param  -    init section state
 * @return -    return the delay time following if the init is not over
 *              return zero if the init is over
 */
uint16 DSPDrv1452_Init(cDSPDrv1452 * me);

/**
* Audio related setting intrance
* @param[in]    me             dsp object
* @param[in]    dspSettId      dsp setting ID
* @param[in]    enable         disable or enable the settings
*/
void DSPDrv1452_SetAudio(cDSPDrv1452 *me, eAudioSettId dspSettId, BOOL enable);

/**
 * Un-Mutes the Lineout
 *
 * @param      void
 * @return     void
 */
void DSPDrv1452_Mute(cDSPDrv1452 *me, eAudioMuteType muteType, BOOL muteEnable);

/**
 * Gets the status of the aux-in jack status
 *
 * @param      void
 * @return     eAuxinStatus         the auxin jack is plugged or not
 */
bool DSPDrv1452_IsAuxin(cDSPDrv1452 *me);

/**
 * Gets the status of the music streaming status
 *
 * @param      void
 * @return     bool         if there is music or not
 */
BOOL DSPDrv1452_HasMusicStream(cDSPDrv1452 *me);

/**
 * Select channel whether I2S or Analog
 * @param  -    channel
 */
void DSPDrv1452_SetInputChannel(cDSPDrv1452 *me, eAudioChannel inputChannel);

/**
 * Set the limiter level
 * @param  limiterLevel     limiter level in dB
 */
void DspDrv1452_SetLimiter(cDSPDrv1452 *me, int16 limiterLevel);


/**
 * Set I2S master/slave mode
 * @param  isMaster    : master or slave mode
 */
void DspDrv1452_SetI2Smode(cDSPDrv1452 *me, BOOL isMaster);

bool DSPDrv1452_IsPllLock(cDSPDrv1452 *me);

void DSPDrv1452_SetSpeakerPosition(cDSPDrv1452 *me, eSpeakerPosition position);

END_CLASS

#endif /* DSPDRV__V2_H */
