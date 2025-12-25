section .text
bits 32

global multiboot2_entry

multiboot2_entry:
    cli

    ; You probably already do:
    ; - GDT
    ; - Enable PAE
    ; - Enable long mode
    ; - Enable paging
    ; - Far jump to 64-bit code

	;; extern long_mode_start
	;; jmp long_mode_start
	jmp $
