set disassemble-next-line on
target remote :1234

# Break at bootloader
break *0x7C00

# break at stage2
break *0x1000

# break at kernel
break *0x100000

# Show registers and nearby instructions when stopping
layout asm
layout regs
layout split

symbol-file ../build/stage2/stage2.elf
symbol-file ../build/kernel/kernel.elf
