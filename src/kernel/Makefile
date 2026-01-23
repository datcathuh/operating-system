# =============================================================================
# C settings
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

# =============================================================================
# C++ settings
# * No exceptions
# * No rtti information (dynamic_cast etc)
CXXFLAGS+=-g
CXXFLAGS+=-std=c++23
CXXFLAGS+=-pedantic
CXXFLAGS+=-Wall -Wextra
CXXFLAGS+=-m64
# Kernel / ABI safety
CXXFLAGS+=-mno-red-zone
CXXFLAGS+=-fno-omit-frame-pointer
CXXFLAGS+=-fno-stack-protector
CXXFLAGS+=-mcmodel=kernel
CXXFLAGS+=-ffreestanding
# Disable C++ runtime features
CXXFLAGS+=-fno-exceptions
CXXFLAGS+=-fno-rtti
CXXFLAGS+=-fno-unwind-tables
CXXFLAGS+=-fno-asynchronous-unwind-tables
CXXFLAGS+=-fno-threadsafe-statics
# No standard library
CXXFLAGS+=-nostdlib
CXXFLAGS+=-nostdinc++
CXXFLAGS+=-I.
# Optimization
CXXFLAGS+=-O3


OBJECTS+=$(BUILD_DIR)/kernel/arithmetics.o
OBJECTS+=$(BUILD_DIR)/kernel/apm.o
OBJECTS+=$(BUILD_DIR)/kernel/cpu_context_asm.o
OBJECTS+=$(BUILD_DIR)/kernel/diagnostics.o
OBJECTS+=$(BUILD_DIR)/kernel/panic.o
OBJECTS+=$(BUILD_DIR)/kernel/keyboard.o
OBJECTS+=$(BUILD_DIR)/kernel/kmain.o
OBJECTS+=$(BUILD_DIR)/kernel/memory.o
OBJECTS+=$(BUILD_DIR)/kernel/memory_heap.o
OBJECTS+=$(BUILD_DIR)/kernel/memory_page.o
OBJECTS+=$(BUILD_DIR)/kernel/pci.o
OBJECTS+=$(BUILD_DIR)/kernel/pci_blacklist.o
OBJECTS+=$(BUILD_DIR)/kernel/pic.o
OBJECTS+=$(BUILD_DIR)/kernel/pit.o
OBJECTS+=$(BUILD_DIR)/kernel/scheduler.o
OBJECTS+=$(BUILD_DIR)/kernel/serial.o
OBJECTS+=$(BUILD_DIR)/kernel/stream_simple.o
OBJECTS+=$(BUILD_DIR)/kernel/string.o
OBJECTS+=$(BUILD_DIR)/kernel/task.o
OBJECTS+=$(BUILD_DIR)/kernel/terminal_gfx.o

# x86_64 / CPU
OBJECTS+=$(BUILD_DIR)/kernel/arch/x86_64/cpu/gdt.o
OBJECTS+=$(BUILD_DIR)/kernel/arch/x86_64/cpu/idt.o
OBJECTS+=$(BUILD_DIR)/kernel/arch/x86_64/cpu/ioapic.o
OBJECTS+=$(BUILD_DIR)/kernel/arch/x86_64/cpu/lapic.o
OBJECTS+=$(BUILD_DIR)/kernel/arch/x86_64/cpu/nxe.o

# x86_64 / entry
OBJECTS+=$(BUILD_DIR)/kernel/arch/x86_64/entry/acpi.o
OBJECTS+=$(BUILD_DIR)/kernel/arch/x86_64/entry/legacy_entry.o
OBJECTS+=$(BUILD_DIR)/kernel/arch/x86_64/entry/multiboot2.o
OBJECTS+=$(BUILD_DIR)/kernel/arch/x86_64/entry/multiboot2_entry.o
OBJECTS+=$(BUILD_DIR)/kernel/arch/x86_64/entry/multiboot2_gdt.o
OBJECTS+=$(BUILD_DIR)/kernel/arch/x86_64/entry/multiboot2_paging.o
OBJECTS+=$(BUILD_DIR)/kernel/arch/x86_64/entry/multiboot2_signature.o

# x86_64 / Interrupt service routines
OBJECTS+=$(BUILD_DIR)/kernel/arch/x86_64/interrupt/irq_gp_asm.o
OBJECTS+=$(BUILD_DIR)/kernel/arch/x86_64/interrupt/irq_gp_c.o
OBJECTS+=$(BUILD_DIR)/kernel/arch/x86_64/interrupt/irq_double_fault_asm.o
OBJECTS+=$(BUILD_DIR)/kernel/arch/x86_64/interrupt/irq_double_fault_c.o
OBJECTS+=$(BUILD_DIR)/kernel/arch/x86_64/interrupt/irq_keyboard_asm.o
OBJECTS+=$(BUILD_DIR)/kernel/arch/x86_64/interrupt/irq_keyboard_c.o
OBJECTS+=$(BUILD_DIR)/kernel/arch/x86_64/interrupt/irq_nm_asm.o
OBJECTS+=$(BUILD_DIR)/kernel/arch/x86_64/interrupt/irq_nm_c.o
OBJECTS+=$(BUILD_DIR)/kernel/arch/x86_64/interrupt/irq_page_fault_asm.o
OBJECTS+=$(BUILD_DIR)/kernel/arch/x86_64/interrupt/irq_page_fault_c.o
OBJECTS+=$(BUILD_DIR)/kernel/arch/x86_64/interrupt/irq_timer_asm.o
OBJECTS+=$(BUILD_DIR)/kernel/arch/x86_64/interrupt/irq_timer_c.o
OBJECTS+=$(BUILD_DIR)/kernel/arch/x86_64/interrupt/irq_ud_asm.o
OBJECTS+=$(BUILD_DIR)/kernel/arch/x86_64/interrupt/irq_ud_c.o

# Devices
OBJECTS+=$(BUILD_DIR)/kernel/devices/intel_440fx.o
OBJECTS+=$(BUILD_DIR)/kernel/devices/intel_82371sb_acpi.o
OBJECTS+=$(BUILD_DIR)/kernel/devices/intel_82371sb_isa.o
OBJECTS+=$(BUILD_DIR)/kernel/devices/intel_82371sb_ide.o
OBJECTS+=$(BUILD_DIR)/kernel/devices/intel_82540em.o
OBJECTS+=$(BUILD_DIR)/kernel/devices/redhat_virtio_block_device.o
OBJECTS+=$(BUILD_DIR)/kernel/devices/usb_xhci.o

# Kernel shell commands.
OBJECTS+=$(BUILD_DIR)/kernel/kshell/kshell.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell/kshell_bga.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell/kshell_help.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell/kshell_julia.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell/kshell_mandelbrot.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell/kshell_shutdown.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell/kshell_snake.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell/kshell_sysinfo.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell/kshell_task.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell/kshell_tetris.o
OBJECTS+=$(BUILD_DIR)/kernel/kshell/kshell_x.o

# Video drivers
OBJECTS+=$(BUILD_DIR)/kernel/video/bga.o
OBJECTS+=$(BUILD_DIR)/kernel/video/font.o
OBJECTS+=$(BUILD_DIR)/kernel/video/gop.o
OBJECTS+=$(BUILD_DIR)/kernel/video/vga.o
OBJECTS+=$(BUILD_DIR)/kernel/video/video.o

all: $(BUILD_DIR)/kernel/kernel.bin

$(BUILD_DIR)/kernel/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/kernel/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/kernel/%.o: %.asm
	@mkdir -p $(dir $@)
	nasm $< -f elf64 -o $@

$(BUILD_DIR)/kernel/arch/x86_64/interrupt/%.o: %.asm
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
	$(RM) -rf $(BUILD_DIR)/kernel

.PHONY: all clean
