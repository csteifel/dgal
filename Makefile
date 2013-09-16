all: lib tests

.PHONY: lib
lib:
	@echo "Making library"
	$(MAKE) -C ./build

.PHONY: tests
tests:
	@echo "Making tests"
	$(MAKE) -C ./tests

.PHONY: debug
debug:
	@echo "Making debug library"
	$(MAKE) -C ./build debug
	@echo "Making tests"
	$(MAKE) -C ./tests

clean:
	$(MAKE) -C ./build clean
	$(MAKE) -C ./tests clean
