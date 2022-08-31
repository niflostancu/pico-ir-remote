#ifndef _LIB_MODEL_BASE_H
#define _LIB_MODEL_BASE_H

#include <stdint.h>


/**
 * Base class (struct of function pointers) for IR controllable devices.
 */
struct base_ir_device_mod {

    /** Internal type / ID of the device (incl. flags). */
    uint32_t type;

    /**
     * Initializes the device (incl. MCU peripherals used, e.g., PIO state
     * machines).
     */
    int (*init)(void *instdata);

    /**
     * Transmits a raw packet over the air (using device-specific IR
     * modulation).
     */
    int (*tx_raw)(void *instdata, const uint8_t *packet, uint32_t len);

    /**
     * Controls the on/off state of the device.
     */
    int (*control_state)(void *instdata, uint8_t new_state);
};

#ifndef CAT
#define PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__
#define CAT(a, ...) PRIMITIVE_CAT(a, __VA_ARGS__)
#endif

#define IR_DEV_MODULE_NAME(__name)  CAT(ir_dev_module__, __name)
#define IR_DEV_INST_DATA_TYPE(__name) CAT(IR_MOD_INST_DATA__, __name)

#endif
