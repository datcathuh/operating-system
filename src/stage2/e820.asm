[bits 16]

%include "defines.asm"

E820_MAGIC       equ 0x534D4150      ; 'SMAP'
E820_TYPE_RAM    equ 1
MAX_E820_ENTRIES equ 64
E820_BUFFER_PHYS equ 0x00008000
E820_ENTRY_SIZE  equ 24

global e820_map
global e820_map_get

e820_map_get:
	push ds
	push es
	push si
	push di

	xor ebx, ebx        ; continuation
	xor bp, bp          ; entry count

.e820_loop:
	; compute buffer address for this entry
	mov ax, bp
	mov cx, E820_ENTRY_SIZE
	mul cx              ; AX = bp * entry_size
	add ax, E820_BUFFER_PHYS

	mov di, ax
	shr ax, 4
	mov es, ax
	and di, 0x0F

	mov eax, 0xE820
	mov edx, E820_MAGIC
	mov ecx, E820_ENTRY_SIZE
	int 0x15

	jc .done
	cmp eax, E820_MAGIC
	jne .done

	; skip zero-length entries
	mov eax, [es:di + 8]
	or  eax, [es:di + 12]
	jz .next

	inc bp
	cmp bp, MAX_E820_ENTRIES
	jae .done

.next:
	test ebx, ebx
	jnz .e820_loop

.done:
	pop di
	pop si
	pop es
	pop ds

	mov [e820_count], bp
	mov word [e820_entry_size], E820_ENTRY_SIZE
	mov dword [e820_entries_phys], E820_BUFFER_PHYS
	mov dword [e820_entries_phys + 4], 0
	ret

section .bss

e820_map:
e820_count:	        resw 1
e820_entry_size:    resw 1
e820_entries_phys:  resq 1
