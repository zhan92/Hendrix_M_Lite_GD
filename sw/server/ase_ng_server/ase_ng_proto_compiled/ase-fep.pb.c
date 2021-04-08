/* Automatically generated nanopb constant definitions */
/* Generated by nanopb-0.3.7 at Tue May 16 18:29:05 2017. */

#include "ase-fep.pb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif



const pb_field_t Proto_AseFep_Req_fields[34] = {
    PB_FIELD(  1, ENUM   , OPTIONAL, STATIC  , FIRST, Proto_AseFep_Req, type, type, 0),
    PB_FIELD(  2, UINT32  , OPTIONAL, STATIC  , OTHER, Proto_AseFep_Req, id, type, 0),
    PB_ONEOF_FIELD(data,  10, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, firmwareUpdateChunk, id, &Proto_FirmwareUpdate_Chunk_fields),
    PB_ONEOF_FIELD(data,  11, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, hdmiInput, id, &Proto_Hdmi_HdmiInput_fields),
    PB_ONEOF_FIELD(data,  12, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, lineInSensitivity, id, &Proto_Dsp_LineInSensitivity_fields),
    PB_ONEOF_FIELD(data,  13, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, audioInput, id, &Proto_Dsp_RequestAudioInput_fields),
    PB_ONEOF_FIELD(data,  14, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, positionSoundMode, id, &Proto_Dsp_RequestPositionSoundMode_fields),
    PB_ONEOF_FIELD(data,  15, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, internalSpeakerCompensation, id, &Proto_Dsp_RequestInternalSpeakerCompensation_fields),
    PB_ONEOF_FIELD(data,  16, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, powerLinkSetOn, id, &Proto_PowerLink_SetON_fields),
    PB_ONEOF_FIELD(data,  17, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, powerLinkSetMute, id, &Proto_PowerLink_SetMute_fields),
    PB_ONEOF_FIELD(data,  18, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, internalAmplifierCommand, id, &Proto_Dsp_InternalAmplifierCommand_fields),
    PB_ONEOF_FIELD(data,  19, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, eebTelegramTransmit, id, &Proto_Eeb_EebTelegram_fields),
    PB_ONEOF_FIELD(data,  20, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, ledMode, id, &Proto_Production_LedModeSet_fields),
    PB_ONEOF_FIELD(data,  21, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, getButtonState, id, &Proto_Production_GetButtonState_fields),
    PB_ONEOF_FIELD(data,  22, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, standbyType, id, &Proto_Hdmi_StandbyCmd_fields),
    PB_ONEOF_FIELD(data,  23, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, wplRequest, id, &Proto_Wpl_Request_fields),
    PB_ONEOF_FIELD(data,  24, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, sendPucCommand, id, &Proto_Puc_PucCommand_fields),
    PB_ONEOF_FIELD(data,  25, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, powerRequest, id, &Proto_System_PowerRequest_fields),
    PB_ONEOF_FIELD(data,  26, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, powerLinkData, id, &Proto_PowerLink_Data_fields),
    PB_ONEOF_FIELD(data,  27, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, toslinkOutSampleRate, id, &Proto_Dsp_ToslinkOutSampleRate_fields),
    PB_ONEOF_FIELD(data,  28, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, speakerEnableCommand, id, &Proto_Dsp_SpeakerEnableCommand_fields),
    PB_ONEOF_FIELD(data,  29, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, audioMode, id, &Proto_Hdmi_AudioModeSelect_fields),
    PB_ONEOF_FIELD(data,  30, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, lightSensorCommand, id, &Proto_LightSensor_Command_fields),
    PB_ONEOF_FIELD(data,  31, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, reqA2Bmode, id, &Proto_SoundWall_A2Bmode_fields),
    PB_ONEOF_FIELD(data,  32, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, reqGainAndDelay, id, &Proto_SoundWall_GainAndDelay_fields),
    PB_ONEOF_FIELD(data,  33, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, reqDriverGain, id, &Proto_SoundWall_DriverGain_fields),
    PB_ONEOF_FIELD(data,  34, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, reqBassAndRoomEQ, id, &Proto_SoundWall_BassAndRoomEQ_fields),
    PB_ONEOF_FIELD(data,  35, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, dspParameter, id, &Proto_Dsp_Parameter_fields),
    PB_ONEOF_FIELD(data,  36, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, reqPowerMode, id, &Proto_SoundWall_PowerMode_fields),
    PB_ONEOF_FIELD(data,  37, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, nodeIndex, id, &Proto_SoundWall_NodeIndex_fields),
    PB_ONEOF_FIELD(data,  38, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, muteMode, id, &Proto_SoundWall_MuteMode_fields),
    PB_ONEOF_FIELD(data,  39, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, reqTestTone, id, &Proto_SoundWall_ReqTestTone_fields),
    PB_ONEOF_FIELD(data,  40, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, dspAbsoluteVolume, id, &Proto_Dsp_AbsoluteVolume_fields),
    PB_LAST_FIELD
};

const pb_field_t Proto_AseFep_Resp_fields[6] = {
    PB_FIELD(  1, ENUM   , OPTIONAL, STATIC  , FIRST, Proto_AseFep_Resp, type, type, 0),
    PB_FIELD(  2, UINT32  , OPTIONAL, STATIC  , OTHER, Proto_AseFep_Resp, id, type, 0),
    PB_FIELD(  3, MESSAGE , OPTIONAL, STATIC  , OTHER, Proto_AseFep_Resp, genericResponse, id, &Proto_Core_GenericResponse_fields),
    PB_ONEOF_FIELD(data,  10, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Resp, networkInfo, genericResponse, &Proto_System_NetworkInfo_fields),
    PB_ONEOF_FIELD(data,  11, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Resp, volume, genericResponse, &Proto_Dsp_AbsoluteVolume_fields),
    PB_LAST_FIELD
};

const pb_field_t Proto_AseFep_Event_fields[9] = {
    PB_FIELD(  1, ENUM   , OPTIONAL, STATIC  , FIRST, Proto_AseFep_Event, type, type, 0),
    PB_ONEOF_FIELD(data,   9, UINT32  , ONEOF, STATIC  , OTHER, Proto_AseFep_Event, soundwallParam, type, 0),
    PB_ONEOF_FIELD(data,  10, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Event, productionTunnel, type, &Proto_Production_Tunnel_fields),
    PB_ONEOF_FIELD(data,  11, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Event, volume, type, &Proto_Dsp_AbsoluteVolume_fields),
    PB_ONEOF_FIELD(data,  12, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Event, mute, type, &Proto_Dsp_Mute_fields),
    PB_ONEOF_FIELD(data,  13, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Event, networkInfo, type, &Proto_System_NetworkInfo_fields),
    PB_ONEOF_FIELD(data,  14, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Event, playerData, type, &Proto_Player_Data_fields),
    PB_ONEOF_FIELD(data,  15, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Event, dspToneTouch, type, &Proto_Dsp_ToneTouch_fields),
    PB_LAST_FIELD
};


/* Check that field information fits in pb_field_t */
#if !defined(PB_FIELD_32BIT)
/* If you get an error here, it means that you need to define PB_FIELD_32BIT
 * compile-time option. You can do that in pb.h or on compiler command line.
 * 
 * The reason you need to do this is that some of your messages contain tag
 * numbers or field sizes that are larger than what can fit in 8 or 16 bit
 * field descriptors.
 */
PB_STATIC_ASSERT((pb_membersize(Proto_AseFep_Req, data.firmwareUpdateChunk) < 65536 && pb_membersize(Proto_AseFep_Req, data.hdmiInput) < 65536 && pb_membersize(Proto_AseFep_Req, data.lineInSensitivity) < 65536 && pb_membersize(Proto_AseFep_Req, data.audioInput) < 65536 && pb_membersize(Proto_AseFep_Req, data.positionSoundMode) < 65536 && pb_membersize(Proto_AseFep_Req, data.internalSpeakerCompensation) < 65536 && pb_membersize(Proto_AseFep_Req, data.powerLinkSetOn) < 65536 && pb_membersize(Proto_AseFep_Req, data.powerLinkSetMute) < 65536 && pb_membersize(Proto_AseFep_Req, data.internalAmplifierCommand) < 65536 && pb_membersize(Proto_AseFep_Req, data.eebTelegramTransmit) < 65536 && pb_membersize(Proto_AseFep_Req, data.ledMode) < 65536 && pb_membersize(Proto_AseFep_Req, data.getButtonState) < 65536 && pb_membersize(Proto_AseFep_Req, data.standbyType) < 65536 && pb_membersize(Proto_AseFep_Req, data.wplRequest) < 65536 && pb_membersize(Proto_AseFep_Req, data.sendPucCommand) < 65536 && pb_membersize(Proto_AseFep_Req, data.powerRequest) < 65536 && pb_membersize(Proto_AseFep_Req, data.powerLinkData) < 65536 && pb_membersize(Proto_AseFep_Req, data.toslinkOutSampleRate) < 65536 && pb_membersize(Proto_AseFep_Req, data.speakerEnableCommand) < 65536 && pb_membersize(Proto_AseFep_Req, data.audioMode) < 65536 && pb_membersize(Proto_AseFep_Req, data.lightSensorCommand) < 65536 && pb_membersize(Proto_AseFep_Req, data.reqA2Bmode) < 65536 && pb_membersize(Proto_AseFep_Req, data.reqGainAndDelay) < 65536 && pb_membersize(Proto_AseFep_Req, data.reqDriverGain) < 65536 && pb_membersize(Proto_AseFep_Req, data.reqBassAndRoomEQ) < 65536 && pb_membersize(Proto_AseFep_Req, data.dspParameter) < 65536 && pb_membersize(Proto_AseFep_Req, data.reqPowerMode) < 65536 && pb_membersize(Proto_AseFep_Req, data.nodeIndex) < 65536 && pb_membersize(Proto_AseFep_Req, data.muteMode) < 65536 && pb_membersize(Proto_AseFep_Req, data.reqTestTone) < 65536 && pb_membersize(Proto_AseFep_Req, data.dspAbsoluteVolume) < 65536 && pb_membersize(Proto_AseFep_Resp, data.networkInfo) < 65536 && pb_membersize(Proto_AseFep_Resp, data.volume) < 65536 && pb_membersize(Proto_AseFep_Resp, genericResponse) < 65536 && pb_membersize(Proto_AseFep_Event, data.productionTunnel) < 65536 && pb_membersize(Proto_AseFep_Event, data.volume) < 65536 && pb_membersize(Proto_AseFep_Event, data.mute) < 65536 && pb_membersize(Proto_AseFep_Event, data.networkInfo) < 65536 && pb_membersize(Proto_AseFep_Event, data.playerData) < 65536 && pb_membersize(Proto_AseFep_Event, data.dspToneTouch) < 65536), YOU_MUST_DEFINE_PB_FIELD_32BIT_FOR_MESSAGES_Proto_AseFep_Req_Proto_AseFep_Resp_Proto_AseFep_Event)
#endif

#if !defined(PB_FIELD_16BIT) && !defined(PB_FIELD_32BIT)
/* If you get an error here, it means that you need to define PB_FIELD_16BIT
 * compile-time option. You can do that in pb.h or on compiler command line.
 * 
 * The reason you need to do this is that some of your messages contain tag
 * numbers or field sizes that are larger than what can fit in the default
 * 8 bit descriptors.
 */
PB_STATIC_ASSERT((pb_membersize(Proto_AseFep_Req, data.firmwareUpdateChunk) < 256 && pb_membersize(Proto_AseFep_Req, data.hdmiInput) < 256 && pb_membersize(Proto_AseFep_Req, data.lineInSensitivity) < 256 && pb_membersize(Proto_AseFep_Req, data.audioInput) < 256 && pb_membersize(Proto_AseFep_Req, data.positionSoundMode) < 256 && pb_membersize(Proto_AseFep_Req, data.internalSpeakerCompensation) < 256 && pb_membersize(Proto_AseFep_Req, data.powerLinkSetOn) < 256 && pb_membersize(Proto_AseFep_Req, data.powerLinkSetMute) < 256 && pb_membersize(Proto_AseFep_Req, data.internalAmplifierCommand) < 256 && pb_membersize(Proto_AseFep_Req, data.eebTelegramTransmit) < 256 && pb_membersize(Proto_AseFep_Req, data.ledMode) < 256 && pb_membersize(Proto_AseFep_Req, data.getButtonState) < 256 && pb_membersize(Proto_AseFep_Req, data.standbyType) < 256 && pb_membersize(Proto_AseFep_Req, data.wplRequest) < 256 && pb_membersize(Proto_AseFep_Req, data.sendPucCommand) < 256 && pb_membersize(Proto_AseFep_Req, data.powerRequest) < 256 && pb_membersize(Proto_AseFep_Req, data.powerLinkData) < 256 && pb_membersize(Proto_AseFep_Req, data.toslinkOutSampleRate) < 256 && pb_membersize(Proto_AseFep_Req, data.speakerEnableCommand) < 256 && pb_membersize(Proto_AseFep_Req, data.audioMode) < 256 && pb_membersize(Proto_AseFep_Req, data.lightSensorCommand) < 256 && pb_membersize(Proto_AseFep_Req, data.reqA2Bmode) < 256 && pb_membersize(Proto_AseFep_Req, data.reqGainAndDelay) < 256 && pb_membersize(Proto_AseFep_Req, data.reqDriverGain) < 256 && pb_membersize(Proto_AseFep_Req, data.reqBassAndRoomEQ) < 256 && pb_membersize(Proto_AseFep_Req, data.dspParameter) < 256 && pb_membersize(Proto_AseFep_Req, data.reqPowerMode) < 256 && pb_membersize(Proto_AseFep_Req, data.nodeIndex) < 256 && pb_membersize(Proto_AseFep_Req, data.muteMode) < 256 && pb_membersize(Proto_AseFep_Req, data.reqTestTone) < 256 && pb_membersize(Proto_AseFep_Req, data.dspAbsoluteVolume) < 256 && pb_membersize(Proto_AseFep_Resp, data.networkInfo) < 256 && pb_membersize(Proto_AseFep_Resp, data.volume) < 256 && pb_membersize(Proto_AseFep_Resp, genericResponse) < 256 && pb_membersize(Proto_AseFep_Event, data.productionTunnel) < 256 && pb_membersize(Proto_AseFep_Event, data.volume) < 256 && pb_membersize(Proto_AseFep_Event, data.mute) < 256 && pb_membersize(Proto_AseFep_Event, data.networkInfo) < 256 && pb_membersize(Proto_AseFep_Event, data.playerData) < 256 && pb_membersize(Proto_AseFep_Event, data.dspToneTouch) < 256), YOU_MUST_DEFINE_PB_FIELD_16BIT_FOR_MESSAGES_Proto_AseFep_Req_Proto_AseFep_Resp_Proto_AseFep_Event)
#endif


/* @@protoc_insertion_point(eof) */
