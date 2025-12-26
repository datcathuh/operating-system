bits 32

;; Valuable documentation exist here for the multiboot2 specification:
;; https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html

extern multiboot2_entry

section .multiboot2

multiboot2_header_start:
    dd 0xe85250d6               ; magic number (multiboot 2)
    dd 0                        ; architecture 0 (protected mode i386)
    dd multiboot2_header_end - multiboot2_header_start ; header length
    ; checksum
    dd 0x100000000 - (0xe85250d6 + 0 + (multiboot2_header_end - multiboot2_header_start))

    ; --- Entry address tag ---
    dw 3                  		; type (entry address)
    dw 0				  		; flags
    dd 12				  		; size
    dd multiboot2_entry

    ; --- End tag ---
    dw 0						; type (end)
    dw 0						; flags
    dd 8						; size

multiboot2_header_end:
