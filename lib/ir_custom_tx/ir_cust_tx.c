#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"    // for clock_get_hz()

#include "./ir_cust_tx.h"

// import the assembled PIO state machine programs
#include "ir_cust_nec_burst.pio.h"
#include "ir_cust_nec_control.pio.h"

/**
 * Claim an unused state machine on the specified PIO and configure it
 * to transmit NEC IR frames on the specificied GPIO pin.
 * Returns: on success, the number of the carrier_control state machine
 * otherwise -1
 */
int ir_custom_nec_tx_init(PIO pio, uint pin_num)
{
    // install the carrier_burst program in the PIO shared instruction space
    uint carrier_burst_offset;
    if (pio_can_add_program(pio, &ir_custom_nec_burst_program)) {
        carrier_burst_offset = pio_add_program(pio, &ir_custom_nec_burst_program);
    } else {
        printf("ERR: burst: can't add sm program!\n");
        return -1;
    }

    // claim an unused state machine on this PIO
    int carrier_burst_sm = pio_claim_unused_sm(pio, true);
    if (carrier_burst_sm == -1) {
        printf("ERR: burst: no unused sm!\n");
        return -1;
    }
    ir_custom_nec_burst_program_init(pio,
                                   carrier_burst_sm,
                                   carrier_burst_offset,
                                   pin_num,
                                   38.222e3);  // 38.222 kHz carrier

    // install the carrier_control program in the PIO shared instruction space
    uint carrier_control_offset;
    if (pio_can_add_program(pio, &ir_custom_nec_control_program)) {
        carrier_control_offset = pio_add_program(pio, &ir_custom_nec_control_program);
    } else {
        printf("ERR: control: can't add sm program!\n");
        return -1;
    }

    // claim an unused state machine on this PIO
    int carrier_control_sm = pio_claim_unused_sm(pio, true);
    if (carrier_control_sm == -1) {
        printf("ERR: control: no unused sm!\n");
        return -1;
    }

    // configure the state machines
    ir_custom_nec_control_program_init(pio,
                                     carrier_control_sm,
                                     carrier_control_offset,
                                     38.222e3,
                                     32);               // 32 bits per frame

    // enable the state machines at the same time
    pio_sm_set_enabled(pio, carrier_burst_sm, true);
    pio_sm_set_enabled(pio, carrier_control_sm, true);

    return carrier_control_sm;
}

// Swap endian (big to little) or (little to big)
uint32_t ir_custom_swap_bytes(uint32_t num)
{
    uint32_t b0, b1, b2, b3;

    b0 = (num & 0x000000ff) << 24u;
    b1 = (num & 0x0000ff00) << 8u;
    b2 = (num & 0x00ff0000) >> 8u;
    b3 = (num & 0xff000000) >> 24u;

    return (b0 | b1 | b2 | b3);
}
