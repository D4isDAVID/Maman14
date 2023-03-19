CCX := gcc -Wall -ansi -pedantic
SRC = $(filter-out assembler.c, $(wildcard *.c))
BIN := $(SRC:%.c=%.o)
TESTS := $(wildcard tests/*.as)

all: assembler

assembler: assembler.c $(BIN)
	$(CCX) assembler.c $(BIN) -o assembler
%.o: %.c %.h
	$(CCX) -c $< -o $@

.PHONY: test clean

test: assembler $(TESTS)
	./assembler $(TESTS:%.as=%)

clean:
	rm -f *.o assembler **/*.am **/*.ob **/*.ent **/*.ext
