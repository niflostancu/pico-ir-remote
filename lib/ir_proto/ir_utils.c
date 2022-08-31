#include <stdint.h>
#include <stdlib.h>

#include "ir_utils.h"



/**
 * Builds a 32-bit word from individual data bytes
 * (controllable byte-ordering).
 */
uint32_t ir_proto_make_word(const uint8_t *data, uint8_t len, uint8_t little_endian)
{
    uint32_t word = 0;
    int i;
    for (i=0; i<len; i++) {
        if (little_endian) {
            word = (word << 8) | data[i];
        } else {
            word |= (data[i] << (8 * i));
        }
    }

    return word;
}


/**
 * Swaps endianness: big to little or little to big.
 */
uint32_t ir_proto_swap_bytes(uint32_t num)
{
    uint32_t b0, b1, b2, b3;

    b0 = (num & 0x000000ff) << 24u;
    b1 = (num & 0x0000ff00) << 8u;
    b2 = (num & 0x00ff0000) >> 8u;
    b3 = (num & 0xff000000) >> 24u;

    return (b0 | b1 | b2 | b3);
}

