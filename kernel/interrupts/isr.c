#include "isr.h"

#include <drivers/input/input_device.h>
#include <drivers/input/ps2.h>
#include <drivers/pit.h>
#include <drivers/serial.h>
#include <interrupts/pic.h>
#include <io.h>
#include <printf.h>

static const char* exception_messages[] = {
    "Divide-by-zero Error",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved error",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Security Exception",
    "Reserved error",
};

static const char* exception_message(uint64_t vector_number) {
    if (vector_number >= 21 && vector_number <= 29) {
        return "Reserved error";
    } else if (vector_number > 29) {
        return exception_messages[vector_number - 9];
    } else {
        return exception_messages[vector_number];
    }
}

void isr_exception_handler(isr_context_t* ctx) {
    const char* exception_msg = exception_message(ctx->base_frame.vector);
    serial_printf("EXCEPTION: %s (%llu, %llu)\n", exception_msg, ctx->base_frame.vector,
                  ctx->base_frame.error_code);

    asm("cli; hlt");
}

void isr_irq0(isr_context_t* ctx) {
    pit_tick_increment();
}

void isr_irq1(isr_context_t* ctx) {
    if (ps2_data_response_req == false) {
        uint8_t scan_code = io_inb(0x60);
        input_device_send_key(0, scan_code);
    } else {
        if (ps2_response_count == 1) {
            ps2_response_count = 0;
            ps2_data_response_req = false;
            uint8_t scan_code = io_inb(0x60);
        } else {
            ps2_response_count++;
            uint8_t scan_code = io_inb(0x60);
        }
    }
}

void isr_irq2(isr_context_t* ctx) {}
void isr_irq3(isr_context_t* ctx) {}
void isr_irq4(isr_context_t* ctx) {}
void isr_irq5(isr_context_t* ctx) {}
void isr_irq6(isr_context_t* ctx) {}
void isr_irq7(isr_context_t* ctx) {}
void isr_irq8(isr_context_t* ctx) {}
void isr_irq9(isr_context_t* ctx) {}
void isr_irq10(isr_context_t* ctx) {}
void isr_irq11(isr_context_t* ctx) {}
void isr_irq12(isr_context_t* ctx) {}
void isr_irq13(isr_context_t* ctx) {}
void isr_irq14(isr_context_t* ctx) {}
void isr_irq15(isr_context_t* ctx) {}
