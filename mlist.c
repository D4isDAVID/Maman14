#include "functions.h"

struct mlist {
	struct mlist *next;
	char *name;
	char *content;
};

static struct mlist *head=NULL;

void addNode(char *name,char *content){
	struct mlist *temp;
	if(head==NULL){
		head=(struct mlist *)malloc(sizeof(struct mlist));
		head->name=(char *)malloc(sizeof(char)*strlen(name));
		head->name=strcpy(head->name,name);
		head->content=(char *)malloc(sizeof(char)*strlen(content));
		head->content=strcpy(head->content,content);
		head->next=NULL;
	}
	else{
		temp=(struct mlist *)malloc(sizeof(struct mlist));
		temp->name=(char *)malloc(sizeof(char)*strlen(name));
		temp->name=strcpy(temp->name,name);
		temp->content=(char *)malloc(sizeof(char)*strlen(content));
		temp->content=strcpy(temp->content,content);
		temp->next=head;
		head=temp;
	}
}
char *lookup(char *s){
	struct mlist *np;
	np=head;
	while(np!=NULL){
		if(strcmp(s,np->name)==0)
			return np->content;
		np=np->next;
	} 
	return NULL;

}
