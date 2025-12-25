all: $(BUILD_DIR)/hda.img

$(BUILD_DIR)/hda.img:
	$(MAKE) -C kernel
	$(MAKE) -C stage2
	$(MAKE) -C mbr
	cat $(BUILD_DIR)/mbr/mbr.bin $(BUILD_DIR)/stage2/stage2.bin $(BUILD_DIR)/kernel/kernel.bin > $@
	truncate -s 1M $@

$(BUILD_DIR)/hdb.img:
	fallocate -l 1M $@

clean:
	$(MAKE) -C mbr $@
	$(MAKE) -C stage2 $@
	$(MAKE) -C kernel $@
	$(RM) $(BUILD_DIR)/hda.img $(BUILD_DIR)/hdb.img

gdb:
	gdb

$(BUILD_DIR)/ovmf/OVMF_VARS.fd:
	mkdir -p $(BUILD_DIR)/ovmf
	cp /usr/share/edk2/ovmf/OVMF_VARS.fd $@

run_uefi: $(BUILD_DIR)/uefi.img $(BUILD_DIR)/ovmf/OVMF_VARS.fd
	qemu-system-x86_64 \
		-smp cpus=2 \
		-device qemu-xhci \
		-drive if=pflash,format=raw,readonly=on,file=/usr/share/edk2/ovmf/OVMF_CODE.fd \
		-drive if=pflash,format=raw,file=$(BUILD_DIR)/ovmf/OVMF_VARS.fd \
		-drive file=$(BUILD_DIR)/uefi.img,format=raw \
		-serial stdio

	# -drive file=$(BUILD_DIR)/hdb.img,if=virtio,format=raw \


run: $(BUILD_DIR)/hda.img $(BUILD_DIR)/hdb.img
	qemu-system-x86_64 \
		-smp cpus=2 \
		-device qemu-xhci \
		-drive file=$(BUILD_DIR)/hda.img,format=raw \
		-drive file=$(BUILD_DIR)/hdb.img,if=virtio,format=raw \
		-serial stdio

run_debug: $(BUILD_DIR)/hda.img $(BUILD_DIR)/hdb.img
	qemu-system-x86_64 \
		-smp cpus=2 \
		-device qemu-xhci \
		-drive file=$(BUILD_DIR)/hda.img,format=raw \
		-drive file=$(BUILD_DIR)/hdb.img,if=virtio,format=raw \
		-S -s -serial stdio

run_debug_extra: $(BUILD_DIR)/hda.img $(BUILD_DIR)/hdb.img
	qemu-system-x86_64 \
		-smp cpus=2 \
		-device qemu-xhci \
		-drive file=$(BUILD_DIR)/hda.img,format=raw \
		-drive file=$(BUILD_DIR)/hdb.img,if=virtio,format=raw \
		-S -s -serial stdio \
		-no-reboot -d int,cpu_reset -no-shutdown

.PHONY: all clean run run_debug
