#include "pit.h"

#include <io.h>
#include <types.h>

static uint32_t tick = 0;

/*
this snippet was copied from:
Molloy, James; 5. IRQs and the PIT, 5.4. The PIT (practical); http://www.jamesmolloy.co.uk/tutorial_html/5.-IRQs%20and%20the%20PIT.html, 17.10.2021
*/
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

/*
this snippet was copied from:
https://wiki.osdev.org/index.php?title=Programmable_Interval_Timer&action=history; Programmable Interval Timer, Reading The Current Count; https://wiki.osdev.org/Programmable_Interval_Timer#Read_Back_Status_Byte, 17.10.2021
*/
uint32_t pit_read(void) {
    uint32_t count = 0;

    asm("cli");

    io_outb(0x43, 0b0000000);

    count = io_inb(0x40);
    count |= io_inb(0x40) << 8;

    asm("sti");

    return count;
}
