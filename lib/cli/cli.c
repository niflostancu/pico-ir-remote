#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"

#include "./cli.h"


static int pico_cli_compare_command_entry(
        const struct pico_cli_command_entry *command,
        const char *user_input);
static const struct pico_cli_command_entry *pico_cli_find_command(
        struct pico_cli_state *cli, const char *user_input);
static int pico_cli_is_control_char(char c);


int pico_cli_init(struct pico_cli_state *cli,
                  const struct pico_cli_command_entry *commands,
                  uint command_num)
{
    cli->commands = commands;
    cli->command_num = command_num;
    cli->state = PICO_CLI_STATE_INIT;
    cli->cur_line_len = 0;
}

/**
 * Reads 1 more char from stdin (if any) and processes the CLI state machine.
 */
int pico_cli_process(struct pico_cli_state *cli)
{
    while (1) {
        if (cli->state == PICO_CLI_STATE_COMPLETE) {
            /* reset CLI state */
            cli->cur_line[0] = 0;
            cli->cur_line_len = 0;
            cli->state = PICO_CLI_STATE_INIT;
        }
        int raw = getchar_timeout_us(PICO_CLI_GETCHAR_TIMEOUT);
        if (raw == PICO_ERROR_TIMEOUT)
            return 0;
        if (cli->state == PICO_CLI_STATE_DISABLED)
            return -1;
        char c = (char)raw;
        switch (c) {
            case '\r': /* carriage return */
            case '\0': /* NULL character or new line */
            case '\n':
                goto _line_complete;
            case '\b':   /* backspace */
            case '\x7f': /* delete */
                /* remove char from screen */
                putchar_raw('\b');
                putchar_raw(' ');
                putchar_raw('\b');
                /* also delete it from the buffer */
                if (cli->cur_line_len > 0) {
                    cli->cur_line[cli->cur_line_len - 1] = 0;
                    cli->cur_line_len--;
                }
                break;
                
            default:
                /* only append displayable chars */
                if ((c >= 32 && c <= 126)) {
                    /* append to buffer */
                    if ((cli->cur_line_len + 1) >= PICO_CLI_MAX_LINE_LENGTH)
                        return -3;
                    cli->cur_line[cli->cur_line_len++] = c;
                    cli->cur_line[cli->cur_line_len] = 0;
                    putchar_raw(c);
                }
                break;
        }
        cli->state = PICO_CLI_STATE_STARTED;
    }

_line_complete:
    cli->state = PICO_CLI_STATE_COMPLETE;
    printf("\r\n");

    if (cli->cur_line_len == 0) {
        return 0;
    }

    const struct pico_cli_command_entry *cmd = pico_cli_find_command(cli, cli->cur_line);
    if (cmd) {
        if (cmd->action) {
            pico_cli_action_ret_t ret = cmd->action(cli, cli->cur_line, NULL);
        }
    } else {
        printf("Invalid input: %s\r\n", cli->cur_line);
        printf("Use 'help' for a list of commands available.\r\n");
    }
    
    return 1;
}

pico_cli_action_ret_t pico_cli_default_help(
    struct pico_cli_state *cli, const char *command, const void *args[])
{
    int i;
    printf("Commands available:\r\n");
    for (i=0; i<cli->command_num; i++) {
        printf(" > %s: %s\r\n", cli->commands[i].name, cli->commands[i].help);
    }
    printf("Use 'help <command>' to get details about a command.\r\n");
}

int pico_cli_compare_command_entry(
        const struct pico_cli_command_entry *command,
        const char *user_input)
{
    int i;
    int u_n = strlen(user_input), c_n = strlen(command->name);
    if (u_n < c_n) {
        /* user input is too short */
        return 0;
    }
    for (i=0; i<c_n; i++) {
        if (user_input[i] != command->name[i])
            return 0;
    }
    /* check if the user input terminates (space character) */
    if (u_n > c_n) {
        if ((user_input[c_n] != ' ') && (user_input[c_n] != '\t'))
            return 0;
    }
    /* command matches! */
    return 1;
}

const struct pico_cli_command_entry *pico_cli_find_command(
        struct pico_cli_state *cli, const char *user_input)
{
    int  i;
    for (i=0; i<cli->command_num; i++) {
        if (pico_cli_compare_command_entry(&cli->commands[i], user_input)) {
            return &cli->commands[i];
        }
    }
    return NULL;
}

