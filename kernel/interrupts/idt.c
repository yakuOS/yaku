#include "idt.h"
#include "../types.h"

static idt_desc_t idt[IDT_MAX_DESCRIPTORS];

static idtr_t idtr;

static bool vectors[IDT_MAX_DESCRIPTORS];

extern uint64_t isr_stub_table[];

void idt_set_descriptor(uint8_t vector, uintptr_t isr, uint8_t flags) {
    idt_desc_t* descriptor = &idt[vector];

    descriptor->base_low = isr & 0xFFFF;
    descriptor->cs = 0x8;
    descriptor->ist = 0;
    descriptor->attributes = flags;
    descriptor->base_mid = (isr >> 16) & 0xFFFF;
    descriptor->base_high = (isr >> 32) & 0xFFFFFFFF;
    descriptor->rsv0 = 0;
}

void idt_init() {
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_desc_t) * IDT_MAX_DESCRIPTORS - 1;

    for (uint8_t vector = 0; vector < IDT_CPU_EXCEPTION_COUNT; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], IDT_DESCRIPTOR_EXCEPTION);
        vectors[vector] = true;
    }

    idt_reload(&idtr);
}
