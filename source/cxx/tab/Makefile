.PHONY: usage build debug clean

TARGET := tab.out
SOURCE := main.cxx

usage:
	@echo 'TODO'

build: $(TARGET)

debug: $(TARGET)
	@gdb $(TARGET)

$(TARGET): $(SOURCE)
	g++ -g -std=c++17 $(SOURCE) -o $(TARGET)

clean:
	@rm -f $(TARGET)
