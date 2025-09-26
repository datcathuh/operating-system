[bits 32]
global _start

_start:
	mov eax, 0xB8000
    mov byte [eax], 'X'      ; write character
    mov byte [eax+1], 0x0F   ; attribute (white on black)

	extern kmain
	call kmain
	;;// hlt
	jmp $
