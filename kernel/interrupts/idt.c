#include "idt.h"

#include <types.h>

static idt_desc_t idt[IDT_MAX_DESCRIPTORS];

static idtr_t idtr;

extern uint64_t isr_stub_table[];

void idt_set_descriptor(uint8_t vector, uintptr_t isr, uint8_t flags) {
    idt_desc_t* descriptor = &idt[vector];

    descriptor->base_low = isr & 0xFFFF;
    descriptor->cs = 5 << 3;
    descriptor->ist = 0;
    descriptor->attributes = flags;
    descriptor->base_mid = (isr >> 16) & 0xFFFF;
    descriptor->base_high = (isr >> 32) & 0xFFFFFFFF;
    descriptor->rsv0 = 0;
}

void idt_init() {
    idtr.base = (uintptr_t)&idt;
    idtr.limit = (uint16_t)sizeof(idt_desc_t) * IDT_MAX_DESCRIPTORS - 1;

    for (uint8_t vector = 0; vector < IDT_CPU_EXCEPTION_COUNT; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], IDT_ATTRIBUTE_TRAP_GATE);
    }

    for (uint8_t vector = IDT_CPU_EXCEPTION_COUNT;
         vector < IDT_CPU_EXCEPTION_COUNT + IDT_IRQ_COUNT; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], IDT_ATTRIBUTE_INT_GATE);
    }
    idt_set_descriptor(128, isr_stub_table[47], IDT_ATTRIBUTE_INT_GATE);
    

    idt_reload(&idtr);
    asm volatile("sti");
}
