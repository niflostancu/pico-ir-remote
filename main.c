/**
 * Copyright (c) 2022 Florin Stancu <niflostancu@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
* Main program unit (a simple demo, for now).
*/

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include <tusb.h>

#include "./lib/ir_custom_tx/ir_cust_tx.h"
#include "./config.h"

volatile uint8_t capture_started = 0;


int main() {
    gpio_init(BOARD_IR_LED_OUT);
    gpio_set_dir(BOARD_IR_LED_OUT, 1);
    gpio_set_slew_rate(BOARD_IR_LED_OUT, GPIO_SLEW_RATE_FAST);
    gpio_set_drive_strength(BOARD_IR_LED_OUT, GPIO_DRIVE_STRENGTH_12MA);

    stdio_init_all();

    int tx_sm;
    char msg[100] = "";
    char c;

    while (!tud_cdc_connected()) { sleep_ms(100);  }

    uint8_t transition_detected = 0;

    tx_sm = ir_custom_nec_tx_init(pio0, BOARD_IR_LED_OUT);
    if (tx_sm == -1) {
        printf("could not configure PIO\n");
        return -1;
    }

    while (1) {
        pio_sm_put_blocking(pio0, tx_sm, ir_custom_swap_bytes(0xCB230000));
        pio_sm_put_blocking(pio0, tx_sm, ir_custom_swap_bytes(0x26020040));
        pio_sm_put_blocking(pio0, tx_sm, ir_custom_swap_bytes(0x00000000));
        pio_sm_put_blocking(pio0, tx_sm, ir_custom_swap_bytes(0x00000065));
        printf("SENT\r\n");
        sleep_ms(1500);
    }
    return 0;
}
