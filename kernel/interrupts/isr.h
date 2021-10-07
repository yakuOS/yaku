#pragma once

#include <types.h>

typedef struct {
    struct {
        uint64_t cr4;
        uint64_t cr3;
        uint64_t cr2;
        uint64_t cr0;
    } control_registers;

    struct {
        uint64_t rdi;
        uint64_t rsi;
        uint64_t rdx;
        uint64_t rcx;
        uint64_t rbx;
        uint64_t rax;
    } general_registers;

    struct {
        uint64_t rbp;
        uint64_t vector;
        uint64_t error_code;
        uint64_t rip;
        uint64_t cs;
        uint64_t rflags;
        uint64_t rsp;
        uint64_t dss;
    } base_frame;
} isr_context_t;

void isr_exception_handler(isr_context_t* ctx);

void isr_irq0(isr_context_t* ctx);
void isr_irq1(isr_context_t* ctx);
void isr_irq2(isr_context_t* ctx);
void isr_irq3(isr_context_t* ctx);
void isr_irq4(isr_context_t* ctx);
void isr_irq5(isr_context_t* ctx);
void isr_irq6(isr_context_t* ctx);
void isr_irq7(isr_context_t* ctx);
void isr_irq8(isr_context_t* ctx);
void isr_irq9(isr_context_t* ctx);
void isr_irq10(isr_context_t* ctx);
void isr_irq11(isr_context_t* ctx);
void isr_irq12(isr_context_t* ctx);
void isr_irq13(isr_context_t* ctx);
void isr_irq14(isr_context_t* ctx);
void isr_irq15(isr_context_t* ctx);
