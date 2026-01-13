[bits 16]
global start

extern disk_load
extern gdt_descriptor
extern gdt64_descriptor
extern print_string
extern print_new_line
extern paging_setup
extern s2main

;; Temporary loading address of 0x20000 for the kernel
KERNEL_16_SEGMENT equ 0x1000
KERNEL_16_OFFSET  equ 0x0000
CODE_SEG equ 0x08
DATA_SEG equ 0x10

%include "kernel_sector_count.asm"

start:
	mov [STAGE2_SIZE], dh
	mov [BOOT_DRIVE], dl

	mov ax, 0x9000     ; pick a safe segment far from loads
	mov ss, ax
	mov sp, 0xFFFF

	;; mov si, msg_stage2_start
	;; call print_string
	;; call print_new_line

	;; Calculate the position of the kernel and store it in
	;; CL which is the start sector to read from.
	mov al, 1
	add al, [STAGE2_SIZE]
	mov [dap_lba_start], al

	;; Load kernel in a safe spot lower than 1MB. This means
	;; that we must copy it to final destination of 0x100000
	;; as soon as we reach 32 bit code.
	mov dl, [BOOT_DRIVE]         ; dl -> disk
	mov si, dap
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
	mov ecx, KERNEL_SECTOR_COUNT * 512
	rep movsd

	;; Make sure that paging is disable for now.
	mov eax, cr0
	and eax, 0x7FFFFFFF     ; Clear bit 31 (PG)
	mov cr0, eax

	;; Enable PAE
	mov eax, cr4
	or eax, 1 << 5           ; Set PAE (bit 5)
	mov cr4, eax

	call paging_setup
	call s2main

	lgdt [gdt64_descriptor]

	jmp 0x08:start_64bit

[bits 64]
start_64bit:
	mov ax, 0x10          ; data segment selector
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov rax, 0xB8000
	mov word [rax], 0x0758

	call 0x100000
	jmp $

section .data

msg_stage2_start db 'Stage2 starting', 0

	;; Disk Address Packet
dap:
	db 0x10                ; size of DAP (16 bytes)
	db 0                   ; reserved
	dw KERNEL_SECTOR_COUNT ; number of sectors to read
	dw KERNEL_16_OFFSET    ; offset (BX)
	dw KERNEL_16_SEGMENT   ; segment (ES)
dap_lba_start:
	dq 0                   ; starting LBA (sector number)


BOOT_DRIVE     db	0
STAGE2_SIZE    db	0
