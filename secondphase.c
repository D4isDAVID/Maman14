#include "secondphase.h"
#include <string.h>
#include "parser.h"
#include "strutil.h"
#include "errutil.h"

void convert(unsigned int num, FILE *ob)
{
	int i;
	for (i = 1 << 13; i > 0; i /= 2)
		fputc((num & i) ? '/' : '.', ob);
}

int secondphase(FILE *ob, char *filename, struct listnode *instructions, struct listnode *data, struct hashmap *labels, struct hashmap *labelattributes)
{
	struct listnode *listptr = instructions;
	struct hashnode *attributesptr;
	int *labelattribute,
		*labelvalue,
		linecount = 100,
		haserrors = 0,
		isdata = 0,
		hasext = 0,
		hasent = 0,
		i;
	char *labelname;
	FILE *ent, *ext;
	instruction *inst;

	strcat(filename, ".ent");
	ent = open(filename, "w");
	replaceextension(filename, ".ext");
	ext = open(filename, "w");
	replaceextension(filename, "");

	while (listptr != NULL) {
		fprintf(ob, "0%d\t", linecount);
		if (!isdata) {
			labelname = NULL;
			inst = listptr->value;
			if(inst->islabel){
				labelname = (char *) inst->value;
				labelattribute = hashmap_getint(labelattributes, labelname);
				labelvalue = hashmap_getint(labels, labelname);
				if (labelvalue == NULL && !(*labelattribute & LABEL_EXTERNAL)) {
					haserrors = 1;
					printerr(filename, linecount, ERROR_LABELNOTDEFINED, labelname);
				}
				if(*labelattribute & LABEL_EXTERNAL){
					convert(ENC_EXTERNAL, ob);
					hasext=1;
					fprintf(ext,"%s\t%d\n", labelname, linecount);
				}
				else
					convert(encodelabel(*labelvalue)->field, ob);
			}else
				convert(((word *)inst->value)->field, ob);
			if(!listptr->next){
				isdata=1;
				listptr = linkedlist_newnode("");
				listptr->next=data;
			}
		}else
			convert(((word *)listptr->value)->field, ob);
		fputc('\n', ob);
		linecount++;
		listptr=listptr->next;
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