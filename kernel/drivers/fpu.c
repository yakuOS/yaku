#include "drivers/vga_text.h"
#include "types.h"
#include <stdint.h>
#include <printf.h>

void set_fpu_cw(const uint16_t cw) {
	asm volatile("fldcw %0;" :: "m"(cw));
}

void enable_fpu() {
    asm volatile("fninit");
}
