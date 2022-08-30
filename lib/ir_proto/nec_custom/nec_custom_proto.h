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

// public API

/**
 * Initializes the IR protocol PIO programs.
 */
int ir_proto_nec_custom_init(PIO pio, uint pin_num);

#endif
