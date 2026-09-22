.PHONY: usage start gencc build

FOLDER := $(shell pwd)
CCJSON := compile_commands.json
TARGET := rat.out
SOURCE := $(FOLDER)/main.mxx
OBJECT := $(FOLDER)/main.obj
IMPORT := /usr/include/c++/16/bits/std.cc
IMPOBJ := $(FOLDER)/stdmodule.o

usage:
	@echo 'usage: make [command]'
	@echo 'gencc: generate compile commands json'
	@echo 'build: with c++23 g++ modules import std'
	@echo 'start: run `rat` applciation'

gencc:
	@echo -e "[{" > $(CCJSON)
	@echo -e '\t"directory": "$(FOLDER)",' >> $(CCJSON)
	@echo -e '\t"command": "clang++ -std=c++23 -c -x c++ -DAPP_INCLUDE_STD",' >> $(CCJSON)
	@echo -e '\t"file": "$(SOURCE)"' >> $(CCJSON)
	@echo -e "}]" >> $(CCJSON)

build: $(TARGET)

start: $(TARGET)
	@./$(TARGET)

$(TARGET): $(OBJECT)
	g++ -std=c++23 $(IMPOBJ) $(OBJECT) -o $(TARGET)

$(OBJECT): $(SOURCE) $(IMPOBJ)
	g++ -std=c++23 -fmodules -c -x c++ $(SOURCE) -o $(OBJECT)

$(IMPOBJ): $(IMPORT)
	g++ -std=c++23 -fmodules -c $(IMPORT) -o $(IMPOBJ)
