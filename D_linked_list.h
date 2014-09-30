#ifndef LIST_H

#define LIST_H
#include <stdio.h>
#include <stdlib.h>

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

Iterator *create_iterator(List *listp);
void *get_value(Node *nodep);
int has_next(Node *nodep);
void *next(Node **nodepp);

#endif