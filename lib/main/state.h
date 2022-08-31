#ifndef MAIN_DEVICE_STATE_H_
#define MAIN_DEVICE_STATE_H_

#include <stdint.h>


/** Encapsulates the control state of an IR device. */
struct ir_device_state {
    /** The on / off state of the device. */
    uint8_t status;
    union {
        /** Air conditioning states. */
        struct {
            uint8_t mode;
            uint16_t temp;
            /* TODO: add the rest of the fields */
        } ac_state;
    };
};

#endif
