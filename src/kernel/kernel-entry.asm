[bits 32]
global _start

_start:
	extern kmain
	call kmain
	jmp $
