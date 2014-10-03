
#include "http_parse_request.h"
#include "http_generate_response.h"
#include "logger.h"

request_obj *create_http_request(){
	request_obj * req_objp;
	
	req_objp=(request_obj *)malloc(sizeof(request_obj));
	
	req_objp->linetype=REQ_LINE;
	req_objp->stucode=OK;
	req_objp->content_length=0;
	//req_objp->connection=NULL;
	req_objp->uri=NULL;
	req_objp->version=NULL;
	req_objp->message_body=NULL;
	req_objp->is_CGI=0;
	req_objp->header_list=create_list();
	
	return req_objp;

}

void free_http_request(request_obj *req_objp){
	//free(req_objp->connection);
	free(req_objp->uri);
	free(req_objp->version);
	free(req_objp->message_body);
	free(req_objp);
}


void parse_request(request_obj* objp,char* rdbufptr,ssize_t rdbufsize){
	
	char *line;
    char *line_ptr;
	ssize_t line_length;
	char *tmp;

	// parseHeader
	while(objp->linetype<COMPLETE && objp->stucode==OK && !objp->is_CGI){

		if(objp->linetype<CONTENT){
			if((line_ptr=strstr(rdbufptr,"\r\n"))==NULL){
				objp->stucode=BAD_REQUEST;
				continue;
			}
			
			line_length=line_ptr-rdbufptr;
					
			if(line_length==0){
				rdbufptr=line_ptr+2;
				if(objp->mtdcode==POST){
					objp->linetype=CONTENT;
				}
				else{
					objp->linetype=COMPLETE;
				}
				continue;
			}
			
		}
		else if(objp->linetype==CONTENT){
			tmp=get_header_value(objp->header_list,"Content-Length");

			if(tmp==NULL){
				objp->stucode=BAD_REQUEST;
				continue;
			}
			//use strtol
			line_length=atoi(tmp);
			objp->content_length=line_length;
		}
		
		line=(char *) malloc(line_length+1);
		line=strncpy(line,rdbufptr,line_length);
		*(line+line_length)='\0';
		
		switch (objp->linetype) {
			case REQ_LINE :
				parse_request_line(objp,line,line_length);
				objp->linetype=HEADER;
				break;
			case HEADER :
				parse_request_header(objp,line,line_length);
				break;	
			case CONTENT :
				parse_request_message(objp,line,line_length);
				objp->linetype=COMPLETE;
				break;			
			default:
				break;
		}
		//sscanf(line,"%s:%s",header.)
		
		//parse_requestline(objp,line);
		
		rdbufptr=line_ptr+2;
		free(line);
	}

}

void parse_request_line(request_obj* objp,char *line,ssize_t line_length){
	//printf("REQLINE:==>%s\n",line);
	int vdigit1,vdigit2;
	char * method;
	enum method mtdcode;
	char * uri;
	char * version;
	char *tmp;
	
	
	method= (void *)malloc(line_length);
	uri = (void *)malloc(line_length);
	version = (void *)malloc(line_length);
	
	if(sscanf(line,"%s %s HTTP/%d.%d",method,uri,&vdigit1,&vdigit2)!=4){
		printf("ERROR PARSING REQUESTLINE");
		objp->stucode=BAD_REQUEST;
		return;
	};
	
	sprintf(version,"HTTP/%d.%d",vdigit1,vdigit2);
	if(strcmp("GET",method)==0){
		mtdcode=GET;
	}
	else if (strcmp("POST",method)==0) {
		mtdcode=POST;
	}
	else if (strcmp("HEAD",method)==0) {
		mtdcode=HEAD;	
	}
	else{
		mtdcode=OTHER;
		objp->stucode=NOT_IMPLEMENTED;
		return;
	}
	
	if(uri[0]!='/'){
		tmp=(char *)malloc(strlen(uri)+2);
		tmp="/";
		strcat(tmp,uri);
		free(uri);
		uri=tmp;
	}

	objp->mtdcode=mtdcode;
	objp->uri=uri;
	objp->version=version;
	
	if(strstr(uri,"/cgi")==uri){
		objp->is_CGI=1;
	}
	
	
}
void parse_request_header(request_obj* objp,char *line,ssize_t line_length){
	
	//printf("REQHEADER:==>%s\n",line);
	//http_request_header header;
	char *name,*value;
	
	name = (void *)malloc(line_length);
	value = (void *)malloc(line_length);
	
	if(sscanf(line,"%[a-zA-Z0-9-]: %s",name,value)!=2){
		printf("ERROR PARSING HEADER: %s\n",line);
		objp->stucode=BAD_REQUEST;
		return;
	}

	add_head(objp->header_list,create_header(name,value));

	/*if (strcmp(name, "Content-Length") == 0){
		//add sanity check
		objp->content_length=atoi(value);
	}
	else if (strcmp(name, "Connection") == 0) {
		//add sanity check
		objp->connection=value;
	}
	*/
	
	free(name);
	free(value);
	//printf("HEADERKEY:%s\nHEADERVAL:%s\n\n",name,value);
}
 
void parse_request_message(request_obj* objp,char *line,ssize_t line_length){
	objp->message_body=(char *)malloc(line_length+1);
	memcpy(objp->message_body,line,line_length);
	objp->message_body[line_length]='\0';
	//printf("REQMESSAGE1:%s\n",line);
}


void print_request(request_obj* objp){

	Iterator *iterp;
	header *hdrp;

	printf("STATUS: %d\nMETHOD:%d \nURI:%s \nHTTPVERSION:%s\n",objp->stucode,objp->mtdcode,objp->uri,objp->version);

	iterp=create_iterator(objp->header_list);
	printf("HEADERS:\n");
	while(iterp->has_next(iterp->currptr)){
		hdrp=(header *)iterp->next(&iterp->currptr);
		printf("%s: %s\n",hdrp->name,hdrp->value);
	}

	printf("\nREQMESSAGE:%s\n",objp->message_body);

	
}

/*
int main(int argc, char *argv[]) {

	request_obj *objp=create_http_request();

	char raw_req [] ="POST / HTTP/1.1\r\nHost: www.baidu.com\r\nConnection: close\r\nUser-Agent: Paw 2.0.9 (Macintosh; Mac OS X 10.9.5; en_US)\r\nContent-Length: 38\r\n\r\nthis is a test for content correctness";
	char * rdbufptr=raw_req;
	parse_request(objp,raw_req,sizeof(raw_req));

	print_request(objp);
		
	//printf("STATUS: %d\n METHOD:%d \nURI:%s \nHTTPVERSION:%s\n\nREQMESSAGE:%s\n",objp->stucode,objp->mtdcode,objp->uri,objp->version,objp->message_body);
	
}
*/
