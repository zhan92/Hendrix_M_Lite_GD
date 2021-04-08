/* Automatically generated nanopb header */
/* Generated by nanopb-0.3.7 at Mon Apr 17 15:09:19 2017. */

#ifndef PB_PROTO_FEPASE_FEP_ASE_REQRESP_PB_H_INCLUDED
#define PB_PROTO_FEPASE_FEP_ASE_REQRESP_PB_H_INCLUDED
#include <pb.h>

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Enum definitions */
typedef enum _Proto_FepAse_ReqResp {
    Proto_FepAse_ReqResp_PING = 0,
    Proto_FepAse_ReqResp_PLAY = 1,
    Proto_FepAse_ReqResp_PAUSE = 2,
    Proto_FepAse_ReqResp_STOP = 3,
    Proto_FepAse_ReqResp_NEXT = 4,
    Proto_FepAse_ReqResp_PREV = 5,
    Proto_FepAse_ReqResp_NEXT_SOURCE = 6,
    Proto_FepAse_ReqResp_JOIN = 7,
    Proto_FepAse_ReqResp_PLAY_PAUSE_TOGGLE = 8,
    Proto_FepAse_ReqResp_SOUND_SILENCE_TOGGLE = 9,
    Proto_FepAse_ReqResp_SOUND = 10,
    Proto_FepAse_ReqResp_SILENCE = 11,
    Proto_FepAse_ReqResp_MUTE = 12,
    Proto_FepAse_ReqResp_UNMUTE = 13,
    Proto_FepAse_ReqResp_BT_PAIRING_ON = 14,
    Proto_FepAse_ReqResp_BT_PAIRING_OFF = 15,
    Proto_FepAse_ReqResp_BTLE_PAIRING_ON = 16,
    Proto_FepAse_ReqResp_BTLE_PAIRING_OFF = 17,
    Proto_FepAse_ReqResp_BT_PAIRING_TOGGLE = 18,
    Proto_FepAse_ReqResp_OFF = 19,
    Proto_FepAse_ReqResp_STORAGE = 20,
    Proto_FepAse_ReqResp_FACTORY_RESET = 21,
    Proto_FepAse_ReqResp_NETWORK_SETUP = 22,
    Proto_FepAse_ReqResp_STANDBY = 23,
    Proto_FepAse_ReqResp_ALL_STANDBY = 24,
    Proto_FepAse_ReqResp_NETWORK_INFO = 25,
    Proto_FepAse_ReqResp_VOLUME_CHANGE = 26,
    Proto_FepAse_ReqResp_WPL_COMMAND = 27
} Proto_FepAse_ReqResp;
#define _Proto_FepAse_ReqResp_MIN Proto_FepAse_ReqResp_PING
#define _Proto_FepAse_ReqResp_MAX Proto_FepAse_ReqResp_WPL_COMMAND
#define _Proto_FepAse_ReqResp_ARRAYSIZE ((Proto_FepAse_ReqResp)(Proto_FepAse_ReqResp_WPL_COMMAND+1))

#ifdef __cplusplus
} /* extern "C" */
#endif
/* @@protoc_insertion_point(eof) */

#endif
