[bits 16]
global start

extern disk_load
extern print_string
extern print_new_line

KERNEL_OFFSET equ 0x1000

%include "kernel_sector_count.asm"

start:
    mov si, msg_stage2_start
    call print_string
	call print_new_line

    mov bx, KERNEL_OFFSET        ; bx -> destination
    mov dh, KERNEL_SECTOR_COUNT  ; dh -> num sectors
    mov dl, [BOOT_DRIVE]         ; dl -> disk
    call disk_load

	;; Enable protected mode (32 bit)
    cli                      ; 1. disable interrupts
    lgdt [gdt_descriptor]    ; 2. load GDT descriptor
    mov eax, cr0
    or eax, 0x1              ; 3. enable protected mode
    mov cr0, eax
    jmp CODE_SEG:start_32bit ; 4. far jump

[bits 32]
start_32bit:
    mov ax, DATA_SEG         ; 5. update segment registers
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000         ; 6. setup stack
    mov esp, ebp

	extern s2main
	call s2main
	jmp $

msg_stage2_start db 'Stage2 starting', 0

;;; gdt_start and gdt_end labels are used to compute size

; null segment descriptor
gdt_start:
    dq 0x0

; code segment descriptor
gdt_code:
    dw 0xffff    ; segment length, bits 0-15
    dw 0x0       ; segment base, bits 0-15
    db 0x0       ; segment base, bits 16-23
    db 10011010b ; flags (8 bits)
    db 11001111b ; flags (4 bits) + segment length, bits 16-19
    db 0x0       ; segment base, bits 24-31

; data segment descriptor
gdt_data:
    dw 0xffff    ; segment length, bits 0-15
    dw 0x0       ; segment base, bits 0-15
    db 0x0       ; segment base, bits 16-23
    db 10010010b ; flags (8 bits)
    db 11001111b ; flags (4 bits) + segment length, bits 16-19
    db 0x0       ; segment base, bits 24-31

gdt_end:

; GDT descriptor
gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; size (16 bit)
    dd gdt_start ; address (32 bit)

CODE_SEG equ 0x08
DATA_SEG equ 0x10
