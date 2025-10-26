[bits 16]
global _start

extern print_string
extern print_new_line

_start:
    mov si, msg_stage2_start
    call print_string
	call print_new_line

	jmp $

	extern s2main
	call s2main
	jmp $

msg_stage2_start db 'Stage2 starting', 0
