.PHONY: usage start debug build clean

FOLDER := $(shell pwd)
TARGET := rat.out
SOURCE := $(FOLDER)/main.mxx
OBJECT := $(FOLDER)/main.obj
IMPORT := /usr/include/c++/16/bits/std.cc
IMPOBJ := $(FOLDER)/stdmodule.o
CFLAGS := -g -std=c++23

usage:
	@echo 'usage: make [command]'
	@echo 'build: with c++23 g++ modules import std'
	@echo 'debug: with gdb'
	@echo 'clean: remove build artifacts'

debug: $(TARGET)
	@gdb $(TARGET)

build: $(TARGET)

$(TARGET): $(OBJECT)
	g++ $(CFLAGS) $(IMPOBJ) $(OBJECT) -o $(TARGET)

$(OBJECT): $(SOURCE) $(IMPOBJ)
	g++ $(CFLAGS) -fmodules -c -x c++ $(SOURCE) -o $(OBJECT)

$(IMPOBJ): $(IMPORT)
	g++ $(CFLAGS) -fmodules -c $(IMPORT) -o $(IMPOBJ)

clean: 
	@rm -f $(TARGET) $(OBJECT) $(IMPOBJ)
