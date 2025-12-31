[bits 64]
global irq_page_fault_asm
extern irq_page_fault_c

irq_page_fault_asm:
    cli

    ;; Stack on entry:
    ;; [rsp + 0]  = error code
    ;; [rsp + 8]  = RIP
    ;; [rsp + 16] = CS
    ;; [rsp + 24] = RFLAGS
    ;; [rsp + 32] = RSP (old, maybe)
    ;; [rsp + 40] = SS  (maybe)

    mov rdi, rsp        ; frame pointer
    mov rsi, [rsp]      ; error code

    call irq_page_fault_c

.hang:
    hlt
    jmp .hang
