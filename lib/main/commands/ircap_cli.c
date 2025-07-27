/**
 * Implements the raw IR capture CLI commands.
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <rp_cli/cli.h>
#include <rp_cli/rp_stdio.h>
#include <rp_util/cc_helpers.h>

#include "lib/ir_proto/capture/ircapture.h"
#include "./commands.h"


// command handlers
RP_CLI_STATIC_CMD(ircap_cmd_start);
RP_CLI_STATIC_CMD(ircap_cmd_stop);
RP_CLI_STATIC_CMD(ircap_cmd_print);
RP_CLI_STATIC_CMD(ircap_cmd_exit);

/** Simple CLI instance */
const struct rp_cli_cmd_entry ircap_commands[] = {
    RP_CLI_HELP_CMD_ENTRY(&main_cli),
    {"start", "Powers the device on/off.", "", ircap_cmd_start},
    {"stop", "Enter raw IR capture app.", "", ircap_cmd_stop},
    {"print", "Prints the contents of the capture buffer.", "", ircap_cmd_print},
    {"exit", "Exits IR capture mode.", "", ircap_cmd_exit},
};

const struct rp_cli_cmd_registry ircap_cmds_reg = {
    .entries = ircap_commands,
    .entries_count = RP_ARRAY_SIZE(ircap_commands),
};


rp_cli_action_ret_t ircap_cmd_start(int argc, const char *argv[],
        void *aux_data)
{
    ir_capture_start();
    printf("Raw IR capture started!\r\n");
    return 0;
}

rp_cli_action_ret_t ircap_cmd_stop(int argc, const char *argv[],
        void *aux_data)
{
    ir_capture_stop();
    printf("Raw IR capture started!\r\n");
    return 0;
}

rp_cli_action_ret_t ircap_cmd_print(int argc, const char *argv[],
        void *aux_data)
{
    uint32_t print_max = 0;
    uint32_t print_offset = 0;
    ir_capture_print(print_max, print_offset);
    return 0;
}

rp_cli_action_ret_t ircap_cmd_exit(int argc, const char *argv[],
        void *aux_data)
{
    main_cli_switch_cmdset(NULL, NULL);
}
