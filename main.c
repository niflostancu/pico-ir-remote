/**
 * Copyright (c) 2022 Florin Stancu <niflostancu@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
* Main program module (MCU entrypoint).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include <tusb.h>
#if defined(RASPBERRYPI_PICO_W)
#include "pico/cyw43_arch.h"
#endif

#include "lib/main/commands.h"
#include "lib/main/device_mgr.h"
#include "lib/main/scheduler.h"
#include "custom_config.h"

static void board_init();
static void board_set_led(uint8_t state);
static void fatal_error(const char *message);

#define STATUS_LED_ON_TIME    250
#define STATUS_LED_OFF_TIME   2000

/** Internal board state. */
static struct {
    uint8_t blink_state;
    absolute_time_t blink_time;

} main_internal_state = {0};


int main() {
    board_init();
    stdio_init_all();

    main_cmd_init();
    scheduler_init();

    int ret = device_mgr_init();
    if (ret < 0) {
        fatal_error("FATAL: unable to initialize the device!\n");
        return 1;
    }

    main_internal_state.blink_time = get_absolute_time();
    while (1) {
        /* only process CLI if the device was connected to a host */
        if (tud_cdc_connected()) {
            main_cli_process();
        }
        /* check whether it's time to blink */
        if (time_reached(main_internal_state.blink_time)) {
            board_set_led(main_internal_state.blink_state);
            main_internal_state.blink_time = make_timeout_time_ms(
                    (main_internal_state.blink_state? STATUS_LED_ON_TIME : STATUS_LED_OFF_TIME));
            main_internal_state.blink_state = !main_internal_state.blink_state;
        }
        /* run scheduler's processing pass */
        scheduler_process(0);
    }
    return 0;
}

/**
 * Initializes the board peripherals.
 */
void board_init()
{
#if defined(PICO_DEFAULT_LED_PIN) && !defined(RASPBERRYPI_PICO_W)
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
#elif defined(RASPBERRYPI_PICO_W)
    if (cyw43_arch_init()) {
        fatal_error("CYW43 arch_init failed!\r\n");
    }
#endif

    gpio_init(BOARD_IR_LED_PIN);
    gpio_set_dir(BOARD_IR_LED_PIN, 1);
    gpio_set_slew_rate(BOARD_IR_LED_PIN, GPIO_SLEW_RATE_FAST);
    gpio_set_drive_strength(BOARD_IR_LED_PIN, GPIO_DRIVE_STRENGTH_12MA);
}

void board_set_led(uint8_t state)
{
#if defined(PICO_DEFAULT_LED_PIN) && !defined(RASPBERRYPI_PICO_W)
    gpio_put(PICO_DEFAULT_LED_PIN, state);
#elif defined(RASPBERRYPI_PICO_W)
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, state);
#endif
}

/**
 * Fatal error: prints a message and stops the MCU execution (using a loop).
 */
void fatal_error(const char *message)
{
    while (!tud_cdc_connected()) { sleep_ms(10); }
    printf(message);
    /* blink very fast to signal the error */
    uint8_t blink = 0;
    while (1) {
        board_set_led(blink);
        blink = !blink;
        sleep_ms(150);
    }
    exit(1);
}

