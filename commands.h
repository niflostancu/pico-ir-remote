#ifndef MAIN_COMMANDS_H_
#define MAIN_COMMANDS_H_
/**
 * Implements the root-level CLI commands.
 */

#include <stdint.h>

#include "cli.h"

pico_cli_action_ret_t main_cmd_apply(
    struct pico_cli_state *cli, const char *command, const void *args[]);

#define MAIN_CLI_COMMANDS  { \
    {.name = "help", \
     .action = pico_cli_default_help, \
     .help = "Show command help." }, \
    {.name = "apply", \
     .action = main_cmd_apply, \
     .help = "Applies (transmits) the current settings over IR to the device." }, \
}

#endif
