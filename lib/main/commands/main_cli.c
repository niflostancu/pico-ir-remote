/**
 * Implements the main CLI commands.
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <rp_cli/cli.h>
#include <rp_cli/rp_stdio.h>
#include <rp_util/cc_helpers.h>

#include "lib/main/device_mgr.h"
#include "./commands.h"


// command handlers
RP_CLI_STATIC_CMD(main_cmd_power);
RP_CLI_STATIC_CMD(main_cmd_ircap);

/** Simple CLI instance */
struct rp_cli_default_desc main_cli;
const struct rp_cli_cmd_entry main_commands[] = {
    RP_CLI_HELP_CMD_ENTRY(&main_cli),
    {"pwr", "Powers the device on/off.", "", main_cmd_power},
    {"ircap", "Enter raw IR capture app.", "", main_cmd_ircap},
};

const struct rp_cli_cmd_registry main_cmds_reg = {
    .entries = main_commands,
    .entries_count = RP_ARRAY_SIZE(main_commands),
};

/**
 * Initializes the main CLI commands.
 */
void main_cli_init()
{
    /* initialize RP CLI stdio library */
    rp_cli_def_init(&main_cli, &main_cmds_reg, &rp_cli_stdio_funcs);
}

/** Runs the main CLI process. */
void main_cli_process()
{
    rp_cli_def_process(&main_cli);
}

/** Changes the current CLI prompt + command set. */
int main_cli_switch_cmdset(const struct rp_cli_cmd_registry *cmd_reg,
						   const char *prompt_str)
{
    if (!cmd_reg) cmd_reg = &main_cmds_reg;
    main_cli.reg = cmd_reg;
    if (!prompt_str) prompt_str = "# ";
    main_cli.rl.prompt_str = prompt_str;
}

rp_cli_action_ret_t main_cmd_power(int argc, const char *argv[],
        void *aux_data)
{
    int res;
    enum device_mgr_basic_cmd dev_cmd = DEVICE_CMD_ON;
    if (argc > 1) {
        if (strcmp(argv[1], "on") == 0 || strcmp(argv[1], "1") == 0) {
            dev_cmd = DEVICE_CMD_ON;
        }
        else if (strcmp(argv[1], "off") == 0 || strcmp(argv[1], "0") == 0) {
            dev_cmd = DEVICE_CMD_OFF;
        } else {
            printf("Invalid argument: %s!\r\n", argv[1]);
            return 0;
        }
    }
    if ((res = device_exec_basic(dev_cmd)) < 0) {
        printf("Device execution error: %i!\r\n", res);
        return 0;
    }
    printf("Command 0x%02X sent!\r\n", dev_cmd);
    return 0;
}

rp_cli_action_ret_t main_cmd_ircap(int argc, const char *argv[],
        void *aux_data)
{
    main_cli_switch_cmdset(&ircap_cmds_reg, "ircap# ");
}
