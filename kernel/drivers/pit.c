#include "pit.h"

#include <io.h>
#include <types.h>

static uint32_t tick = 0;

void pit_init(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;

    io_outb(0x43, 0x36);

    uint8_t lower = (uint8_t)(divisor & 0xFF);
    uint8_t upper = (uint8_t)((divisor >> 8) & 0xFF);

    io_outb(0x40, lower);
    io_outb(0x40, upper);
}

void pit_tick_increment(void) {
    tick++;
}

uint32_t pit_tick_get(void) {
    return tick;
}

uint32_t pit_read(void) {
    uint32_t count = 0;

    asm("cli");

    io_outb(0x43, 0b0000000);

    count = io_inb(0x40);
    count |= io_inb(0x40) << 8;

    asm("sti");

    return count;
}
