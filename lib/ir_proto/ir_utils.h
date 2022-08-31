#ifndef _LIB_IR_PROTO_UTILS_H
#define _LIB_IR_PROTO_UTILS_H 1

#include <stdint.h>


/**
 * Builds a 32-bit word from individual data bytes
 * (controllable byte-ordering).
 */
uint32_t ir_proto_make_word(const uint8_t *data, uint8_t len, uint8_t little_endian);

/**
 * Swaps endianness: big to little or little to big.
 */
uint32_t ir_proto_swap_bytes(uint32_t num);

#endif
