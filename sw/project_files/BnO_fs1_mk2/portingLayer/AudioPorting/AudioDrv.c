/*
-------------------------------------------------------------------------------
TYMPHANY LTD





                  Audio Driver
                  -------------------------

                  SW Module Document
 
@file        AudioDrv.c
@brief       This file implementes the middle layer of the audio service BnO Playbox Specific
@author      Edmond Sung
@date        2015-10-15
@copyright (c) Tymphany Ltd. All rights reserved.

Audio Driver have three layer DISABLE operation.
1. SW Mute     -> FepAseCommand_Command_MUTE, FepAseCommand_Command_UNMUTE
2. SW Shutdown -> AseFepEvent_Event_SYSTEM_STATUS_STANDBY, AseFepEvent_Event_SYSTEM_STATUS_ON
3. HW Shutdown -> Overheat occurs/recover

Change History:
VERSION    : 1    DRAFT      2015-06-15     Bob.Xu 
DESCRIPTION: First Draft. Generated by newclass.py
SCO/ERROR  : 
-------------------------------------------------------------------------------
*/

#include "./AudioDrv_priv.h"
#include "PowerDrv_v2.h"
#include "SettingSrv.h"
#include "trace.h"
#include "AudioSrv.h"
#include "AseNgSrv.h"
#include "I2CDrv.h"
#include "ADCDrv.h"
#include "GPIODrv.h"
#include "DspDrv1451.h"
#include "AmpDrvTas5760.h"
#include "timer.h"
#include "AudioDrv.config"
#include "AudioSrv.config"
#include "math.h"


/***********************************************
 * Type
 ***********************************************/
typedef struct tCtrIdEQIdMap
{
    eAudioSettId dspSettid;
    eSettingId settingId;
}tCtrIdEQIdMap;

static tCtrIdEQIdMap ctrIdEQIdMap[] =
{
    /* DSP setting ID  index of setting db*/
    {DSP_CAL_GAIN1_ID,    SETID_DSP_CAL_GAIN1_WF},
    {DSP_CAL_GAIN2_ID,    SETID_DSP_CAL_GAIN2_TW},
};


#define PVDD_MAX_mVOLT (24000)
#define PVDD_MAX_ADC   (2708)
#define PVDD_TO_mVOLT(adc)  (int16)((adc * PVDD_MAX_mVOLT) / PVDD_MAX_ADC)



/***********************************************
 * Global Variable
 ***********************************************/
static cI2CDrv      dspI2cDrv;
static cDSPDrv1451  dspAdau1451Drv;

static cI2CDrv      ampWooferI2cDrv;
static cAudioAmpDrv audioAmpWooferDrv;

static cI2CDrv      ampTweeterI2cDrv;
static cAudioAmpDrv audioAmpTweeterDrv;

static cADCDrv      audioAdcDrv;

static cGpioDrv audioGpioDrv;

static bool audioOverheat= FALSE;

#ifdef HAS_DSP_TUNING_DETECT
static bool dspTuneCurr= FALSE;
static bool dspTuneTmp= FALSE;
static uint32 debounceTick= 0; //the time to hold the same DspTune value
#endif

static uint16        tempWfAmp    = 0;
static uint16        tempWfSpk    = 0;
static uint16        tempTwSpk    = 0;

static eTempLevel    tempLevelWfAmp = TL_NORMAL;
static eTempLevel    tempLevelWfSpk = TL_NORMAL;
static eTempLevel    tempLevelTwSpk = TL_NORMAL;

static eTempLevel    tempLevelAudio = TL_NORMAL;


/* fading volume */
static uint16 volFadeTimerId= 0;
static tVolFadeParam volFadeParam= {0};
static uint32 volFadeReqId = 0;


/* Private functions / variables. Declare and drivers here */

/*****************************************************************************************************************
 *
 * Start-up / shut-down functions
 *
 *****************************************************************************************************************/
void AudioDrv_Ctor(cAudioDrv *me)
{
    /* When bootup, AudioSrv call AudioDrv_Ctor() very soon, wait few seconds (after ASE-TK MCLK is ready),
     * then execute AudioDrv_Init() to initialize DSP.
     */
    ASSERT(me);
    audioOverheat= FALSE;
    
#ifdef HAS_DSP_TUNING_DETECT
    dspTuneCurr= FALSE;
    dspTuneTmp= FALSE;
    debounceTick= 0;
#endif    

    //Initialize Audio ADC pins
    audioAdcDrv.ADCConfig= (tADCDevice*)getDevicebyIdAndType(AUDIO_DEV_ID, ADC_DEV_TYPE, NULL);
    ASSERT(audioAdcDrv.ADCConfig);
    ADCDrv_Ctor(&audioAdcDrv, audioAdcDrv.ADCConfig);
    ADCDrv_StartScanning(&audioAdcDrv);    
    
    //Initialize Audio GPIO pins
    audioGpioDrv.gpioConfig= (tGPIODevice*)getDevicebyIdAndType(AUDIO_DEV_ID, GPIO_DEV_TYPE, NULL);
    ASSERT(audioGpioDrv.gpioConfig);
    GpioDrv_Ctor(&audioGpioDrv, audioGpioDrv.gpioConfig);
    
    //Initialize DSP
    dspI2cDrv.pConfig = (tI2CDevice*)getDevicebyIdAndType(DSP_DEV_ID, I2C_DEV_TYPE, NULL);
    ASSERT(dspI2cDrv.pConfig);
    DSPDrv1451_Ctor(&dspAdau1451Drv, &dspI2cDrv);

    //Shutdown all amplifier
    AMP_SHUTDOWN(audioGpioDrv); //pull low SDZ
    
    uint16 printTempPeriod   = AUDIO_PRINT_PREIOD_IN_MS;
    if (FALSE == Setting_IsReady(SETID_TEMP_PRINT_PERIOD))
    {
        Setting_Set(SETID_TEMP_PRINT_PERIOD,   &printTempPeriod);
    }
    
    //Enable power for DSP/AMP/NTC
    //SYSPWR_ENABLE(audioGpioDrv);
}

void AudioDrv_Xtor(cAudioDrv *me)
{
    AudioAmpDrv_Xtor(&audioAmpWooferDrv);
    AudioAmpDrv_Xtor(&audioAmpTweeterDrv);
    DSPDrv1451_Xtor(&dspAdau1451Drv);
    
    //Disable power for DSP/AMP/NTC
    SYSPWR_DISABLE(audioGpioDrv);

    //Shutdown all amplifier
    AMP_SHUTDOWN(audioGpioDrv); //pull low SDZ   

    //Reset all temperature, to avoid MainApp power down again when wakeup
    Setting_Reset(SETID_TEMP_WF_AMP);
    Setting_Reset(SETID_TEMP_WF_SPK);
    Setting_Reset(SETID_TEMP_TW_SPK);
    Setting_Reset(SETID_TEMP_LEVEL_AUDIO);    
    
    me->drvIsReady = FALSE;
}


/**
 * @Function to Init the low layer Audio related driver
 * @return This function return TRUE when audioDrv initialization is finished.
 */
BOOL AudioDrv_Init(cAudioDrv *me)
{
    if(!(me->drvIsReady))
    {        
        if(me->nextDelayTime == 0)
        {
            //Enable power for DSP/AMP/NTC
            SYSPWR_ENABLE(audioGpioDrv);
        }

        me->nextDelayTime = DSPDrv1451_Init(&dspAdau1451Drv);            
        if(me->nextDelayTime == 0) 
        {         
            //Read DSP Version
            float fVer= DSPDrv1451_GetDspVer(&dspAdau1451Drv);
            char dspVersion[DSP_VERSION_LENGTH]= {0};
            snprintf(dspVersion, sizeof(dspVersion), "%0.2f", fVer);
            Setting_Set(SETID_DSP_VER, dspVersion);

#ifdef MCU_STORE_DSP_GAIN
            //Set DSP calibration gain, read 0xFFFFFFFF means never set
            #define SETTING_INAVLID_VALUE  0xFFFFFFFF
            const float defaultGainDb= 0.0;
            const float* p_gainLinearWoofer=  Setting_Get(SETID_DSP_CAL_GAIN1_WF); //Woofer (gain1)
            if((*(uint32*)p_gainLinearWoofer)!=SETTING_INAVLID_VALUE) {
                TP_PRINTF("\r\n\r\n*** DSP: Flash have WOOFER gain: %f ***\r\n\r\n", *p_gainLinearWoofer);
                DSPDrv1451_SetCalibrateGain(&dspAdau1451Drv, DSP_CAL_GAIN1_ID, *p_gainLinearWoofer);
            }
            else {
                TP_PRINTF("\r\n\r\n*** DSP: Flash do not have WOOFER gain, set default: %f ***\r\n\r\n", defaultGainDb);
                DSPDrv1451_SetCalibrateGain(&dspAdau1451Drv, DSP_CAL_GAIN1_ID, defaultGainDb);
                //Setting_Set(SETID_DSP_CAL_GAIN1_WF, &defaultGainDb);
            }

            const float* p_gainLinearTweeter= Setting_Get(SETID_DSP_CAL_GAIN2_TW); //Tweeter (gain2)
            if((*(uint32*)p_gainLinearTweeter)!=SETTING_INAVLID_VALUE) {
                TP_PRINTF("\r\n\r\n*** DSP: Flash have TWETTER gain: %f ***\r\n\r\n", *p_gainLinearTweeter);
                DSPDrv1451_SetCalibrateGain(&dspAdau1451Drv, DSP_CAL_GAIN2_ID, *p_gainLinearTweeter);
            }
            else {              
                TP_PRINTF("\r\n\r\n*** DSP: Flash do not have TWETTER gain, set default: %f ***\r\n\r\n", defaultGainDb);
                DSPDrv1451_SetCalibrateGain(&dspAdau1451Drv, DSP_CAL_GAIN2_ID, defaultGainDb);
                //Setting_Set(SETID_DSP_CAL_GAIN2_TW, &defaultGainDb);
            }
#else
            DSPDrv1451_SetCalibrateGain(&dspAdau1451Drv, DSP_CAL_GAIN1_ID, 0.0/*dB*/);
            DSPDrv1451_SetCalibrateGain(&dspAdau1451Drv, DSP_CAL_GAIN2_ID, 0.0/*dB*/);
#endif
            /* Enable "SPDIF auto restart when remove cable then re-insert", to avoid production line problem
             * Note B&O old DSP flow (v2.3) do not enable, thus we manually enable here.
             */
            DSPDrv1451_SetSpdifAutoRestart(&dspAdau1451Drv, /*enable:*/TRUE);

            //Set DSP channel
            DSPDrv1451_SetInputChannel(&dspAdau1451Drv, AUDIO_CHANNEL_I2S_1); //ASE

            //Set initial DC status to DSP: DC=on, do not enable dynamic boost
            DSPDrv1451_SetDcStatus(&dspAdau1451Drv, /*dc_on:*/TRUE);
            
            /* Set default volume
             * Note ASE-TK may output noise when bootup, thus we must set ZERO volume before AES-TK boot up.
             */
            //uint32 vol_readback= DSPDrv1451_ReadDcValue32_0(&dspAdau1451Drv, /*reg:*/ADAU1451_VOLUME_ADDR);
            //TP_PRINTF("\r\n\r\n\r\nDSP flow initial vol = %d\r\n\r\n\r\n", vol_readback);
            DSPDrv1451_SetVol(&dspAdau1451Drv, /*vol*/0);
            
            //Default DSP parameters         
            DSPDrv1451_SetBase(&dspAdau1451Drv, DSP_PARAM_BASS);
            DSPDrv1451_SetTreble(&dspAdau1451Drv, DSP_PARAM_TREBLE);
            DSPDrv1451_SetPosition(&dspAdau1451Drv, DSP_PARAM_POSITION);
            DSPDrv1451_SetLoudness(&dspAdau1451Drv, DSP_PARAM_LOUDNESS);
            
            //Initialize Woofer Amplifier
            TP_PRINTF("Init Woofer Amplifier\r\n");
            ampWooferI2cDrv.pConfig = (tI2CDevice*)getDevicebyIdAndType(AMP_WOOFER_1_DEV_ID, I2C_DEV_TYPE, NULL);
            ASSERT(ampWooferI2cDrv.pConfig);
            AudioAmpDrv_Ctor(&audioAmpWooferDrv, &ampWooferI2cDrv);

            //Initialize Tweeter Amplifier 
            TP_PRINTF("Init Tweeter Amplifier\r\n");
            ampTweeterI2cDrv.pConfig = (tI2CDevice*)getDevicebyIdAndType(AMP_TWEETER_1_DEV_ID, I2C_DEV_TYPE, NULL);
            ASSERT(ampTweeterI2cDrv.pConfig);
            AudioAmpDrv_Ctor(&audioAmpTweeterDrv, &ampTweeterI2cDrv);
            
            /* 1. Set amplifer to PBTL mode, means two output to one speaker
             * 2. Datasheet said, PBTL select right channel when REG[6].bit1==1, left chanel when REG[6].bit1==0.
             *    But accurary result is inverse.
             * 2. Vpk should < amplifier PVDD(15~24v), or analog output will clip on PVDD
             */
            //TODO: move this customization to attachedDevices.c
            TP_PRINTF("Set Amplifier Analog Control\r\n");
            AudioAmpDrv_setAnalogControl(&audioAmpWooferDrv, 0x97); //PBTL=1, PWM Rate=LRCK*8=384KHz, Analog Gain=22.6dBV(Vpk=19.08V), PBTL Channel selection=left
            AudioAmpDrv_setAnalogControl(&audioAmpTweeterDrv, 0xB5); //PBTL=1, PWM Rate=LRCK*12=576KHz, Analog Gain=22.6dBV(Vpk=19.08V), PBTL Channel selection=right

            /* It is important to note that control port register changes should only occur when the device is placed into
             *  shutdown. This can be accomplished either by pulling the SPK_SD pin "LOW" or clearing the SPK_SD bit in the
             *  control port.
             */            
            /* Pull SDZ to high after I2C programming */
            AMP_WAKEUP(audioGpioDrv);
            
            //AudioAmpDrv_printError(&audioAmpWoofer1Drv);
            AudioDrv_Mute(AUDIO_AMP_SOFT_MUTE, /*muteEnable:*/TRUE);
            AudioAmpDrv_setShutdown(&audioAmpWooferDrv, TRUE);
            AudioAmpDrv_setShutdown(&audioAmpTweeterDrv, TRUE);
            
            //Finish
            me->drvIsReady = TRUE;

            memset(&volFadeParam, 0, sizeof(volFadeParam));
            volFadeParam.isFadingInProg = FALSE;
            uint8 defaultVol = 0;
            Setting_Set(SETID_VOLUME, &defaultVol);
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    
    return TRUE;
}

void AudioDrv_Reset(cAudioDrv *me)
{
    me->drvIsReady = FALSE;
}
/*****************************************************************************************************************
 *
 * Public functions
 *
 *****************************************************************************************************************/
/**
 * Function to change the channel. To call this function, normally you have to
 * call AudioDrv_Mute to mute the system in order to avoid pop noise, then set 
 * the channel and last,call AudioDrv_Mute to umute the system
 * @param[in]     theChannelToSet, the channel which will replace the current channel
 */
void AudioDrv_SetChannel(eAudioChannel audioChannel)
{
    DSPDrv1451_SetInputChannel(&dspAdau1451Drv, audioChannel);
}


void AudioDrv_dumpAmpReg(void)
{
    AudioAmpDrv_dumpReg(&audioAmpWooferDrv, "ampWoofer");
    AudioAmpDrv_dumpReg(&audioAmpTweeterDrv, "ampTweeter"); 
}

/**
 * Function to update audio status including jackin status,  music steam status and so on.
 */
void AudioDrv_UpdateStatus(cAudioDrv *me)
{
    AudioDrv_AmpHealthMonitor(me);
    AudioDrv_AmpSafetyMonitor(me);
#ifdef HAS_DSP_TUNING_DETECT    
    AudioDrv_DspTuneUpdate(me);
#endif
}


static void AudioDrv_StopFadeVolume()
{
    if (volFadeParam.isFadingInProg == TRUE)
    {
        TP_PRINTF("Volume fade: Stop volume fading and reply ASE (red_id=%d)\r\n", volFadeReqId);
        AseNgSrv_ReplyVolumeFade(volFadeReqId, TRUE);
        bool ret= Timer_StopTimer(volFadeTimerId);
        ASSERT(ret); //FALSE means Fade volume timer is not running
        volFadeParam.isFadingInProg = FALSE;
        volFadeReqId = 0;
    }
}


static void AudioDrv_SetFadeVolume(cAudioDrv *me, uint32 target_volume, uint32 fade_duration)
{
    if (volFadeParam.isFadingInProg == TRUE)
    {
        TP_PRINTF("    Volume fade: Stop previous fade volume and start new one\r\n");
        bool ret= Timer_StopTimer(volFadeTimerId);
        ASSERT(ret); //FALSE means Fade volume timer is not running
    }
    
        
    /* The start volume should always be equal to current volume. */
    uint8 start_volume = *(uint8*)Setting_Get(SETID_VOLUME);
    uint32 fadeRange = TYM_DIFF(target_volume, start_volume);
        
    /* fade_duration should not be less than ADUIO_MIN_FADE_STEP_TIME */
    if(fade_duration < ADUIO_MIN_FADE_STEP_TIME)
    {
        fade_duration= ADUIO_MIN_FADE_STEP_TIME;
    }
    
    if(((fadeRange <= (MAX_VOLUME - MIN_VOLUME)) && fadeRange > 0)
        && (target_volume >= MIN_VOLUME && target_volume <= MAX_VOLUME)
        && (start_volume >= MIN_VOLUME && start_volume <= MAX_VOLUME))
    {
        volFadeParam.fadeType     = (target_volume > start_volume)? TRUE : FALSE;
        volFadeParam.fadeStep     = ADUIO_MIN_FADE_STEP;
        volFadeParam.fadeStepTime = (fade_duration * volFadeParam.fadeStep)/fadeRange;
        volFadeParam.targetVol    = target_volume;
        volFadeParam.isFadingInProg = TRUE;
        if(ADUIO_MIN_FADE_STEP_TIME > volFadeParam.fadeStepTime)
        {
            volFadeParam.fadeStepTime = ADUIO_MIN_FADE_STEP_TIME;
            volFadeParam.fadeStep = (volFadeParam.fadeStepTime * fadeRange)/fade_duration;
            ASSERT(volFadeParam.fadeStep>0);
        }
        Timer_StartTimer((uint32)volFadeParam.fadeStepTime, &volFadeTimerId, AudioDrv_VolFadeTimerCallBack, &volFadeParam);
    }
    else
    {
        TP_PRINTF("The volume fade parameters are invalid. \r\n");
        AseNgSrv_ReplyVolumeFade(volFadeReqId, TRUE);
    }
    

}

/**
 * Function to change audio settings, such as EQ.
 * @param[in]     pAudioEvt, a pointer of the audio event
 */
void AudioDrv_SetAudio(cAudioDrv *me, eAudioSettId audioSettId, BOOL enabled, uint32 param, uint32 param2)
{
    ASSERT(audioSettId < AUDIO_SETT_ID_MAX);

    /* When bootup, DSP do not initialize, do not access DSP on this moment */
    if(!(me->drvIsReady)) {
        TP_PRINTF("AudioDrv_SetAudio: driver is not ready\r\n");
        return;
    }

    switch (audioSettId)
    {
        case DSP_CAL_GAIN1_ID: //Woofer
        case DSP_CAL_GAIN2_ID: //Tweeter
        {
            float fGainDb= *((float*)&param);
            
            /* Golden sample will write gainAllow=1, means do not allow gain writeing and keep gain to 0dB
             */
            uint32 gainAllow= TRUE;
            gainAllow= *(uint32*)Setting_GetEx(SETID_GAIN_ALLOW, &gainAllow);
            
            /* Normal sample did not write gainAllow, the initial value is 0xFF, the follower decision treat as TRUE.
             */
            if(gainAllow)
            {
                //TP_PRINTF("AudioDrv_SetAudio: id=(%d)DSP_CAL_GAINx_ID, gain=%1.2f\r\n", audioSettId, fGainDb);
                AudioDrv_DspCalGainSetId_Handler(audioSettId, fGainDb);
            }
            else
            {
                TP_PRINTF("\r\n\r\n\r\n*** AudioDrv_SetAudio: reject gain writing because gainAllow is set (id=%d, gain=%1.2f) ***\r\n\r\n\r\n", audioSettId, fGainDb);
            }
            break;
        }        
            
        case DSP_VOLUME_SETT_ID:
        {
            uint8 volume= (uint8)param;
            //TP_PRINTF("AudioDrv_SetAudio: id=(%d)DSP_VOLUME_SETT_ID\r\n", audioSettId);
            if(!volFadeParam.isFadingInProg)
            {
                TP_PRINTF("AudioDrv_SetAudio: vol=%d\r\n", volume);
                AudioDrv_Set_DspVol(volume);
            }
            else
            {
                TP_PRINTF("AudioDrv_SetAudio: ignore volume change (vol=%d) because fading volume is on-going\r\n", volume);
            }
            break;
        }
        case DSP_PASSTHROUGH_SETT_ID:
            TP_PRINTF("AudioDrv_SetAudio: id=(%d)DSP_PASSTHROUGH_SETT_ID, bypass=%d\r\n", audioSettId, enabled);
            DSPDrv1451_SetPassthrough(&dspAdau1451Drv, enabled);
            break;
            
#ifdef HAS_DSP_DYNA_BOOST
        case DSP_DC_STATUS_SETT_ID:
            TP_PRINTF("AudioDrv_SetAudio: id=(%d)DSP_DC_STATUS_SETT_ID, dc=%d\r\n", audioSettId, enabled);
            DSPDrv1451_SetDcStatus(&dspAdau1451Drv, /*dc_on:*/enabled);
            break;
#endif
            
        case AMP_SLEEP_MODE_ID:
            if(!audioOverheat)
            {
                TP_PRINTF("AudioDrv_SetAudio: AMP_SLEEP_MODE_ID, enabled=%d\r\n", enabled);
                AudioAmpDrv_setShutdown(&audioAmpWooferDrv, enabled);
                AudioAmpDrv_setShutdown(&audioAmpTweeterDrv, enabled);
            }
            break;
            
        case AUDIO_OVERHEAT_MODE_ID:
            AudioDrv_SetOverheat(me, enabled);
            break;
            
        case DSP_TUNNING_SETT_ID:
            /* DSP and AMP I2C bus should be disable when tuning DSP. */
            DSPDrv1451_I2cEnable(&dspAdau1451Drv, !enabled);
            AudioAmpDrv_I2cEnable(&audioAmpWooferDrv, !enabled);
            AudioAmpDrv_I2cEnable(&audioAmpTweeterDrv, !enabled);
            break;
            
        case AUDIO_VOL_FADE_SETT_ID:
        {  /* param ( vv:ffffff    8 bits volume : 24 bits fade duration )
              param2 request id */
            uint32 target_volume= param >> 24;
            volFadeReqId = param2;
            uint32 fade_duration= param & 0xFFFFFF;
            AudioDrv_StopFadeVolume();
            AudioDrv_SetFadeVolume(me, target_volume, fade_duration);
            break;
        }
        case AUDIO_DSP_TREBLE_SETT_ID:
        {
            uint32 value= param;
            DSPDrv1451_SetTreble(&dspAdau1451Drv, value);
            break;
        }
        case AUDIO_DSP_BASS_SETT_ID:
        {
            uint32 value= param;
            DSPDrv1451_SetBase(&dspAdau1451Drv, value);
            break;
        } 
        case AUDIO_DSP_LOUDNESS_SETT_ID:
        {
            bool loudness= (bool)param;
            DSPDrv1451_SetLoudness(&dspAdau1451Drv, loudness);
            break;
        }
        case AUDIO_POS_SOUND_MODE_SETT_ID:
        {
            tDspPosition position= (tDspPosition)param;
            DSPDrv1451_SetPosition(&dspAdau1451Drv, position);
            break;
        }
        default:
             break;
    }
}



static void AudioDrv_DspCalGainSetId_Handler(eAudioSettId audioSettId, float fGainDb)
{
   int32 ctlId= AudioDrv_GetCrtIdIndex(audioSettId);
   ASSERT(ctlId>=0); //-1 means fail
   DSPDrv1451_SetCalibrateGain(&dspAdau1451Drv, audioSettId, fGainDb);
}


static void AudioDrv_VolFadeTimerCallBack(void *pCbPara)
{
    tVolFadeParam* pVolFadeParam = (tVolFadeParam*)pCbPara;
    /* get current volume value */
    uint8 volLevel = *(uint8*)Setting_Get(SETID_VOLUME);

    //TP_PRINTF("AudioDrv_VolFadeTimerCallBack: fadeType=%d, step=%d, vol:(curr=%d,tgt=%d) \r\n", 
    //           pVolFadeParam->fadeType, pVolFadeParam->fadeStep, volLevel, pVolFadeParam->targetVol);
    
    ASSERT(pVolFadeParam->isFadingInProg==TRUE);

    /* For issue FS-202, sometimes AudioDrv_VolFadeTimerCallBack() repeated execute and set the same volume.
     * The only possible condition is fadeStep==0, but there is no reason for it.
     * Before find root cause, we add workaround here.
     */
    if(pVolFadeParam->fadeStep==0)
    {
        ASSERT(0);
        TP_PRINTF("\r\n\r\n\r\n*** ERROR: AudioDrv_VolFadeTimerCallBack fadeStep==0 ***\r\n\r\n\r\n\r\n");
        return;
    }
    
    /* change volume*/
    if(pVolFadeParam->fadeType)
    {
        /* volume fade up*/
        if(pVolFadeParam->targetVol < (volLevel + pVolFadeParam->fadeStep))
        {
            volLevel = pVolFadeParam->targetVol;
        }
        else if(MAX_VOLUME < (volLevel + pVolFadeParam->fadeStep))
        {
            volLevel = MAX_VOLUME;
        }
        else
        {
            volLevel += pVolFadeParam->fadeStep;
        }
    }
    else
    {
        /* volume fade down*/
        if(pVolFadeParam->targetVol > (volLevel - pVolFadeParam->fadeStep))
        {
            volLevel = pVolFadeParam->targetVol;
        }
        else if(pVolFadeParam->fadeStep > (volLevel - MIN_VOLUME))
        {
            volLevel = MIN_VOLUME;
        }
        else
        {
            volLevel -= pVolFadeParam->fadeStep;
        }
    }
    AudioDrv_Set_DspVol(volLevel); //also store volLevel to SETID_VOLUME

    if((volLevel != pVolFadeParam->targetVol)
        && (volLevel < MAX_VOLUME) && (volLevel > MIN_VOLUME))
    {
        Timer_StartTimer(pVolFadeParam->fadeStepTime, &volFadeTimerId, AudioDrv_VolFadeTimerCallBack, pVolFadeParam);
    }
    else
    {
        /* Reply Ase-tk when volume fade completed. */
        AudioDrv_StopFadeVolume();
        TP_PRINTF("Volume fade: finish, vol=%d, req_id=%d\r\n", volLevel, volFadeReqId);
    }
}

/**
 * Function to mute the system.
 * @param[in]   muteType    This parameter defines the way of how to mute the system,
 * you could mute the input source, or mute by dsp or mute the output
 * @param[in]   muteEnable  mute or unmute
 */
void AudioDrv_Mute(eAudioMuteType muteType, bool muteEnable)
{
    ASSERT(muteType < AUDIO_MUTE_TYPE_MAX);
    switch (muteType)
    {
        case AUDIO_AMP_SOFT_MUTE:
            TP_PRINTF("AudioDrv_Mute: mute=%d\r\n", muteEnable);
            AudioAmpDrv_setSoftMute(&audioAmpWooferDrv, muteEnable);
            AudioAmpDrv_setSoftMute(&audioAmpTweeterDrv, muteEnable);
            break;
        default:
            //Support only soft mute
            ASSERT(0);
            break;
    }
 }
    
    
 static void AudioDrv_Set_DspVol(uint8 volLevel)
 {
    DSPDrv1451_SetVol(&dspAdau1451Drv, volLevel);
    Setting_Set(SETID_VOLUME, &volLevel);
 }
 
    

//return -1 means can not find
static int32 AudioDrv_GetCrtIdIndex(eAudioSettId audioSettId)
{
    int32 ret= -1;
    uint8 i;
    for(i = 0; i < ArraySize(ctrIdEQIdMap); i++)
    {
        if(audioSettId == ctrIdEQIdMap[i].dspSettid)
        {
            ret= i;
            break;
        }
    } 
    
    return ret;
}


static uint16 AudioDrv_ConvertNtcToTemperature(uint16 ntcValue)
{
    uint16 degreeC;

    for (degreeC = 0; degreeC < TEMPERATURE_POSITIONS_NUM; degreeC++)
    {
        if (ntcValue >= audioNtcTempTable[degreeC])
        {
            break;
        }
    }
    return degreeC;
}


static void AudioDrv_SetOverheat(cAudioDrv *me, bool overheat)
{  
    /* audioOverheat occurs: Shutdown ampliifer for over-heat
     * recover from audioOverheat: do nothing, but allow MainApp to wakeup amplifier
     */
    TP_PRINTF("AudioDrv_SetOverheat: overheat=%d\r\n", overheat);
    audioOverheat= overheat;
    if(overheat) 
    {
        AMP_SHUTDOWN(audioGpioDrv);
    }
    else
    {
        AMP_WAKEUP(audioGpioDrv);
    }
}


static void AudioDrv_AmpHealthMonitor(cAudioDrv *me)
{
    bool amp_health = (uint32)GpioDrv_ReadBit(&audioGpioDrv, GPIO_IN_AMP_FAULTZ);
    bool ampWoofer_error = FALSE;
    bool ampTweeter_error = FALSE;
	
    if(!amp_health)
    {	
        AudioAmpDrv_getErrorStatus(&audioAmpWooferDrv,  &ampWoofer_error);
        AudioAmpDrv_getErrorStatus(&audioAmpTweeterDrv, &ampTweeter_error);
    }
    
    /* byte[1]: woofer amplifier reason 
     * byte[0]: tweeter amplifier reason 
     */
    uint32 ampErrorReason= (ampWoofer_error<<8) | ampTweeter_error;
    Setting_Set(SETID_AMP_ERROR_REASON, &ampErrorReason);

    bool amp_healthPrev= TRUE;    
    amp_healthPrev= *(bool*)Setting_GetEx(SETID_AMP_HEALTH, &amp_healthPrev);
    Setting_Set(SETID_AMP_HEALTH, &amp_health);
 
    if(amp_healthPrev!=amp_health)
    {
        TP_PRINTF("\r\n\r\n\r\n*** AMP_HEALTH: %d (reason: wf:0x%x, tw:0x%x) ***\r\n\r\n\r\n", amp_health, ampWoofer_error, ampTweeter_error);
    }    
}


static void AudioDrv_AmpSafetyMonitor(cAudioDrv *me)
{  
    uint16 tempWfAmpNew= 0;
    uint16 tempWfSpkNew= 0;
    uint16 tempTwSpkNew= 0;    
    uint16 tempLevelAudioNew = TL_NORMAL;
    int32 rawResult= 0;
    ASSERT(audioAdcDrv.isCreated);

    //(1)Get temperature for Woofer amplifier 
    rawResult = ADCDrv_GetData(&audioAdcDrv, ADC_NTC_WF_AMP);
    if (ADC_DATA_IS_NOT_READY != rawResult)
    {
        tempWfAmpNew = AudioDrv_ConvertNtcToTemperature(rawResult);
        PowerDrv_UpdateTempLevel(tempWfAmpNew, &tempWfAmp, &tempLevelWfAmp, ampTempLevels,  ArraySize(ampTempLevels));
        Setting_Set(SETID_TEMP_WF_AMP, &tempWfAmp);
        DSPDrv1451_WriteDcValue32_0(&dspAdau1451Drv, (uint16)DSP_TEMP_WF_AMP_REGISTER, (int32)tempWfAmpNew );
        //uint32 value= DSPDrv1451_ReadDcValue32_0(&dspAdau1451Drv, DSP_READBACK_TEMP_WF_AMP_REGISTER);
        //TP_PRINTF("DSP Read back TempWfAmp= %d\r\n", value);

        //DSPDrv1451_WriteValue8_24(&dspAdau1451Drv, DSP_TEMP_WF_AMP_REGISTER, ((double)tempWfAmpNew)/100.0 );
        //float fValue= DSPDrv1451_ReadValue8_24(&dspAdau1451Drv, DSP_READBACK_TEMP_WF_AMP_REGISTER);
        //TP_PRINTF("DSP Read back TempWfAmp= %d\r\n", fValue);
    }

    
    //(2)Get temperature for Woofer speaker 
    rawResult = ADCDrv_GetData(&audioAdcDrv, ADC_NTC_WF_SPK);
    if (ADC_DATA_IS_NOT_READY != rawResult)
    {
        tempWfSpkNew = AudioDrv_ConvertNtcToTemperature(rawResult);
        PowerDrv_UpdateTempLevel(tempWfSpkNew, &tempWfSpk, &tempLevelWfSpk, spkTempLevels,  ArraySize(spkTempLevels));
        Setting_Set(SETID_TEMP_WF_SPK, &tempWfSpk);
        DSPDrv1451_WriteDcValue32_0(&dspAdau1451Drv, DSP_TEMP_WF_SPK_REGISTER, (int32)tempWfSpkNew );
        //uint32 value= DSPDrv1451_ReadDcValue32_0(&dspAdau1451Drv, DSP_READBACK_TEMP_WF_SPK_REGISTER);
        //TP_PRINTF("DSP Read back TempTwAmp= %d\r\n", value);

        //DSPDrv1451_WriteValue8_24(&dspAdau1451Drv, DSP_TEMP_WF_SPK_REGISTER, ((double)tempWfSpkNew)/100.0 );
        //float fValue= DSPDrv1451_ReadValue8_24(&dspAdau1451Drv, DSP_READBACK_TEMP_WF_SPK_REGISTER);
        //TP_PRINTF("DSP Read back TempTwAmp= %f\r\n", fValue);
    }
   
    
    //(3)Get temperature for Tweeter spekaer 
    rawResult = ADCDrv_GetData(&audioAdcDrv, ADC_NTC_TW_SPK);
    if (ADC_DATA_IS_NOT_READY != rawResult)
    {
        tempTwSpkNew = AudioDrv_ConvertNtcToTemperature(rawResult);
        PowerDrv_UpdateTempLevel(tempTwSpkNew, &tempTwSpk, &tempLevelTwSpk, spkTempLevels,  ArraySize(spkTempLevels));
        Setting_Set(SETID_TEMP_TW_SPK, &tempTwSpk);
        DSPDrv1451_WriteDcValue32_0(&dspAdau1451Drv, DSP_TEMP_TW_SPK_REGISTER, (int32)tempTwSpkNew);
        //uint32 value= DSPDrv1451_ReadDcValue32_0(&dspAdau1451Drv, DSP_READBACK_TEMP_TW_SPK_REGISTER);
        //TP_PRINTF("DSP Read back TempTwSpk= %d\r\n", value);

        //DSPDrv1451_WriteValue8_24(&dspAdau1451Drv, DSP_TEMP_TW_SPK_REGISTER, ((double)tempTwSpkNew)/100.0 );
        //float fValue= DSPDrv1451_ReadValue8_24(&dspAdau1451Drv, DSP_READBACK_TEMP_TW_SPK_REGISTER);
        //TP_PRINTF("DSP Read back TempTwSpk= %f\r\n", fValue);
    }
   
    
    //(4)Write Battery temperature to DSP
    int16 tempBatt= 0;
    tempBatt= *(int16*)Setting_GetEx(SETID_BATTERY_TEMP, &tempBatt);
    DSPDrv1451_WriteDcValue32_0(&dspAdau1451Drv, DSP_TEMP_BATT_REGISTER, (uint32)(tempBatt/10) );
    //uint32 value= DSPDrv1451_ReadDcValue32_0(&dspAdau1451Drv, DSP_READBACK_TEMP_BATT_REGISTER);
    //TP_PRINTF("DSP Read back TempBatt= %d\r\n", value);

    //DSPDrv1451_WriteValue8_24(&dspAdau1451Drv, DSP_TEMP_BATT_REGISTER, ((double)tempBatt)/1000.0 );
    //float fValue= DSPDrv1451_ReadValue8_24(&dspAdau1451Drv, DSP_READBACK_TEMP_BATT_REGISTER);
    //TP_PRINTF("DSP Read back TempBatt= %f\r\n", fValue);
    

    //(4)Error Handling
    tempLevelAudioNew= MIN3(tempLevelWfAmp, tempLevelWfSpk, tempLevelTwSpk);
//    if(tempLevelAudio!=tempLevelAudioNew)
//    {
//        switch(tempLevelAudioNew)
//        {
//        case TL_CRITICAL:
//            TP_PRINTF("\r\nERROR: AudioDrv_AmpSafetyMonitor: temp level %d -> %d (CRITICAL) ***\r\n\r\n", tempLevelAudio, tempLevelAudioNew);
//            AudioDrv_SetOverheat(me, TRUE);
//            break;
//        
//        case TL_SERIOUS:
//            TP_PRINTF("\r\nERROR: AudioDrv_AmpSafetyMonitor: temp level %d -> %d (SERIOUS) ***\r\n\r\n", tempLevelAudio, tempLevelAudioNew);
//            AudioDrv_SetOverheat(me, TRUE);
//            break;
//        
//        case TL_WARN:
//            TP_PRINTF("\r\nERROR: AudioDrv_AmpSafetyMonitor: temp level %d -> %d (WARN) ***\r\n\r\n", tempLevelAudio, tempLevelAudioNew);
//            AudioDrv_SetOverheat(me, TRUE);
//            break;
//       
//        case TL_NORMAL:
//            TP_PRINTF("\r\nERROR: AudioDrv_AmpSafetyMonitor: temp level %d -> %d (NORMAL) ***\r\n\r\n", tempLevelAudio, tempLevelAudioNew);            
//            AudioDrv_SetOverheat(me, FALSE);
//            break;
//             
//        default:
//            ASSERT(0);
//            break;
//        }
//    }
    tempLevelAudio= tempLevelAudioNew;
    Setting_Set(SETID_TEMP_LEVEL_AUDIO, &tempLevelAudioNew);
    
    
    //(5)Continues ADC scanning
    ADCDrv_StartScanning(&audioAdcDrv);
        
    
    //(6)Print every 5sec (AUDIO_PRINT_PREIOD_IN_MS)
    static uint32 timeStamp = 0;
    uint16 printPeriod = *(uint16*)Setting_Get(SETID_TEMP_PRINT_PERIOD);
    if((getSysTime() - timeStamp) >= printPeriod)
    {
        timeStamp = getSysTime();
        int16 tempBatt=-1;
        uint16 batteryTempLevel= -1;
        tempBatt=         *(int16*)Setting_GetEx(SETID_BATTERY_TEMP, &tempBatt);
        batteryTempLevel= *(uint16*)Setting_GetEx(SETID_BATTERY_TEMP_LEVEL, &batteryTempLevel);
    
        uint32 ampErrorReason=0, ampWoofer_error=0, ampTweeter_error=0;
        
        /* byte[1]: woofer amplifier reason 
         * byte[0]: tweeter amplifier reason 
         */
        ampErrorReason= *(uint32*)Setting_GetEx(SETID_AMP_ERROR_REASON, &ampErrorReason);
        ampWoofer_error= ampErrorReason>>8;
        ampTweeter_error= ampErrorReason&0xf;

        //PVDD
        int32 adcRawResult = ADCDrv_GetData(&audioAdcDrv, ADC_PVDD_SEN);
        uint16 pvddSen= 0;
        if (ADC_DATA_IS_NOT_READY != rawResult)
        {
            pvddSen = PVDD_TO_mVOLT(adcRawResult);
            Setting_Set(SETID_PVDD_SEN, &pvddSen);
        }

        bool amp_health= TRUE;    
        amp_health= *(bool*)Setting_GetEx(SETID_AMP_HEALTH, &amp_health);

        float volInputLinear= DSPDrv1451_ReadInputVolume(&dspAdau1451Drv);
        float volWfOutputLinear= DSPDrv1451_ReadWooferOutputVolume(&dspAdau1451Drv) * DSP_OUTPUT_GAIN_LINEAR;
        float volTwOutputLinear= DSPDrv1451_ReadTweeterOutputVolume(&dspAdau1451Drv) * DSP_OUTPUT_GAIN_LINEAR;
        float volInputdB= log10(volInputLinear) * 20.0 + DSP_GAIN_READ_ADD_DB;
        float volWfOutputdB= log10(volWfOutputLinear) * 20.0 + DSP_GAIN_READ_ADD_DB;
        float volTwOutputdB= log10(volTwOutputLinear) * 20.0 + DSP_GAIN_READ_ADD_DB;
        
        Setting_Set(SETID_AUDIO_SIGNAL_VOL_INPUT,     &volInputdB);
        Setting_Set(SETID_AUDIO_SIGNAL_VOL_WF_OUTPUT, &volWfOutputdB);
        Setting_Set(SETID_AUDIO_SIGNAL_VOL_TW_OUTPUT, &volTwOutputdB);
        
        uint32 dspOverheatGainWf= DSPDrv1451_ReadDcValue32_0(&dspAdau1451Drv, DSP_READBACK_OVERHEAT_GAIN_WF_REGISTER);
        uint32 dspOverheatGainTw= DSPDrv1451_ReadDcValue32_0(&dspAdau1451Drv, DSP_READBACK_OVERHEAT_GAIN_TW_REGISTER);
        uint32 dspOverheatCoilTempWf= DSPDrv1451_ReadDcValue32_0(&dspAdau1451Drv, DSP_READBACK_OVERHEAT_COIL_TEMP_WF_REGISTER);
        uint32 dspOverheatCoilTempTw= DSPDrv1451_ReadDcValue32_0(&dspAdau1451Drv, DSP_READBACK_OVERHEAT_COIL_TEMP_TW_REGISTER);
        //uint32 dboostLevel= DSPDrv1451_GetDynaBoostLevel(&dspAdau1451Drv);

        Setting_Set(SETID_DSP_OVERHEAT_GAIN_WF,      &dspOverheatGainWf);
        Setting_Set(SETID_DSP_OVERHEAT_GAIN_TW,      &dspOverheatGainTw);
        Setting_Set(SETID_DSP_OVERHEAT_COIL_TEMP_WF, &dspOverheatCoilTempWf);
        Setting_Set(SETID_DSP_OVERHEAT_COIL_TEMP_TW, &dspOverheatCoilTempTw);
        //Setting_Set(SETID_DSP_DBOOST_LEVEL,          &dboostLevel);
        
//        printf("DSP-Vol: in=%.4f, out=%.4f, Boost=L%d(PVDD=%.1fV), Over-heat gain: wf=%d%%, tw=%d%%\r\n", 
//                        volInput, volOutput, dboostLevel, ((float)pvddSen)/1000.0,
//                        dspOverheatGainWf, dspOverheatGainTw);

        TP_PRINTF("dB/RMS: in=%.1f, wf-out=%.1f, tw-out=%.1f. PVDD=%.1fV. AMP: health=%d, fail=(wf:0x%x,tw:0x%x)\r\n", 
                        volInputdB, volWfOutputdB, volTwOutputdB, ((float)pvddSen)/1000.0,
                        amp_health, ampWoofer_error, ampTweeter_error);
        TP_PRINTF("Over-heat gain: wf=%d%%, tw=%d%%,  Coil Temp: wf=%dC, tw=%dC\r\n", 
                        dspOverheatGainWf, dspOverheatGainTw,
                        dspOverheatCoilTempWf, dspOverheatCoilTempTw);
        TP_PRINTF("Temp: WfAmp=%dC(L%d), WfSpk=%dC(L%d), TwSpk=%dC(L%d) ==> audio=(L%d), Batt=%.1fC(L%d)\r\n\r\n", 
                      tempWfAmp,   tempLevelWfAmp,
                      tempWfSpk,   tempLevelWfSpk,
                      tempTwSpk,   tempLevelTwSpk,
                      tempLevelAudio,
                      ((float)tempBatt)/10.0, batteryTempLevel);
     }
 }


#ifdef HAS_DSP_TUNING_DETECT    
static void AudioDrv_DspTuneUpdate(cAudioDrv *me)
{
    /* When insert DSP USBi cable, PA15 get high input from PA15,
     * but noise cause it trigger low sometime. Thus we need to debounce here.
     */
     
    //debounce for 0.5 seconds
    const uint32 timeoutTick= 100 / AUDIO_SRV_TIMEOUT_IN_MS;
    int8 dspTuneNew= GpioDrv_ReadBit(&audioGpioDrv, GPIO_IN_DSP_TUNE);
    
    if(dspTuneTmp != dspTuneNew) 
    {   //(Step-1) start debounce different DspTune
        debounceTick= 0;
        TP_PRINTF("DspTune (1)\r\n");
    }
    else
    {
        if(dspTuneCurr != dspTuneNew)
        {
            if( debounceTick < timeoutTick )
            {   //(Step-2) wait DspTune hold for 1s
                debounceTick++;
                TP_PRINTF("DspTune (2)\r\n");
            }
            else
            {   //(Setep-3) timeout, apply DspTune change, and turn on amplifier
                //When set dspTuneCurr, audio driver never turn off amplifier
                TP_PRINTF("DSP-TUNE= %d\r\n", dspTuneNew);
                if( dspTuneNew )
                {   //Enable amplifier when DSP cable insert
                    AudioDrv_SetAudio(me, AMP_SLEEP_MODE_ID, FALSE, /*not used*/0);
                    AudioDrv_Mute(AUDIO_AMP_SOFT_MUTE, /*muteEnable:*/FALSE);
                }
                DSPDrv1451_I2cEnable(&dspAdau1451Drv, !dspTuneNew);
                AudioAmpDrv_I2cEnable(&audioAmpWooferDrv, !dspTuneNew);
                AudioAmpDrv_I2cEnable(&audioAmpTweeterDrv, !dspTuneNew);
                debounceTick= 0;
                dspTuneCurr= dspTuneNew;
            }
        }
        else
        {
            //(Step-4) DspTune not change
        }
    }
    dspTuneTmp= dspTuneNew;
}
#endif /* HAS_DSP_TUNING_DETECT */

