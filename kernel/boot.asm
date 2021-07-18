bits 32

MB_MAGIC         equ 0x1BADB002
MB_PAGE_ALIGN    equ 1 << 0
MB_MEM_INFO      equ 1 << 1
MB_USE_GFX       equ 0  ; disable graphics for now
MB_FLAGS         equ MB_PAGE_ALIGN | MB_MEM_INFO | MB_USE_GFX
MB_CHECKSUM      equ -(MB_MAGIC + MB_FLAGS)

section .multiboot
align 4
    dd MB_MAGIC
    dd MB_FLAGS
    dd MB_CHECKSUM
    dd 0     ; header_addr
    dd 0     ; load_addr
    dd 0     ; load_end_addr
    dd 0     ; bss_end_addr
    dd 0     ; entry_addr
    dd 0     ; 0 = linear graphics
    dd 1024  ; width
    dd 768   ; height
    dd 32    ; color depth


extern kmain  ; kernel entrypoint in 'main.c'

section .text
global start
start:
    ; set stack pointer
    mov esp, stack_top

    ; push multiboot info struct
    push ebx

    ; disable interrupts
    cli

    ; call kernel entrypoint
    call kmain

    hlt

section .bss
align 32
stack_bottom:
    resb 8192  ; = 8KB
stack_top:
