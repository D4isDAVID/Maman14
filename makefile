assembler: assembler.o preassembler.o mlist.o
	gcc -g -ansi -Wall -pedantic assembler.o -o assembler 
assembler.o: assembler.c
	gcc -g -c -ansi -Wall -pedantic assembler.c -o assembler.o
preassembler.o: preassembler.c
	gcc -g -c -ansi -Wall -pedantic preassembler.c -o preassembler.o
mlist.o: mlist.c
	gcc -g -c -ansi -Wall -pedantic mlist.c -o mlist.o	
