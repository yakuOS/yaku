bits 32

global enable_sse
enable_sse:
    mov eax, cr0
    and ax, 0xFFFB
    or ax, 0x2
    mov cr0, eax
    mov eax, cr4
    or ax, 3 << 9
    mov cr4, eax
    ret

bits 64
global test_sse
test_sse:
    ; pxor	xmm0, xmm0
	movaps	[rbp-32],xmm0
	; movq	[rbp-16],xmm0
    ret
