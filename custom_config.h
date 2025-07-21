#ifndef MAIN_CUSTOM_CONFIG_H_
#define MAIN_CUSTOM_CONFIG_H_
/**
 * RPI configuration file.
 *
 * Personalize your GPIO mappings / desired features here.
 */

#define PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS 1000

/** GPIOs */
#define BOARD_IR_LED_PIN 			2
#define BOARD_SCHEDULER_NEXT_BTN 	4

/** Raw IR receiver / decoder function */
#define IR_RECEIVER_PIN   1
#define IR_RECV_FILTER_CARRIER_US  29

/** The controlled IR device model name. */
#define IR_DEVICE_MODULE starlight_ac
#define IR_DEV_MODULE_H "starlight_ac.h"

/* Scheduler default setting */
#define SCHEDULER_DEFAULT_ON_TIME_MS   (15 * 60 * 1000)
#define SCHEDULER_DEFAULT_OFF_TIME_MS  (60 * 60 * 1000)

#endif
