ENTRY(_start)

SECTIONS {
    . = 0x100000;

    .text BLOCK(4K) : ALIGN(4K) {
        *kernel-entry.o(.text)
        *(.text)
    }

    .rodata BLOCK(4K) : ALIGN(4K) {
        *(.rodata)
    }

    .data BLOCK(4K) : ALIGN(4K) {
        *(.data)
    }

    .bss BLOCK(4K) : ALIGN(4K) {
         __bss_start = .;
        *(COMMON)
        *(.bss)
        __bss_end = .;
    }

	kernel_end = ALIGN(4096);
	kernel_end = .;
}
