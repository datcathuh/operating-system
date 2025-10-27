[bits 16]
global start

extern disk_load
extern print_string
extern print_new_line

KERNEL_16_SEGMENT equ 0x1000
KERNEL_16_OFFSET  equ 0x0000

%include "kernel_sector_count.asm"

start:
	mov [STAGE2_SIZE], dh
    mov [BOOT_DRIVE], dl

    ;; mov si, msg_stage2_start
    ;; call print_string
	;; call print_new_line

	;; Calculate the position of the kernel and store it in
	;; CL which is the start sector to read from.
	mov al, 2
	add al, [byte STAGE2_SIZE]
	mov cl, al

	;; Load kernel in a safe spot lower than 1MB. This means
	;; that we must copy it to final destination of 0x100000
	;; as soon as we reach 32 bit code.
	mov ax, KERNEL_16_SEGMENT
	mov es, ax
    mov bx, KERNEL_16_OFFSET     ; bx -> destination
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

	mov esi, 0x00010000
    mov edi, 0x00100000
    mov ecx, KERNEL_SECTOR_COUNT * 512   ; assuming KERNEL_SIZE_BYTES is divisible by 4
    rep movsd

	extern s2main
	call s2main

	call 0x100000
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

BOOT_DRIVE     db	0
STAGE2_SIZE    db	0

CODE_SEG equ 0x08
DATA_SEG equ 0x10
