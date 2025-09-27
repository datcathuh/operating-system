set disassemble-next-line on
target remote :1234

# Break at bootloader
break *0x7C00

# break at kernel entry if known
break *0x1000

# Show registers and nearby instructions when stopping
layout asm
layout regs
layout split

symbol-file ../build/kernel/kernel.elf
