/* Automatically generated nanopb constant definitions */
/* Generated by nanopb-0.3.3 at Fri Mar 17 15:38:03 2017. */

#include "ase-fep.pb.h"

#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif



const pb_field_t Proto_AseFep_Req_fields[10] = {
    PB_FIELD(  1, ENUM    , OPTIONAL, STATIC  , FIRST, Proto_AseFep_Req, type, type, 0),
    PB_FIELD(  2, UINT32  , OPTIONAL, STATIC  , OTHER, Proto_AseFep_Req, id, type, 0),
    PB_ONEOF_FIELD(data,   9, UINT32  , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, dummy, id, 0),
    PB_ONEOF_FIELD(data,  10, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, requestVersionInfo, id, &Proto_SoundWallMessage_RequestVersionInfo_fields),
    PB_ONEOF_FIELD(data,  11, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, requestSetAudioInput, id, &Proto_SoundWallMessage_RequestSetAudioInput_fields),
    PB_ONEOF_FIELD(data,  12, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, requestSelectSoundMode, id, &Proto_SoundWallMessage_RequestSelectSoundMode_fields),
    PB_ONEOF_FIELD(data,  13, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, requestActiveSoundMode, id, &Proto_SoundWallMessage_RequestActiveSoundMode_fields),
    PB_ONEOF_FIELD(data,  14, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, requestCreateSoundMode, id, &Proto_SoundWallMessage_RequestCreateSoundMode_fields),
    PB_ONEOF_FIELD(data,  15, MESSAGE , ONEOF, STATIC  , OTHER, Proto_AseFep_Req, requestSetA2Bmode, id, &Proto_SoundWallMessage_RequestSetA2Bmode_fields),
    PB_LAST_FIELD
};

const pb_field_t Proto_AseFep_Resp_fields[4] = {
    PB_FIELD(  1, ENUM    , OPTIONAL, STATIC  , FIRST, Proto_AseFep_Resp, type, type, 0),
    PB_FIELD(  2, UINT32  , OPTIONAL, STATIC  , OTHER, Proto_AseFep_Resp, id, type, 0),
    PB_FIELD(  3, MESSAGE , OPTIONAL, STATIC  , OTHER, Proto_AseFep_Resp, genericResponse, id, &Proto_Core_GenericResponse_fields),
    PB_LAST_FIELD
};

const pb_field_t Proto_AseFep_Event_fields[3] = {
    PB_FIELD(  1, ENUM    , OPTIONAL, STATIC  , FIRST, Proto_AseFep_Event, type, type, 0),
    PB_FIELD( 10, UINT32  , OPTIONAL, STATIC  , OTHER, Proto_AseFep_Event, param, type, 0),
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
PB_STATIC_ASSERT((pb_membersize(Proto_AseFep_Req, data.requestVersionInfo) < 65536 && pb_membersize(Proto_AseFep_Resp, genericResponse) < 65536), YOU_MUST_DEFINE_PB_FIELD_32BIT_FOR_MESSAGES_Proto_AseFep_Req_Proto_AseFep_Resp_Proto_AseFep_Event)
#endif

#if !defined(PB_FIELD_16BIT) && !defined(PB_FIELD_32BIT)
/* If you get an error here, it means that you need to define PB_FIELD_16BIT
 * compile-time option. You can do that in pb.h or on compiler command line.
 * 
 * The reason you need to do this is that some of your messages contain tag
 * numbers or field sizes that are larger than what can fit in the default
 * 8 bit descriptors.
 */
PB_STATIC_ASSERT((pb_membersize(Proto_AseFep_Req, data.requestVersionInfo) < 256 && pb_membersize(Proto_AseFep_Resp, genericResponse) < 256), YOU_MUST_DEFINE_PB_FIELD_16BIT_FOR_MESSAGES_Proto_AseFep_Req_Proto_AseFep_Resp_Proto_AseFep_Event)
#endif

