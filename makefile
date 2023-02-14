CCX = gcc -Wall -ansi -pedantic

assembler: assembler.c preassembler.o parser.o mlist.o
	$(CCX) assembler.c preassembler.o parser.o mlist.o -o assembler
preassembler.o: preassembler.c preassembler.h
	$(CCX) -c preassembler.c -o preassembler.o
parser.o: parser.c parser.h
	$(CCX) -c parser.c -o parser.o
mlist.o: mlist.c mlist.h
	$(CCX) -c mlist.c -o mlist.o

.PHONY: clean

clean:
	rm *.o assembler
