# Detest Makefile

CC=gcc
INCLUDE=include
COPTS=-Wall -std=c99
mkfile_path:=$(abspath $(lastword $(MAKEFILE_LIST)))
current_dir:=.
example_lib:=calculator

example:
	mkdir -p bin
	mkdir -p build
	$(CC) -c -I$(INCLUDE) $(COPTS) -o build/$(example_lib).o examples/$(example_lib).c
	$(CC) -I$(INCLUDE) $(COPTS) -o bin/$(example_lib)_test examples/$(example_lib)_test.c build/$(example_lib).o
	bin/$(example_lib)_test

clean:
	rm -rf bin
	rm -rf build

.PHONY: clean example

