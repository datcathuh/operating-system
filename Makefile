BUILD_DIR := $(CURDIR)/build
KERNEL_DIR := src/kernel

export BUILD_DIR

all:
	@echo "Make targets"
	@echo " rebuild               Rebuild from scratch"
	@echo " clean                 Removes all built files"
	@echo " run                   Starts virtual machine"
	@echo ""
	@echo " run_debug             Starts virtual machine in debug mode"
	@echo " gdb                   Starts debugger"
	@echo ""
	@echo "compile_commands.json  Creates file for code analytics"

rebuild: clean
	$(MAKE) -C src

clean:
	$(MAKE) -C src $@

gdb:
	$(MAKE) -C src $@

run:
	$(MAKE) -C src $@

run_debug:
	$(MAKE) -C src $@

run_debug_extra:
	$(MAKE) -C src $@

# A target for generating a definition of all compile commands.
# This is used if you have Emacs as code editor together with
# lsp-mode and clangd for quick code navigation.
compile_commands.json:
	bear -- $(MAKE) rebuild

.PHONY: all clean gdb run run_debug
