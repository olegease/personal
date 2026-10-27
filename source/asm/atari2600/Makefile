INC := /usr/local/include/atari2600
NAME := cart
OUT := $(NAME).bin

usage:
	@echo "make [command]"
template:
	dasm template.s -f3 -v0 -I$(INC) -o$(OUT) -s$(NAME).sym -l$(NAME).lst
verpmv:
	dasm verpmv.s -f3 -v0 -I$(INC) -o$(OUT)
horpmv:
	dasm horpmv.s -f3 -v0 -I$(INC) -o$(OUT)
run:
	@stella $(OUT)
clean:
	@rm -f $(OUT)
