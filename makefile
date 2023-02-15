CCX := gcc -Wall -ansi -pedantic

assembler: assembler.c preassembler.o parser.o hashmap.o firstpass.o
	$(CCX) assembler.c preassembler.o parser.o hashmap.o firstpass.o -o assembler
preassembler.o: preassembler.c preassembler.h
	$(CCX) -c preassembler.c -o preassembler.o
parser.o: parser.c parser.h
	$(CCX) -c parser.c -o parser.o
hashmap.o: hashmap.c hashmap.h
	$(CCX) -c hashmap.c -o hashmap.o
firstpass.o: firstpass.c firstpass.h
	$(CCX) -c firstpass.c -o firstpass.o

.PHONY: clean
clean:
	rm -f *.o assembler
