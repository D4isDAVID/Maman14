CCX := gcc -Wall -ansi -pedantic

assembler: assembler.c preassembler.o firstphase.o parser.o hashmap.o linkedlist.o symbols.o strutil.o errutil.o
	$(CCX) assembler.c preassembler.o firstphase.o parser.o hashmap.o linkedlist.o symbols.o strutil.o errutil.o -o assembler
preassembler.o: preassembler.c preassembler.h
	$(CCX) -c preassembler.c -o preassembler.o
firstphase.o: firstphase.c firstphase.h
	$(CCX) -c firstphase.c -o firstphase.o
parser.o: parser.c parser.h
	$(CCX) -c parser.c -o parser.o
hashmap.o: hashmap.c hashmap.h
	$(CCX) -c hashmap.c -o hashmap.o
linkedlist.o: linkedlist.c linkedlist.h
	$(CCX) -c linkedlist.c -o linkedlist.o
symbols.o: symbols.c symbols.h
	$(CCX) -c symbols.c -o symbols.o
strutil.o: strutil.c strutil.h
	$(CCX) -c strutil.c -o strutil.o
errutil.o: errutil.c errutil.h
	$(CCX) -c errutil.c -o errutil.o

.PHONY: clean
clean:
	rm -f *.o assembler
