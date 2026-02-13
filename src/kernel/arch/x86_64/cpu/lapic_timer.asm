bits 64

global lapic_timer

extern cpu_tick

lapic_timer:
	; CPU has already pushed: SS, RSP, RFLAGS, CS, RIP
	push    rax     ; offset 0x70
    push    rbx     ; offset 0x68
    push    rcx     ; offset 0x60
    push    rdx     ; offset 0x58
    push    rbp     ; offset 0x50
    push    rdi     ; offset 0x48
    push    rsi     ; offset 0x40
    push    r8      ; offset 0x38
    push    r9      ; offset 0x30
    push    r10     ; offset 0x28
    push    r11     ; offset 0x20
    push    r12     ; offset 0x18
    push    r13     ; offset 0x10
    push    r14     ; offset 0x08
    push    r15     ; offset 0x00  ← rsp points here = start of struct ✓

	mov     rdi, rsp 			; rdi points to saved context
	call cpu_tick

	pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rsi
    pop     rdi
    pop     rbp
    pop     rdx
    pop     rcx
    pop     rbx
    pop     rax

	iretq
