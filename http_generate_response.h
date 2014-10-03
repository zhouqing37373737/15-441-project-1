#ifndef HTTPRES_H

#define HTTPRES_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "D_linked_list.h"
#include "file_loader.h"
#include "http_parse_request.h"
#include "common.h"



typedef struct http_response{	
    char *status_line ;
    List *header_list;
	file_obj *fobjp;
	
	//char *writebuffer;
    
} response_obj;


response_obj *create_http_response();
void free_http_response(response_obj *res_objp);

void build_http_response(response_obj *objp,request_obj *req_objp);
size_t serailize_http_response(char *buffer,response_obj *objp);

void get_time(char* date,time_t rawtime);
void checkstatus(response_obj *objp,request_obj *req_objp);

#endif
