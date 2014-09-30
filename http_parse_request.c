
#include "http_parse_request.h"

void parse_request(request_obj* objp,char* rdbufptr,ssize_t rdbufsize){
	
	char *line;
    char *tmp;
	ssize_t line_length;
	objp->linetype=0;
	objp->stucode=OK;
	objp->content_length=0;
	objp->connection=NULL;
	// parseHeader
	while(objp->linetype<3&&objp->stucode==OK){
		if(objp->linetype<2){
			if((tmp=strstr(rdbufptr,"\r\n"))==NULL){
				objp->stucode=BAD_REQUEST;
				continue;
			}
			
			line_length=tmp-rdbufptr;
					
			if(line_length==0){
				rdbufptr=tmp+2;
				if(objp->mtdcode==POST && objp->content_length>0){
					objp->linetype=2;
				}
				else{
					objp->linetype=3;
				}
				continue;
			}
			
		}
		else if(objp->linetype==2){
			line_length=objp->content_length;
		}
		
		line=(char *) malloc(line_length+1);
		line=strncpy(line,rdbufptr,line_length);
		*(line+line_length)='\0';
		
		switch (objp->linetype) {
			case 0 :
				parse_request_line(objp,line,line_length);
				objp->linetype=1;
				break;
			case 1 :
				parse_request_header(objp,line,line_length);
				break;	
			case 2 :
				parse_request_message(objp,line,line_length);
				objp->linetype=3;
				break;			
			default:
				break;
		}
		//sscanf(line,"%s:%s",header.)
		
		//parse_requestline(objp,line);
		
		rdbufptr=tmp+2;
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
	}
	
	objp->mtdcode=mtdcode;
	objp->uri=uri;
	objp->version=version;
	
	
	
}
void parse_request_header(request_obj* objp,char *line,ssize_t line_length){
	
	//printf("REQHEADER:==>%s\n",line);
	//http_request_header header;
	char *name,*value;
	
	name = (void *)malloc(line_length);
	value = (void *)malloc(line_length);
	
	if(sscanf(line,"%[^:]: %s",name,value)!=2){
		printf("ERROR PARSING HEADER");
		objp->stucode=BAD_REQUEST;
		return;
	}
	
	if (strcmp(name, "Content-Length") == 0){
		//add sanity check
		objp->content_length=atoi(value);
	}
	else if (strcmp(name, "Connection") == 0) {
		//add sanity check
		objp->connection=value;
	}
	
	//printf("HEADERKEY:%s\nHEADERVAL:%s\n\n",name,value);
}
 
void parse_request_message(request_obj* objp,char *line,ssize_t line_length){
	objp->message_body=line;
}


/*
int main(int argc, char *argv[]) {
	
	request_obj reqobj;
	request_obj *objp=&reqobj;
	char raw_req [] ="POST / HTTP/1.1\r\nHost: www.baidu.com\r\nConnection: close\r\nUser-Agent: Paw 2.0.9 (Macintosh; Mac OS X 10.9.5; en_US)\r\nContent-Length: 24\r\n\r\ndfgsdfasdfsadfsdfsdfsdaf";
	char * rdbufptr=raw_req;
	parse_request(&reqobj,raw_req,sizeof(raw_req));
		
	printf("STATUS: %d\n METHOD:%d \nURI:%s \nHTTPVERSION:%s\n\nREQMESSAGE:%s\n",objp->stucode,objp->mtdcode,objp->uri,objp->version,objp->message_body);
	
}
*/