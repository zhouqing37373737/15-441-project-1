
#include "D_linked_list.h"

void *get_value(Node *nodep){
	if(nodep==NULL){
		return NULL;
	}
	//printf("DATA: %d\n",*(int *)(nodep->data));
	return nodep->data;
}

int has_next(Node *nodep){
	if(nodep==NULL){
		return 0;
	}
	else
		return 1;
}

void *next(Node **nodepp){
	void *data=get_value(*nodepp);
	*nodepp=(*nodepp)->next;
	return data;
}

Iterator *create_iterator(List *listp){
	if(listp==NULL){
		return NULL;
	}
	Iterator *iter;
	iter=(Iterator *)malloc(sizeof(Iterator));
	iter->currptr=listp->header;
	iter->has_next=&has_next;
	iter->next=&next;
	return iter;
}

List *create_list(){
	List* listp;
	listp=(List *)malloc(sizeof(List));
	listp->header=NULL;
	listp->tailer=NULL;
	listp->count=0;
	
	return listp;
}

void free_list(List * listp){
	
	if(listp==NULL){
		return;
	}
	
	Node *currptr;
	Node *tmp;
	
	currptr=listp->header;
	while(currptr!=NULL){
		
		tmp=currptr;
		currptr=currptr->next;
		//free(tmp->data);
		free(tmp);
		
	}

	free(listp);
	
}

Node *create_node(){
	Node *nodep;
	nodep=(Node *)malloc(sizeof(Node));
	
	nodep->prev=NULL;
	nodep->next=NULL;
	nodep->data=NULL;
	return nodep;
}

void add_head(List *listp,void *data){
	
	Node *nodep;
	nodep=create_node();
	nodep->data=data;
	if(listp->header==NULL){
		listp->header=nodep;
		listp->tailer=nodep;
	}
	else{
		listp->header->prev=nodep;
		nodep->next=listp->header;
			
		listp->header=nodep;
	}
	
	listp->count+=1;
		
}

void add_tail(List *listp,void *data){
	
	Node *nodep;
	nodep=create_node();
	nodep->data=data;
	
	if(listp->tailer==NULL){
		listp->header=nodep;
		listp->tailer=nodep;
	}
	else{
		listp->tailer->next=nodep;
		nodep->prev=listp->tailer;
		
		listp->tailer=nodep;
	}
	listp->count+=1;
}

void remove_node_index(List *listp, int index){
	
	Node *currptr;
	int currindex;
	
	currptr=listp->header;
	currindex=0;
	
	while(currptr!=NULL&&currindex<index){
		currptr=currptr->next;
		currindex+=1;
	}
	
	//if null just return,included
	return remove_node(listp,currptr);
}

void remove_node(List *listp,Node *nodep){
	
	if( nodep== NULL){
		return;
	}	
	
	if( listp->header == NULL && listp->tailer==NULL){
		return;
	}
	
	if( listp->header == nodep && nodep->prev==NULL){
		listp->header=nodep->next;
	}
	else{
		nodep->prev->next=nodep->next;
	}
	
	if( listp->tailer == nodep && nodep->next==NULL){
		listp->tailer = nodep->prev;
	}
	else{
		nodep->next->prev=nodep->prev;
	}
	listp->count-=1;
	free(nodep);
}


/*
int main(int argc, char *argv[]) {
	List * listp=create_list();
	int i,j,k;
	i=1;
	add_tail(listp,(void*)&i);
	j=2;
	add_tail(listp,(void*)&j);
	k=3;
	add_tail(listp,(void*)&k);
	
	//printf("DATA: %d\n",*(int *)(get_value(listp->tailer)));
	
	
	Iterator *iterp=create_iterator(listp);
	while(iterp->has_next(iterp->currptr)){
		printf("DATA: %d\n",*(int*)iterp->next(&iterp->currptr));
	}
	
	free_list(listp);
}
*/
