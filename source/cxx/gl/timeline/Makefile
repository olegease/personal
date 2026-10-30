.PHONY: setup

BUILD := build
SON8PATH := $(HOME)/code/ghub/soneight

usage:
	@echo "make [command]"
	@echo TODO

setup:
	@ln -s $(SON8PATH) son8
	@mkdir -p $(BUILD)

cmake:
	@cmake -B $(BUILD) -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

ninja:
	@ninja -C $(BUILD)

clean:
	@rm -f son8
	@rm -fr build
