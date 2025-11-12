;===========================================================
; Paging structures for entering long mode (identity map)
;===========================================================

[BITS 32]
SECTION .bss
align 4096
pml4_table:   resb 4096

align 4096
pdpt_table:   resb 4096

align 4096
pd_table:     resb 4096

align 4096
pt_table:     resb 4096

;===========================================================
; Setup function (runs in protected mode)
;===========================================================
SECTION .text
global setup_paging

setup_paging:
    ;-------------------------------------------------------
    ; Zero out all tables (optional but good practice)
    ;-------------------------------------------------------
    xor eax, eax
    mov edi, pml4_table
    mov ecx, (4096*4)/4        ; 4 tables * 4K / 4 bytes = # of dwords
    rep stosd

    ;-------------------------------------------------------
    ; PML4 -> PDPT
    ;-------------------------------------------------------
    mov eax, pdpt_table
    or  eax, 0x003             ; Present + RW
    mov [pml4_table], eax

    ;-------------------------------------------------------
    ; PDPT -> PD
    ;-------------------------------------------------------
    mov eax, pd_table
    or  eax, 0x003
    mov [pdpt_table], eax

    ;-------------------------------------------------------
    ; PD -> PT
    ;-------------------------------------------------------
    mov eax, pt_table
    or  eax, 0x003
    mov [pd_table], eax

    ;-------------------------------------------------------
    ; PT -> physical pages (identity map)
    ;-------------------------------------------------------
    mov ecx, 512               ; map 512 * 4K = 2 MiB
    xor edi, edi
.set_pt_loop:
    mov eax, edi               ; physical address
    or  eax, 0x003             ; Present + RW
    mov [pt_table + edi*8/8], eax  ; each entry is 8 bytes
    add edi, 0x1000            ; next 4 KiB page
    loop .set_pt_loop

    ret
