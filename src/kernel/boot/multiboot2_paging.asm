bits 32

section .bss
align 4096
pml4_table:   resb 4096

align 4096
pdpt_table:   resb 4096

align 4096
pd_table:     resb 4096

align 4096
pt_table:     resb 4096

section .text
global multiboot2_paging_setup

multiboot2_paging_setup:
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
	mov ecx, 512          ; number of entries
	xor ebx, ebx          ; entry index

pt_fill_loop:
    mov eax, ebx
    shl eax, 12       ; physical address = index * 0x1000
    or  eax, 0x003    ; present + writable

    mov [pt_table + ebx*8], eax     ; low 32 bits
    mov dword [pt_table + ebx*8 + 4], 0 ; high 32 bits = 0

    inc ebx
    loop pt_fill_loop

	;; Load the table into the CPU
	mov eax, pml4_table
	mov cr3, eax

	ret
