all: hdd.img

hdd.img:
	$(MAKE) -C mbr
	$(MAKE) -C kernel
	cat mbr/mbr.bin kernel/kernel.bin > $@

clean:
	$(MAKE) -C mbr $@
	$(MAKE) -C kernel $@
	$(RM) hdd.img

gdb:
	gdb

run: hdd.img
	qemu-system-x86_64 -drive file=hdd.img,format=raw

run_debug: hdd.img
	# -S Start in paused state
	# -s open GDB on port 1234
	qemu-system-x86_64 -drive file=hdd.img,format=raw -S -s

.PHONY: all clean run run_debug

