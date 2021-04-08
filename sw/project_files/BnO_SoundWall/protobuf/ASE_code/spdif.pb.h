/* Automatically generated nanopb header */
/* Generated by nanopb-0.3.7 at Mon Apr 17 15:09:20 2017. */

#ifndef PB_PROTO_SPDIF_SPDIF_PB_H_INCLUDED
#define PB_PROTO_SPDIF_SPDIF_PB_H_INCLUDED
#include <pb.h>

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Enum definitions */
typedef enum _Proto_Spdif_AudioFormatChanged_AudioMode {
    Proto_Spdif_AudioFormatChanged_AudioMode_PCM = 0,
    Proto_Spdif_AudioFormatChanged_AudioMode_NONPCM = 1
} Proto_Spdif_AudioFormatChanged_AudioMode;
#define _Proto_Spdif_AudioFormatChanged_AudioMode_MIN Proto_Spdif_AudioFormatChanged_AudioMode_PCM
#define _Proto_Spdif_AudioFormatChanged_AudioMode_MAX Proto_Spdif_AudioFormatChanged_AudioMode_NONPCM
#define _Proto_Spdif_AudioFormatChanged_AudioMode_ARRAYSIZE ((Proto_Spdif_AudioFormatChanged_AudioMode)(Proto_Spdif_AudioFormatChanged_AudioMode_NONPCM+1))

typedef enum _Proto_Spdif_AudioFormatChanged_SampleFrequency {
    Proto_Spdif_AudioFormatChanged_SampleFrequency_FS_ERROR = 0,
    Proto_Spdif_AudioFormatChanged_SampleFrequency_FS_8000 = 1,
    Proto_Spdif_AudioFormatChanged_SampleFrequency_FS_11025 = 2,
    Proto_Spdif_AudioFormatChanged_SampleFrequency_FS_12000 = 3,
    Proto_Spdif_AudioFormatChanged_SampleFrequency_FS_16000 = 4,
    Proto_Spdif_AudioFormatChanged_SampleFrequency_FS_22050 = 5,
    Proto_Spdif_AudioFormatChanged_SampleFrequency_FS_24000 = 6,
    Proto_Spdif_AudioFormatChanged_SampleFrequency_FS_32000 = 7,
    Proto_Spdif_AudioFormatChanged_SampleFrequency_FS_44100 = 8,
    Proto_Spdif_AudioFormatChanged_SampleFrequency_FS_48000 = 9,
    Proto_Spdif_AudioFormatChanged_SampleFrequency_FS_64000 = 10,
    Proto_Spdif_AudioFormatChanged_SampleFrequency_FS_88200 = 11,
    Proto_Spdif_AudioFormatChanged_SampleFrequency_FS_96000 = 12,
    Proto_Spdif_AudioFormatChanged_SampleFrequency_FS_128000 = 13,
    Proto_Spdif_AudioFormatChanged_SampleFrequency_FS_176400 = 14,
    Proto_Spdif_AudioFormatChanged_SampleFrequency_FS_192000 = 15
} Proto_Spdif_AudioFormatChanged_SampleFrequency;
#define _Proto_Spdif_AudioFormatChanged_SampleFrequency_MIN Proto_Spdif_AudioFormatChanged_SampleFrequency_FS_ERROR
#define _Proto_Spdif_AudioFormatChanged_SampleFrequency_MAX Proto_Spdif_AudioFormatChanged_SampleFrequency_FS_192000
#define _Proto_Spdif_AudioFormatChanged_SampleFrequency_ARRAYSIZE ((Proto_Spdif_AudioFormatChanged_SampleFrequency)(Proto_Spdif_AudioFormatChanged_SampleFrequency_FS_192000+1))

/* Struct definitions */
typedef struct _Proto_Spdif_AudioFormatChanged {
    char dummy_field;
/* @@protoc_insertion_point(struct:Proto_Spdif_AudioFormatChanged) */
} Proto_Spdif_AudioFormatChanged;

/* Default values for struct fields */

/* Initializer values for message structs */
#define Proto_Spdif_AudioFormatChanged_init_default {0}
#define Proto_Spdif_AudioFormatChanged_init_zero {0}

/* Field tags (for use in manual encoding/decoding) */

/* Struct field encoding specification for nanopb */
extern const pb_field_t Proto_Spdif_AudioFormatChanged_fields[1];

/* Maximum encoded size of messages (where known) */
#define Proto_Spdif_AudioFormatChanged_size      0

/* Message IDs (where set with "msgid" option) */
#ifdef PB_MSGID

#define SPDIF_MESSAGES \


#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
/* @@protoc_insertion_point(eof) */

#endif
