all: $(BUILD_DIR)/hdd.img

$(BUILD_DIR)/hdd.img:
	$(MAKE) -C kernel
	$(MAKE) -C stage2
	$(MAKE) -C mbr
	cat $(BUILD_DIR)/mbr/mbr.bin $(BUILD_DIR)/stage2/stage2.bin $(BUILD_DIR)/kernel/kernel.bin > $@

clean:
	$(MAKE) -C mbr $@
	$(MAKE) -C stage2 $@
	$(MAKE) -C kernel $@
	$(RM) $(BUILD_DIR)/hdd.img

gdb:
	gdb

run: $(BUILD_DIR)/hdd.img
	qemu-system-x86_64 -smp cpus=2 -device qemu-xhci -drive file=$(BUILD_DIR)/hdd.img,format=raw -serial stdio

run_debug: $(BUILD_DIR)/hdd.img
	# -S Start in paused state
	# -s open GDB on port 1234
	qemu-system-x86_64 -smp cpus=2 -device qemu-xhci -drive file=$(BUILD_DIR)/hdd.img,format=raw -S -s -serial stdio

run_debug_extra: $(BUILD_DIR)/hdd.img
	qemu-system-x86_64 -smp cpus=2 -device qemu-xhci -drive file=$(BUILD_DIR)/hdd.img,format=raw -S -s -serial stdio -no-reboot -d int,cpu_reset -no-shutdown

.PHONY: all clean run run_debug
