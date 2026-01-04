/* legacy_entry is used when booting through MBR -> stage2 -> kernel */
ENTRY(legacy_entry)

SECTIONS {
	. = 0x100000;

    .text BLOCK(128) : ALIGN(4K) {
        *legacy_entry.o(.text)
    }

	.boot : ALIGN(8) {
		/* The multiboot signature must exist very early in the file in order
		   for UEFI to pick it up. The signature contains a pointer to the
		   actual multiboot2_entry for the kernel. */
		*(.multiboot2)
	}

    .text BLOCK(4K) : ALIGN(4K) {
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

    .stack : ALIGN(16)
    {
        stack_bottom = .;      /* lowest address of stack */
        . += 0x4000;           /* 16 KB stack */
        stack_top = .;         /* highest address of stack */
    }

    .stack_df : ALIGN(16)
    {
        stack_df_bottom = .;   /* lowest address of stack */
        . += 0x4000;           /* 16 KB stack */
        stack_df_top = .;      /* highest address of stack */
    }

    kernel_end = ALIGN(4096);
	kernel_end = .;
}
