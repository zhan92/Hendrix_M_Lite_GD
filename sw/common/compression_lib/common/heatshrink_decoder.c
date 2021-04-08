#include <stdlib.h>
#include <string.h>
#include "heatshrink_decoder.h"

/* States for the polling state machine. */
typedef enum {
    HSDS_TAG_BIT,               /* tag bit */
    HSDS_YIELD_LITERAL,         /* ready to yield literal byte */
    HSDS_BACKREF_INDEX_MSB,     /* most significant byte of index */
    HSDS_BACKREF_INDEX_LSB,     /* least significant byte of index */
    HSDS_BACKREF_COUNT_MSB,     /* most significant byte of count */
    HSDS_BACKREF_COUNT_LSB,     /* least significant byte of count */
    HSDS_YIELD_BACKREF,         /* ready to yield back-reference */
} HSD_state;

#if HEATSHRINK_DEBUGGING_LOGS
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#define LOG(...) fprintf(stderr, __VA_ARGS__)
#define ASSERT(X) assert(X)
static const char *state_names[] = {
    "tag_bit",
    "yield_literal",
    "backref_index_msb",
    "backref_index_lsb",
    "backref_count_msb",
    "backref_count_lsb",
    "yield_backref",
};
#else
#define LOG(...) /* no-op */
#define ASSERT(X) /* no-op */
#endif

typedef struct {
    unsigned char *buf;               /* output buffer */
    unsigned short buf_size;            /* buffer size */
    unsigned short *output_size;        /* bytes pushed to buffer, so far */
} output_info;

#define NO_BITS ((unsigned short)-1)

/* Forward references. */
static unsigned short get_bits(heatshrink_decoder *hsd, unsigned char count);
static void push_byte(heatshrink_decoder *hsd, output_info *oi, unsigned char byte);

void heatshrink_decoder_reset(heatshrink_decoder *hsd) {
    unsigned short buf_sz = 1 << config_get_w();
    unsigned short input_sz = HEATSHRINK_DECODER_INPUT_BUFFER_SIZE(hsd);
    memset(hsd->buffers, 0, buf_sz + input_sz);
    hsd->state = HSDS_TAG_BIT;
    hsd->input_size = 0;
    hsd->input_index = 0;
    hsd->bit_index = 0x00;
    hsd->current_byte = 0x00;
    hsd->output_count = 0;
    hsd->output_index = 0;
    hsd->head_index = 0;
}

/* Copy SIZE bytes into the decoder's input buffer, if it will fit. */
HSD_sink_res heatshrink_decoder_sink(heatshrink_decoder *hsd,
        const unsigned char *in_buf, unsigned short size, unsigned short *input_size) {
    if ((hsd == NULL) || (in_buf == NULL) || (input_size == NULL)) {
        return HSDR_SINK_ERROR_NULL;
    }

    unsigned short rem = HEATSHRINK_DECODER_INPUT_BUFFER_SIZE(hsd) - hsd->input_size;
    if (rem == 0) {
        *input_size = 0;
        return HSDR_SINK_FULL;
    }

    size = rem < size ? rem : size;
    LOG("-- sinking %zd bytes\n", size);
    /* copy into input buffer (at head of buffers) */
    memcpy(&hsd->buffers[hsd->input_size], in_buf, size);
    hsd->input_size += size;
    *input_size = size;
    return HSDR_SINK_OK;
}


/*****************
 * Decompression *
 *****************/

#define BACKREF_COUNT_BITS(HSD) (config_get_l())
#define BACKREF_INDEX_BITS(HSD) (config_get_w())

// States
static HSD_state st_tag_bit(heatshrink_decoder *hsd);
static HSD_state st_yield_literal(heatshrink_decoder *hsd,
    output_info *oi);
static HSD_state st_backref_index_msb(heatshrink_decoder *hsd);
static HSD_state st_backref_index_lsb(heatshrink_decoder *hsd);
static HSD_state st_backref_count_msb(heatshrink_decoder *hsd);
static HSD_state st_backref_count_lsb(heatshrink_decoder *hsd);
static HSD_state st_yield_backref(heatshrink_decoder *hsd,
    output_info *oi);

HSD_poll_res heatshrink_decoder_poll(heatshrink_decoder *hsd,
        unsigned char *out_buf, unsigned short out_buf_size, unsigned short *output_size) {
    if ((hsd == NULL) || (out_buf == NULL) || (output_size == NULL)) {
        return HSDR_POLL_ERROR_NULL;
    }
    *output_size = 0;

    output_info oi;
    oi.buf = out_buf;
    oi.buf_size = out_buf_size;
    oi.output_size = output_size;

    while (1) {
        LOG("-- poll, state is %d (%s), input_size %d\n",
            hsd->state, state_names[hsd->state], hsd->input_size);
        unsigned char in_state = hsd->state;
        switch (in_state) {
        case HSDS_TAG_BIT:
            hsd->state = st_tag_bit(hsd);
            break;
        case HSDS_YIELD_LITERAL:
            hsd->state = st_yield_literal(hsd, &oi);
            break;
        case HSDS_BACKREF_INDEX_MSB:
            hsd->state = st_backref_index_msb(hsd);
            break;
        case HSDS_BACKREF_INDEX_LSB:
            hsd->state = st_backref_index_lsb(hsd);
            break;
        case HSDS_BACKREF_COUNT_MSB:
            hsd->state = st_backref_count_msb(hsd);
            break;
        case HSDS_BACKREF_COUNT_LSB:
            hsd->state = st_backref_count_lsb(hsd);
            break;
        case HSDS_YIELD_BACKREF:
            hsd->state = st_yield_backref(hsd, &oi);
            break;
        default:
            return HSDR_POLL_ERROR_UNKNOWN;
        }
        
        /* If the current state cannot advance, check if input or output
         * buffer are exhausted. */
        if (hsd->state == in_state) {
            if (*output_size == out_buf_size) { return HSDR_POLL_MORE; }
            return HSDR_POLL_EMPTY;
        }
    }
}

static HSD_state st_tag_bit(heatshrink_decoder *hsd) {
    uint32_t bits = get_bits(hsd, 1);  // get tag bit
    if (bits == NO_BITS) {
        return HSDS_TAG_BIT;
    } else if (bits) {
        return HSDS_YIELD_LITERAL;
    } else if (HEATSHRINK_STATIC_WINDOW_BITS > 8) {
        return HSDS_BACKREF_INDEX_MSB;
    } else {
        hsd->output_index = 0;
        return HSDS_BACKREF_INDEX_LSB;
    }
}

static HSD_state st_yield_literal(heatshrink_decoder *hsd,
        output_info *oi) {
    /* Emit a repeated section from the window buffer, and add it (again)
     * to the window buffer. (Note that the repetition can include
     * itself.)*/
    if (*oi->output_size < oi->buf_size) {
        unsigned short byte = get_bits(hsd, 8);
        if (byte == NO_BITS) { return HSDS_YIELD_LITERAL; } /* out of input */
        unsigned char *buf = &hsd->buffers[HEATSHRINK_DECODER_INPUT_BUFFER_SIZE(hsd)];
        unsigned short mask = (1 << HEATSHRINK_STATIC_WINDOW_BITS)  - 1;
        unsigned char c = byte & 0xFF;
        LOG("-- emitting literal byte 0x%02x ('%c')\n", c, isprint(c) ? c : '.');
        buf[hsd->head_index++ & mask] = c;
        push_byte(hsd, oi, c);
        return HSDS_TAG_BIT;
    } else {
        return HSDS_YIELD_LITERAL;
    }
}

static HSD_state st_backref_index_msb(heatshrink_decoder *hsd) {
    unsigned char bit_ct = BACKREF_INDEX_BITS(hsd);
    ASSERT(bit_ct > 8);
    unsigned short bits = get_bits(hsd, bit_ct - 8);
    LOG("-- backref index (msb), got 0x%04x (+1)\n", bits);
    if (bits == NO_BITS) { return HSDS_BACKREF_INDEX_MSB; }
    hsd->output_index = bits << 8;
    return HSDS_BACKREF_INDEX_LSB;
}

static HSD_state st_backref_index_lsb(heatshrink_decoder *hsd) {
    unsigned char bit_ct = BACKREF_INDEX_BITS(hsd);
    unsigned short bits = get_bits(hsd, bit_ct < 8 ? bit_ct : 8);
    LOG("-- backref index (lsb), got 0x%04x (+1)\n", bits);
    if (bits == NO_BITS) { return HSDS_BACKREF_INDEX_LSB; }
    hsd->output_index |= bits;
    hsd->output_index++;
    unsigned char br_bit_ct = BACKREF_COUNT_BITS(hsd);
    hsd->output_count = 0;
    return (br_bit_ct > 8) ? HSDS_BACKREF_COUNT_MSB : HSDS_BACKREF_COUNT_LSB;
}

static HSD_state st_backref_count_msb(heatshrink_decoder *hsd) {
    unsigned char br_bit_ct = BACKREF_COUNT_BITS(hsd);
    ASSERT(br_bit_ct > 8);
    unsigned short bits = get_bits(hsd, br_bit_ct - 8);
    LOG("-- backref count (msb), got 0x%04x (+1)\n", bits);
    if (bits == NO_BITS) { return HSDS_BACKREF_COUNT_MSB; }
    hsd->output_count = bits << 8;
    return HSDS_BACKREF_COUNT_LSB;
}

static HSD_state st_backref_count_lsb(heatshrink_decoder *hsd) {
    unsigned char br_bit_ct = BACKREF_COUNT_BITS(hsd);
    unsigned short bits = get_bits(hsd, br_bit_ct < 8 ? br_bit_ct : 8);
    LOG("-- backref count (lsb), got 0x%04x (+1)\n", bits);
    if (bits == NO_BITS) { return HSDS_BACKREF_COUNT_LSB; }
    hsd->output_count |= bits;
    hsd->output_count++;
    return HSDS_YIELD_BACKREF;
}

static HSD_state st_yield_backref(heatshrink_decoder *hsd,
        output_info *oi) {
    unsigned short count = oi->buf_size - *oi->output_size;
	unsigned char *buf = &hsd->buffers[HEATSHRINK_DECODER_INPUT_BUFFER_SIZE(hsd)];
	unsigned short mask = (1 << HEATSHRINK_STATIC_WINDOW_BITS) - 1;
	unsigned short neg_offset = hsd->output_index;

    if (count > 0) {
        unsigned short i = 0;
        if (hsd->output_count < count) count = hsd->output_count;
        LOG("-- emitting %zu bytes from -%u bytes back\n", count, neg_offset);
        ASSERT(neg_offset <= mask + 1);
        ASSERT(count <= (unsigned short)(1 << BACKREF_COUNT_BITS(hsd)));

        for (i=0; i<count; i++) {
            unsigned char c = buf[(hsd->head_index - neg_offset) & mask];
            push_byte(hsd, oi, c);
            buf[hsd->head_index & mask] = c;
            hsd->head_index++;
            LOG("  -- ++ 0x%02x\n", c);
        }
        hsd->output_count -= count;
        if (hsd->output_count == 0) { return HSDS_TAG_BIT; }
    }
    return HSDS_YIELD_BACKREF;
}

/* Get the next COUNT bits from the input buffer, saving incremental progress.
 * Returns NO_BITS on end of input, or if more than 15 bits are requested. */
static unsigned short get_bits(heatshrink_decoder *hsd, unsigned char count) {
    unsigned short accumulator = 0;
    int i = 0;
    if (count > 15) { return NO_BITS; }
    LOG("-- popping %u bit(s)\n", count);

    /* If we aren't able to get COUNT bits, suspend immediately, because we
     * don't track how many bits of COUNT we've accumulated before suspend. */
    if (hsd->input_size == 0) {
        if (hsd->bit_index < (1 << (count - 1))) { return NO_BITS; }
    }

    for (i = 0; i < count; i++) {
        if (hsd->bit_index == 0x00) {
            if (hsd->input_size == 0) {
                LOG("  -- out of bits, suspending w/ accumulator of %u (0x%02x)\n",
                    accumulator, accumulator);
                return NO_BITS;
            }
            hsd->current_byte = hsd->buffers[hsd->input_index++];
            LOG("  -- pulled byte 0x%02x\n", hsd->current_byte);
            if (hsd->input_index == hsd->input_size) {
                hsd->input_index = 0; /* input is exhausted */
                hsd->input_size = 0;
            }
            hsd->bit_index = 0x80;
        }
        accumulator <<= 1;
        if (hsd->current_byte & hsd->bit_index) {
            accumulator |= 0x01;
            if (0) {
                LOG("  -- got 1, accumulator 0x%04x, bit_index 0x%02x\n",
                accumulator, hsd->bit_index);
            }
        } else {
            if (0) {
                LOG("  -- got 0, accumulator 0x%04x, bit_index 0x%02x\n",
                accumulator, hsd->bit_index);
            }
        }
        hsd->bit_index >>= 1;
    }

    if (count > 1) { LOG("  -- accumulated %08x\n", accumulator); }
    return accumulator;
}

HSD_finish_res heatshrink_decoder_finish(heatshrink_decoder *hsd) {
    if (hsd == NULL) { return HSDR_FINISH_ERROR_NULL; }
    switch (hsd->state) {
    case HSDS_TAG_BIT:
        return hsd->input_size == 0 ? HSDR_FINISH_DONE : HSDR_FINISH_MORE;

    /* If we want to finish with no input, but are in these states, it's
     * because the 0-bit padding to the last byte looks like a backref
     * marker bit followed by all 0s for index and count bits. */
    case HSDS_BACKREF_INDEX_LSB:
    case HSDS_BACKREF_INDEX_MSB:
    case HSDS_BACKREF_COUNT_LSB:
    case HSDS_BACKREF_COUNT_MSB:
        return hsd->input_size == 0 ? HSDR_FINISH_DONE : HSDR_FINISH_MORE;

    /* If the output stream is padded with 0xFFs (possibly due to being in
     * flash memory), also explicitly check the input size rather than
     * uselessly returning MORE but yielding 0 bytes when polling. */
    case HSDS_YIELD_LITERAL:
        return hsd->input_size == 0 ? HSDR_FINISH_DONE : HSDR_FINISH_MORE;

    default:
        return HSDR_FINISH_MORE;
    }
}

static void push_byte(heatshrink_decoder *hsd, output_info *oi, unsigned char byte) {
    LOG(" -- pushing byte: 0x%02x ('%c')\n", byte, isprint(byte) ? byte : '.');
    oi->buf[(*oi->output_size)++] = byte;
    (void)hsd;
}