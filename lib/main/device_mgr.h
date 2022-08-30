#ifndef MAIN_DEVICE_MGR_H_
#define MAIN_DEVICE_MGR_H_

#include <stdint.h>

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


#endif
