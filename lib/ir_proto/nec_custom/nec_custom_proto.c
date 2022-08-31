#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#include "./nec_custom_proto.h"
#include "../ir_utils.h"

// import the assembled PIO state machine programs
#include "cust_nec_burst.pio.h"
#include "cust_nec_control.pio.h"


/**
 * Initializes the IR protocol PIO programs.
 */
int ir_proto_nec_custom_init(PIO pio, uint pin_num)
{
    // install the carrier_burst program in the PIO shared instruction space
    uint carrier_burst_offset;
    if (pio_can_add_program(pio, &cust_nec_burst_program)) {
        carrier_burst_offset = pio_add_program(pio, &cust_nec_burst_program);
    } else {
        printf("ERR: burst: can't add sm program!\n");
        return -1;
    }

    int carrier_burst_sm = pio_claim_unused_sm(pio, true);
    if (carrier_burst_sm == -1) {
        printf("ERR: burst: no unused sm!\n");
        return -1;
    }
    cust_nec_burst_program_init(
            pio, carrier_burst_sm, carrier_burst_offset,
            pin_num, IR_PROTO_NEC_CUSTOM_FREQ);

    // install the carrier_control program in the PIO shared instruction space
    uint carrier_control_offset;
    if (pio_can_add_program(pio, &cust_nec_control_program)) {
        carrier_control_offset = pio_add_program(pio, &cust_nec_control_program);
    } else {
        printf("ERR: control: can't add sm program!\n");
        return -1;
    }

    int carrier_control_sm = pio_claim_unused_sm(pio, true);
    if (carrier_control_sm == -1) {
        printf("ERR: control: no unused sm!\n");
        return -1;
    }
    cust_nec_control_program_init(
            pio, carrier_control_sm, carrier_control_offset,
            IR_PROTO_NEC_CUSTOM_FREQ, 32);

    // enable the state machines at the same time
    pio_sm_set_enabled(pio, carrier_burst_sm, true);
    pio_sm_set_enabled(pio, carrier_control_sm, true);

    return carrier_control_sm;
}

/**
 * Sends an arbitrary-length packet over the IR (using custom NEC-like/PDM modulation).
 */
int ir_proto_nec_custom_send_pkt(PIO pio, int pio_sm,
        const uint8_t *data, int len)
{
    /** current data position */
    int i = 0;
    /* note: the first word sent to the PIO encodes the length of the packet, in
     * 4-byte words, with special case when not divisible by 4 */
    uint32_t data_word = 0;

    uint8_t len_words = len / 4;
    uint8_t len_mod = len % 4;
    if (len_mod == 0) {
        /* first word is the header, no data bytes */
        data_word = (len_words & 0xF) | (3 << 4);

    } else {
        /* up to 3 bytes may be encoded inside the header word (as MSB) */
        uint8_t len_diff = (3 - len_mod);
        uint8_t cur_shift = (1 + len_diff) * 8;
        for (; i<len_mod; i++) {
            data_word |= (data[i] << cur_shift);
            cur_shift += 8;
        }
        data_word |= (len_words & 0xF) | (len_diff << 4);
    }
    /* first, send the header */
    pio_sm_put_blocking(pio, pio_sm, data_word);
    // printf("W: 0x%08lX\r\n", data_word);
    /* then the individual words */
    for (; (i + 4) <= len; i += 4) {
        data_word = ir_proto_make_word(data + i, 4, 0);
        pio_sm_put_blocking(pio, pio_sm, data_word);
        // printf("w: 0x%08lX\r\n", data_word);
    }
    return 0;
}

