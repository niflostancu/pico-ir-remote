/**
 * Implements the raw IR capture CLI commands.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <rp_cli/cli.h>
#include <rp_cli/rp_stdio.h>
#include <rp_util/cc_helpers.h>

#include "lib/ir_proto/capture/ircapture.h"
#include "lib/ir_proto/decoder/decode.h"
#include "./commands.h"

#ifndef IR_DECODE_BUF_MAX
#define IR_DECODE_BUF_MAX  256
#endif

// command handlers
RP_CLI_STATIC_CMD(ircap_cmd_start);
RP_CLI_STATIC_CMD(ircap_cmd_stop);
RP_CLI_STATIC_CMD(ircap_cmd_print);
RP_CLI_STATIC_CMD(ircap_cmd_decode);
RP_CLI_STATIC_CMD(ircap_cmd_exit);

static void _print_buf_data(uint32_t *buf, unsigned int length);

/** Simple CLI instance */
const struct rp_cli_cmd_entry ircap_commands[] = {
    RP_CLI_HELP_CMD_ENTRY(&main_cli),
    {"start", "Powers the device on/off.", "", ircap_cmd_start},
    {"stop", "Enter raw IR capture app.", "", ircap_cmd_stop},
    {"print", "Prints the contents of a buffer.", "", ircap_cmd_print},
    {"decode", "Decodes the captured pulses.", "", ircap_cmd_decode},
    {"exit", "Exits IR capture mode.", "", ircap_cmd_exit},
};

const struct rp_cli_cmd_registry ircap_cmds_reg = {
    .entries = ircap_commands,
    .entries_count = RP_ARRAY_SIZE(ircap_commands),
};

// internal decode buffer
static uint32_t decode_buf[IR_DECODE_BUF_MAX];
static unsigned int decode_buf_idx = 0;

static const struct ir_decode_mod_config default_decode_cfg = {
    .type = IR_DECODE_PDM,
    .flags = {0},
    .adv_timing = {
        .tolerance_percent = 20,
        .idle_ms = 30,
        .datap_us = 483,
        .zero_bit_us = 336,
        .one_bit_us = 1104,
    }
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
    const char *buf_name = "raw";
    if (argc > 1) {
        if ((strcmp(argv[1], "raw") == 0) || (strcmp(argv[1], "dec") == 0)) {
            buf_name = argv[1];
        }
        else {
            printf("Invalid argument: '%s'!\r\n", argv[1]);
            return 0;
        }
    }
    if (strcmp(buf_name, "raw") == 0) {
        ir_capture_print(print_max, print_offset);
    } else if (strcmp(buf_name, "dec") == 0) {
        printf("Decode buffer [%u]:\r\n", decode_buf_idx);
        _print_buf_data(decode_buf, decode_buf_idx);
    }
    return 0;
}

rp_cli_action_ret_t ircap_cmd_decode(int argc, const char *argv[],
        void *aux_data)
{
    struct ir_decode_mod_config cfg = default_decode_cfg;
    const uint32_t *raw_buf = NULL;
    int raw_len = ir_capture_get_buffer(&raw_buf);
    int res = ir_decode_mod_pdm(raw_buf, raw_len, &cfg, decode_buf, IR_DECODE_BUF_MAX);
    if (res < 0) {
        printf("Decode failed with %i!\r\n", res);
        return 0;
    }
    decode_buf_idx = (unsigned int)res;
    printf("Decoding finished: %i symbols!\r\n", res);
    return 0;
}

rp_cli_action_ret_t ircap_cmd_exit(int argc, const char *argv[],
        void *aux_data)
{
    main_cli_switch_cmdset(NULL, NULL);
}

/**
 * Prints the contents of a 32-bit data buffer (in hex formtat).
 */
void _print_buf_data(uint32_t *buf, unsigned int length)
{
    unsigned int prev_split = 0, i, prev_special = 0;

    for (i=0; i<length; i++) {
        bool is_special = !!(buf[i] & IR_DEC_SPECIAL_SYM);
        if (((i - prev_split) >= 15) || (is_special != prev_special)) {
            printf("\r\n");
            prev_split = i;
        }
        prev_special = is_special;
        printf("0x%02X\t", buf[i]);
    }
    if ((prev_split + 1) < length)
        printf("\r\n");
}
