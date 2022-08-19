/**
 * Copyright (c) 2022 Florin Stancu <niflostancu@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
* Main program unit (a simple demo, for now).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include <tusb.h>

#include "ir_cust_tx.h"
#include "cli.h"
#include "./custom_config.h"
#include "./commands.h"
#include "./device_mgr.h"


static struct pico_cli_state cli;

const struct pico_cli_command_entry main_cli_commands[] = MAIN_CLI_COMMANDS;

static void fatal_error(const char *message);


int main() {
    gpio_init(BOARD_IR_LED_OUT);
    gpio_set_dir(BOARD_IR_LED_OUT, 1);
    gpio_set_slew_rate(BOARD_IR_LED_OUT, GPIO_SLEW_RATE_FAST);
    gpio_set_drive_strength(BOARD_IR_LED_OUT, GPIO_DRIVE_STRENGTH_12MA);

    stdio_init_all();

    pico_cli_init(&cli, main_cli_commands,
                  PICO_CLI_ARRAY_SIZE(main_cli_commands));

    int ret = device_mgr_init();
    if (ret < 0) {
        fatal_error("FATAL: unable to initialize PIO!\n");
        return 1;
    }

    while (1) {
        if (tud_cdc_connected()) {
            /* only enable CLI if the device was plugged into a bhopst */
            pico_cli_process(&cli);
        }
    }
    return 0;
}

void fatal_error(const char *message)
{
    while (!tud_cdc_connected()) { sleep_ms(10); }
    printf(message);
    exit(1);
}

