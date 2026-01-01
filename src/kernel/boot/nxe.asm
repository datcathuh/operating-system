; -------------------------------------------------------
; Enable NXE (No-Execute Enable) in IA32_EFER
; Must be run after LME (Long Mode) is enabled
; This enables the NX (no execute) flag in page tables
; -------------------------------------------------------
%define IA32_EFER 0xC0000080
%define EFER_NXE (1 << 11)

section .text
global nxe_enable

nxe_enable:
    mov     ecx, IA32_EFER	;; Read IA32_EFER MSR into RDX:RAX
    rdmsr               	;; RDX:RAX = IA32_EFER
    or      eax, EFER_NXE
    wrmsr
    ret
