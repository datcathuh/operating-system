OBJECTS+=$(BUILD_DIR)/kernel/acpi.o
OBJECTS+=$(BUILD_DIR)/kernel/apm.o
OBJECTS+=$(BUILD_DIR)/kernel/diagnostics.o
OBJECTS+=$(BUILD_DIR)/kernel/gdt.o
OBJECTS+=$(BUILD_DIR)/kernel/gdt_flush.o
OBJECTS+=$(BUILD_DIR)/kernel/idt.o

# Devices
OBJECTS+=$(BUILD_DIR)/kernel/devices/intel_440fx.o
OBJECTS+=$(BUILD_DIR)/kernel/devices/intel_82371sb_acpi.o
OBJECTS+=$(BUILD_DIR)/kernel/devices/intel_82371sb_isa.o
OBJECTS+=$(BUILD_DIR)/kernel/devices/intel_82371sb_ide.o
OBJECTS+=$(BUILD_DIR)/kernel/devices/intel_82540em.o
OBJECTS+=$(BUILD_DIR)/kernel/devices/usb_xhci.o

# Interrupt service routines
OBJECTS+=$(BUILD_DIR)/kernel/isr/irq_gp_asm.o
OBJECTS+=$(BUILD_DIR)/kernel/isr/irq_gp_c.o
OBJECTS+=$(BUILD_DIR)/kernel/isr/irq_double_fault_asm.o
OBJECTS+=$(BUILD_DIR)/kernel/isr/irq_double_fault_c.o
OBJECTS+=$(BUILD_DIR)/kernel/isr/irq_keyboard_asm.o
OBJECTS+=$(BUILD_DIR)/kernel/isr/irq_keyboard_c.o
OBJECTS+=$(BUILD_DIR)/kernel/isr/irq_timer_asm.o
OBJECTS+=$(BUILD_DIR)/kernel/isr/irq_timer_c.o

OBJECTS+=$(BUILD_DIR)/kernel/keyboard.o
OBJECTS+=$(BUILD_DIR)/kernel/kernel-entry.o
OBJECTS+=$(BUILD_DIR)/kernel/kernel.o

# Kernel shell commands.
OBJECTS+=$(BUILD_DIR)/kernel/kshell/kshell.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell/kshell_bga.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell/kshell_help.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell/kshell_julia.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell/kshell_mandelbrot.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell/kshell_shutdown.o

OBJECTS+=$(BUILD_DIR)/kernel/lapic.o
OBJECTS+=$(BUILD_DIR)/kernel/memory.o
OBJECTS+=$(BUILD_DIR)/kernel/pci.o
OBJECTS+=$(BUILD_DIR)/kernel/pic.o
OBJECTS+=$(BUILD_DIR)/kernel/pit.o
OBJECTS+=$(BUILD_DIR)/kernel/serial.o
OBJECTS+=$(BUILD_DIR)/kernel/string.o

OBJECTS+=$(BUILD_DIR)/kernel/terminal_gfx.o

# Video drivers
OBJECTS+=$(BUILD_DIR)/kernel/video/bga.o
OBJECTS+=$(BUILD_DIR)/kernel/video/vga.o
OBJECTS+=$(BUILD_DIR)/kernel/video/video.o

CFLAGS=-g -std=c23 -pedantic -Wall -Wextra -m32 -ffreestanding -O2 -nostdlib -nostdinc -I.

all: $(BUILD_DIR)/kernel/kernel.bin

$(BUILD_DIR)/kernel/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/kernel/%.o: %.asm
	@mkdir -p $(dir $@)
	nasm $< -f elf32 -o $@

$(BUILD_DIR)/kernel/isr/%.o: %.asm
	@mkdir -p $(dir $@)
	nasm $< -f elf32 -o $@

$(BUILD_DIR)/kernel/kernel.elf: $(OBJECTS)
	ld -m elf_i386 -o $@ -T linker.ld $^

$(BUILD_DIR)/kernel/kernel.bin: $(BUILD_DIR)/kernel/kernel.elf
	objcopy -O binary $(BUILD_DIR)/kernel/kernel.elf $(BUILD_DIR)/kernel/kernel.bin
	@echo -n "Kernel sector count: "
	@./kernel-sector-count

clean:
	$(RM) -f $(BUILD_DIR)/kernel/*.o $(BUILD_DIR)/kernel/devices/*.o $(BUILD_DIR)/kernel/isr/*.o $(BUILD_DIR)/kernel/video/*.o $(BUILD_DIR)/kernel/kshell/*.o $(BUILD_DIR)/kernel/kernel.bin $(BUILD_DIR)/kernel/kernel.elf

.PHONY: all clean
