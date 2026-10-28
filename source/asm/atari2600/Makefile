INCL := /usr/local/include/atari2600
NAME := cart
CART := $(NAME).bin
FILE := $(NAME).bin $(NAME).sym $(NAME).lst
ARGS := -f3 -v0 -I$(INCL) -o$(NAME).bin -s$(NAME).sym -l$(NAME).lst

usage:
	@echo "make [command]"
template:
	dasm template.s $(ARGS)
verpmv:
	dasm verpmv.s $(ARGS)
horpmv:
	dasm horpmv.s $(ARGS)
run:
	@stella $(CART)
clean:
	@rm -f $(FILE)
