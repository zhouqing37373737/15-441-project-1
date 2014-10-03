#ifndef LIST_H

#define LIST_H
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

typedef struct Linked_List_Node{
	struct Linked_List_Node *prev;
	struct Linked_List_Node *next;
	void *data;	
} Node;

typedef struct Linked_List{
	Node *header;
	Node *tailer;
	int count;
} List;

typedef struct List_Iterator{
	Node *currptr;
	int (*has_next)(Node *);
	void *(*next)(Node **);
} Iterator;

List *create_list();
Node *create_node();
void free_list(List * listp);
void add_tail(List *listp,void *data);
void add_head(List *listp,void *data);
void remove_node(List *listp,Node *nodep);
void remove_node_index(List *listp, int index);
void remove_node_content(List *listp, void *data);

Iterator *create_iterator(List *listp);
void reset_iterator(List *listp,Iterator *iterp);
void *get_value(Node *nodep);
int has_next(Node *nodep);
void *next(Node **nodepp);

typedef struct http_header{
	char *name;
	char *value;
} header;

header *create_header(char *name,char *value);
header *create_environ_header(char *name,char *value);
void free_header(header *hdrp);
char *get_header_value(List *hdr_list,char *name);


#endif