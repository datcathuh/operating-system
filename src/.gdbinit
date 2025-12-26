set disassemble-next-line on
target remote :1234

# Break at bootloader
break *0x7C00
#commands
#  set architecture i8086
#end

# break at stage2
break *0x0500
#commands
#  set architecture i8086
#end

# break at kernel
break *0x100000

break legacy_entry
break multiboot2_entry

# Show registers and nearby instructions when stopping
#layout asm
#layout regs
layout split

symbol-file ../build/stage2/stage2.elf
add-symbol-file ../build/kernel/kernel.elf
