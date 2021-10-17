; this snippet was copied from:
; rizet; skylight; https://github.com/rizet/skylight/blob/trunk/glass/src/cpu/interrupts/idt.s, 10.09.2021
global idt_reload
idt_reload:
    push rbp
    mov rbp, rsp
    pushfq
    cli
    lidt [rdi]
    popfq
    pop rbp
    ret
