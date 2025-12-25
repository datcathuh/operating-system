bits 32

extern multiboot2_entry

section .text
align 8

MB2_MAGIC        equ 0xE85250D6
MB2_ARCH_X86_64  equ 0
MB2_HEADER_LEN   equ multiboot2_header_end - multiboot2_header_start
MB2_CHECKSUM     equ -(MB2_MAGIC + MB2_ARCH_X86_64 + MB2_HEADER_LEN)

multiboot2_header_start:
    dd MB2_MAGIC
    dd MB2_ARCH_X86_64
    dd MB2_HEADER_LEN
    dd MB2_CHECKSUM

    ; --- Entry address tag ---
    dw 3                  ; type = entry address
    dw 0
    dd 12
    dd multiboot2_entry

    ; --- End tag ---
    dw 0
    dw 0
    dd 8

multiboot2_header_end:
