#ifndef _LIB_IR_DECODE_MODULATION_H
#define _LIB_IR_DECODE_MODULATION_H 1
/**
 * IR raw protocol decoder: de-modulators.
 */

#include <stdint.h>

/** Enumeration of supported modulations (also used as indices inside the
 * modulations vector). */
enum ir_decode_modulation {
    IR_DECODE_AUTO = 0,
    IR_DECODE_PDM,
    IR_DECODE_PWM,
    IR_DECODE_MANCHESTER,
};

/** De-modulator configuration structure */
struct ir_decode_mod_config {
    /** Modulation type. */
    enum ir_decode_modulation type;
    /** Number of bits to group into a word. */
    int grp_bits;
    /** PDM decode flags */
    struct {
        /** Whether to admit no sync/header phase */
        unsigned int no_header: 1;
        unsigned int header_pulses: 4;
        unsigned int lsb_first: 1;
        /** Whether the space pulse may be variable */
        unsigned int var_space_pulse: 1;
        unsigned int inverse_pulse: 1;
    } flags;
    /** Advanced timing options */
    struct {
        /** Timing tolerance (%) */
        uint8_t tolerance;
        /** Typical pause period to switch to idle state */
        uint16_t idle_ms;
        /** Typical data pulse length (microseconds) */
        uint16_t datap_us;
        /** Typical 0-bit pulse length (in us) */
        uint16_t zero_bit_us;
        /** Typical 1-bit pulse length (in us) */
        uint16_t one_bit_us;
    } adv_timing;
};

/** 
 * The data type of a decoded word.
 * Note that when the MSB = 1, special `ir_dec_special_words` symbols are used.
 */
typedef uint32_t ir_dec_word_t;

/** Special decoded words. */
enum ir_dec_special_words {
    /* Special symbols mask */
    IR_DEC_SPECIAL_SYM = 0xF0000000,
    /** Unknown/discarded symbol */
    IR_DEC_UNKNOWN_SYM = 0xF0000000,
    /** Inter-frame spacing */
    IR_DEC_IFRAME_SPACE = 0xF0000001,
    /** Intra-frame spacing */
    IR_DEC_INNER_SPACE = 0xF000000A,
    /** Frame header & footer symbols. */
    IR_DEC_FRAME_HEADER = 0xFA000001,
    IR_DEC_FRAME_FOOTER = 0xFA00000F,
};

#define IR_DECODE_MODULATION_REGISTER(_name_) \
    int _name_( \
        const uint32_t *raw_pulses, unsigned int raw_length, \
        const struct ir_decode_mod_config *cfg, \
        ir_dec_word_t *out_words, unsigned int out_length)


#endif
