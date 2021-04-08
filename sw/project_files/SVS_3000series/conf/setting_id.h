#ifndef SETTING_ID_H
#define SETTING_ID_H

typedef enum
{
    PAGE_SETTING_LP_FRE = 0,
    PAGE_SETTING_LP_SLO,
    PAGE_SETTING_HP_FRE,
    PAGE_SETTING_HP_SLO,
    PAGE_SETTING_POLARITY,
    PAGE_SETTING_PEQ1_FRE,
    PAGE_SETTING_PEQ1_BOOST,
    PAGE_SETTING_PEQ1_Q,
    PAGE_SETTING_PEQ2_FRE,
    PAGE_SETTING_PEQ2_BOOST,
    PAGE_SETTING_PEQ2_Q,
    PAGE_SETTING_PEQ3_FRE,
    PAGE_SETTING_PEQ3_BOOST,
    PAGE_SETTING_PEQ3_Q,
    PAGE_SETTING_PRE1_LOAD,
    PAGE_SETTING_PRE2_LOAD,
    PAGE_SETTING_PRE3_LOAD,
    PAGE_SETTING_PRE4_LOAD,
    PAGE_SETTING_PRE1_SAVE,
    PAGE_SETTING_PRE2_SAVE,
    PAGE_SETTING_PRE3_SAVE,
    PAGE_SETTING_PRE4_SAVE,
    PAGE_SETTING_RGC,
    PAGE_SETTING_VOL,
    PAGE_SETTING_DELAY,
    PAGE_SETTING_DISPLAY,
    PAGE_SETTING_TIMEOUT,
    PAGE_SETTING_STANDBY,
    PAGE_SETTING_TUNNING,
    PAGE_SETTING_LP_ON,
    PAGE_SETTING_HP_ON,
    PAGE_SETTING_PEQ1_ON,
    PAGE_SETTING_PEQ2_ON,
    PAGE_SETTING_PEQ3_ON, 
    PAGE_SETTING_RGC_ON,
    PAGE_SETTING_LP_OFF,
    PAGE_SETTING_HP_OFF,
    PAGE_SETTING_PEQ1_OFF,
    PAGE_SETTING_PEQ2_OFF,
    PAGE_SETTING_PEQ3_OFF,
    PAGE_SETTING_RGC_OFF,
    PAGE_SETTING_LP_STATUS,
    PAGE_SETTING_HP_STATUS,
    PAGE_SETTING_PEQ1_STATUS,
    PAGE_SETTING_PEQ2_STATUS,
    PAGE_SETTING_PEQ3_STATUS,
    PAGE_SETTING_RGC_STATUS,
    PAGE_SETTING_BT_ON,
    PAGE_SETTING_BT_OFF,
    PAGE_SETTING_CLEAR_BT, 
    PAGE_SETTING_RESET_YES,
    PAGE_SETTING_RESET_NO,
    PAGE_SETTING_RGC_FREQ,
    PAGE_SETTING_RGC_SLOPE,
    PAGE_SETTING_PHASE,
    PAGE_SETTING_MUTE,
    PAGE_SETTING_BRIGHTNESS,
    PAGE_SETTING_ID_MAX,
}ePageSettingId;



typedef enum
{
    SETID_START = 0,
    SETID_DSP_INIT_DATA = SETID_START,
    SETID_DSP_TUNABLE_PART,
    SETID_SCREEN_STATUS,
    SETID_MUSIC_DET,
    SETID_MENU_DATA,
    SETID_PRESET_1,
    SETID_PRESET_2,
    SETID_PRESET_3,
    SETID_PRESET_1_NAME,
    SETID_PRESET_2_NAME,
    SETID_PRESET_3_NAME,
    SETID_SW_VER,
    SETID_HW_VER,
    SETID_DSP_VER,

    SETID_QUEUE_MIN_DEBUG_SRV,
    SETID_QUEUE_MIN_SETTING_SRV,
    SETID_QUEUE_MIN_AUDIO_SRV,
    SETID_QUEUE_MIN_LEDS_SRV,
    SETID_QUEUE_MIN_KEYS_SRV,
    SETID_QUEUE_MIN_POWER_SRV,
    SETID_POOL_MIN_SMALL,
    SETID_POOL_MIN_MEDIUM,
    SETID_POOL_MIN_LARGE,

    SETID_AMP_TEMP_LEVEL,
    SETID_JACKIN_STATUS,
    SETID_JACKIN_LEVEL,
    SETID_BOOT_MODE,                // keep it at last
    SETID_MAX
}eSettingId;

#endif
