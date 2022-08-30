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

#include "lib/main/commands.h"
#include "lib/main/device_mgr.h"
#include "custom_config.h"


static void board_init();
static void fatal_error(const char *message);


int main() {
    board_init();
    stdio_init_all();

    main_cmd_init();

    int ret = device_mgr_init();
    if (ret < 0) {
        fatal_error("FATAL: unable to initialize the device!\n");
        return 1;
    }

    while (1) {
        if (tud_cdc_connected()) {
            /* only enable CLI if the device was connected to a host */
            main_cli_process();
        }
    }
    return 0;
}

/**
 * Initializes the board peripherals.
 */
void board_init()
{
    gpio_init(BOARD_IR_LED_PIN);
    gpio_set_dir(BOARD_IR_LED_PIN, 1);
    gpio_set_slew_rate(BOARD_IR_LED_PIN, GPIO_SLEW_RATE_FAST);
    gpio_set_drive_strength(BOARD_IR_LED_PIN, GPIO_DRIVE_STRENGTH_12MA);
}

void fatal_error(const char *message)
{
    while (!tud_cdc_connected()) { sleep_ms(10); }
    printf(message);
    exit(1);
}

