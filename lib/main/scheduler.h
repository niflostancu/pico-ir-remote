#ifndef MAIN_DEVICE_SCHEDULER_H_
#define MAIN_DEVICE_SCHEDULER_H_
/**
 * Implements the IR command scheduler.
 */

#include <stdint.h>

#include "custom_config.h"

#include "./state.h"


#ifndef SCHEDULER_MAX_INTERVALS
#define SCHEDULER_MAX_INTERVALS 2
#endif


/** Encapsulates the settings of a scheduling period. */
struct scheduler_interval_entry {
    /** State of the interval (1 = enabled) */
    uint8_t status;
    /** The index of the next interval to apply. */
    uint8_t next_idx;
    /** Time to keep the settings (in milliseconds) */
    uint32_t period_ms;
    /* TODO: add alternative setting for RTC time */

    /** Device state to apply. */
    struct ir_device_state apply_state;
};

/** Encapsulates the scheduler's full configuration. */
struct scheduler_cfg {
    struct scheduler_interval_entry intervals[SCHEDULER_MAX_INTERVALS];
};

int scheduler_init();

int scheduler_process(uint8_t force_next);

#endif

