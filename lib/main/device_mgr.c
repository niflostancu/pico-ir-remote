/**
 * Implements the main CLI commands.
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "./device_mgr.h"
#include "custom_config.h"

#include IR_DEV_MODULE_H


static struct {
    struct base_ir_device_mod *mod;
    IR_DEV_INST_DATA_TYPE(IR_DEVICE_MODULE) instdata;
} main_device = {
    .mod = &IR_DEV_MODULE_NAME(IR_DEVICE_MODULE),
    .instdata = {0,}
};

/**
 * Initializes the IR device manager.
 */
int device_mgr_init()
{
    main_device.mod->init(&main_device.instdata);
    return 0;
}

/**
 * Sends a basic command to the device.
 */
int device_exec_basic(enum device_mgr_basic_cmd cmd)
{
    switch (cmd) {
        case DEVICE_CMD_ON:
        case DEVICE_CMD_OFF:
            main_device.mod->control_state(&main_device.instdata, cmd == DEVICE_CMD_ON);
            return 0;
        case DEVICE_CMD_STATUS:
            break;
    }
    return -0xFF;
}

