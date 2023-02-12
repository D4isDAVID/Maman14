#include <stdio.h>

void sort(char *data,char *sours,int length, FILE *fp);
int isEmpty(char *str);
int whitespace(char line[], int i);
int notWhiteSpace(char line[], int i, int count);
FILE *preassembler(FILE *fd, char *nameF);
