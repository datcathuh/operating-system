; gdt_flush.asm
[bits 64]
global gdt_flush

gdt_flush:
	mov eax, [esp + 4]     ; argument: pointer to gdt_ptr struct
	;;  	lgdt [eax]             ; load GDTR with it

								; far jump to reload CS
	;;  	jmp 0x08:flush_label

flush_label:
	mov ax, 0x10           ; data selector
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	ret
