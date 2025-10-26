[bits 16]
global cursor_pos_set
global clear_screen
global print_string
global print_new_line

;; DH = row
;; DL = column
cursor_pos_set:
	pusha
	mov ah, 2       ;BH = page, DH = row, DL = column
    int 10h
	popa
	ret

; Clear screen function
clear_screen:
    pusha
    mov ah, 0x06    ; Scroll up function
    mov al, 0x00    ; Clear entire window
    mov bh, 0x07    ; White on black
    mov cx, 0x0000  ; Upper left corner (0,0)
    mov dx, 0x184F  ; Lower right corner (24,79)
    int 0x10
    popa
    ret

; Print string function
; Input: SI = pointer to null-terminated string
print_string:
    pusha
    mov ah, 0x0E    ; BIOS teletype function
.print_loop:
    lodsb           ; Load byte from SI into AL
    cmp al, 0       ; Check for null terminator
    je .done
    int 0x10        ; Print character
    jmp .print_loop
.done:
    popa
    ret

print_new_line:
    pusha
    mov ah, 0x0E
    mov al, 0x0D    ; Carriage return
    int 0x10
    mov al, 0x0A    ; Line feed
    int 0x10
    popa
    ret
