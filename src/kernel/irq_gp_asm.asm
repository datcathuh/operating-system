global irq_gp_asm

irq_gp_asm:
	pushad

	push ds
	push es
	push fs
	push gs

	mov ax, 0x10            ; kernel data selector (adjust if your GDT uses a different value)
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	extern irq_gp_c
	call irq_gp_c

	pop gs
	pop fs
	pop es
	pop ds

	mov al, 0x20
	out 0x20, al     ; EOI

	popad
	iret
