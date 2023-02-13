/* preassembler code */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "mlist.h"
#include "preassembler.h"

void sort(char *data, char *sours, int length, FILE *am) {
	int lines = 0, i = 0, ind;
	char *space;
	while (i < strlen(data)) {
		if (lines != 0 && length != 0) {
			space = (char *) malloc(sizeof(char) * (length+1));
			strncpy(space, sours, length);
			for (ind = 0; ind < length; ind++)
				if (!isspace(space[ind]))
					space[ind] = ' ';
			space[length] = '\0';
			fputs(space, am);
			free(space);
		}
		while (data[i] != '\n') {
			fputc(data[i], am);
			i++;
		}
		fputc(data[i], am);
		i++;
		lines++;
	}
}

int skipwhitespace(char line[], int i) {
	for (; line[i] == ' ' || line[i] == '\t'; i++)
	    ;
    return i;
}

int countnonwhitespace(char line[], int i, int count) {
	for (; !isspace(line[i]); i++, count++)
	    ;
    return count;	
}

FILE *preassembler(FILE *as, char *filename) {
	int i = 0, flag = 0, count, read = 0, countline, width = 0;
	fpos_t end;
	char line[82], *name, *data;
	FILE *am;
	strcat(filename, ".am");
	am = fopen(filename, "w");
	while (fgets(line, 82, as) != NULL) {
		i = skipwhitespace(line, 0);
		count = countnonwhitespace(line, i, 0);
		if (line[i+count-1] == ':' && flag == 1) {
			i += count;
			i = skipwhitespace(line, i);
			count = countnonwhitespace(line, i, 0);
		}
		if (strncmp(&line[i], "mcr ", count+1) == 0) {
			i += count;
			i = skipwhitespace(line, i);
			if (line[i] == '\n') {
				fputs(line, am);
				flag = 0;
				continue;
			}
			count = 0;
			count = countnonwhitespace(line,i,0);
			name = (char *) malloc(sizeof(char) * (count+1));
			strncpy(name, &line[i], count);
			name[count] = '\0';
			i += count;
			i = skipwhitespace(line,i);
			if (line[i] != '\n' || count == 0) {
				fputs(line, am);
				free(name);
				flag=0;
			} else {
				fgetpos(as,&end);
				flag=1;	
				countline=0;
			}
		} else if (flag == 1 && read < 2) {
			if (read == 0)
				countline++;
			if (strncmp(&line[i], "endmcr", count) == 0) {
				i += count;
				i = skipwhitespace(line, i);
				if (line[i] == '\n') {
					read++;
					if (read != 2)
						fsetpos(as, &end);
					if (read == 1)
						data = (char *) malloc(sizeof(char) * (countline*81 + 1));
					else if (read == 2) {
						mlist_add(name, data);
						free(name);
						free(data);
						read=0;
						flag=0;
					}
				} else if (read != 0)
					strcat(data, &line[count]);
			} else if (read != 0)
				strcat(data, &line[i]);
		} else {
			i = 0;
			width = 0;			
			while (i < strlen(line)) {
				while (isspace(line[i]) && i < strlen(line)-1) {
					fputc(line[i], am);
					i++;
				}
				count = countnonwhitespace(line, i, 0);
				name = (char *) malloc(sizeof(char) * (count+1));
				strncpy(name, &line[i], count);
				name[count] = '\0';
				i += count;
				if (mlist_lookup(name) != NULL) {
					sort(mlist_lookup(name), line, i-count-width, am);	
					width = i;
				} else if (name != NULL)
					fputs(name, am);
				if (i == strlen(line)-1) {
					if (mlist_lookup(name) != NULL)
						i++;
					else {
						fputc(line[i], am);
						i++;
					}
				}
				free(name);
			}
		}
	}
	mlist_clear();
	fclose(am);
	am = fopen(filename, "r");
	return am;
}
