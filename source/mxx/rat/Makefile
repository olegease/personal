.PHONY: usage start gencc debug build clean

FOLDER := $(shell pwd)
CCJSON := compile_commands.json
TARGET := rat.out
SOURCE := $(FOLDER)/main.mxx
OBJECT := $(FOLDER)/main.obj
IMPORT := /usr/include/c++/16/bits/std.cc
IMPOBJ := $(FOLDER)/stdmodule.o
CFLAGS := -g -std=c++23
JFLAGS := -std=c++23 -c -x c++ -DAPP_INCLUDE_STD

usage:
	@echo 'usage: make [command]'
	@echo 'gencc: generate compile commands json'
	@echo 'build: with c++23 g++ modules import std'

gencc:
	@echo -e "[{" > $(CCJSON)
	@echo -e '\t"directory": "$(FOLDER)",' >> $(CCJSON)
	@echo -e '\t"command": "clang++ $(JFLAGS)",' >> $(CCJSON)
	@echo -e '\t"file": "$(SOURCE)"' >> $(CCJSON)
	@echo -e "}]" >> $(CCJSON)

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
