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

#ifndef IR_CAPTURE_FILTER_DEBOUNCE_US
/** Filter any carrier signals using a debounce period. */
#define IR_CAPTURE_FILTER_DEBOUNCE_US 0
#endif

// public API

/** Encapsulates metadata about a raw signal. */
struct ir_capture_metadata {
    /** Whether the carrier signal was captured unfiltered
     * (i.e., with individual pulses). */
    unsigned int unfiltered: 1;
    /** If unfiltered carrier was captured, its original frequency will be
     * detected. */
    unsigned int freq_hz;
};

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
 * IR capture processing loop. Call regularly to update the internal state, run
 * callbacks etc.
 */
int ir_capture_process();

/**
 * Resets the capture buffers and internal state.
 */
void ir_capture_reset();

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
unsigned int ir_capture_get_buffer(const uint32_t * *out_buf,
                                   struct ir_capture_metadata *out_meta);

/**
 * Prints the contents of the raw IR capture buffer in ASCII format.
 *
 * Useful for quick debugging.
 */
void ir_capture_print(uint32_t max_entries, uint32_t offset);

#endif
