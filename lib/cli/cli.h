#ifndef _LIB_PICO_CLI_H
#define _LIB_PICO_CLI_H
/**
 * Custom IR protocol transmitter library for RPI Pico.
 */

#include <stdint.h>

#include "pico/stdlib.h"

/* getchar timeout (us) */
#define PICO_CLI_GETCHAR_TIMEOUT  10

/** Max. command line buffer length. */
#define PICO_CLI_MAX_LINE_LENGTH 80

/** Max. command arguments. */
#define PICO_CLI_MAX_CMD_ARGS  4

/** Helper array size macro. */
#define PICO_CLI_ARRAY_SIZE(_arr)   ( sizeof(_arr) / sizeof(_arr[0]) )

/** RPI CLI action processing effects. */
typedef enum {
    PICO_CLI_ACTION_CONTINUE = 0,
    PICO_CLI_ACTION_BREAK = 1,

} pico_cli_action_ret_t;

struct pico_cli_state;

typedef pico_cli_action_ret_t (*pico_cli_action_func)(
        struct pico_cli_state *cli, const char *command, const void *args[]);

/** RPI CLI command entry. */
struct pico_cli_command_entry {
    /** The command name / pattern to match. */
    const char *name;
    /** Help string for this command. */
    const char *help;
    /** Command argument type specifiers. */
    int num_args;
    /** The callback to execute for this action. */
    const pico_cli_action_func action;
};

typedef enum {
    PICO_CLI_STATE_DISABLED = 0,
    PICO_CLI_STATE_INIT = 1,
    PICO_CLI_STATE_STARTED = 2,
    PICO_CLI_STATE_COMPLETE = 0xF,

} pico_cli_states_t;

struct pico_cli_state {
    /** Current input state. */
    pico_cli_states_t state;

    const struct pico_cli_command_entry *commands;
    uint32_t command_num;

    /** Current line contents & length. */
    char cur_line[PICO_CLI_MAX_LINE_LENGTH];
    int cur_line_len;
};

// public API

/**
 * Initializes the CLI library and registers the command pattern / callbacks
 * list.
 */
int pico_cli_init(struct pico_cli_state *cli,
                  const struct pico_cli_command_entry *commands,
                  uint command_num);

/**
 * Runs a CLI processing step, reading from the input buffer and updating the
 * internal state machine.
 *
 * Make sure to call it periodically inside your main loop.
 * Automatically invokes the registered callback functions when a complete
 * command is received.
 */
int pico_cli_process(struct pico_cli_state *cli);

/**
 * Builds & prints the default help screen. 
 */
pico_cli_action_ret_t pico_cli_default_help(
    struct pico_cli_state *cli, const char *command, const void *args[]);

#endif
