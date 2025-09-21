all:
	@echo "Make targets"
	@echo " rebuild"
	@echo " clean"
	@echo " run"

rebuild: clean
	$(MAKE) -C src

clean:
	$(MAKE) -C src $@

run:
	$(MAKE) -C src $@

.PHONY: all clean run

