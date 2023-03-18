CCX := gcc -Wall -ansi -pedantic
SRC := $(wildcard *.c)
BIN := $(SRC:%.c=%.o)
TEST := $(wildcard tests/*.as)

all: assembler

assembler: $(BIN)
	$(CCX) $(BIN) -o assembler
%.o: %.c
	$(CCX) -c $< -o $@

test: assembler $(TEST)
	./assembler $(TEST:%.as=%)

clean:
	rm -f *.o assembler **/*.am **/*.ob **/*.ent **/*.ext

.PHONY: test clean
