all: tests

.PHONY: tests
tests:
	@echo "Making tests"
	$(MAKE) -C ./tests

.PHONY: debug
debug:
	@echo "Making tests"
	$(MAKE) -C ./tests

clean:
	$(MAKE) -C ./tests clean
