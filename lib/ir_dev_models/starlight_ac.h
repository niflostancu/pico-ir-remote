#ifndef _LIB_MODEL_STARLIGHT_AC_H
#define _LIB_MODEL_STARLIGHT_AC_H
/**
 * Definitions for Starlight AC model.
 */

#include <stdint.h>

#include "hardware/pio.h"

#include "./base.h"

/**
 * Encapsulates instance data for the Starlight AC device model.
 */
struct ir_mod_starlight_ac_inst_data {
    /** The PIO hardware instance configured. */
    PIO pio;
    /** The state machine . */
    int pio_sm;
    /** The current IR packet encapsulating the AC's current internal state. */
    uint32_t cur_packet[4];
};

#define IR_MOD_INST_DATA__starlight_ac  struct ir_mod_starlight_ac_inst_data

#define IR_AC_STARLIGHT_COUNT  2
#define IR_AC_STARLIGHT_BYTES  14
#define IR_AC_STARLIGHT_DISTANCE_MS  200

/** The first protocol/device identification packet for Starlight models */
#define IR_AC_STARLIGHT_PKT0 { \
    0x23, 0xCB, 0x26, 0x02, \
    0x00, 0x40, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, \
    0x00, 0x65, \
}

/** Template for all Starlight AC IR packets.  */
#define IR_AC_STARLIGHT_PKT_TEMPLATE { \
    0x23, 0xCB, 0x26, 0x00, \
    0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, \
}

/** Hardcoded on / off packets */
#define IR_AC_STARLIGHT_PKT_ON { \
    0x23, 0xCB, 0x26, 0x01, \
    0x00, 0x24, 0x03, 0x0B, \
    0x3A, 0x00, 0x00, 0x00, \
    0x80, 0x01, \
}
#define IR_AC_STARLIGHT_PKT_OFF { \
    0x23, 0xCB, 0x26, 0x01, \
    0x00, 0x20, 0x03, 0x0B, \
    0x02, 0x00, 0x00, 0x00, \
    0x80, 0xC5, \
}

// bit positions & masks for each of the AC's settings

#define IR_AC_STARLIGHT_POWER_IDX   6
#define IR_AC_STARLIGHT_POWER_BIT   1
#define IR_AC_STARLIGHT_POWER_MASK  (1 << 1)

#define IR_AC_STARLIGHT_CRC_IDX     13
#define IR_AC_STARLIGHT_CRC_BIT     0
#define IR_AC_STARLIGHT_CRC_MASK    (0xFF)


// exported members

int ir_mod_starlight_init(void *instdata);
int ir_mod_starlight_tx_raw(void *instdata, const uint8_t *packet, uint32_t len);
int ir_mod_starlight_control_state(void *instdata, uint8_t state);

extern struct base_ir_device_mod IR_DEV_MODULE_NAME(starlight_ac);

#endif
