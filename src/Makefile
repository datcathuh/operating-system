all: hdd.img

hdd.img:
	$(MAKE) -C mbr
	$(MAKE) -C kernel
	cat mbr/mbr.bin kernel/kernel.bin > $@

clean:
	$(MAKE) -C mbr $@
	$(MAKE) -C kernel $@
	$(RM) hdd.img

run: hdd.img
	qemu-system-i386 -drive file=hdd.img,format=raw

.PHONY: all clean run

