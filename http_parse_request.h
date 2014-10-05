#ifndef HTTPPARSER_H

#define HTTPPARSER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "D_linked_list.h"
#include "common.h"


#define MAXTAGSIZE 100

enum status_code{
	OK=200,
	BAD_REQUEST=400,
	SERVER_ERROR=500,
	NOT_FOUND=404,
	NOT_IMPLEMENTED=505,
};

enum method{
	GET,
	POST,
	HEAD,
	OTHER,
};

enum line_type{
	REQ_LINE=1,
	HEADER=2,
	CONTENT=3,
	COMPLETE=4,
	ERROR=5,
};

typedef struct http_request{
	char *lineptr;
	char *messageptr;
	enum line_type linetype ;
	enum status_code stucode;
    enum method mtdcode ;
    char *uri    ;
    char *version;
  //  char *connection;
    List *header_list;
	int content_length;
	char *message_body;
	int is_CGI;
	int is_open;

	
} request_obj;

typedef struct http_request_header{
	char *name ;
	char *value ;
} http_request_header;

request_obj *create_http_request();
void free_http_request(request_obj *);


void parse_request(request_obj* objp,char* rdbufptr,size_t *rdbufsizep);
void parse_request_line(request_obj* objp,char *line,size_t line_length);
void parse_request_header(request_obj* objp,char *line,size_t line_length);
//void parse_request_message(request_obj* objp,char *line,ssize_t line_length);
void print_request(request_obj* objp);
char *find_token(char *buffer,size_t buffer_size);


#endif