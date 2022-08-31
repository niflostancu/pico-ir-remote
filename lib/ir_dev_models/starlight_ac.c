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

int ir_mod_starlight_tx_raw(void *instdata, const uint8_t *packet, uint32_t len)
{
    int i;
    struct ir_mod_starlight_ac_inst_data *data = instdata;
    const uint8_t pkt0[] = IR_AC_STARLIGHT_PKT0;

    if (len != IR_AC_STARLIGHT_BYTES) {
        return -1;
    }

    ir_proto_nec_custom_send_pkt(data->pio, data->pio_sm, pkt0, len);
    sleep_ms(IR_AC_STARLIGHT_DISTANCE_MS);

    ir_proto_nec_custom_send_pkt(data->pio, data->pio_sm, packet, len);
    sleep_ms(IR_AC_STARLIGHT_DISTANCE_MS);
    return 0;
}

int ir_mod_starlight_control_state(void *instdata, uint8_t state)
{
    int i;
    struct ir_mod_starlight_ac_inst_data *data = instdata;

    switch (state) {
        case 0: {
            const uint8_t pkt[] = IR_AC_STARLIGHT_PKT_OFF;
            ir_mod_starlight_tx_raw(instdata, pkt, IR_AC_STARLIGHT_BYTES);
            break;
        }
        case 1: {
            const uint8_t pkt[] = IR_AC_STARLIGHT_PKT_ON;
            ir_mod_starlight_tx_raw(instdata, pkt, IR_AC_STARLIGHT_BYTES);
            break;
        }
    }
    return 0;
}

