#include <hardware/gpio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"

#include "ircapture.h"

#define IR_CAPTURE_PRINT_SPLIT  16


/** Buffer with the recorded pulse timestamps. */
static volatile uint32_t ir_capture_buf[IR_CAPTURE_BUF_SIZE] = {0};
/** The next free index inside the capture buffer. */
static volatile uint32_t ir_capture_idx = 0;
static volatile bool ir_pulse_state = false;
/** Monotonic MCU time when previous unfiltered pulse was received. */
static volatile uint64_t ir_prev_pulse_us = 0;
/** Monotonic MCU time when prev. complete bit was received. */
static volatile uint64_t ir_prev_bit_us = 0;

static void ir_capture_irq_callback(uint gpio, uint32_t events);

/**
 * Initializes the GPIO used for capturing IR data.
 */
int ir_capture_init()
{
    if (IR_RECEIVER_PIN < 0) return -1;
    gpio_init(IR_RECEIVER_PIN);
    gpio_set_dir(IR_RECEIVER_PIN, GPIO_IN);
    gpio_set_pulls(IR_RECEIVER_PIN, true, false);

    /* ir_capture_start(); */
    return 0;
}

/**
 * Starts a IR capture using the GPIO bank IRQ.
 */
int ir_capture_start()
{
    if (IR_RECEIVER_PIN < 0) return -1;

    ir_capture_idx = 0;
    ir_prev_bit_us = time_us_64();
    ir_prev_pulse_us = ir_prev_bit_us;
    ir_pulse_state = false;

    gpio_set_irq_enabled_with_callback(
        IR_RECEIVER_PIN, (GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE),
        true, ir_capture_irq_callback);
    return 0;
}

/**
 * Stops the IR capture (keeps the buffer's contents intact).
 */
int ir_capture_stop()
{
    if (IR_RECEIVER_PIN < 0) return -1;

    gpio_set_irq_enabled(IR_RECEIVER_PIN,
                         (GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE), false);

    return 0;
}

/**
 * Prints the contents of the raw IR capture buffer in ASCII format.
 */
void ir_capture_print(uint32_t max_entries, uint32_t offset)
{
    unsigned int prev_split = offset, i;
    if (!max_entries || (max_entries > IR_CAPTURE_BUF_SIZE)) max_entries = IR_CAPTURE_BUF_SIZE;

    printf("IR_CAPTURE_SIZE=%i (gpio %i)\r\n", ir_capture_idx, IR_RECEIVER_PIN);
    for (i=offset; i<max_entries && i < ir_capture_idx; i++) {
        uint32_t sign_bit = ir_capture_buf[i] >> 31;
        printf("%b:%d\t", sign_bit, (ir_capture_buf[i] & ~(1U << 31)));
        if ((i - prev_split) >= IR_CAPTURE_PRINT_SPLIT) {
            printf("\r\n");
            prev_split = i;
        }
    }
}

static void ir_capture_record_bit(bool bit, uint32_t duration)
{
    if ((ir_capture_idx) >= IR_CAPTURE_BUF_SIZE)
        return;
    if (bit) duration |= (1 << 31);
    ir_capture_buf[ir_capture_idx++] = duration;
}

/** GPIO interrupt callback. */
void ir_capture_irq_callback(uint gpio, uint32_t events)
{
    if (gpio != IR_RECEIVER_PIN)
        return;

    /* end time of the previous pulse */
    uint64_t end_time_us = time_us_64();
    uint32_t pulse_duration = (end_time_us - ir_prev_pulse_us);

#if defined(IR_RECV_FILTER_CARRIER_US) && (IR_RECV_FILTER_CARRIER_US)
    if (pulse_duration > IR_RECV_FILTER_CARRIER_US) {
        ir_capture_record_bit(1, ir_prev_pulse_us - ir_prev_bit_us);
        ir_capture_record_bit(0, pulse_duration);
        ir_prev_bit_us = end_time_us;
    }
#else
    ir_capture_record_bit((events & GPIO_IRQ_EDGE_RISE), pulse_duration);
#endif
    ir_prev_pulse_us = end_time_us;
}

