#ifndef _LIB_IR_CAPTURE_H
#define _LIB_IR_CAPTURE_H 1
/**
 * Raw IR capture routines.
 *
 * Uses a GPIO interrupt to capture the pulse start / end times and records
 * their duration.
 */

#include <stdint.h>
#include "pico/stdlib.h"
#include "pico/config.h"

#ifndef IR_CAPTURE_BUF_SIZE
/** Maximum number of samples to capture (pulses). */
#define IR_CAPTURE_BUF_SIZE  4096
#endif

#ifndef IR_RECEIVER_PIN
/** Dummy IR receiver GPIO. */
#define IR_RECEIVER_PIN  -1
#endif

// public API

#define IR_CAPTURE_BURST  (1U << 31) 
#define IR_CAPTURE_IS_BURST(data)  \
    ((data) >> 31)
#define IR_CAPTURE_GET_WIDTH(data) \
    (((data) & ~IR_CAPTURE_BURST))

/**
 * Initializes the GPIO used for capturing IR data.
 */
int ir_capture_init();

/**
 * Starts a IR capture using the GPIO bank IRQ.
 */
int ir_capture_start();

/**
 * Stops the IR capture (keeps the buffer's contents intact).
 */
int ir_capture_stop();

/**
 * Use to fetch the capture buffer (read-only!).
 *
 * Also returns the number of items inside the buffer.
 */
unsigned int ir_capture_get_buffer(const uint32_t * *out_buf);

/**
 * Prints the contents of the raw IR capture buffer in ASCII format.
 *
 * Useful for quick debugging.
 */
void ir_capture_print(uint32_t max_entries, uint32_t offset);

#endif
