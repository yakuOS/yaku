/*
this snippet was copied and adapted from:
https://wiki.osdev.org/index.php?title=Interrupts_Tutorial&action=history; Interrupts Tutorial; https://wiki.osdev.org/Interrupts_tutorial, 17.10.2021
*/

#pragma once

#include <types.h>

#define IDT_MAX_DESCRIPTORS 256
#define IDT_CPU_EXCEPTION_COUNT 32
#define IDT_IRQ_COUNT 15

#define IDT_ATTRIBUTE_INT_GATE 0x8E
#define IDT_ATTRIBUTE_TRAP_GATE 0x8F

#pragma pack(push, 1)

typedef struct {
    uint16_t base_low;
    uint16_t cs;
    uint8_t ist;
    uint8_t attributes;
    uint16_t base_mid;
    uint32_t base_high;
    uint32_t rsv0;
} idt_desc_t;

typedef struct {
    uint16_t limit;
    uint64_t base;
} idtr_t;

#pragma pack(pop)

void idt_reload(idtr_t* idtr);
void idt_set_descriptor(uint8_t vector, uintptr_t isr, uint8_t flags);
void idt_init(void);
