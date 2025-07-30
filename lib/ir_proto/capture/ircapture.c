#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/platform.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"

#include "ircapture.h"

#define IR_CAPTURE_PRINT_SPLIT  16

enum ir_capture_state {
    IR_CAP_RESET = 0,
    IR_CAP_IDLE,
    IR_CAP_STARTED,
#if IR_CAPTURE_FILTER_DEBOUNCE_US
    IR_CAP_STARTED_RAW,
#endif
};

/** Buffer with the recorded pulse timestamps. */
static volatile uint32_t ir_capture_buf[IR_CAPTURE_BUF_SIZE] = {0};
/** The next free index inside the capture buffer. */
static volatile uint32_t ir_capture_idx = 0;

static volatile struct {
    /** Whether the capture is started. */
    bool started;
    /** Whether a transmission was detected. */
    enum ir_capture_state rstate;
    /** Monotonic MCU time when previous (unfiltered/carrier) pulse was received. */
    uint64_t prev_pulse_us;
    /** Monotonic MCU time when the burst was started. */
    uint64_t burst_started_us;
    /** Number of individual pulses inside current burst (used for determining freq.). */
    uint32_t burst_count;

    /** Metadata about the capture */
    struct ir_capture_metadata meta;
    /* The longest burst count the frequency is be based on */
    uint32_t longest_burst_count;

} cap_state = {0};


/** Checks if a carrier burst has ended (debounce period passed) */
#define CHECK_RAW_BURST_ENDED(pulse_duration)  \
    ((cap_state.rstate == IR_CAP_STARTED_RAW && \
            (pulse_duration) > IR_CAPTURE_FILTER_DEBOUNCE_US))

static void cap_record_state(bool is_burst, uint32_t duration);
static void cap_try_compute_carrier_freq(uint32_t burst_duration_us);
static void cap_irq_control_raw(bool enable);
static void cap_irq_callback(uint gpio, uint32_t events);


/**
 * Initializes the GPIO used for capturing IR data.
 */
int ir_capture_init()
{
    if (IR_RECEIVER_PIN < 0) return -1;
    gpio_init(IR_RECEIVER_PIN);
    gpio_set_dir(IR_RECEIVER_PIN, GPIO_IN);
    gpio_set_pulls(IR_RECEIVER_PIN, true, false);

    ir_capture_reset();
    return 0;
}

/**
 * Resets the capture buffers and internal state.
 */
void ir_capture_reset()
{
    ir_capture_idx = 0;
    cap_state.rstate = IR_CAP_RESET;
    cap_state.burst_started_us = 0;
    cap_state.prev_pulse_us = 0;
    cap_state.burst_count = 0;
    cap_state.longest_burst_count = 0;
    cap_state.meta.freq_hz = 0;
    cap_state.meta.unfiltered = 0;
}

/**
 * Starts a IR capture using the GPIO bank IRQ.
 */
int ir_capture_process()
{
    if (!cap_state.started) return 0;
    return 0;
#if IR_CAPTURE_FILTER_DEBOUNCE_US
    const uint64_t now = time_us_64();
    if (CHECK_RAW_BURST_ENDED(now - cap_state.prev_pulse_us)) {
        /* start of critical section */
        cap_irq_control_raw(false);
        /* evaluate condition again (might have been changed inside interrupt) */
        if (!CHECK_RAW_BURST_ENDED(now - cap_state.prev_pulse_us))
            goto _ir_cap_continue;
        /* previous burst has finished a while ago, record it */
        uint32_t duration = (uint32_t)(cap_state.prev_pulse_us - cap_state.burst_started_us);
        cap_record_state(1, duration);
        cap_try_compute_carrier_freq(duration);
        cap_state.burst_count = 0;
        cap_state.rstate = IR_CAP_IDLE;
        goto _ir_cap_continue;
    }
#endif
_ir_cap_continue:
    cap_irq_control_raw(true);

    return 0;
}

/**
 * Starts a IR capture using the GPIO bank IRQ.
 */
int ir_capture_start()
{
    if (IR_RECEIVER_PIN < 0) return -1;
    ir_capture_reset();

    gpio_set_irq_enabled_with_callback(
        IR_RECEIVER_PIN, (GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE),
        true, cap_irq_callback);
    cap_state.started = true;
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

    cap_state.started = false;
    return 0;
}

/**
 * Prints the contents of the raw IR capture buffer in ASCII format.
 */
void ir_capture_print(uint32_t max_entries, uint32_t offset)
{
    unsigned int prev_split = offset, i;
    if (!max_entries || (max_entries > IR_CAPTURE_BUF_SIZE)) max_entries = IR_CAPTURE_BUF_SIZE;

    printf("Capture data: gpio %i => %i transitions\r\n", IR_RECEIVER_PIN, ir_capture_idx);
    if (cap_state.meta.unfiltered) {
        printf("Unfiltered (raw) signal: %u Hz (%u bursts)\r\n", 
               cap_state.meta.freq_hz, cap_state.longest_burst_count);
    }
    for (i=offset; i<max_entries && i < ir_capture_idx; i++) {
        bool is_burst = IR_CAPTURE_IS_BURST(ir_capture_buf[i]);
        printf("%b:%d\t", is_burst, IR_CAPTURE_GET_WIDTH(ir_capture_buf[i]));
        if ((i - prev_split + 1) >= IR_CAPTURE_PRINT_SPLIT) {
            printf("\r\n");
            prev_split = i;
        }
    }
    if ((prev_split + 1) < max_entries)
        printf("\r\n");
}

/**
 * Use to fetch the capture buffer (read-only!).
 *
 * Also returns the number of items inside the buffer.
 */
unsigned int ir_capture_get_buffer(const uint32_t * *out_buf,
    struct ir_capture_metadata *out_meta)
{
    /* note: it's okay to get rid of volatile */
    *out_buf = (const uint32_t *)ir_capture_buf;
    if (out_meta) {
        *out_meta = cap_state.meta;
    }
    return ir_capture_idx;
}

/** Records a burst/idle state transition. */
static void cap_record_state(bool is_burst, uint32_t duration)
{
    if ((ir_capture_idx) >= IR_CAPTURE_BUF_SIZE)
        return;
    if (is_burst) duration = (duration & ~IR_CAPTURE_BURST) | IR_CAPTURE_BURST;
    ir_capture_buf[ir_capture_idx++] = duration;
}

/** Computes & records the carrier frequency for the longest pulse yet. */
static void cap_try_compute_carrier_freq(uint32_t burst_duration_us)
{
    if (cap_state.longest_burst_count >= cap_state.burst_count)
        return;  /* only the best is taken */
    cap_state.meta.unfiltered = 1;
    uint64_t ffreq = ((uint64_t)cap_state.burst_count * 1000000UL) / burst_duration_us;
    cap_state.meta.freq_hz = (unsigned int)ffreq;
    cap_state.longest_burst_count = cap_state.burst_count;
}

/**
 * Raw interrupt enable control (without clearing any pending flags).
 */
static void cap_irq_control_raw(bool enabled)
{
    uint gpio = IR_RECEIVER_PIN;
    uint events = GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE;
    io_irq_ctrl_hw_t *irq_ctrl_base = (
        get_core_num() ? &iobank0_hw->proc1_irq_ctrl : &iobank0_hw->proc0_irq_ctrl);
    io_rw_32 *en_reg = &irq_ctrl_base->inte[gpio / 8];
    events <<= 4 * (gpio % 8);

    if (enabled)
        hw_set_bits(en_reg, events);
    else
        hw_clear_bits(en_reg, events);
}

/** GPIO interrupt callback. */
void cap_irq_callback(uint gpio, uint32_t events)
{
    if (gpio != IR_RECEIVER_PIN)
        return;

    const uint64_t now = time_us_64();
    uint64_t pulse_duration = (now - cap_state.prev_pulse_us);
    const bool falling_edge = (events & GPIO_IRQ_EDGE_FALL);
    const bool rising_edge = (events & GPIO_IRQ_EDGE_RISE);

#if IR_CAPTURE_FILTER_DEBOUNCE_US
    /* check if we need to transition from STARTED_RAW to IDLE */
    if (CHECK_RAW_BURST_ENDED(pulse_duration)) {
        uint32_t duration = (uint32_t)(cap_state.prev_pulse_us - cap_state.burst_started_us);
        cap_record_state(1, duration);
        cap_try_compute_carrier_freq(duration);
        cap_state.burst_count = 0;
        cap_state.rstate = IR_CAP_IDLE;
    }
#endif
    cap_state.prev_pulse_us = now;

    if (cap_state.rstate == IR_CAP_RESET) {
        /* reset duration and transition from IDLE */
        cap_state.rstate = IR_CAP_IDLE;
        pulse_duration = 0;
    }

    if (cap_state.rstate == IR_CAP_IDLE) {
        /* invalid transition from IDLE, so just ignore */
        if (!falling_edge) return;
        /* record the idle period */
        if (pulse_duration > 0) {
            cap_record_state(0, pulse_duration);
        }
        cap_state.rstate = IR_CAP_STARTED;
        /* record the time of the first carrier pulse (either continuous or raw,
         * we don't know yet) */
        cap_state.burst_started_us = now;

    } else if (cap_state.rstate == IR_CAP_STARTED) {
#if IR_CAPTURE_FILTER_DEBOUNCE_US
        if (pulse_duration <= IR_CAPTURE_FILTER_DEBOUNCE_US) {
            /* we're receiving pulsed (unfiltered) signal, apply a soft debounce */
            cap_state.rstate = IR_CAP_STARTED_RAW;
            if (rising_edge) {
                cap_state.burst_count++;
            }
        } else
#endif
        if (rising_edge) {
            /* continuous (already filtered) burst ended, record it */
            cap_record_state(1, pulse_duration);
            cap_state.rstate = IR_CAP_IDLE;
            cap_state.burst_count = 0;
        }
    }
#if IR_CAPTURE_FILTER_DEBOUNCE_US
    else if (cap_state.rstate == IR_CAP_STARTED_RAW) {
        if (rising_edge) {
            cap_state.burst_count++;
        }
    }
#endif
}

