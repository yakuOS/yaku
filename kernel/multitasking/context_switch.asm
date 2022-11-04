; x86_64
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
%macro popagrd 0
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
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
%macro isr_wrapper_after 0
    pop rax
    pop rax
    popacrd
    popagrd
    pop rbp
    add rsp, 0x10
    iretq
%endmacro

; void switchTask(uint64_t *to_rsp);
global switch_to_task
switch_to_task:
    mov rsp, rdi ; load rsp from to_rsp into rsp
    popa ; pop all registers
    sti
    iretq

; paramterer in rdi, adress where to copy to
global rflags_copy
rflags_copy
    push rax
    pushfq
    pop qword rax
    mov [rdi], rax
    pop rax
    ret
