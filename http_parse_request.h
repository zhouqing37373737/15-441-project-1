#ifndef HTTPPARSER_H

#define HTTPPARSER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

typedef struct http_request{
	int linetype ;
	enum status_code stucode;
    enum method mtdcode ;
    char *uri    ;
    char *version;
    char *connection;
	int content_length;
	char *message_body;
	
} request_obj;

typedef struct http_request_header{
	char *name ;
	char *value ;
} http_request_header;



void parse_request(request_obj* objp,char* rdbufptr,ssize_t rdbufsize);
void parse_request_line(request_obj* objp,char *line,ssize_t line_length);
void parse_request_header(request_obj* objp,char *line,ssize_t line_length);
void parse_request_message(request_obj* objp,char *line,ssize_t line_length);

#endif