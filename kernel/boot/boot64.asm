extern kernel_main

section .text
global long_mode_start
long_mode_start:

    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call kernel_main

    hlt
