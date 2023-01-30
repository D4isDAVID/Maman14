#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "preassembler.h"
#include "mlist.h"

int whitespace(char line[], int i, FILE *fp) {
    for (; line[i] == ' ' || line[i] == '\t'; i++)
        ;
    return i;
}

int notWhiteSpace(char line[], int i, int count) {
	for (; !(line[i] == ' ' || line[i] == '\t'); i++)
        	count++;
    	return count;
}

FILE *preassembler(FILE *fd, char *name){
	int i=0,flag=1,count;
	char line[82],*name;
	FILE *fp;
	strcat(name,".am");	
	fp=fopen(name,"w+");
	while(fgets(line,82,fd)!=NULL){
		i=whitespace(line,0,fd);
		if(strncmp(&line[i],"mcr ",4)==0){
			count=0;
			i=whitespace(line,i,fd);
			count=notWhiteSpace(&line[i],i,count);
			name=(char *)malloc((sizeof(char)*count);
			strncpy(name,&line[i],count);
			i+=count;
			i=whitespace(line,i,fd);
			if(line[i]!='\n'){
				fputs(line,fp);
				free(name);
			}
			else
				flag=1;	
		}
		else
			fputc(line[i],fp);
		if(flag==1){
			i=whitespace(line,i,fd);			
			if(strncmp(&line[i],"endmcr",6)==0){	
				addNode(name,data);
				free(name);
				free(data);
				flag=0;
			}
			else
					
		
		}	
	}
	return fp;
}
