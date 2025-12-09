ENTRY(start)

SECTIONS {
    . = 0x0500;

    .text : {
        *stage2-entry.o(.text)
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(COMMON)
        *(.bss)
    }
}
