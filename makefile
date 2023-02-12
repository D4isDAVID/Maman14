assembler: assembler.c preassembler.o mlist.o
	gcc -ansi -Wall -pedantic assembler.c preassembler.o mlist.o -o assembler
preassembler.o: preassembler.c preassembler.h
	gcc -c -ansi -Wall -pedantic preassembler.c -o preassembler.o
mlist.o: mlist.c mlist.h
	gcc -c -ansi -Wall -pedantic mlist.c -o mlist.o
