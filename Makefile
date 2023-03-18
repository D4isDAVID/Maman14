CCX := gcc -Wall -ansi -pedantic
SRC := $(wildcard *.c)
BIN := $(SRC:%.c=%.o)

all: assembler

assembler: $(BIN)
	$(CCX) $(BIN) -o assembler
%.o: %.c
	$(CCX) -c $< -o $@

test: assembler tests/booktest.as tests/empty.as tests/overflow.as
	./assembler tests/booktest tests/empty tests/overflow

clean:
	rm -f *.o assembler **/*.am **/*.ob **/*.ent **/*.ext

.PHONY: test clean
