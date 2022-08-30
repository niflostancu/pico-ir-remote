#ifndef _LIB_IR_PROTO_UTILS_H
#define _LIB_IR_PROTO_UTILS_H 1

#include <stdint.h>

/**
 * Swaps endianness: big to little or little to big.
 */
uint32_t ir_proto_swap_bytes(uint32_t num);

#endif
