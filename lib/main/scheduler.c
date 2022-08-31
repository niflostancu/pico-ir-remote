/**
 * Implements the IR device control scheduler.
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "pico/time.h"

#include "./device_mgr.h"
#include "./scheduler.h"
#include "custom_config.h"


/** Current scheduler configuration. */
static struct scheduler_cfg scheduler_config = {
    .intervals = {
        {
            .status = 1, .next_idx = 1,
            .period_ms = SCHEDULER_DEFAULT_ON_TIME_MS,
            .apply_state = {.status = 1}
        },
        {
            .status = 1, .next_idx = 0,
            .period_ms = SCHEDULER_DEFAULT_OFF_TIME_MS,
            .apply_state = {.status = 0}
        }
    },
};

/** Dynamic scheduler state. */
static struct {
    uint8_t status;
    uint8_t next_idx;
    absolute_time_t next_time;

} scheduler_state = {0};


int scheduler_init()
{
    scheduler_state.status = 1;
    scheduler_state.next_idx = 0;
    scheduler_state.next_time = get_absolute_time();
    return 0;
}

int scheduler_process(uint8_t force_next)
{
    int res;
    if (!scheduler_state.status)
        return 0; /* disabled */
    if (!time_reached(scheduler_state.next_time) && !force_next)
        return 0;
    if (scheduler_state.next_idx > SCHEDULER_MAX_INTERVALS) {
        printf("Scheduler: invalid next idx: %i!\r\n", scheduler_state.next_idx);
        scheduler_state.status = 0;
        return -1;
    }
    struct scheduler_interval_entry *next_sched = &scheduler_config.intervals[scheduler_state.next_idx];
    if (!next_sched->status) {
        /* TODO: search for a valid schedule interval when RTC is implemented */
        printf("Scheduler: stopped at #%i!\r\n", scheduler_state.next_idx);
        scheduler_state.status = 0;
        return -2;
    }

    /* apply the scheduled device state */
    if ((res = device_apply_state(next_sched->apply_state)) < 0) {
        printf("Scheduler: Device execution error: %i!\r\n", res);
        /* TODO: try again for a # of retries? */
    } else {
        printf("Scheduler: applied schedule #%i!\r\n", scheduler_state.next_idx);
    }
    /* perpare for the next schedule */
    scheduler_state.next_time = make_timeout_time_ms(next_sched->period_ms);
    scheduler_state.next_idx = next_sched->next_idx;
    return 1;
}

