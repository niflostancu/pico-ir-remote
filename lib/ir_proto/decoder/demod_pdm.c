/**
 * Pulse Distance Modulation decoder for IR streams.
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "../capture/ircapture.h"
#include "./modulation.h"


/** PDM decoder's internal state machine. */
enum ir_pdm_states {
    IR_PDM_IDLE = 0,
    IR_PDM_HDR_STARTED = 0x10,
    IR_PDM_HDR_SYNCED = 0x11,
    IR_PDM_DATA_WAIT = 0xD0,
    IR_PDM_DATA_REC = 0xD1,
    IR_PDM_ERROR = 0xEE,
    IR_PDM_END = 0xFF,
};
enum ir_pdm_pulse_tim_type {
    IR_PDM_PULSE_INVALID = 0,
    IR_PDM_PULSE_DATA = 0x1,
    IR_PDM_PULSE_BIT0 = 0x10,
    IR_PDM_PULSE_BIT1 = 0x11,
    IR_PDM_PULSE_LONG_SPACE = 0x20,
    IR_PDM_PULSE_LONG_BURST = 0x21,
    IR_PDM_PULSE_IDLE = 0xD1,
};

#define COMPARE_WITHIN_THRS(_val_, _threshold_, _tolerance_)  ( \
        ((_val_) >= ((_threshold_) * (1.0 - _tolerance_))) && \
        ((_val_) <= ((_threshold_) * (1.0 + _tolerance_))) )
#define COMPARE_BELOW_THRS(_val_, _threshold_, _tolerance_)  \
        ((_val_) < ((_threshold_) * (1.0 - _tolerance_)))
#define COMPARE_ABOVE_THRS(_val_, _threshold_, _tolerance_)  \
        ((_val_) > ((_threshold_) * (1.0 + _tolerance_)))

#define CHECK_SYMBOL_NODUP(_out_buf_, _idx_, _sym_)  \
        ( (_idx_ == 0) || ( (_out_buf_)[_idx_] != _sym_) )

static enum ir_pdm_pulse_tim_type _pdm_classify_pulse_timing(
        uint32_t raw_pulse, const struct ir_decode_mod_config *cfg);


/**
 * Pulse Distance Modulation decoder routine.
 *
 * Receives a buffer of raw pulse timing (in us) and stores it inside a words
 * buffer. Both buffers must be pre-allocated and their usable lengths given as
 * parameters.
 */
int ir_decode_mod_pdm(const uint32_t *raw_pulses, unsigned int raw_length,
                      const struct ir_decode_mod_config *cfg,
                      ir_dec_word_t *out_words, unsigned int out_length)
{
    unsigned int ridx, out_idx = 0;
    enum ir_pdm_states state = IR_PDM_IDLE;
    unsigned int bits_accum = 0, bits_idx = 0;
    unsigned int gbits = cfg->grp_bits;
    if (!gbits) gbits = 8;

    for (ridx=0; ridx<raw_length; ridx++) {
        /* output/feedback actions */
        unsigned int add_symbol = 0, append_bit = 0;
        bool is_burst = IR_CAPTURE_IS_BURST(raw_pulses[ridx]);
        unsigned int width = IR_CAPTURE_GET_WIDTH(raw_pulses[ridx]);
        enum ir_pdm_pulse_tim_type pulse_type = _pdm_classify_pulse_timing(raw_pulses[ridx], cfg);
        /* printf("DBG: pulse %i: %u => 0x%X\r\n", is_burst, width, pulse_type); */

_ir_pdm_repeat:
        switch (state) {
            case IR_PDM_IDLE:  /* wait for a new frame */
                if (pulse_type == IR_PDM_PULSE_LONG_SPACE || pulse_type == IR_PDM_PULSE_DATA) {
                    /* a data bit could follow... so stay in a more advanced state */
                    state = IR_PDM_HDR_SYNCED;
                    if (CHECK_SYMBOL_NODUP(out_words, out_idx, IR_DEC_FRAME_HEADER))
                        add_symbol = IR_DEC_FRAME_HEADER;
                    break;
                }
                if (pulse_type == IR_PDM_PULSE_LONG_BURST) {
                    /* usually signals the header */
                    state = IR_PDM_HDR_STARTED;
                    break;
                }
                if (pulse_type == IR_PDM_PULSE_BIT0 || pulse_type == IR_PDM_PULSE_BIT1) {
                    /* force a sync/header section unless disabled */
                    if (cfg->flags.no_header) {
                        state = IR_PDM_DATA_REC;
                        goto _ir_pdm_repeat; /* force process this bit */
                    } else {
                        state = IR_PDM_HDR_SYNCED;
                        if (CHECK_SYMBOL_NODUP(out_words, out_idx, IR_DEC_FRAME_HEADER))
                            add_symbol = IR_DEC_FRAME_HEADER;
                    }
                    break;
                }
                /* else: remain in this state */
                continue;
            case IR_PDM_HDR_STARTED:
                if (pulse_type == IR_PDM_PULSE_IDLE) {
                    /* guess we go back to being idle... */
                    state = IR_PDM_IDLE;
                    add_symbol = IR_DEC_IFRAME_SPACE;
                    break;
                }
                /* wait until a space is received */
                if (pulse_type == IR_PDM_PULSE_LONG_SPACE || pulse_type == IR_PDM_PULSE_DATA) {
                    state = IR_PDM_HDR_SYNCED;
                    if (CHECK_SYMBOL_NODUP(out_words, out_idx, IR_DEC_FRAME_HEADER))
                        add_symbol = IR_DEC_FRAME_HEADER;
                    break;
                }
                if (pulse_type == IR_PDM_PULSE_BIT0 || pulse_type == IR_PDM_PULSE_BIT1) {
                    if (cfg->flags.no_header) {
                        state = IR_PDM_DATA_REC;
                        goto _ir_pdm_repeat; /* force process this bit */
                    } else {
                        /* consider it part of header and sync to it */
                        state = IR_PDM_HDR_SYNCED;
                        if (CHECK_SYMBOL_NODUP(out_words, out_idx, IR_DEC_FRAME_HEADER))
                            add_symbol = IR_DEC_FRAME_HEADER;
                    }
                    break;
                }
                continue;
            case IR_PDM_HDR_SYNCED:
                if (pulse_type == IR_PDM_PULSE_IDLE) {
                    /* guess we go back to being idle... */
                    state = IR_PDM_IDLE;
                    add_symbol = IR_DEC_IFRAME_SPACE;
                    break;
                }
                if (pulse_type == IR_PDM_PULSE_LONG_SPACE || pulse_type == IR_PDM_PULSE_LONG_BURST) {
                    /* remain in this state */
                    continue;
                }
                if (pulse_type == IR_PDM_PULSE_BIT0 || pulse_type == IR_PDM_PULSE_BIT1) {
                    state = IR_PDM_DATA_REC;
                    goto _ir_pdm_repeat; /* force process this bit */
                }
                continue;
            case IR_PDM_DATA_WAIT:
                if (pulse_type == IR_PDM_PULSE_IDLE) {
                    state = IR_PDM_IDLE;
                    add_symbol = IR_DEC_IFRAME_SPACE;
                    break;
                }
                /* this state is always entered after a data SPACE pulse */
                if (pulse_type == IR_PDM_PULSE_DATA) {
                    state = IR_PDM_DATA_REC;
                    break;
                }
                /* everything else is error */
                state = IR_PDM_ERROR;
                add_symbol = IR_DEC_UNKNOWN_SYM;
                continue;
            case IR_PDM_DATA_REC:
                if (pulse_type == IR_PDM_PULSE_IDLE) {
                    state = IR_PDM_IDLE;
                    add_symbol = IR_DEC_IFRAME_SPACE;
                    break;
                }
                /* just record the bit and switch back to the DATA_WAIT state */
                if (pulse_type == IR_PDM_PULSE_BIT0 || pulse_type == IR_PDM_PULSE_BIT1) {
                    state = IR_PDM_DATA_WAIT;
                    append_bit = 0x2;
                    if (pulse_type == IR_PDM_PULSE_BIT1)
                        append_bit |= 1;
                    break;
                }
                /* everything else is error */
                state = IR_PDM_ERROR;
                add_symbol = IR_DEC_UNKNOWN_SYM;
                break;
            case IR_PDM_END:
                /* we just expect to idle in this state... */
                if (pulse_type == IR_PDM_PULSE_IDLE) {
                    state = IR_PDM_IDLE;
                    add_symbol = IR_DEC_IFRAME_SPACE;
                    break;
                }
                /* everything else is an error */
                state = IR_PDM_ERROR;
                add_symbol = IR_DEC_UNKNOWN_SYM;
                break;
            case IR_PDM_ERROR:
                /* the only thing leaving it is an IDLE period */
                if (pulse_type == IR_PDM_PULSE_IDLE) {
                    state = IR_PDM_IDLE;
                    add_symbol = IR_DEC_IFRAME_SPACE;
                    break;
                }
                continue;
        }
        /* execute output actions */
        if (append_bit) {
            unsigned int bit = (append_bit & 0x1);
            if (cfg->flags.lsb_first) {
                bits_accum = (bits_accum << 1) | bit;
            } else {
                bits_accum = bits_accum | (bit << bits_idx);
            }
            bits_idx ++;
            if (bits_idx >= gbits) {
                out_words[out_idx++] = bits_accum;
                bits_accum = 0; bits_idx = 0;
            }
        }
        else if (add_symbol) {
            /* if there is a pending/unfinished word, add a special symbol */
            if (bits_idx > 0) {
                out_words[out_idx++] = IR_DEC_UNKNOWN_SYM;
            }
            /* now add the requested symbol */
            out_words[out_idx++] = add_symbol;
        }
        /* check for output buffer space */
        if ((out_idx + 2) >= out_length)
            return out_idx;
    }
    return out_idx;
}

enum ir_pdm_pulse_tim_type _pdm_classify_pulse_timing(
        uint32_t raw_pulse, const struct ir_decode_mod_config *cfg)
{
    bool is_burst = IR_CAPTURE_IS_BURST(raw_pulse);
    float width = (float)IR_CAPTURE_GET_WIDTH(raw_pulse);
    const float tolerance = (float)cfg->adv_timing.tolerance / 100.0;
    float min_width = cfg->adv_timing.datap_us;
    if (min_width > cfg->adv_timing.zero_bit_us) min_width = cfg->adv_timing.zero_bit_us;
    if (min_width > cfg->adv_timing.one_bit_us) min_width = cfg->adv_timing.one_bit_us;
    min_width = min_width * (1 - tolerance);

    if (is_burst) {
        if (width < min_width)
            return IR_PDM_PULSE_INVALID;
        if (COMPARE_WITHIN_THRS(width, cfg->adv_timing.datap_us, tolerance)) {
            return IR_PDM_PULSE_DATA;
        }
        if (COMPARE_ABOVE_THRS(width, cfg->adv_timing.datap_us, tolerance)) {
            return IR_PDM_PULSE_LONG_BURST;
        }
        return IR_PDM_PULSE_INVALID;

    } else /* it's a space */ {
        if (width < min_width)
            return IR_PDM_PULSE_INVALID;
        if ((width / 1000) >= cfg->adv_timing.idle_ms)
            return IR_PDM_PULSE_IDLE;
        if (COMPARE_WITHIN_THRS(width, cfg->adv_timing.zero_bit_us, tolerance)) {
            return IR_PDM_PULSE_BIT0;
        }
        if (COMPARE_WITHIN_THRS(width, cfg->adv_timing.one_bit_us, tolerance)) {
            return IR_PDM_PULSE_BIT1;
        }
        float bit_pulse_max = cfg->adv_timing.one_bit_us;
        if (bit_pulse_max < cfg->adv_timing.zero_bit_us)
            bit_pulse_max = cfg->adv_timing.zero_bit_us;
        if (COMPARE_ABOVE_THRS(width, bit_pulse_max, tolerance)) {
            return IR_PDM_PULSE_LONG_SPACE;
        }
        return IR_PDM_PULSE_INVALID;
    }
}
