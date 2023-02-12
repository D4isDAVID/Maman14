#include <stdio.h>

void sort(char *data,char *sours,int length, FILE *fp);
int skipwhitespace(char line[], int i);
int countnonwhitespace(char line[], int i, int count);
FILE *preassembler(FILE *fd, char *nameF);
