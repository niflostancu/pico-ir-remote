/**
 * Implements the main CLI commands.
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "lib/cli/cli.h"
#include "lib/ir_proto/capture/ircapture.h"

#include "./device_mgr.h"
#include "./commands.h"


// command handlers
static pico_cli_action_ret_t main_cmd_on(struct pico_cli_state *cli,
            const char *command, const void *args[]);
static pico_cli_action_ret_t main_cmd_off(struct pico_cli_state *cli,
            const char *command, const void *args[]);
static pico_cli_action_ret_t main_cmd_apply(struct pico_cli_state *cli,
            const char *command, const void *args[]);

pico_cli_action_ret_t main_cmd_ir_cap_start(struct pico_cli_state *cli,
            const char *command, const void *args[]);
pico_cli_action_ret_t main_cmd_ir_cap_stop(struct pico_cli_state *cli,
            const char *command, const void *args[]);
pico_cli_action_ret_t main_cmd_ir_cap_print(struct pico_cli_state *cli,
            const char *command, const void *args[]);

const struct pico_cli_command_entry main_cli_commands[] = {
    {.name = "help",
     .action = pico_cli_default_help,
     .help = "Show command help." },
    {.name = "apply",
     .action = main_cmd_apply,
     .help = "Applies (transmits) the current settings over IR to the device." },
    {.name = "on",
     .action = main_cmd_on,
     .help = "Turns the device on." },
    {.name = "off",
     .action = main_cmd_off,
     .help = "Turns the device off." },
    {.name = "ir_cap_start",
     .action = main_cmd_ir_cap_start,
     .help = "Starts raw IR capture." },
    {.name = "ir_cap_stop",
     .action = main_cmd_ir_cap_stop,
     .help = "Stops the raw IR capture." },
    {.name = "ir_cap_print",
     .action = main_cmd_ir_cap_print,
     .help = "Starts raw IR capture." },
};

static struct pico_cli_state cli;

/** The MCU timestamp to end IR capture at. */
static uint64_t _ir_capture_end_us = 0;

/**
 * Initializes the main CLI commands.
 */
void main_cmd_init()
{
    pico_cli_init(&cli, main_cli_commands, PICO_CLI_ARRAY_SIZE(main_cli_commands));
    ir_capture_init();
}

/** Runs the main CLI process. */
void main_cli_process()
{
    pico_cli_process(&cli);
    if (_ir_capture_end_us && (_ir_capture_end_us < time_us_64())) {

    }
}

pico_cli_action_ret_t main_cmd_on(struct pico_cli_state *cli,
            const char *command, const void *args[])
{
    int res;
    if ((res = device_exec_basic(DEVICE_CMD_ON)) < 0) {
        printf("Device execution error: %i!\r\n", res);
        return 0;
    }
    printf("IR packet sent!\r\n");
    return 0;
}

pico_cli_action_ret_t main_cmd_off(struct pico_cli_state *cli,
            const char *command, const void *args[])
{
    int res;
    if ((res = device_exec_basic(DEVICE_CMD_OFF)) < 0) {
        printf("Device execution error: %i!\r\n", res);
        return 0;
    }
    printf("IR packet sent!\r\n");
    return 0;
}

pico_cli_action_ret_t main_cmd_apply(struct pico_cli_state *cli,
            const char *command, const void *args[])
{
    printf("Not implemented yet!\r\n");
    return 0;
}

pico_cli_action_ret_t main_cmd_ir_cap_start(struct pico_cli_state *cli,
            const char *command, const void *args[])
{
    ir_capture_start();
    printf("Raw IR capture started!\r\n");
    return 0;
}

pico_cli_action_ret_t main_cmd_ir_cap_stop(struct pico_cli_state *cli,
            const char *command, const void *args[])
{
    ir_capture_stop();
    printf("Raw IR capture stopped!\r\n");
    return 0;
}

pico_cli_action_ret_t main_cmd_ir_cap_print(struct pico_cli_state *cli,
            const char *command, const void *args[])
{
    uint32_t print_max = 0;
    uint32_t print_offset = 0;
    ir_capture_print(print_max, print_offset);
    return 0;
}

