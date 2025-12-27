OBJECTS+=$(BUILD_DIR)/kernel/arithmetics.o
OBJECTS+=$(BUILD_DIR)/kernel/acpi.o
OBJECTS+=$(BUILD_DIR)/kernel/apm.o
OBJECTS+=$(BUILD_DIR)/kernel/diagnostics.o
OBJECTS+=$(BUILD_DIR)/kernel/idt.o

# Boot
OBJECTS+=$(BUILD_DIR)/kernel/boot/legacy_entry.o
OBJECTS+=$(BUILD_DIR)/kernel/boot/multiboot2.o
OBJECTS+=$(BUILD_DIR)/kernel/boot/multiboot2_entry.o
OBJECTS+=$(BUILD_DIR)/kernel/boot/multiboot2_gdt.o
OBJECTS+=$(BUILD_DIR)/kernel/boot/multiboot2_paging.o
OBJECTS+=$(BUILD_DIR)/kernel/boot/multiboot2_signature.o

# Devices
OBJECTS+=$(BUILD_DIR)/kernel/devices/intel_440fx.o
OBJECTS+=$(BUILD_DIR)/kernel/devices/intel_82371sb_acpi.o
OBJECTS+=$(BUILD_DIR)/kernel/devices/intel_82371sb_isa.o
OBJECTS+=$(BUILD_DIR)/kernel/devices/intel_82371sb_ide.o
OBJECTS+=$(BUILD_DIR)/kernel/devices/intel_82540em.o
OBJECTS+=$(BUILD_DIR)/kernel/devices/redhat_virtio_block_device.o
OBJECTS+=$(BUILD_DIR)/kernel/devices/usb_xhci.o

# Interrupt service routines
OBJECTS+=$(BUILD_DIR)/kernel/isr/irq_gp_asm.o
OBJECTS+=$(BUILD_DIR)/kernel/isr/irq_gp_c.o
OBJECTS+=$(BUILD_DIR)/kernel/isr/irq_double_fault_asm.o
OBJECTS+=$(BUILD_DIR)/kernel/isr/irq_double_fault_c.o
OBJECTS+=$(BUILD_DIR)/kernel/isr/irq_keyboard_asm.o
OBJECTS+=$(BUILD_DIR)/kernel/isr/irq_keyboard_c.o
OBJECTS+=$(BUILD_DIR)/kernel/isr/irq_page_fault_asm.o
OBJECTS+=$(BUILD_DIR)/kernel/isr/irq_page_fault_c.o
OBJECTS+=$(BUILD_DIR)/kernel/isr/irq_timer_asm.o
OBJECTS+=$(BUILD_DIR)/kernel/isr/irq_timer_c.o

OBJECTS+=$(BUILD_DIR)/kernel/keyboard.o
OBJECTS+=$(BUILD_DIR)/kernel/kmain.o

# Kernel shell commands.
OBJECTS+=$(BUILD_DIR)/kernel/kshell/kshell.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell/kshell_bga.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell/kshell_help.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell/kshell_julia.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell/kshell_mandelbrot.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell/kshell_shutdown.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell/kshell_snake.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell/kshell_sysinfo.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell/kshell_tetris.o

OBJECTS+=$(BUILD_DIR)/kernel/lapic.o
OBJECTS+=$(BUILD_DIR)/kernel/memory.o
OBJECTS+=$(BUILD_DIR)/kernel/memory_heap.o
OBJECTS+=$(BUILD_DIR)/kernel/memory_page.o
OBJECTS+=$(BUILD_DIR)/kernel/pci.o
OBJECTS+=$(BUILD_DIR)/kernel/pci_blacklist.o
OBJECTS+=$(BUILD_DIR)/kernel/pic.o
OBJECTS+=$(BUILD_DIR)/kernel/pit.o
OBJECTS+=$(BUILD_DIR)/kernel/serial.o
OBJECTS+=$(BUILD_DIR)/kernel/string.o

OBJECTS+=$(BUILD_DIR)/kernel/terminal_gfx.o

# Video drivers
OBJECTS+=$(BUILD_DIR)/kernel/video/bga.o
OBJECTS+=$(BUILD_DIR)/kernel/video/font.o
OBJECTS+=$(BUILD_DIR)/kernel/video/gop.o
OBJECTS+=$(BUILD_DIR)/kernel/video/vga.o
OBJECTS+=$(BUILD_DIR)/kernel/video/video.o

CFLAGS+=-g -std=c23 -pedantic -Wall -Wextra -m64
CFLAGS+=-mno-red-zone
CFLAGS+=-fno-omit-frame-pointer
CFLAGS+=-fno-stack-protector
CFLAGS+=-mcmodel=kernel
CFLAGS+=-ffreestanding
CFLAGS+=-O3
CFLAGS+=-nostdlib
CFLAGS+=-nostdinc
CFLAGS+=-I.

all: $(BUILD_DIR)/kernel/kernel.bin

$(BUILD_DIR)/kernel/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/kernel/%.o: %.asm
	@mkdir -p $(dir $@)
	nasm $< -f elf64 -o $@

$(BUILD_DIR)/kernel/isr/%.o: %.asm
	@mkdir -p $(dir $@)
	nasm $< -f elf64 -o $@

$(BUILD_DIR)/kernel/kernel.elf: $(OBJECTS)
	ld -m elf_x86_64 -o $@ -T linker.ld $^

$(BUILD_DIR)/kernel/kernel.bin: $(BUILD_DIR)/kernel/kernel.elf
	objcopy -O binary $(BUILD_DIR)/kernel/kernel.elf $(BUILD_DIR)/kernel/kernel.bin
	truncate -s $$(( ( $$(stat -c%s $@) + 511 ) / 512 * 512 )) $@
	@echo -n "Kernel sector count: "
	@./kernel-sector-count

clean:
	$(RM) -f $(BUILD_DIR)/kernel/*.o $(BUILD_DIR)/kernel/devices/*.o $(BUILD_DIR)/kernel/isr/*.o $(BUILD_DIR)/kernel/video/*.o $(BUILD_DIR)/kernel/kshell/*.o $(BUILD_DIR)/kernel/kernel.bin $(BUILD_DIR)/kernel/kernel.elf

.PHONY: all clean
