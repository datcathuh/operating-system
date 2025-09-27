[bits 32]
global _start

_start:
	extern s2main
	call s2main
	jmp $
