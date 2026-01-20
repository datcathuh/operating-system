bits 64

global context_switch
global context_fpu_save

context_fpu_save:
	fninit
    fxsave [rdi]
    ret

; void context_switch(struct cpu_context *old,
;                     struct cpu_context *new);
;
; rdi = old
; rsi = new

context_switch:
    ; ---------------------------
    ; Save current CPU state
    ; ---------------------------

    mov [rdi + 0x00], r15
    mov [rdi + 0x08], r14
    mov [rdi + 0x10], r13
    mov [rdi + 0x18], r12
    mov [rdi + 0x20], r11
    mov [rdi + 0x28], r10
    mov [rdi + 0x30], r9
    mov [rdi + 0x38], r8
    mov [rdi + 0x40], rsi    ; save caller's rsi
    mov [rdi + 0x48], rdi    ; save caller's rdi
    mov [rdi + 0x50], rbp
    mov [rdi + 0x58], rdx
    mov [rdi + 0x60], rcx
    mov [rdi + 0x68], rbx
    mov [rdi + 0x70], rax
    mov [rdi + 0x78], rsp

    ; Save RIP
    lea rax, [rel .resume]
    mov [rdi + 0x80], rax

	; Save RFLAGS
    pushfq
    pop qword [rdi + 0x88]

	; ---------------------------
    ; Save SSE/FPU state (fxsave)
    ; ---------------------------
    lea rax, [rdi + 0x90]        ; fxsave_area offset (aligned 16)
    fxsave [rax]

    ; ---------------------------
    ; Load new CPU state
    ; ---------------------------

    ; ---------------------------
    ; Restore SSE/FPU state for new task
    ; ---------------------------
    lea rax, [rsi + 0x90]
    fxrstor [rax]

    mov r15, [rsi + 0x00]
    mov r14, [rsi + 0x08]
    mov r13, [rsi + 0x10]
    mov r12, [rsi + 0x18]
    mov r11, [rsi + 0x20]
    mov r10, [rsi + 0x28]
    mov r9,  [rsi + 0x30]
    mov r8,  [rsi + 0x38]
    mov rbp, [rsi + 0x50]
    mov rdx, [rsi + 0x58]
    mov rcx, [rsi + 0x60]
    mov rbx, [rsi + 0x68]
    mov rax, [rsi + 0x70]
    mov rsp, [rsi + 0x78]

    ; Restore RFLAGS
    push qword [rsi + 0x88]
    popfq

	clts

    ; Jump to new RIP
    jmp qword [rsi + 0x80]

.resume:
    ret
