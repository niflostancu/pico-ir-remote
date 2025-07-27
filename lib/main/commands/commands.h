#ifndef MAIN_COMMANDS_H_
#define MAIN_COMMANDS_H_
/**
 * Implements the root-level CLI commands.
 */

#include <stdint.h>

#include "rp_cli/cmd_registry.h"

/**
 * Initializes the main CLI commands.
 */
void main_cli_init();

/** Runs the main CLI process. */
void main_cli_process();

/** Changes the current CLI prompt + command set. */
int main_cli_switch_cmdset(const struct rp_cli_cmd_registry *cmd_reg,
						   const char *prompt_str);

/** Main CLI */
extern struct rp_cli_default_desc main_cli;

/* Command registries for each app screen */
extern const struct rp_cli_cmd_registry main_cmds_reg;
extern const struct rp_cli_cmd_registry ircap_cmds_reg;

#endif
