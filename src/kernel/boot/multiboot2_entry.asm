section .text
bits 32

global multiboot2_entry
global multiboot2_64bit_entry

extern multiboot2_paging_setup
extern multiboot2_gdt64_descriptor
extern kmain

multiboot2_entry:
    cli

	;; Enable PAE
	mov eax, cr4
	or eax, 1 << 5           ; Set PAE (bit 5)
	mov cr4, eax

	call multiboot2_paging_setup

	;; Enable long mode by setting the LME flag
	mov ecx, 0xC0000080     ; EFER
	rdmsr
	or eax, (1 << 8)        ; LME
	wrmsr

	;; Enable paging
	mov eax, cr0
	or eax, 0x80000000      ; PG
	mov cr0, eax

	lgdt [multiboot2_gdt64_descriptor]

	jmp 0x08:multiboot2_64bit_entry

bits 64
multiboot2_64bit_entry:
	call kmain
	jmp $
