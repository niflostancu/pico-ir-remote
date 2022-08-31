#ifndef _LIB_IR_PROTO_NEC_CUSTOM_H
#define _LIB_IR_PROTO_NEC_CUSTOM_H 1
/**
 * Custom NEC-like IR protocol TX library for RPI Pico.
 *
 * Uses the PIO state machine for efficient IR data modulation.
 */

#include <stdint.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"

/* IR modulation frequency */
#define IR_PROTO_NEC_CUSTOM_FREQ 38.222e3

// public API

/**
 * Initializes the IR protocol PIO programs.
 */
int ir_proto_nec_custom_init(PIO pio, uint pin_num);

/**
 * Sends an arbitrary-length packet over the IR (using custom NEC-like/PDM modulation).
 */
int ir_proto_nec_custom_send_pkt(PIO pio, int pio_sm, const uint8_t *data, int len);

#endif
