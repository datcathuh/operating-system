bits 16
org 0x7c00

; where to load the kernel to
STAGE2_OFFSET equ 0x1000

start:
    ; BIOS sets boot drive in 'dl'; store for later use
    mov [BOOT_DRIVE], dl

    ; setup stack
    mov bp, 0x9000
    mov sp, bp

    call clear_screen

	mov dh, 0
	mov dl, 0
	call cursor_pos_set

    mov si, msg_booting
    call print_string
    call new_line

    mov si, msg_loading_stage2
    call print_string

    mov bx, STAGE2_OFFSET        ; bx -> destination
    mov dh, STAGE2_SECTOR_COUNT  ; dh -> num sectors
    mov dl, [BOOT_DRIVE]         ; dl -> disk
    call disk_load

    mov si, msg_stage2_loaded
    call print_string
    call new_line

    mov si, msg_call_stage2
    call print_string
    call new_line

	call STAGE2_OFFSET

    jmp $

%include "disk.asm"
%include "stage2_sector_count.asm"
%include "print.asm"

; boot drive variable
BOOT_DRIVE db 0
msg_booting db 'Bootloader starting', 0
msg_loading_stage2 db 'Loading stage2', 0
msg_stage2_loaded db ' ... stage2 loaded', 0
msg_call_stage2 db 'Calling stage2', 0

; padding
times 510 - ($-$$) db 0

; magic number
dw 0xaa55
