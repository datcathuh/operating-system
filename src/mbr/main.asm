bits 16
org 0x7c00

; where to load the kernel to
KERNEL_OFFSET equ 0x1000

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

    mov si, msg_loading_kernel
    call print_string
    call new_line

    call load_kernel

    mov si, msg_kernel_loaded
    call print_string
    call new_line

    call switch_to_32bit

    jmp $

%include "disk.asm"
%include "gdt.asm"
%include "print.asm"
%include "switch-to-32bit.asm"

bits 16
load_kernel:
    mov bx, KERNEL_OFFSET ; bx -> destination
    mov dh, 25            ; dh -> num sectors
    mov dl, [BOOT_DRIVE]  ; dl -> disk
    call disk_load
    ret

bits 32
BEGIN_32BIT:
    call KERNEL_OFFSET ; give control to the kernel
    jmp $ ; loop in case kernel returns

; boot drive variable
BOOT_DRIVE db 0
msg_booting db 'Bootloader starting', 0
msg_loading_kernel db 'Loading kernel', 0
msg_kernel_loaded db 'Kernel loaded. Entering 32bit mode', 0
msg_call_kernel db 'Calling kernel', 0

; padding
times 510 - ($-$$) db 0

; magic number
dw 0xaa55
