#ifndef SETTING_ID_H
#define SETTING_ID_H

typedef enum
{
    SETID_START = 0,
    SETID_BATT_INFO = SETID_START,
    SETID_VOLUME,        // 1
    SETID_AC_STATUS,     // 2
    SETID_ALLPLAY_INFO,  // 3
    SETID_DSP_INIT_DATA,
    SETID_DSP_TUNABLE_PART,  //5
    SETID_DSP_EQ_GAINS,
    SETID_CHANNEL,
    SETID_IS_AUXIN_PLUG_IN,
    SETID_IS_RCA_PLUG_IN,
    SETID_IS_SPDIF_PLUG_IN,
    SETID_MAX_VOLUME,
    SETID_POWER_MODE,       //10
    SETID_BT_STATUS,
    SETID_CALLING_STATUS,
    SETID_MENU_DATA,
    SETID_PRESET_1,
    SETID_PRESET_2,         //15
    SETID_PRESET_3,
    SETID_MUSIC_DET,
    SETID_RGB_LED_COLOR,    //18
    SETID_AUDIO_SOURCE,
    SETID_MAX
}eSettingId;


#endif