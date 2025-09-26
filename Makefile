all:
	@echo "Make targets"
	@echo " rebuild     Rebuild from scratch"
	@echo " clean       Removes all built files"
	@echo " run         Starts virtual machine"
	@echo ""
	@echo " run_debug   Starts virtual machine in debug mode"
	@echo " gdb         Starts debugger"

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

.PHONY: all clean gdb run run_debug

