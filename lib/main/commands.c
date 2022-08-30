/**
 * Implements the main CLI commands.
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "lib/cli/cli.h"

#include "./device_mgr.h"
#include "./commands.h"


// command handlers
static pico_cli_action_ret_t main_cmd_on(struct pico_cli_state *cli,
            const char *command, const void *args[]);
static pico_cli_action_ret_t main_cmd_off(struct pico_cli_state *cli,
            const char *command, const void *args[]);
static pico_cli_action_ret_t main_cmd_apply(struct pico_cli_state *cli,
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
};

static struct pico_cli_state cli;


/**
 * Initializes the main CLI commands.
 */
void main_cmd_init()
{
    pico_cli_init(&cli, main_cli_commands, PICO_CLI_ARRAY_SIZE(main_cli_commands));
}

/** Runs the main CLI process. */
void main_cli_process()
{
    pico_cli_process(&cli);
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

