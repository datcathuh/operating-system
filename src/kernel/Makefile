OBJECTS+=keyboard.o
OBJECTS+=kernel-entry.o
OBJECTS+=kernel.o
OBJECTS+=vga.o

CFLAGS=-m32 -ffreestanding -O2 -nostdlib

all: kernel.bin

kernel-entry.o: kernel-entry.asm
	nasm $< -f elf32 -o $@

kernel.elf: $(OBJECTS)
	ld -m elf_i386 -o $@ -T linker.ld $^

kernel.bin: kernel.elf
	objcopy -O binary kernel.elf kernel.bin

clean:
	$(RM) -f *.o kernel.bin kernel.elf

.PHONY: all clean
