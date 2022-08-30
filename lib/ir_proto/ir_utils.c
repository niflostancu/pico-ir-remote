#include <stdint.h>
#include <stdlib.h>

#include "ir_utils.h"


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

