#pragma once

#include <types.h>

#define IDT_MAX_DESCRIPTORS 256
#define IDT_CPU_EXCEPTION_COUNT 32
#define IDT_IRQ_COUNT 15

#define IDT_DESCRIPTOR_X16_INTERRUPT 0x06
#define IDT_DESCRIPTOR_X16_TRAP 0x07
#define IDT_DESCRIPTOR_X32_TASK 0x05
#define IDT_DESCRIPTOR_X32_INTERRUPT 0x0E
#define IDT_DESCRIPTOR_X32_TRAP 0x0F
#define IDT_DESCRIPTOR_RING1 0x40
#define IDT_DESCRIPTOR_RING2 0x20
#define IDT_DESCRIPTOR_RING3 0x60
#define IDT_DESCRIPTOR_PRESENT 0x80

#define IDT_DESCRIPTOR_HARDWARE (IDT_DESCRIPTOR_X32_INTERRUPT | IDT_DESCRIPTOR_PRESENT)
#define IDT_DESCRIPTOR_CALL                                                              \
    (IDT_DESCRIPTOR_X32_INTERRUPT | IDT_DESCRIPTOR_PRESENT | IDT_DESCRIPTOR_RING3)

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
void idt_cli(void);
void idt_sti(void);
