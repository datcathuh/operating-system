all: $(BUILD_DIR)/mbr/mbr.bin

$(BUILD_DIR)/mbr/mbr.bin: main.asm
	mkdir -p $(dir $@)
	nasm $< -f bin -o $@

clean:
	$(RM) -f $(BUILD_DIR)/mbr/*.o $(BUILD_DIR)/mbr/mbr.bin

.PHONY: all clean
