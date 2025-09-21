all: mbr.bin

mbr.bin: main.asm
	nasm $< -f bin -o $@

clean:
	$(RM) -f *.o mbr.bin

.PHONY: all clean
