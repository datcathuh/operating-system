[bits 64]
global _start

_start:
	extern kmain
	call kmain
	jmp $
