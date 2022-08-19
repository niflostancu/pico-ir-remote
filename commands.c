/**
 * Implements the main CLI commands.
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "ir_cust_tx.h"
#include "./device_mgr.h"
#include "./commands.h"


pico_cli_action_ret_t main_cmd_apply(
    struct pico_cli_state *cli, const char *command, const void *args[])
{
    pio_sm_put_blocking(main_device_pio, main_device_pio_sm, ir_custom_swap_bytes(0xCB230000));
    pio_sm_put_blocking(main_device_pio, main_device_pio_sm, ir_custom_swap_bytes(0x26020040));
    pio_sm_put_blocking(main_device_pio, main_device_pio_sm, ir_custom_swap_bytes(0x00000000));
    pio_sm_put_blocking(main_device_pio, main_device_pio_sm, ir_custom_swap_bytes(0x00000065));
    printf("IR packet sent!\r\n");
}

