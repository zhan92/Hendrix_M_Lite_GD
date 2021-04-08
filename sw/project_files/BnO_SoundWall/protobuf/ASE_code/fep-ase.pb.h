/* Automatically generated nanopb header */
/* Generated by nanopb-0.3.7 at Mon Apr 17 15:09:19 2017. */

#ifndef PB_PROTO_FEPASE_FEP_ASE_PB_H_INCLUDED
#define PB_PROTO_FEPASE_FEP_ASE_PB_H_INCLUDED
#include <pb.h>

#include "ase-fep-ReqResp.pb.h"

#include "fep-ase-ReqResp.pb.h"

#include "common.pb.h"

#include "hdmi.pb.h"

#include "firmware-update.pb.h"

#include "production.pb.h"

#include "system.pb.h"

#include "dsp.pb.h"

#include "power-link.pb.h"

#include "eeb.pb.h"

#include "wpl.pb.h"

#include "light-sensor.pb.h"

#include "soundwall.pb.h"

#include "spdif.pb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Enum definitions */
typedef enum _Proto_FepAse_Event_Type {
    Proto_FepAse_Event_Type_LINE_IN_SENSE_ACTIVE = 0,
    Proto_FepAse_Event_Type_LINE_IN_SENSE_INACTIVE = 1,
    Proto_FepAse_Event_Type_TOSLINK_SENSE_ACTIVE = 2,
    Proto_FepAse_Event_Type_TOSLINK_SENSE_INACTIVE = 3,
    Proto_FepAse_Event_Type_HDMI_ARC_START_REQUEST = 4,
    Proto_FepAse_Event_Type_HDMI_ARC_STARTED = 5,
    Proto_FepAse_Event_Type_HDMI_ARC_END_REQUEST = 6,
    Proto_FepAse_Event_Type_HDMI_ARC_ENDED = 7,
    Proto_FepAse_Event_Type_HDMI_ARC_AUDIO_MODE_ON = 8,
    Proto_FepAse_Event_Type_HDMI_ARC_AUDIO_MODE_OFF = 9,
    Proto_FepAse_Event_Type_TUNNEL = 10,
    Proto_FepAse_Event_Type_BOOTLOADER_READY = 11,
    Proto_FepAse_Event_Type_FIRMWARE_UPDATE_FINISHED = 12,
    Proto_FepAse_Event_Type_POWERLINK_SENSE_EVENT = 13,
    Proto_FepAse_Event_Type_EEB_TELEGRAM_RECEIVE = 14,
    Proto_FepAse_Event_Type_WPL_EVENT = 15,
    Proto_FepAse_Event_Type_DSP_NTC_DATA_EVENT = 16,
    Proto_FepAse_Event_Type_HDMI_AUDIO_FORMAT = 17,
    Proto_FepAse_Event_Type_LOG_MESSAGE = 18,
    Proto_FepAse_Event_Type_HDMI_CEC_VOLUME_UP = 19,
    Proto_FepAse_Event_Type_HDMI_CEC_VOLUME_DOWN = 20,
    Proto_FepAse_Event_Type_HDMI_CEC_VOLUME_MUTE = 21,
    Proto_FepAse_Event_Type_HDMI_INPUT_SELECTED = 22,
    Proto_FepAse_Event_Type_POWERFAIL = 23,
    Proto_FepAse_Event_Type_HDMI_INPUT_SENSE_CHANGED = 24,
    Proto_FepAse_Event_Type_LIGHT_SENSOR_LIGHT_LEVEL = 25,
    Proto_FepAse_Event_Type_SPDIF_AUDIO_FORMAT_CHANGED = 26
} Proto_FepAse_Event_Type;
#define _Proto_FepAse_Event_Type_MIN Proto_FepAse_Event_Type_LINE_IN_SENSE_ACTIVE
#define _Proto_FepAse_Event_Type_MAX Proto_FepAse_Event_Type_SPDIF_AUDIO_FORMAT_CHANGED
#define _Proto_FepAse_Event_Type_ARRAYSIZE ((Proto_FepAse_Event_Type)(Proto_FepAse_Event_Type_SPDIF_AUDIO_FORMAT_CHANGED+1))

/* Struct definitions */
typedef struct _Proto_FepAse_Req {
    char dummy_field;
/* @@protoc_insertion_point(struct:Proto_FepAse_Req) */
} Proto_FepAse_Req;

typedef struct _Proto_FepAse_Event {
    pb_size_t which_data;
    union {
        Proto_Dsp_NTCDataEvent ntcData;
    } data;
/* @@protoc_insertion_point(struct:Proto_FepAse_Event) */
} Proto_FepAse_Event;

typedef struct _Proto_FepAse_Resp {
    bool has_type;
    Proto_AseFep_ReqResp type;
    bool has_id;
    uint32_t id;
    bool has_genericResponse;
    Proto_Core_GenericResponse genericResponse;
    pb_size_t which_data;
    union {
        Proto_FirmwareUpdate_VersionInfo firmwareUpdateVersionInfo;
        Proto_SoundWall_A2Bmode respA2Bmode;
        Proto_SoundWall_RespGetTotalNodes respTotalNodes;
        Proto_SoundWall_PowerMode respPowerMode;
        Proto_SoundWall_DriverGain respDriverGain;
        Proto_SoundWall_BassAndRoomEQ respBassAndRoomEQ;
        Proto_SoundWall_GainAndDelay respGainAndDelay;
    } data;
/* @@protoc_insertion_point(struct:Proto_FepAse_Resp) */
} Proto_FepAse_Resp;

/* Default values for struct fields */

/* Initializer values for message structs */
#define Proto_FepAse_Event_init_default          {0, {Proto_Dsp_NTCDataEvent_init_default}}
#define Proto_FepAse_Req_init_default            {0}
#define Proto_FepAse_Resp_init_default           {false, (Proto_AseFep_ReqResp)0, false, 0, false, Proto_Core_GenericResponse_init_default, 0, {Proto_FirmwareUpdate_VersionInfo_init_default}}
#define Proto_FepAse_Event_init_zero             {0, {Proto_Dsp_NTCDataEvent_init_zero}}
#define Proto_FepAse_Req_init_zero               {0}
#define Proto_FepAse_Resp_init_zero              {false, (Proto_AseFep_ReqResp)0, false, 0, false, Proto_Core_GenericResponse_init_zero, 0, {Proto_FirmwareUpdate_VersionInfo_init_zero}}

/* Field tags (for use in manual encoding/decoding) */
#define Proto_FepAse_Event_ntcData_tag           14
#define Proto_FepAse_Resp_firmwareUpdateVersionInfo_tag 12
#define Proto_FepAse_Resp_respA2Bmode_tag        27
#define Proto_FepAse_Resp_respTotalNodes_tag     28
#define Proto_FepAse_Resp_respPowerMode_tag      29
#define Proto_FepAse_Resp_respDriverGain_tag     30
#define Proto_FepAse_Resp_respBassAndRoomEQ_tag  31
#define Proto_FepAse_Resp_respGainAndDelay_tag   32
#define Proto_FepAse_Resp_type_tag               1
#define Proto_FepAse_Resp_id_tag                 2
#define Proto_FepAse_Resp_genericResponse_tag    3

/* Struct field encoding specification for nanopb */
extern const pb_field_t Proto_FepAse_Event_fields[2];
extern const pb_field_t Proto_FepAse_Req_fields[1];
extern const pb_field_t Proto_FepAse_Resp_fields[11];

/* Maximum encoded size of messages (where known) */
/* Proto_FepAse_Event_size depends on runtime parameters */
#define Proto_FepAse_Req_size                    0
/* Proto_FepAse_Resp_size depends on runtime parameters */

/* Message IDs (where set with "msgid" option) */
#ifdef PB_MSGID

#define FEP_ASE_MESSAGES \


#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
/* @@protoc_insertion_point(eof) */

#endif