/* Automatically generated nanopb constant definitions */
/* Generated by nanopb-0.3.7 at Tue May 16 18:29:08 2017. */

#include "production.pb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif



const pb_field_t Proto_Production_Tunnel_fields[2] = {
    PB_FIELD(  1, BYTES   , OPTIONAL, STATIC  , FIRST, Proto_Production_Tunnel, data, data, 0),
    PB_LAST_FIELD
};

const pb_field_t Proto_Production_LedModeSet_fields[4] = {
    PB_FIELD(  1, ENUM   , OPTIONAL, STATIC  , FIRST, Proto_Production_LedModeSet, led, led, 0),
    PB_FIELD(  2, INT32   , REQUIRED, STATIC  , OTHER, Proto_Production_LedModeSet, onTimeMs, led, 0),
    PB_FIELD(  3, INT32   , REQUIRED, STATIC  , OTHER, Proto_Production_LedModeSet, offTimeMs, onTimeMs, 0),
    PB_LAST_FIELD
};

const pb_field_t Proto_Production_GetButtonState_fields[2] = {
    PB_FIELD(  1, ENUM   , OPTIONAL, STATIC  , FIRST, Proto_Production_GetButtonState, buttonId, buttonId, 0),
    PB_LAST_FIELD
};

const pb_field_t Proto_Production_ButtonState_fields[2] = {
    PB_FIELD(  2, BOOL    , REQUIRED, STATIC  , FIRST, Proto_Production_ButtonState, pressed, pressed, 0),
    PB_LAST_FIELD
};


/* @@protoc_insertion_point(eof) */
