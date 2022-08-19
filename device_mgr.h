#ifndef MAIN_DEVICE_MGR_H_
#define MAIN_DEVICE_MGR_H_

#include <stdint.h>
#include "hardware/pio.h"

/**
 * Initializes the IR device manager.
 */
int device_mgr_init();

/** Exports the PIO & SM ID used to queue commands. */
extern PIO main_device_pio;
extern int main_device_pio_sm;

#endif
