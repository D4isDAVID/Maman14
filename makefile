CCX := gcc -Wall -ansi -pedantic

assembler: assembler.c preassembler.o firstphase.o parser.o hashmap.o symbols.o
	$(CCX) assembler.c preassembler.o firstphase.o parser.o hashmap.o symbols.o -o assembler
preassembler.o: preassembler.c preassembler.h
	$(CCX) -c preassembler.c -o preassembler.o
firstphase.o: firstphase.c firstphase.h
	$(CCX) -c firstphase.c -o firstphase.o
parser.o: parser.c parser.h
	$(CCX) -c parser.c -o parser.o
hashmap.o: hashmap.c hashmap.h
	$(CCX) -c hashmap.c -o hashmap.o
symbols.o: symbols.c symbols.h
	$(CCX) -c symbols.c -o symbols.o

.PHONY: clean
clean:
	rm -f *.o assembler
