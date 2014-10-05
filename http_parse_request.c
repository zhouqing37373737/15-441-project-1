
#include "http_parse_request.h"
#include "http_generate_response.h"
#include "logger.h"

request_obj *create_http_request(){
	request_obj * req_objp;
	
	req_objp=(request_obj *)malloc(sizeof(request_obj));
	req_objp->lineptr=NULL;
	req_objp->messageptr=NULL;
	req_objp->linetype=REQ_LINE;
	req_objp->stucode=OK;
	req_objp->content_length=0;
	//req_objp->connection=NULL;
	req_objp->uri=NULL;
	req_objp->version=NULL;
	req_objp->message_body=NULL;
	req_objp->is_CGI=0;
	req_objp->is_open=0;
	req_objp->header_list=create_list();
	
	return req_objp;

}

void free_http_request(request_obj *req_objp){
	//free(req_objp->connection);
	if(req_objp->uri!=NULL){
		free(req_objp->uri);
	}
	if(req_objp->version!=NULL){
	        free(req_objp->version);
	}
	if(req_objp->message_body!=NULL){
	        free(req_objp->message_body);
	}
	//free header list
	free(req_objp);
}


void parse_request(request_obj* objp,char* rdbufptr,size_t *rdbufsizep){
	
	char *line;
    char *line_ptr;
    char *line_str;
	size_t line_length;
	size_t rdbufsize;
	char *tmp;

	rdbufsize=*rdbufsizep;
	line_ptr=objp->lineptr;

	if(line_ptr==NULL){
		line_ptr=rdbufptr;
	}

	line_str=line_ptr;
	
	// parseHeader
	while(objp->linetype<COMPLETE){
		line_str=line_ptr;
		if(objp->linetype<CONTENT){
			logger(INFO,"BEFORE LINE STRSTR\n");
			
			if((line_ptr=find_token(line_str,rdbufsize-(line_str-rdbufptr)))==NULL){
				//objp->stucode=BAD_REQUEST;
				return;
			}
			
			line_length=line_ptr-line_str;
					
			if(line_length==strlen("\r\n")){
				//line_ptr+=strlen("\r\n");
				//rdbufptr=line_ptr+2;
				tmp=get_header_value(objp->header_list,"Host");
				if(tmp==NULL){
					objp->stucode=BAD_REQUEST;
					objp->linetype=ERROR;
					break;
				}

				tmp=get_header_value(objp->header_list,"Connection");
				if(tmp!=NULL && strcmp(tmp,"close")==0){
					objp->is_open=0;
				}
				else{
					objp->is_open=1;
				}

				if(objp->mtdcode==POST){
					tmp=get_header_value(objp->header_list,"Content-Length");
					if(tmp==NULL){
						objp->stucode=BAD_REQUEST;
						objp->linetype=ERROR;
						break;
					}
					objp->linetype=CONTENT;
				}
				else{
					objp->linetype=COMPLETE;
				}
				continue;
			}

			logger(INFO,"LINE LEN IS %zu\n",line_length);	
			line=(char *) malloc(line_length+2);
			line=memcpy(line,line_str,line_length);
			*(line+line_length)='\0';
			logger(INFO,"LINE: %s(%zu)\n",line,line_length);	
			
		}

		if(objp->linetype==CONTENT){

			if(objp->message_body==NULL){

				line_length=atoi(tmp);
				objp->content_length=line_length;
				objp->message_body=(char *) malloc(line_length+2);
				objp->messageptr=objp->message_body;
				//line_ptr=objp->lineptr;
			}
			size_t bufcontsize=rdbufsize-(line_ptr-rdbufptr);
			size_t readcontsize=objp->messageptr-objp->message_body;


			if(bufcontsize<objp->content_length-readcontsize){
				line_length=bufcontsize;
				memcpy(objp->messageptr,line_ptr,line_length);
				objp->messageptr+=line_length;
				line_ptr+=line_length;
				break;
			}
			else{
				line_length=objp->content_length-readcontsize;
				memcpy(objp->messageptr,line_ptr,line_length);
				objp->messageptr+=line_length;
				line_ptr+=line_length;

				objp->message_body[objp->content_length]='\0';
			}	
			//use strtol

		}

		switch (objp->linetype) {
			case REQ_LINE :
				parse_request_line(objp,line,line_length);
				objp->linetype=HEADER;
				break;
			case HEADER :
				parse_request_header(objp,line,line_length);
				break;	
			case CONTENT :
				//parse_request_message(objp,line,line_length);
				objp->linetype=COMPLETE;
				break;			
			default:
				break;
		}

		//sscanf(line,"%s:%s",header.)
		
		//parse_requestline(objp,line);
		
		//rdbufptr=line_ptr+2;
		free(line);
	}

	if(objp->linetype!=ERROR){
		logger(INFO,"MEMSET %zu\n",line_ptr-rdbufptr);
	//copy line ptr and move buffer
		objp->lineptr=line_ptr;
		memset(rdbufptr,0,line_ptr-rdbufptr);

		if(line_ptr-rdbufptr<rdbufsize){
			memmove(rdbufptr,line_ptr,line_ptr-rdbufptr);
		}

		*rdbufsizep=rdbufsize-(line_ptr-rdbufptr);
	}


}

void parse_request_line(request_obj* objp,char *line,size_t line_length){
	//printf("REQLINE:==>%s\n",line);
	int vdigit1,vdigit2;
	char * method;
	enum method mtdcode;
	char * uri;
	char * version;
	char *tmp;
	
	logger(INFO,"ENTER PARSE REQ LINE\n");	
	method= (void *)malloc(line_length);
	uri = (void *)malloc(line_length);
	version = (void *)malloc(line_length);
	
	if(sscanf(line,"%s %s HTTP/%d.%d",method,uri,&vdigit1,&vdigit2)!=4){
		logger(INFO,"ERROR PARSING REQUESTLINE");
		objp->stucode=BAD_REQUEST;
		objp->linetype=ERROR;
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
		objp->linetype=ERROR;
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
	logger(INFO,"BEFORE CGI STRSTR\n");	
	if(strstr(uri,"/cgi")==uri){
		objp->is_CGI=1;
	}
	
	
}
void parse_request_header(request_obj* objp,char *line,size_t line_length){
	logger(INFO,"ENTER PARSE HDR\n");	
	//printf("REQHEADER:==>%s\n",line);
	//http_request_header header;
	char *name,*value;
	
	name = (void *)malloc(line_length);
	value = (void *)malloc(line_length);
	
	if(sscanf(line,"%[a-zA-Z0-9-]: %s",name,value)!=2){
		logger(INFO,"ERROR PARSING HEADER: %s\n",line);
		objp->stucode=BAD_REQUEST;
		objp->linetype=ERROR;
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
 
 /*
void parse_request_message(request_obj* objp,char *line,ssize_t line_length){
	objp->message_body=(char *)malloc(line_length+1);
	memcpy(objp->message_body,line,line_length);
	objp->message_body[line_length]='\0';
	//printf("REQMESSAGE1:%s\n",line);
}

*/
void print_request(request_obj* objp){

	Iterator *iterp;
	header *hdrp;

	logger(INFO,"STATUS: %d\nMETHOD:%d \nURI:%s \nHTTPVERSION:%s\n",objp->stucode,objp->mtdcode,objp->uri,objp->version);

	iterp=create_iterator(objp->header_list);
	logger(INFO,"HEADERS:\n");
	while(iterp->has_next(iterp->currptr)){
		hdrp=(header *)iterp->next(&iterp->currptr);
		logger(INFO,"%s: %s\n",hdrp->name,hdrp->value);
	}

	logger(INFO,"\nREQMESSAGE:%s\n",objp->message_body);

	
}

char *find_token(char *buffer,size_t buffer_size){
	logger(INFO,"TOKEN\n");
	size_t size;
	char *retptr;

	for(size=0;size<buffer_size-1;size++){
		if(buffer[size]=='\r' && buffer[size+1]=='\n'){
			//go_ahead;
			retptr=buffer+size+2;
			return retptr;
		}
	}

	retptr=NULL;
	return retptr;

}

/*
int main(int argc, char *argv[]) {

	request_obj *objp=create_http_request();

	char raw_req [] ="POST / HTTP/1.1\r\nHost: www.baidu.com\r\nConnection: close\r\nUser-Agent: Paw 2.0.9 (Macintosh; Mac OS X 10.9.5; en_US)\r\nContent-Length: 38\r\n\r\nthis is a test for content correctness";
	char * rdbufptr=raw_req;
	size_t size=sizeof(raw_req);
	parse_request(objp,rdbufptr,&size);

	print_request(objp);
		
	//printf("STATUS: %d\n METHOD:%d \nURI:%s \nHTTPVERSION:%s\n\nREQMESSAGE:%s\n",objp->stucode,objp->mtdcode,objp->uri,objp->version,objp->message_body);
	
}
*/

