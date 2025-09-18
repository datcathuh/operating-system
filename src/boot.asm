section .text
    align 4
    dd 0x1BADB002              ; Magic number
    dd 0x00                    ; Flags
    dd - (0x1BADB002 + 0x00)   ; Checksum

global start
extern kmain

start:
    cli
    mov esp, stack_space
    call kmain
    hlt

section .bss
resb 8192
stack_space: