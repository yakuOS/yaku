extern isr_exception_handler
extern pic_send_eoi


%macro pushagrd 0
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
%endmacro

%macro popagrd 0
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

%macro pushacrd 0
    mov rax, cr0
    push rax
    mov rax, cr2
    push rax
    mov rax, cr3
    push rax
    mov rax, cr4
    push rax
%endmacro

%macro popacrd 0
    pop rax
    mov cr4, rax
    pop rax
    mov cr3, rax
    pop rax
    mov cr2, rax
    pop rax
    mov cr0, rax
%endmacro

%macro isr_wrapper_before 0
    push rbp
    mov rbp, rsp
    pushagrd
    pushacrd
    mov ax, ds
    push rax
    push qword 0
    mov ax, 0x10
    lea rdi, [rsp + 0x10]
%endmacro

%macro isr_wrapper_after 0
    pop rax
    pop rax
    popacrd
    popagrd
    pop rbp
    add rsp, 0x10
    iretq
%endmacro

%macro isr_err_stub 1
isr_stub_%+%1:
    push %1
    isr_wrapper_before
    call isr_exception_handler
    isr_wrapper_after
%endmacro

%macro isr_no_err_stub 1
isr_stub_%+%1:
    push 0
    push %1
    isr_wrapper_before
    call isr_exception_handler
    isr_wrapper_after
%endmacro
%macro pusha 0
    push rbp
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro 

%macro popa 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    pop rbp
%endmacro
%macro isr_irq_stub 2
isr_stub_%+%1:
    cli
    pusha
    call isr_irq%+%2
    mov rdi, %2
    call pic_send_eoi
    popa
    sti
    iretq
%endmacro
extern isr_syscall
isr_stub_128: ; syscall software interrupt
    push rbp
    ; push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    call isr_syscall
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    ; pop rax ;<- return value of isr_syscall should stay there
    pop rbp
    iretq


isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31


%assign vec 33
%assign irq 1
%rep 14
    extern isr_irq%+irq
    isr_irq_stub vec, irq
    %assign vec vec+1
    %assign irq irq+1
%endrep

; sets up isr stub for irq 0
%macro isr_irq_stub0 2
isr_stub_%+%1:
    cli
    pusha
    mov rdi, rsp
    call isr_irq0
    mov rdi, 0
    call pic_send_eoi
    popa
    sti
    iretq
%endmacro
%assign vec 32
%assign irq 0
extern isr_irq0
isr_irq_stub0 vec, irq


global isr_stub_table
isr_stub_table:
    %assign i 0
    %rep 47
        dq isr_stub_%+i
        %assign i i+1
    %endrep
dq isr_stub_128
