#ifndef _LIB_IR_CUST_TX_H
#define _LIB_IR_CUST_TX_H 1
/**
 * Custom IR protocol transmitter library for RPI Pico.
 */

#include <stdint.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"

// public API

int ir_custom_nec_tx_init(PIO pio, uint pin);

uint32_t ir_custom_swap_bytes(uint32_t num);

#endif
