#include "fpu.h"

#include <types.h>

void fpu_set_cw(uint16_t cw) {
    asm volatile("fldcw %0;" ::"m"(cw));
}

void fpu_enable() {
    asm volatile("fninit");
}
