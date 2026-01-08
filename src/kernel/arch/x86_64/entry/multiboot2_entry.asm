section .bss
align 8
mb_magic:    resd 1
mb_addr:     resd 1

section .text
bits 32

global multiboot2_entry
global multiboot2_64bit_entry

extern multiboot2_paging_setup
extern multiboot2_gdt64_descriptor
extern nxe_enable
extern kmain

multiboot2_entry:
    cli

	;; multiboot2 magic number and address to tags
    mov [mb_magic], eax
    mov [mb_addr], ebx

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

	;; Make it possible to use NX (no execute) flag in page tables
	call nxe_enable

	lgdt [multiboot2_gdt64_descriptor]

	jmp 0x08:multiboot2_64bit_entry

bits 64
multiboot2_64bit_entry:
	mov edi, [mb_magic]   ; 1st arg
    mov rsi, [mb_addr]    ; 2nd arg
	call kmain

	jmp $
