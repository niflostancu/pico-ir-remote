/**
 * Implements the Starlight AC control model.
 */
#include <stdlib.h>
#include <stdio.h>

#include "nec_custom_proto.h"
#include "ir_utils.h"
#include "./starlight_ac.h"


struct base_ir_device_mod IR_DEV_MODULE_NAME(starlight_ac) = {
    .init = ir_mod_starlight_init,
    .tx_raw = ir_mod_starlight_tx_raw,
    .control_state = ir_mod_starlight_control_state,
};


int ir_mod_starlight_init(void *instdata)
{
    struct ir_mod_starlight_ac_inst_data *data = instdata;

    data->pio = pio0;
    data->pio_sm = ir_proto_nec_custom_init(data->pio, BOARD_IR_LED_PIN);
    return 0;
}

int ir_mod_starlight_tx_raw(void *instdata, const uint32_t *packet, uint32_t len)
{
    int i;
    struct ir_mod_starlight_ac_inst_data *data = instdata;
    const uint32_t pkt0[] = IR_AC_STARLIGHT_PKT0;

    if (len != 4) {
        return -1;
    }

    for (i=0; i<len; i++) {
        /* send first packet (fixed) */
        pio_sm_put_blocking(data->pio, data->pio_sm,
                ir_proto_swap_bytes(pkt0[i]));
    }
    sleep_ms(IRDEV_PROTO_PACKET_DISTANCE_MS);
    for (i=0; i<len; i++) {
        /* send first packet (fixed) */
        pio_sm_put_blocking(data->pio, data->pio_sm,
                ir_proto_swap_bytes(packet[i]));
    }
    sleep_ms(IRDEV_PROTO_PACKET_DISTANCE_MS);
    return 0;
}

int ir_mod_starlight_control_state(void *instdata, uint8_t state)
{
    int i;
    struct ir_mod_starlight_ac_inst_data *data = instdata;

    switch (state) {
        case 0: {
            const uint32_t pkt[] = IR_AC_STARLIGHT_PKT_OFF;
            ir_mod_starlight_tx_raw(instdata, pkt, 4);
            break;
        }
        case 1: {
            const uint32_t pkt[] = IR_AC_STARLIGHT_PKT_ON;
            ir_mod_starlight_tx_raw(instdata, pkt, 4);
            break;
        }
    }
    return 0;
}

