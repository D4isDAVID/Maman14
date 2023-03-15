#include "secondphase.h"
#include <string.h>
#include "parser.h"
#include "strutil.h"
#include "errutil.h"


void convert(unsigned int num, FILE *ob)
{
	int i;
	for (i = 1 << 13; i > 0; i /= 2)
		fputc((num & i) ? '.' : '/', ob);
}

int secondphase(FILE *ob, char *filename, struct listnode *instructions, struct listnode *data, struct hashmap *labels, struct hashmap *labelattributes){
	struct listnode *instructionptr=instructions;
	struct hashnode *attributesptr;
	int *labelattribute,linecount=100, haserrors=0,isdata = 0, hasext=0, hasent=0, i;
	char *labelname;
	FILE *ent,*ext;
	instruction *inst;

	strcat(filename, ".ent");
	ent = open(filename, "w");
	replaceextension(filename, ".ext");
	ext = open(filename, "w");
	replaceextension(filename, "");
	while(instructionptr != NULL){
		fprintf(ob, "0%d\t", linecount);
		if(!isdata){
			labelname = NULL;
			inst=instructionptr->value;
			if(inst->islabel){
				labelname = strdupl((char *)instructionptr->value);
				labelattribute = hashmap_getint(labelattributes, labelname);
				if((hashmap_getint(labels, labelname)==NULL) || !(*labelattribute & LABEL_EXTERNAL)){
					haserrors=1;
					printerr(filename, linecount, ERROR_LABELNOTDEFINED, labelname);
				}
				if(*labelattribute & LABEL_EXTERNAL){
					convert(ENC_EXTERNAL, ob);
					hasext=1;
					fprintf(ext,"%s\t%d\n", labelname, linecount);
				}
				else
					convert(encodelabel(*hashmap_getint(labels, labelname))->field, ob);
			}else
				convert(((word *)inst->value)->field, ob);
			if(!instructionptr->next){
				isdata=1;
				instructionptr->next=data;
			}				
		}else
			convert(((word *)instructionptr->value)->field, ob);
		fputc('\n', ob);
		linecount++;
		instructionptr=instructionptr->next;
	}
	for (i = 0; i < HASHMAP_CAPACITY; i++) {
		attributesptr = (labelattributes)->tab[i];
		while (attributesptr != NULL) {
			labelattribute = (int *) attributesptr->value;
			if (labelattribute != NULL && *labelattribute & LABEL_ENTRY){
				if(*labelattribute & (LABEL_DATA | LABEL_INSTRUCTION)){
					hasent=1;
					fprintf(ent,"%s\t%d\n", attributesptr->key, *hashmap_getint(labels,attributesptr->key));
				}else{
					haserrors = 1;
					printerr(filename, linecount, ERROR_LABELNOTDEFINED, attributesptr->key);
				}
			}				
			attributesptr = attributesptr->next;
		}
	}
	fclose(ob);
	fclose(ent);
	fclose(ext);
	if (!hasent) {
		strcat(filename, ".ent");
		remove(filename);
		replaceextension(filename, "");
	}
	if (!hasext) {
		strcat(filename, ".ext");
		remove(filename);
		replaceextension(filename, "");
	}
	return haserrors;
}
