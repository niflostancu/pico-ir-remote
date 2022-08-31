#ifndef MAIN_DEVICE_MGR_H_
#define MAIN_DEVICE_MGR_H_

#include <stdint.h>

#include "./state.h"

/** Basic device commands. */
enum device_mgr_basic_cmd {
    DEVICE_CMD_OFF = 0,
    DEVICE_CMD_ON = 1,
    DEVICE_CMD_STATUS = 2,
};

/**
 * Initializes the IR device manager.
 */
int device_mgr_init();

/**
 * Sends a basic command to the device.
 */
int device_exec_basic(enum device_mgr_basic_cmd cmd);

/**
 * Applies a standard device control state.
 */
int device_apply_state(struct ir_device_state apply_state);

#endif
