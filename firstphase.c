/* first assembler phase */
#include "firstphase.h"

#include <stdarg.h>
#include <string.h>
#include "parser.h"
#include "strutil.h"
			
void printwarn(char *, int, int, char *, ...);
void printerr(char *, int, int, char *, ...);
int isvaliddata(char *filename, int linecount, char *line,int i, int *haserrors, struct hashmap **data, enum symbol opcode);

int isvaliddata(char *filename, int linecount, char *line,int i, int *haserrors, struct hashmap **data, enum symbol opcode){
	int count,
	index,
	datacount=0,
	flag;
	int num;

	skipwhitespace(line, &i);
	count=countnonwhitespace(line,&i);
	if(count==0){
		printerr(filename, linecount, i-count, "instruction with no parameters");
		*haserrors=1;
		return 0;
	}
	if(opcode == DIRECTIVE_STRING){
		if(line[i]!='\"' || line[i-count]!= '\"'){
			printerr(filename, linecount, i-count, "instruction with no legal quote marks");
			*haserrors=1;
			return 0;	
		}
		index=i;
		skipwhitespace(line, &i);
		if(line[i]!='\n'){
			printerr(filename, linecount, i-count, "line isn't terminated after instruction ");
			*haserrors=1;
			return 0;
		}
		count-=2;/* the two quotation marks are included in count*/
		datacount=count+1;
		for(;count;count--)
			hashmap_setint(*data,"a",(int)line[index+count-1]);
		hashmap_setint(*data,"a",*(int *)NULL);	
		return datacount;			
	} else{
		i-=count;
		while(line[i]!='\n'){
			flag=0;
			count=0;
			while(line[i]!=','){
				if(!('0'<=line[i+count] && line[i+count]<='9')){
					if(count==0 && line[i+count]=='-')
						flag=1;		
				}				
				count++;
			}
			if(count==0){
				printerr(filename, linecount, i-count, "no number");
				*haserrors=1;
				return 0;	
			}
			if(flag && count==1){
				printerr(filename, linecount, i-count, "illegal number for data entry");
				*haserrors=1;
				return 0;
			}
			i+=count;
			num = atoi(strndupl(&line[i-count], count));
			hashmap_setint(*data,"a",num);	
			datacount++;
			skipwhitespace(line, &i);
			if(line[i]!='\n' || line[i]!=','){
				printerr(filename, linecount, i-count, "no legal seperation between numbers");
				*haserrors=1;
				return 0;	
			}
			if(line[i]==','){
				i++;
				skipwhitespace(line, &i);
				if(line[i]=='\n'){
					printerr(filename, linecount, i-count, "terminated without number agter comma");
					*haserrors=1;
					return datacount;	
				}
			}
		}
		return datacount;
	}
}


void prettyprint(char *prefix, char *filename, int line, int c, char *s, va_list args)
{
	fprintf(stderr, "%s: %s.as:%d:%d - ", prefix, filename, line, c);
	vfprintf(stderr, s, args);
	fprintf(stderr, "\n");
}

void printwarn(char *filename, int line, int c, char *s, ...)
{
	va_list args;
	va_start(args, s);
	prettyprint("warning", filename, line, c, s, args);
	va_end(args);
}

void printerr(char *filename, int line, int c, char *s, ...)
{
	va_list args;
	va_start(args, s);
	prettyprint("error", filename, line, c, s, args);
	va_end(args);
}


/* phase entry point */
FILE *firstphase(FILE *am, char *filename, struct hashmap **labels, struct hashmap **entext, struct hashmap **data)
{
	struct hashmap *data_labels=hashmap_new();
	char line[MAX_LINE_LENGTH + 2],
		*labelname,
		*opname;
	int i,
		count,
		labeldef,
		linecount = 0,
		datacount = 0,
		instructioncount = 0,
		haserrors = 0;
	enum symbol opcode;
	FILE *ob;
	strcat(filename, ".ob");
	ob = fopen(filename, "w");
	*labels = hashmap_new();
	*data=hashmap_new();
	*entext = hashmap_new();
	replaceextension(filename, ""); /* we need the filename without .ob extension to print in error messages */
	while (fgets(line, MAX_LINE_LENGTH + 2, am)) {
		linecount++;
		if (line[0] == ';')
			continue;
		i = 0;
		labeldef = 0;
		skipwhitespace(line, &i);
		count = countnonwhitespace(line, &i);
		if (line[i-1] == ':') {
			labelname = strndupl(&line[i-count], count-1);
			labeldef = 1;
			skipwhitespace(line, &i);
			count = countnonwhitespace(line, &i);
			if (count == 0) {
				haserrors = 1;
				printerr(filename, linecount, i-count, "label defined without instruction");
			} 
			
		}
		if(count == 0)
			continue;
		opname = strndupl(&line[i-count], count);
		opcode = symbols_get(opname);
		if(isdirective(opname)){
			if(isdatadirective(opcode)){
				if(labeldef){
					if(!getnode(*labels,labelname) && !getnode(*entext,labelname)){
						hashmap_setint(*labels,labelname,datacount);	
						hashmap_setint(data_labels,labelname,0);	
					}
					else{
						haserrors = 1;
						printerr(filename, linecount, i-count, "pre-existing label (%s)",labelname);
					}
				}
				datacount+=isvaliddata(filename,linecount,line,i,&haserrors,data,opcode);
				continue;

			} else{
				if(labeldef)
					printwarn(filename, linecount, i-count, "useless label definition");
				if(opcode == DIRECTIVE_ENTRY)
					continue;
				skipwhitespace(line, &i);
				count=countnonwhitespace(line,&i);
				labelname = strndupl(&line[i-count], count);
				if(!getnode(*labels,labelname) && !getnode(*entext,labelname)){
					skipwhitespace(line,&i);
					if(line[i]=='\n')
						hashmap_setint(*entext,labelname,0);
					else{
						haserrors=1;
						printerr(filename,linecount,i-count,"line isn't terminated after instruction");
					}					
				}
				else{
					haserrors = 1;
					printerr(filename, linecount, i-count, "pre-existing label (%s)",labelname);
				}
				continue;
			}					
		} else{
			if(labeldef){
				if(!getnode(*labels,labelname) && !getnode(*entext,labelname))
					hashmap_setint(*labels,labelname,instructioncount);
				else{
					haserrors = 1;
					printerr(filename, linecount, i-count, "pre-existing label (%s)",labelname);
				}
			}
			if(opcode==UNKNOWN_SYMBOL){
				haserrors = 1;
				printerr(filename, linecount, i-count, "illegal operation name (%s)",opname);	
			}
			/*operation memory coding */	
			continue;
		}	
	}
	fclose(ob);
	ob = NULL;
	if (!haserrors && (instructioncount > 0 || datacount > 0)) {
		strcat(filename, ".ob");
		ob = fopen(filename, "r");
		replaceextension(filename, "");
	}
	return ob;
}
