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

#define IRDEV_PROTO_PACKET_COUNT  2
#define IRDEV_PROTO_PACKET_BYTES  14
#define IRDEV_PROTO_PACKET_DISTANCE_MS  200

/** The first protocol/device identification packet for Starlight models */
#define IR_AC_STARLIGHT_PKT0 { \
    0x23CB0000, 0x26020040, 0x00000000, 0x00000065 \
}

/** Template for all Starlight AC IR packets.  */
#define IR_AC_STARLIGHT_PKT_TEMPLATE { \
    0xCB230000, 0x26020000, 0x00000000, 0x00000000 \
}

/** On / Off Commands */
#define IR_AC_STARLIGHT_PKT_ON { \
    0x23CB0000, 0x26010024, 0x030B3A00, 0x00008001 \
}
#define IR_AC_STARLIGHT_PKT_OFF { \
    0x23CB0000, 0x26010020, 0x030B0200, 0x000080C5 \
}

// bit positions & masks for each of the AC's settings

#define IR_AC_STARLIGHT_POWER_IDX  2
#define IR_AC_STARLIGHT_POWER_BIT  7

#define IR_AC_STARLIGHT_CRC_IDX    3
#define IR_AC_STARLIGHT_CRC_BIT    24
#define IR_AC_STARLIGHT_CRC_MASK   (0xFF << IR_AC_STARLIGHT_CRC_BIT)


// exported members

int ir_mod_starlight_init(void *instdata);
int ir_mod_starlight_tx_raw(void *instdata, const uint32_t *packet, uint32_t len);
int ir_mod_starlight_control_state(void *instdata, uint8_t state);

extern struct base_ir_device_mod IR_DEV_MODULE_NAME(starlight_ac);

#endif
