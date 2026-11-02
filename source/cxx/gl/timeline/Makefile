.PHONY: usage setup cmake ninja clean

BUILD := build
SON8PATH := $(HOME)/code/ghub/soneight

usage:
	@echo "make [command]"
	@echo TODO

setup:
	@ln -s $(SON8PATH) son8
	@mkdir -p $(BUILD)

cmake:
	@cmake -B $(BUILD) -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_COLOR_DIAGNOSTICS=ON

ninja:
	@ninja -C $(BUILD)

run:
	@make ninja
	@./$(BUILD)/vertices-pure
	@./$(BUILD)/vertices-wrap

clean:
	@rm -f son8
	@rm -fr build
