bits 64
default rel

section .data

global gdt64_descriptor

gdt64:
	dq 0                          ; null descriptor
	dq 0x00AF9A000000FFFF         ; 64-bit code segment
	dq 0x00AF92000000FFFF         ; 64-bit data segment

gdt64_descriptor:
	dw gdt64_end - gdt64 - 1
	dd gdt64
	dd 0

gdt64_end:
