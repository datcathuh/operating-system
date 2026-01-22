bits 16

global disk_load

; INPUT:
;   DL = drive (0x80)
;   DS:SI = pointer to DAP structure
; OUTPUT:
;   CF clear on success
disk_load:
    pusha
    mov ah, 0x42        ; Extended Read
    int 0x13
    jc disk_error
    popa
    ret

disk_error:
    cli
    hlt
    jmp $
