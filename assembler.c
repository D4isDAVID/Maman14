#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "preassembler.c

int main(int argc, char**argv){
	int i;
	FILE **fd,*temp;
	fd=(FILE **)malloc(sizeof(FILE *)*(argc-1));
	for(i=0; i+1<argc; i++){
		temp=fopen(argv[i+1],"r");
		if(temp==NULL){
			fprintf(stderr,"error in opening %s.as\n",argv[i+1]);
			exit(1);
		}
		fd[i]=preassembler(temp,argv[i+1]);
		close(temp);
	}
	return 0;
}
