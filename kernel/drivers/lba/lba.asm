; @param EAX Logical Block Address of sector
; @param CL  Number of sectors to read
; @param RDI The address of buffer to put data obtained from disk
global lba_read_primary_controller_first_drive
lba_read_primary_controller_first_drive:
    pushfq
    push rax
    push rcx
    mov rax, rdi
    mov rcx, rsi
    mov rdi, rdx

    and rax, 0x0FFFFFFF
    push rax
    push rbx
    push rcx
    push rdx
    push rdi
    mov rbx, rax         ; Save LBA in RBX
    mov edx, 0x01F6      ; Port to send drive and bit 24 - 27 of LBA
    shr eax, 24          ; Get bit 24 - 27 in al
    or al, 11100000b     ; Set bit 6 in al for LBA mode
    out dx, al
    mov edx, 0x01F2      ; Port to send number of sectors
    mov al, cl           ; Get number of sectors from CL
    out dx, al
    mov edx, 0x1F3       ; Port to send bit 0 - 7 of LBA
    mov eax, ebx         ; Get LBA from EBX
    out dx, al
    mov edx, 0x1F4       ; Port to send bit 8 - 15 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 8           ; Get bit 8 - 15 in AL
    out dx, al
    mov edx, 0x1F5       ; Port to send bit 16 - 23 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 16          ; Get bit 16 - 23 in AL
    out dx, al
    mov edx, 0x1F7       ; Command port
    mov al, 0x20         ; Read with retry.
    out dx, al
    
.still_going:  in al, dx
    test al, 8           ; the sector buffer requires servicing.
    jz .still_going      ; until the sector buffer is ready.
 
    mov rax, 256         ; to read 256 words = 1 sector
    xor bx, bx
    mov bl, cl           ; read CL sectors
    mul bx
    mov rcx, rax         ; RCX is counter for INSW
    mov rdx, 0x1F0       ; Data port, in and out
    rep insw             ; in to [RDI]
 
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    pop rcx
    pop rax
    popfq
    ret





; @param EAX Logical Block Address of sector
; @param CL  Number of sectors to write
; @param RDI The address of data to write to the disk
global lba_write_primary_controller_first_drive
lba_write_primary_controller_first_drive:
    pushfq
    mov rax, rdi
    mov rcx, rsi
    mov rdi, rdx
    and rax, 0x0FFFFFFF
    push rax
    push rbx
    push rcx
    push rdx
    push rdi
 
    mov rbx, rax         ; Save LBA in RBX
 
    mov edx, 0x01F6      ; Port to send drive and bit 24 - 27 of LBA
    shr eax, 24          ; Get bit 24 - 27 in al
    or al, 11100000b     ; Set bit 6 in al for LBA mode
    out dx, al
 
    mov edx, 0x01F2      ; Port to send number of sectors
    mov al, cl           ; Get number of sectors from CL
    out dx, al
 
    mov edx, 0x1F3       ; Port to send bit 0 - 7 of LBA
    mov eax, ebx         ; Get LBA from EBX
    out dx, al
 
    mov edx, 0x1F4       ; Port to send bit 8 - 15 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 8           ; Get bit 8 - 15 in AL
    out dx, al
 
 
    mov edx, 0x1F5       ; Port to send bit 16 - 23 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 16          ; Get bit 16 - 23 in AL
    out dx, al
 
    mov edx, 0x1F7       ; Command port
    mov al, 0x30         ; Write with retry.
    out dx, al
 
.still_going:  in al, dx
    test al, 8           ; the sector buffer requires servicing.
    jz .still_going      ; until the sector buffer is ready.
 
    mov rax, 256         ; to read 256 words = 1 sector
    xor bx, bx
    mov bl, cl           ; write CL sectors
    mul bx
    mov rcx, rax         ; RCX is counter for OUTSW
    mov rdx, 0x1F0       ; Data port, in and out
    mov rsi, rdi
    rep outsw            ; out
    add rdx, 7

    
    writesectors_wait_again:
	    in al, dx
	    bt ax, 7		; Check the BSY flag (Bit 7)
	    jc writesectors_wait_again
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    popfq
    ret
; @param EAX Logical Block Address of sector
; @param CL  Number of sectors to read
; @param RDI The address of buffer to put data obtained from disk
global lba_read_primary_controller_second_drive
lba_read_primary_controller_second_drive:
    pushfq
    mov rax, rdi
    mov rcx, rsi
    mov rdi, rdx

    and rax, 0x0FFFFFFF
    push rax
    push rbx
    push rcx
    push rdx
    push rdi
    mov rbx, rax         ; Save LBA in RBX
    mov edx, 0x01F6      ; Port to send drive and bit 24 - 27 of LBA
    shr eax, 24          ; Get bit 24 - 27 in al
    or al, 11110000b     ; Set bit 6 in al for LBA mode
    out dx, al
    mov edx, 0x01F2      ; Port to send number of sectors
    mov al, cl           ; Get number of sectors from CL
    out dx, al
    mov edx, 0x1F3       ; Port to send bit 0 - 7 of LBA
    mov eax, ebx         ; Get LBA from EBX
    out dx, al
    mov edx, 0x1F4       ; Port to send bit 8 - 15 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 8           ; Get bit 8 - 15 in AL
    out dx, al
    mov edx, 0x1F5       ; Port to send bit 16 - 23 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 16          ; Get bit 16 - 23 in AL
    out dx, al
    mov edx, 0x1F7       ; Command port
    mov al, 0x20         ; Read with retry.
    out dx, al
    
.still_going:  in al, dx
    test al, 8           ; the sector buffer requires servicing.
    jz .still_going      ; until the sector buffer is ready.
 
    mov rax, 256         ; to read 256 words = 1 sector
    xor bx, bx
    mov bl, cl           ; read CL sectors
    mul bx
    mov rcx, rax         ; RCX is counter for INSW
    mov rdx, 0x1F0       ; Data port, in and out
    rep insw             ; in to [RDI]
 
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    popfq
    ret





; @param EAX Logical Block Address of sector
; @param CL  Number of sectors to write
; @param RDI The address of data to write to the disk
global lba_write_primary_controller_second_drive
lba_write_primary_controller_second_drive:
    pushfq
    mov rax, rdi
    mov rcx, rsi
    mov rdi, rdx
    and rax, 0x0FFFFFFF
    push rax
    push rbx
    push rcx
    push rdx
    push rdi
 
    mov rbx, rax         ; Save LBA in RBX
 
    mov edx, 0x01F6      ; Port to send drive and bit 24 - 27 of LBA
    shr eax, 24          ; Get bit 24 - 27 in al
    or al, 11110000b     ; Set bit 6 in al for LBA mode
    out dx, al
 
    mov edx, 0x01F2      ; Port to send number of sectors
    mov al, cl           ; Get number of sectors from CL
    out dx, al
 
    mov edx, 0x1F3       ; Port to send bit 0 - 7 of LBA
    mov eax, ebx         ; Get LBA from EBX
    out dx, al
 
    mov edx, 0x1F4       ; Port to send bit 8 - 15 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 8           ; Get bit 8 - 15 in AL
    out dx, al
 
 
    mov edx, 0x1F5       ; Port to send bit 16 - 23 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 16          ; Get bit 16 - 23 in AL
    out dx, al
 
    mov edx, 0x1F7       ; Command port
    mov al, 0x30         ; Write with retry.
    out dx, al
 
.still_going:  in al, dx
    test al, 8           ; the sector buffer requires servicing.
    jz .still_going      ; until the sector buffer is ready.
 
    mov rax, 256         ; to read 256 words = 1 sector
    xor bx, bx
    mov bl, cl           ; write CL sectors
    mul bx
    mov rcx, rax         ; RCX is counter for OUTSW
    mov rdx, 0x1F0       ; Data port, in and out
    mov rsi, rdi
    rep outsw            ; out
 
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    popfq
    ret
; @param EAX Logical Block Address of sector
; @param CL  Number of sectors to read
; @param RDI The address of buffer to put data obtained from disk
global lba_read_secondary_controller_first_drive
lba_read_secondary_controller_first_drive:
    pushfq
    mov rax, rdi
    mov rcx, rsi
    mov rdi, rdx

    and rax, 0x0FFFFFFF
    push rax
    push rbx
    push rcx
    push rdx
    push rdi
    mov rbx, rax         ; Save LBA in RBX
    mov edx, 0x0176      ; Port to send drive and bit 24 - 27 of LBA
    shr eax, 24          ; Get bit 24 - 27 in al
    or al, 11100000b     ; Set bit 6 in al for LBA mode
    out dx, al
    mov edx, 0x0172      ; Port to send number of sectors
    mov al, cl           ; Get number of sectors from CL
    out dx, al
    mov edx, 0x173       ; Port to send bit 0 - 7 of LBA
    mov eax, ebx         ; Get LBA from EBX
    out dx, al
    mov edx, 0x174       ; Port to send bit 8 - 15 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 8           ; Get bit 8 - 15 in AL
    out dx, al
    mov edx, 0x175       ; Port to send bit 16 - 23 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 16          ; Get bit 16 - 23 in AL
    out dx, al
    mov edx, 0x177       ; Command port
    mov al, 0x20         ; Read with retry.
    out dx, al
    
.still_going:  in al, dx
    test al, 8           ; the sector buffer requires servicing.
    jz .still_going      ; until the sector buffer is ready.
 
    mov rax, 256         ; to read 256 words = 1 sector
    xor bx, bx
    mov bl, cl           ; read CL sectors
    mul bx
    mov rcx, rax         ; RCX is counter for INSW
    mov rdx, 0x170       ; Data port, in and out
    rep insw             ; in to [RDI]
 
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    popfq
    ret





; @param EAX Logical Block Address of sector
; @param CL  Number of sectors to write
; @param RDI The address of data to write to the disk
global lba_write_secondary_controller_first_drive
lba_write_secondary_controller_first_drive:
    pushfq
    mov rax, rdi
    mov rcx, rsi
    mov rdi, rdx
    and rax, 0x0FFFFFFF
    push rax
    push rbx
    push rcx
    push rdx
    push rdi
 
    mov rbx, rax         ; Save LBA in RBX
 
    mov edx, 0x0176      ; Port to send drive and bit 24 - 27 of LBA
    shr eax, 24          ; Get bit 24 - 27 in al
    or al, 11100000b     ; Set bit 6 in al for LBA mode
    out dx, al
 
    mov edx, 0x0172      ; Port to send number of sectors
    mov al, cl           ; Get number of sectors from CL
    out dx, al
 
    mov edx, 0x173       ; Port to send bit 0 - 7 of LBA
    mov eax, ebx         ; Get LBA from EBX
    out dx, al
 
    mov edx, 0x174       ; Port to send bit 8 - 15 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 8           ; Get bit 8 - 15 in AL
    out dx, al
 
 
    mov edx, 0x175       ; Port to send bit 16 - 23 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 16          ; Get bit 16 - 23 in AL
    out dx, al
 
    mov edx, 0x177       ; Command port
    mov al, 0x30         ; Write with retry.
    out dx, al
 
.still_going:  in al, dx
    test al, 8           ; the sector buffer requires servicing.
    jz .still_going      ; until the sector buffer is ready.
 
    mov rax, 256         ; to read 256 words = 1 sector
    xor bx, bx
    mov bl, cl           ; write CL sectors
    mul bx
    mov rcx, rax         ; RCX is counter for OUTSW
    mov rdx, 0x170       ; Data port, in and out
    mov rsi, rdi
    rep outsw            ; out
 
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    popfq
    ret
; @param EAX Logical Block Address of sector
; @param CL  Number of sectors to read
; @param RDI The address of buffer to put data obtained from disk
global lba_read_secondary_controller_second_drive
lba_read_secondary_controller_second_drive:
    pushfq
    mov rax, rdi
    mov rcx, rsi
    mov rdi, rdx

    and rax, 0x0FFFFFFF
    push rax
    push rbx
    push rcx
    push rdx
    push rdi
    mov rbx, rax         ; Save LBA in RBX
    mov edx, 0x0176      ; Port to send drive and bit 24 - 27 of LBA
    shr eax, 24          ; Get bit 24 - 27 in al
    or al, 11110000b     ; Set bit 6 in al for LBA mode
    out dx, al
    mov edx, 0x0172      ; Port to send number of sectors
    mov al, cl           ; Get number of sectors from CL
    out dx, al
    mov edx, 0x173       ; Port to send bit 0 - 7 of LBA
    mov eax, ebx         ; Get LBA from EBX
    out dx, al
    mov edx, 0x174       ; Port to send bit 8 - 15 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 8           ; Get bit 8 - 15 in AL
    out dx, al
    mov edx, 0x175       ; Port to send bit 16 - 23 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 16          ; Get bit 16 - 23 in AL
    out dx, al
    mov edx, 0x177       ; Command port
    mov al, 0x20         ; Read with retry.
    out dx, al
    
.still_going:  in al, dx
    test al, 8           ; the sector buffer requires servicing.
    jz .still_going      ; until the sector buffer is ready.
 
    mov rax, 256         ; to read 256 words = 1 sector
    xor bx, bx
    mov bl, cl           ; read CL sectors
    mul bx
    mov rcx, rax         ; RCX is counter for INSW
    mov rdx, 0x170       ; Data port, in and out
    rep insw             ; in to [RDI]
 
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    popfq
    ret





; @param EAX Logical Block Address of sector
; @param CL  Number of sectors to write
; @param RDI The address of data to write to the disk
global lba_write_secondary_controller_second_drive
lba_write_secondary_controller_second_drive:
    pushfq
    mov rax, rdi
    mov rcx, rsi
    mov rdi, rdx
    and rax, 0x0FFFFFFF
    push rax
    push rbx
    push rcx
    push rdx
    push rdi
 
    mov rbx, rax         ; Save LBA in RBX
 
    mov edx, 0x0176      ; Port to send drive and bit 24 - 27 of LBA
    shr eax, 24          ; Get bit 24 - 27 in al
    or al, 11110000b     ; Set bit 6 in al for LBA mode
    out dx, al
 
    mov edx, 0x0172      ; Port to send number of sectors
    mov al, cl           ; Get number of sectors from CL
    out dx, al
 
    mov edx, 0x173       ; Port to send bit 0 - 7 of LBA
    mov eax, ebx         ; Get LBA from EBX
    out dx, al
 
    mov edx, 0x174       ; Port to send bit 8 - 15 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 8           ; Get bit 8 - 15 in AL
    out dx, al
 
 
    mov edx, 0x175       ; Port to send bit 16 - 23 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 16          ; Get bit 16 - 23 in AL
    out dx, al
 
    mov edx, 0x177       ; Command port
    mov al, 0x30         ; Write with retry.
    out dx, al
 
.still_going:  in al, dx
    test al, 8           ; the sector buffer requires servicing.
    jz .still_going      ; until the sector buffer is ready.
 
    mov rax, 256         ; to read 256 words = 1 sector
    xor bx, bx
    mov bl, cl           ; write CL sectors
    mul bx
    mov rcx, rax         ; RCX is counter for OUTSW
    mov rdx, 0x170       ; Data port, in and out
    mov rsi, rdi
    rep outsw            ; out
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    popfq
    ret
