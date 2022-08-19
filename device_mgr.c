/**
 * Implements the main CLI commands.
 */

#include <stdlib.h>
#include <stdint.h>

#include "ir_cust_tx.h"
#include "./device_mgr.h"

PIO main_device_pio;
int main_device_pio_sm = -0xFF;


/**
 * Initializes the IR device manager.
 */
int device_mgr_init()
{
    main_device_pio = pio0;
    main_device_pio_sm = ir_custom_nec_tx_init(pio0, BOARD_IR_LED_OUT);
    return main_device_pio_sm;
}

