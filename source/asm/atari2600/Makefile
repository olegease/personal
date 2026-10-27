INC := /usr/local/include/atari2600
OUT := cart.bin

usage:
	@echo "make [command]"
template:
	dasm template.s -f3 -v0 -I$(INC) -o$(OUT)
verpmv:
	dasm verpmv.s -f3 -v0 -I$(INC) -o$(OUT)
run:
	@stella $(OUT)
clean:
	@rm -f $(OUT)
