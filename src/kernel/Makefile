OBJECTS+=$(BUILD_DIR)/kernel/keyboard.o
OBJECTS+=$(BUILD_DIR)/kernel/kernel-entry.o
OBJECTS+=$(BUILD_DIR)/kernel/kernel.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell_help.o
OBJECTS+=$(BUILD_DIR)/kernel/memory.o
OBJECTS+=$(BUILD_DIR)/kernel/string.o
OBJECTS+=$(BUILD_DIR)/kernel/vga.o

CFLAGS=-std=c23 -pedantic -Wall -Wextra -m32 -ffreestanding -O2 -nostdlib

$(BUILD_DIR)/kernel/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

all: $(BUILD_DIR)/kernel/kernel.bin

$(BUILD_DIR)/kernel/kernel-entry.o: kernel-entry.asm
	nasm $< -f elf32 -o $@

$(BUILD_DIR)/kernel/kernel.elf: $(OBJECTS)
	ld -m elf_i386 -o $@ -T linker.ld $^

$(BUILD_DIR)/kernel/kernel.bin: $(BUILD_DIR)/kernel/kernel.elf
	objcopy -O binary $(BUILD_DIR)/kernel/kernel.elf $(BUILD_DIR)/kernel/kernel.bin

clean:
	$(RM) -f $(BUILD_DIR)/kernel/*.o $(BUILD_DIR)/kernel/kernel.bin $(BUILD_DIR)/kernel/kernel.elf

.PHONY: all clean
