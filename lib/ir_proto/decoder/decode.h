#ifndef _LIB_IR_DECODE_H
#define _LIB_IR_DECODE_H 1
/**
 * IR raw protocol decoder API.
 */

#include <stdint.h>

#include "./modulation.h"

/* export all available decoder routines */
IR_DECODE_MODULATION_REGISTER(ir_decode_mod_pdm);

#endif
