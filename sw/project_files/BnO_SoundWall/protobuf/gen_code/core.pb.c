/* Automatically generated nanopb constant definitions */
/* Generated by nanopb-0.3.3 at Fri Mar 17 15:38:04 2017. */

#include "core.pb.h"

#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif



const pb_field_t Proto_Core_AseFepMessage_fields[4] = {
    PB_ONEOF_FIELD(OneOf,   1, MESSAGE , ONEOF, STATIC  , FIRST, Proto_Core_AseFepMessage, aseFepEvent, aseFepEvent, &Proto_AseFep_Event_fields),
    PB_ONEOF_FIELD(OneOf,   2, MESSAGE , ONEOF, STATIC  , FIRST, Proto_Core_AseFepMessage, aseFepReq, aseFepReq, &Proto_AseFep_Req_fields),
    PB_ONEOF_FIELD(OneOf,   3, MESSAGE , ONEOF, STATIC  , FIRST, Proto_Core_AseFepMessage, aseFepResp, aseFepResp, &Proto_AseFep_Resp_fields),
    PB_LAST_FIELD
};

const pb_field_t Proto_Core_FepAseMessage_fields[4] = {
    PB_ONEOF_FIELD(OneOf,   1, MESSAGE , ONEOF, STATIC  , FIRST, Proto_Core_FepAseMessage, fepAseEvent, fepAseEvent, &Proto_FepAse_Event_fields),
    PB_ONEOF_FIELD(OneOf,   2, MESSAGE , ONEOF, STATIC  , FIRST, Proto_Core_FepAseMessage, fepAseReq, fepAseReq, &Proto_FepAse_Req_fields),
    PB_ONEOF_FIELD(OneOf,   3, MESSAGE , ONEOF, STATIC  , FIRST, Proto_Core_FepAseMessage, fepAseResp, fepAseResp, &Proto_FepAse_Resp_fields),
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
PB_STATIC_ASSERT((pb_membersize(Proto_Core_AseFepMessage, OneOf.aseFepResp) < 65536 && pb_membersize(Proto_Core_FepAseMessage, OneOf.fepAseResp) < 65536), YOU_MUST_DEFINE_PB_FIELD_32BIT_FOR_MESSAGES_Proto_Core_AseFepMessage_Proto_Core_FepAseMessage)
#endif

#if !defined(PB_FIELD_16BIT) && !defined(PB_FIELD_32BIT)
/* If you get an error here, it means that you need to define PB_FIELD_16BIT
 * compile-time option. You can do that in pb.h or on compiler command line.
 * 
 * The reason you need to do this is that some of your messages contain tag
 * numbers or field sizes that are larger than what can fit in the default
 * 8 bit descriptors.
 */
PB_STATIC_ASSERT((pb_membersize(Proto_Core_AseFepMessage, OneOf.aseFepResp) < 256 && pb_membersize(Proto_Core_FepAseMessage, OneOf.fepAseResp) < 256), YOU_MUST_DEFINE_PB_FIELD_16BIT_FOR_MESSAGES_Proto_Core_AseFepMessage_Proto_Core_FepAseMessage)
#endif


