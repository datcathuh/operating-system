OBJECTS+=$(BUILD_DIR)/kernel/apm.o
OBJECTS+=$(BUILD_DIR)/kernel/idt.o
OBJECTS+=$(BUILD_DIR)/kernel/irq_keyboard_asm.o
OBJECTS+=$(BUILD_DIR)/kernel/irq_keyboard_c.o
OBJECTS+=$(BUILD_DIR)/kernel/irq_timer_asm.o
OBJECTS+=$(BUILD_DIR)/kernel/irq_timer_c.o
OBJECTS+=$(BUILD_DIR)/kernel/keyboard.o
OBJECTS+=$(BUILD_DIR)/kernel/kernel-entry.o
OBJECTS+=$(BUILD_DIR)/kernel/kernel.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell_bga.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell_help.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell_julia.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell_mandelbrot.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell_shutdown.o
OBJECTS+=$(BUILD_DIR)/kernel/memory.o
OBJECTS+=$(BUILD_DIR)/kernel/pci.o
OBJECTS+=$(BUILD_DIR)/kernel/pic.o
OBJECTS+=$(BUILD_DIR)/kernel/pit.o
OBJECTS+=$(BUILD_DIR)/kernel/serial.o
OBJECTS+=$(BUILD_DIR)/kernel/string.o
OBJECTS+=$(BUILD_DIR)/kernel/video/bga.o
OBJECTS+=$(BUILD_DIR)/kernel/video/vga.o
OBJECTS+=$(BUILD_DIR)/kernel/video/video.o

CFLAGS=-std=c23 -pedantic -Wall -Wextra -m32 -ffreestanding -O2 -nostdlib -nostdinc -I.

all: $(BUILD_DIR)/kernel/kernel.bin

$(BUILD_DIR)/kernel/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/kernel/%.o: %.asm
	nasm $< -f elf32 -o $@

$(BUILD_DIR)/kernel/kernel.elf: $(OBJECTS)
	ld -m elf_i386 -o $@ -T linker.ld $^

$(BUILD_DIR)/kernel/kernel.bin: $(BUILD_DIR)/kernel/kernel.elf
	objcopy -O binary $(BUILD_DIR)/kernel/kernel.elf $(BUILD_DIR)/kernel/kernel.bin
	@echo -n "Kernel sector count: "
	@./kernel-sector-count

clean:
	$(RM) -f $(BUILD_DIR)/kernel/*.o $(BUILD_DIR)/kernel/video/*.o $(BUILD_DIR)/kernel/kernel.bin $(BUILD_DIR)/kernel/kernel.elf

.PHONY: all clean
