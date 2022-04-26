/*
this snippet was copied from:
https://wiki.osdev.org/index.php?title=Inline_Assembly/Examples&action=history; Inline Assembly/Examples; https://wiki.osdev.org/Inline_Assembly/Examples, 17.10.2021
*/

#pragma once
#include "types.h"

static inline void io_outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t io_inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void io_wait(void) {
    io_outb(0x80, 0);
}

static inline bool are_interrupts_enabled() {
    unsigned long flags;
    asm volatile("pushf\n\t"
                 "pop %0"
                 : "=g"(flags));
    return flags & (1 << 9);
}
