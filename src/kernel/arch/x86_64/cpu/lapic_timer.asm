bits 64

global lapic_timer

extern cpu_tick

lapic_timer:
	; CPU has already pushed: SS, RSP, RFLAGS, CS, RIP
    push    r15
    push    r14
    push    r13
    push    r12
    push    r11
    push    r10
    push    r9
    push    r8
    push    rsi
    push    rdi
    push    rbp

    push    rdx
    push    rcx
    push    rbx
    push    rax

    sub     rsp, 8        ; stack align

	call cpu_tick

    add     rsp, 8

    pop     rax
    pop     rbx
    pop     rcx
    pop     rdx

    pop     rbp
    pop     rdi
    pop     rsi
    pop     r8
    pop     r9
    pop     r10
    pop     r11
    pop     r12
    pop     r13
    pop     r14
    pop     r15

	iretq
