/**
 * Implements the main CLI commands.
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include <rp_cli/cli.h>
#include <rp_cli/rp_stdio.h>
#include <rp_util/cc_helpers.h>

#include "lib/ir_proto/capture/ircapture.h"

#include "./device_mgr.h"
#include "./commands.h"


// command handlers
RP_CLI_STATIC_CMD(main_cmd_on);
RP_CLI_STATIC_CMD(main_cmd_off);
RP_CLI_STATIC_CMD(main_cmd_apply);
RP_CLI_STATIC_CMD(main_cmd_ir_cap_start);
RP_CLI_STATIC_CMD(main_cmd_ir_cap_stop);
RP_CLI_STATIC_CMD(main_cmd_ir_cap_print);

/** Simple CLI instance */
struct rp_cli_default_desc main_cli;
const struct rp_cli_cmd_entry main_cli_commands[] = {
    RP_CLI_HELP_CMD_ENTRY(&main_cli),
    {"apply", "Applies (transmits) the current settings over IR to the device.",
     "", main_cmd_apply},
    {"on", "Turns the device on.", "", main_cmd_on },
    {"off", "Turns the device off.", "", main_cmd_off },
    {"ir_cap_start", "Starts raw IR capture.", "", main_cmd_ir_cap_start },
    {"ir_cap_stop", "Stops the raw IR capture.", "", main_cmd_ir_cap_stop },
    {"ir_cap_print", "Starts raw IR capture.", "", main_cmd_ir_cap_print },
};

const struct rp_cli_cmd_registry main_reg = {
    .entries = main_cli_commands,
    .entries_count = RP_ARRAY_SIZE(main_cli_commands),
};

/**
 * Initializes the main CLI commands.
 */
void main_cmd_init()
{
    /* initialize RP CLI stdio library */
    rp_cli_def_init(&main_cli, &main_reg, &rp_cli_stdio_funcs);
    ir_capture_init();
}

/** Runs the main CLI process. */
void main_cli_process()
{
    rp_cli_def_process(&main_cli);
}

pico_cli_action_ret_t main_cmd_on(int argc, const char *argv[],
        void *aux_data)
{
    int res;
    if ((res = device_exec_basic(DEVICE_CMD_ON)) < 0) {
        printf("Device execution error: %i!\r\n", res);
        return 0;
    }
    printf("IR packet sent!\r\n");
    return 0;
}

pico_cli_action_ret_t main_cmd_off(int argc, const char *argv[],
        void *aux_data)
{
    int res;
    if ((res = device_exec_basic(DEVICE_CMD_OFF)) < 0) {
        printf("Device execution error: %i!\r\n", res);
        return 0;
    }
    printf("IR packet sent!\r\n");
    return 0;
}

pico_cli_action_ret_t main_cmd_apply(int argc, const char *argv[],
        void *aux_data)
{
    printf("Not implemented yet!\r\n");
    return 0;
}

pico_cli_action_ret_t main_cmd_ir_cap_start(int argc, const char *argv[],
        void *aux_data)
{
    ir_capture_start();
    printf("Raw IR capture started!\r\n");
    return 0;
}

pico_cli_action_ret_t main_cmd_ir_cap_stop(int argc, const char *argv[],
        void *aux_data)
{
    ir_capture_stop();
    printf("Raw IR capture stopped!\r\n");
    return 0;
}

pico_cli_action_ret_t main_cmd_ir_cap_print(int argc, const char *argv[],
        void *aux_data)
{
    uint32_t print_max = 0;
    uint32_t print_offset = 0;
    ir_capture_print(print_max, print_offset);
    return 0;
}

