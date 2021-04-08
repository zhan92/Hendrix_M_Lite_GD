/* Automatically generated nanopb constant definitions */
/* Generated by nanopb-0.3.8 at Thu Mar 22 17:57:36 2018. */

#include "mcu-bt.pb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif



const pb_field_t Proto_McuBt_Event_fields[8] = {
    PB_FIELD(  1, ENUM   , OPTIONAL, STATIC  , FIRST, Proto_McuBt_Event, type, type, 0),
    PB_ONEOF_FIELD(OneOf,  10, UINT32  , ONEOF, STATIC  , OTHER, Proto_McuBt_Event, data, type, 0),
    PB_ONEOF_FIELD(OneOf,  11, UINT32  , ONEOF, STATIC  , UNION, Proto_McuBt_Event, absoluteVolume, type, 0),
    PB_ONEOF_FIELD(OneOf,  12, UINT32  , ONEOF, STATIC  , UNION, Proto_McuBt_Event, volumeStep, type, 0),
    PB_ONEOF_FIELD(OneOf,  13, UINT32  , ONEOF, STATIC  , UNION, Proto_McuBt_Event, source, type, 0),
    PB_ONEOF_FIELD(OneOf,  14, MESSAGE , ONEOF, STATIC  , UNION, Proto_McuBt_Event, eq, type, &Proto_BleEqTuning_Data_fields),
    PB_ONEOF_FIELD(OneOf,  15, MESSAGE , ONEOF, STATIC  , UNION, Proto_McuBt_Event, light, type, &Proto_BleLight_Data_fields),
    PB_LAST_FIELD
};

const pb_field_t Proto_McuBt_Req_fields[3] = {
    PB_FIELD(  1, ENUM   , OPTIONAL, STATIC  , FIRST, Proto_McuBt_Req, type, type, 0),
    PB_ONEOF_FIELD(OneOf,  10, UINT32  , ONEOF, STATIC  , OTHER, Proto_McuBt_Req, data, type, 0),
    PB_LAST_FIELD
};

const pb_field_t Proto_McuBt_Resp_fields[4] = {
    PB_FIELD(  1, ENUM   , OPTIONAL, STATIC  , FIRST, Proto_McuBt_Resp, type, type, 0),
    PB_ONEOF_FIELD(OneOf,  10, UINT32  , ONEOF, STATIC  , OTHER, Proto_McuBt_Resp, data, type, 0),
    PB_ONEOF_FIELD(OneOf,  11, BOOL    , ONEOF, STATIC  , UNION, Proto_McuBt_Resp, audioCuePlayReady, type, 0),
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
PB_STATIC_ASSERT((pb_membersize(Proto_McuBt_Event, OneOf.eq) < 65536 && pb_membersize(Proto_McuBt_Event, OneOf.light) < 65536), YOU_MUST_DEFINE_PB_FIELD_32BIT_FOR_MESSAGES_Proto_McuBt_Event_Proto_McuBt_Req_Proto_McuBt_Resp)
#endif

#if !defined(PB_FIELD_16BIT) && !defined(PB_FIELD_32BIT)
/* If you get an error here, it means that you need to define PB_FIELD_16BIT
 * compile-time option. You can do that in pb.h or on compiler command line.
 * 
 * The reason you need to do this is that some of your messages contain tag
 * numbers or field sizes that are larger than what can fit in the default
 * 8 bit descriptors.
 */
PB_STATIC_ASSERT((pb_membersize(Proto_McuBt_Event, OneOf.eq) < 256 && pb_membersize(Proto_McuBt_Event, OneOf.light) < 256), YOU_MUST_DEFINE_PB_FIELD_16BIT_FOR_MESSAGES_Proto_McuBt_Event_Proto_McuBt_Req_Proto_McuBt_Resp)
#endif


/* @@protoc_insertion_point(eof) */
