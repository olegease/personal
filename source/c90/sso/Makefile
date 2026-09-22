.PHONY: build

ERR := -Werror -Wpedantic -Wall -Wextra 
FLG := -std=c90
SRC := main.c
OUT := main.out 

usage:
	@echo "usage: make [command]"
	@echo "build: compile program"
	@echo "start: execute program"
	@echo "clean: remove program"

build:
	gcc $(FLG) $(ERR) $(SRC) -o $(OUT)

run:
	@./$(OUT)

clean:
	@rm -f $(OUT)
