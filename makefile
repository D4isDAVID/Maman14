assembler: assembler.c preassembler.o parser.o mlist.o
	gcc -ansi -Wall -pedantic assembler.c preassembler.o parser.o mlist.o -o assembler
preassembler.o: preassembler.c preassembler.h
	gcc -c -ansi -Wall -pedantic preassembler.c -o preassembler.o
parser.o: parser.c parser.h
	gcc -c -ansi -Wall -pedantic parser.c -o parser.o
mlist.o: mlist.c mlist.h
	gcc -c -ansi -Wall -pedantic mlist.c -o mlist.o
