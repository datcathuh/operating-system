[bits 64]
global _start

extern __bss_start
extern __bss_end
extern stack_top

_start:
	lea rsp, [stack_top]   ; set RSP to top of stack
	and rsp, -16           ; align stack to 16 bytes

	;; Clear BSS manually (no SSE, no memset)
	lea rdi, __bss_start
	lea rcx, __bss_end
    sub rcx, rdi
    shr rcx, 3
    xor rax, rax
    rep stosq

	call sse_enable

	extern kmain
	call kmain
	jmp $

sse_enable:
	;; Enable SSE / SSE2
	;; Safe to run in long mode
	;; Must be executed before any C code that may use XMM registers

    ; --- Enable FPU ---
    mov     rax, cr0
    and     rax, ~(1 << 2)     ; CR0.EM = 0 (disable emulation)
    or      rax,  (1 << 1)     ; CR0.MP = 1 (monitor coprocessor)
    mov     cr0, rax

    ; --- Enable SSE instructions ---
    mov     rax, cr4
    or      rax,  (1 << 9)     ; CR4.OSFXSR = 1 (FXSAVE/FXRSTOR)
    or      rax,  (1 << 10)    ; CR4.OSXMMEXCPT = 1 (SSE exceptions)
    mov     cr4, rax

    ; --- Initialize FPU / XMM state ---
    fninit                     ; or finit

    ret
