/*
this snippet was copied and adapted from:
https://wiki.osdev.org/index.php?title=8259_PIC&action=history; 8259 PIC; https://wiki.osdev.org/PIC, 17.10.2021
*/
#include "pic.h"

#include <io.h>
#include <types.h>

void pic_mask_irq(uint8_t irq) {
    uint16_t port;
    uint8_t value;

    if (irq < 8) {
        port = PIC_MASTER_DATA;
    } else {
        port = PIC_SLAVE_DATA;
        irq -= 8;
    }
    value = io_inb(port) | (1 << irq);
    io_outb(port, value);
}

void pic_unmask_irq(uint8_t irq) {
    uint16_t port;
    uint8_t value;

    if (irq < 8) {
        port = PIC_MASTER_DATA;
    } else {
        port = PIC_SLAVE_DATA;
        irq -= 8;
    }
    value = io_inb(port) & ~(1 << irq);
    io_outb(port, value);
}

void pic_remap_offsets(uint8_t offset) {
    uint8_t master_mask, slave_mask;

    master_mask = io_inb(PIC_MASTER_DATA);
    slave_mask = io_inb(PIC_SLAVE_DATA);

    io_outb(PIC_MASTER_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
    io_wait();
    io_outb(PIC_SLAVE_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
    io_wait();
    io_outb(PIC_MASTER_DATA, offset);
    io_wait();
    io_outb(PIC_SLAVE_DATA, offset + 0x08);
    io_wait();
    io_outb(PIC_MASTER_DATA, 0x04);
    io_wait();
    io_outb(PIC_SLAVE_DATA, 0x02);
    io_wait();

    io_outb(PIC_MASTER_DATA, PIC_ICW4_8086);
    io_wait();
    io_outb(PIC_SLAVE_DATA, PIC_ICW4_8086);
    io_wait();

    io_outb(PIC_MASTER_DATA, master_mask);
    io_outb(PIC_SLAVE_DATA, slave_mask);
}

void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        io_outb(PIC_SLAVE_COMMAND, PIC_EOI);
    }
    io_outb(PIC_MASTER_COMMAND, PIC_EOI);
}

void pic_init(void) {
    pic_remap_offsets(0x20);
    for (uint8_t irq = 0; irq < 16; irq++) {
        pic_mask_irq(irq);
    }
    pic_unmask_irq(0);
    pic_unmask_irq(1);
    pic_unmask_irq(2); // cascade irq for com between PICs (needed for IRQs >= 8)
    // pic_unmask_irq(3);
    // pic_unmask_irq(4);
    // pic_unmask_irq(5);
    // pic_unmask_irq(6);
    // pic_unmask_irq(7);
    // pic_unmask_irq(8);
    // pic_unmask_irq(9);
    // pic_unmask_irq(10);
    // pic_unmask_irq(11);
    pic_unmask_irq(12);
    // pic_unmask_irq(13);
    // pic_unmask_irq(14);
    // pic_unmask_irq(15);
}

static uint16_t __pic_get_irq_reg(uint16_t ocw3) {
    /* OCW3 to PIC CMD to get the register values.  PIC2 is chained, and
     * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain */
    io_outb(PIC_MASTER_COMMAND, ocw3);
    io_outb(PIC_SLAVE_COMMAND, ocw3);
    return (io_inb(PIC_MASTER_COMMAND) << 8) | io_inb(PIC_SLAVE_COMMAND);
}

/* Returns the combined value of the cascaded PICs irq request register */
uint16_t pic_get_irr(void) {
    return __pic_get_irq_reg(PIC_READ_IRR);
}

/* Returns the combined value of the cascaded PICs in-service register */
uint16_t pic_get_isr(void) {
    return __pic_get_irq_reg(PIC_READ_ISR);
}
