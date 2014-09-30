#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "D_linked_list.h"
#include "file_loader.h"
#include "http_parse_request.h"

#define MAXLINESIZE 255

typedef struct http_response{	
    char *status_line ;
    List *header_list;
	file_obj *fobjp;
	
	//char *writebuffer;
    
} response_obj;

typedef struct http_header{
	char *name;
	char *value;
} header;

void generate_http_response(response_obj *objp,request_obj *req_objp);
size_t serailize_response(char *buffer,response_obj *objp);
header *create_header(char *name,char *value);
char *get_date();
void checkstatus(response_obj *objp,request_obj *req_objp);

header *create_header(char *name,char *value){
	header *hdrp;
	hdrp=(header*)malloc(sizeof(header));
	hdrp->name=(char*)malloc(strlen(name)+1);
	hdrp->value=(char*)malloc(strlen(value)+1);
	strcpy(hdrp->name,name);
	strcpy(hdrp->value,value);
	return hdrp;
}

void get_time(char* date,time_t rawtime){
	struct tm * ptm;
	ptm = gmtime (&rawtime);
	strftime(date, 256, "%a, %d %b %Y %H:%M:%S %Z", ptm);
	
}

void generate_http_response(response_obj *objp,request_obj *req_objp){
	char *tmpbuffer;
	time_t rawtime;
	
	objp->header_list=create_list();
	
	tmpbuffer=malloc(MAXLINESIZE);
	
	time(&rawtime);
	get_time(tmpbuffer,rawtime);
	
	add_tail(objp->header_list,create_header("Date",tmpbuffer));
	add_tail(objp->header_list,create_header("Server","Liso/1.0"));
	
	
	if(req_objp->stucode==OK){
		
		objp->fobjp=(file_obj*)malloc(sizeof(file_obj));
				
		if(access_file(req_objp,objp->fobjp)==0){
			
			if(req_objp->connection!=NULL&&strcmp(req_objp->connection,"close")==0){
				add_head(objp->header_list,create_header("Connection","close"));
			}
			else {
				add_head(objp->header_list,create_header("Connection","Keep-Alive"));
			}
					
			add_head(objp->header_list,create_header("Content-Type",objp->fobjp->content_type));
			sprintf(tmpbuffer,"%zu",objp->fobjp->file_size);
			add_head(objp->header_list,create_header("Content-Length",tmpbuffer));
			get_time(tmpbuffer,objp->fobjp->last_modified);
			add_tail(objp->header_list,create_header("Last-Modified",tmpbuffer));
		}
						
	}
	
	if(req_objp->stucode!=OK) {
		add_head(objp->header_list,create_header("Connection","close"));
	}
	
	objp->status_line=(char*)malloc(255);
	checkstatus(objp,req_objp);
	
	//generate general header
	//if request err jmp
	//check file
	
	free(tmpbuffer);
}

size_t serailize_response(char *buffer,response_obj *objp){
	Iterator *iterp;
	header *hdrp;
	size_t str_pos;
	
	str_pos=0;
	
	strcpy(buffer,objp->status_line);
	str_pos+=strlen(objp->status_line);
		
	iterp=create_iterator(objp->header_list);
	
	while(iterp->has_next(iterp->currptr)){
		hdrp=(header*)iterp->next(&iterp->currptr);
		sprintf(buffer+str_pos,"%s :%s\r\n",hdrp->name,hdrp->value);
		str_pos+=(strlen(hdrp->name)+strlen(" :\r\n")+strlen(hdrp->value));
	}
	
	sprintf(buffer+str_pos,"\r\n");
	str_pos+=strlen("\r\n");
	
	memcpy(buffer+str_pos,objp->fobjp->content, objp->fobjp->file_size);
	str_pos+=objp->fobjp->file_size;
	
	return str_pos;		
}

void checkstatus(response_obj *objp,request_obj *req_objp){
	char *tmpstr;
	
	switch (req_objp->stucode) {
		case OK:
			tmpstr="200 OK";
			break;
		case NOT_FOUND:
			tmpstr="404 NOT FOUND";
			break;
		case SERVER_ERROR:
			tmpstr="500 INTERNAL SERVER ERROR";
			break;
		case NOT_IMPLEMENTED:
			tmpstr="501 NOT IMPLEMENTED";
			break;
		case BAD_REQUEST:
			tmpstr="400 BAD REQUEST";
			break;
		default:
			tmpstr="503 SERVICE UNAVAILABLE";
			break;
	}
	
	strcpy(objp->status_line, "HTTP/1.1 ");
	strcat(objp->status_line, tmpstr);
	strcat(objp->status_line, "\r\n");
}


int main(int argc, char *argv[]) {
	
	request_obj reqobj;
	response_obj resobj;
	char *buffer=NULL;
	
	root_folder=malloc(20);
	strcpy(root_folder,"/home/qing/www");
	
	request_obj *req_objp=&reqobj;
	response_obj *res_objp=&resobj;
	char raw_req [] ="GET / HTTP/1.1\r\nHost: www.baidu.com\r\nConnection: close\r\nUser-Agent: Paw 2.0.9 (Macintosh; Mac OS X 10.9.5; en_US)\r\nContent-Length: 24\r\n\r\ndfgsdfasdfsadfsdfsdfsdaf";
	//char * rdbufptr=raw_req;
	parse_request(req_objp,raw_req,sizeof(raw_req));
	generate_http_response(res_objp,req_objp);
	buffer=(char*)malloc(strlen(res_objp->status_line)+res_objp->header_list->count*MAXLINESIZE+res_objp->fobjp->file_size);
	serailize_response(buffer,res_objp);
	
	printf("RESULT IS ----------\n%s",buffer);
	return 0;
	
}
