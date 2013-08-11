all: lib tests

.PHONY: lib
lib:
	@echo "Making library"
	$(MAKE) -C ./build

.PHONY: tests
tests:
	@echo "Making tests"
	$(MAKE) -C ./tests

clean:
	$(MAKE) -C ./build clean
	$(MAKE) -C ./tests clean