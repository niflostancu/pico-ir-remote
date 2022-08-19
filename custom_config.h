#ifndef MAIN_CUSTOM_CONFIG_H_
#define MAIN_CUSTOM_CONFIG_H_
/**
 * RPI configuration file.
 *
 * Personalize your GPIO mappings / desired features here.
 */

#define PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS 1000


/** IR LED GPIOs */
#define BOARD_IR_LED_OUT	1

/** The model header of the controlled device. */
#define IRDEV_MODEL_H "models/starlight.h"

#endif
