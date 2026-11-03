.PHONY: usage setup cmake ninja clean

BUILD := build
SON8PATH := $(HOME)/code/ghub/soneight
FORKPATH := $(HOME)/code/ghub/forkease

usage:
	@echo "make [command]"
	@echo TODO

setup:
	@ln -sf $(SON8PATH) son8
	@ln -sf $(FORKPATH) fork
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
	@rm -fr build
