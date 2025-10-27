# Assembler 16 bit
OBJECTS+=$(BUILD_DIR)/stage2/a20.o
OBJECTS+=$(BUILD_DIR)/stage2/disk.o
OBJECTS+=$(BUILD_DIR)/stage2/print.o
OBJECTS+=$(BUILD_DIR)/stage2/stage2-entry.o

OBJECTS+=$(BUILD_DIR)/stage2/main.o

CFLAGS=-std=c23 -pedantic -Wall -Wextra -m32 -ffreestanding -O2 -nostdlib

all: $(BUILD_DIR)/stage2/stage2.bin

$(BUILD_DIR)/stage2/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/stage2/%.o: %.asm
	@mkdir -p $(dir $@)
	nasm $< -f elf32 -o $@

$(BUILD_DIR)/stage2/stage2.elf: $(OBJECTS)
	ld -m elf_i386 -o $@ -T linker.ld $^

$(BUILD_DIR)/stage2/stage2.bin: $(BUILD_DIR)/stage2/stage2.elf
	objcopy -O binary $(BUILD_DIR)/stage2/stage2.elf $(BUILD_DIR)/stage2/stage2.bin
	truncate -s $$(( ( $$(stat -c%s $@) + 511 ) / 512 * 512 )) $@
	@echo -n "Stage2 sector count: "
	@./stage2-sector-count

$(BUILD_DIR)/stage2/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -f $(BUILD_DIR)/stage2/*

.PHONY: all clean
