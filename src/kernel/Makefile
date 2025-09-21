OBJECTS+=keyboard.o
OBJECTS+=kernel-entry.o
OBJECTS+=kernel.o
OBJECTS+=vga.o

CFLAGS=-m32 -ffreestanding

all: kernel.bin

kernel-entry.o: kernel-entry.asm
	nasm $< -f elf -o $@

kernel.bin: $(OBJECTS)
	ld -m elf_i386 -o $@ -Ttext 0x1000 $^ --oformat binary

clean:
	$(RM) -f *.o kernel.bin

.PHONY: all clean
