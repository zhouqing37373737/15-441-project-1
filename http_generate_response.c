#include "http_generate_response.h"




response_obj *create_http_response(){
	response_obj *res_objp;
	
	res_objp=(response_obj *)malloc(sizeof(res_objp));
	res_objp->status_line=(char*)malloc(MAXLINESIZE);
	res_objp->header_list=create_list();
	res_objp->fobjp=create_file_wrapper();
	return res_objp;
}

void free_http_response(response_obj *res_objp){
	Iterator *iterp;
	header *hdrp;
	iterp=create_iterator(res_objp->header_list);
	
	while(iterp->has_next(iterp->currptr)){
		hdrp=(header *)iterp->next(&iterp->currptr);
		free_header(hdrp);
	}	
	
	free_list(res_objp->header_list);
	free(res_objp->status_line);
	free_file_wrapper(res_objp->fobjp);
	free(iterp);
}

void get_time(char* date,time_t rawtime){
	struct tm * ptm;
	ptm = gmtime (&rawtime);
	strftime(date, 256, "%a, %d %b %Y %H:%M:%S %Z", ptm);
	
}

void build_http_response(response_obj *objp,request_obj *req_objp){
	char *tmpbuffer;
	time_t rawtime;
	char *tmpptr;
	
	objp->header_list=create_list();
	
	tmpbuffer=malloc(MAXLINESIZE);
	
	time(&rawtime);
	get_time(tmpbuffer,rawtime);
	
	add_tail(objp->header_list,create_header("Date",tmpbuffer));
	add_tail(objp->header_list,create_header("Server","Liso/1.0"));
	
	
	if(req_objp->mtdcode!=HEAD && req_objp->stucode==OK){
		
		objp->fobjp=(file_obj*)malloc(sizeof(file_obj));
				
		if(access_file(req_objp,objp->fobjp)==0){

			
			tmpptr=get_header_value(req_objp->header_list,"Content-Type");
			if(tmpptr!=NULL){
				add_head(objp->header_list,create_header("Content-Type",tmpptr));
			}

			sprintf(tmpbuffer,"%zu",objp->fobjp->file_size);
			add_head(objp->header_list,create_header("Content-Length",tmpbuffer));
			get_time(tmpbuffer,objp->fobjp->last_modified);
			add_tail(objp->header_list,create_header("Last-Modified",tmpbuffer));
		}
						
	}
	
	if(req_objp->stucode!=OK) {
		add_head(objp->header_list,create_header("Connection","close"));
	}
	else{
		tmpptr=get_header_value(req_objp->header_list,"Connection");
		if(tmpptr!=NULL && strcmp(tmpptr,"close")==0){
			add_head(objp->header_list,create_header("Connection","close"));
		}
		else {
			add_head(objp->header_list,create_header("Connection","Keep-Alive"));
		}
	}
	
	//objp->status_line=(char*)malloc(255);
	checkstatus(objp,req_objp);
	
	//generate general header
	//if request err jmp
	//check file
	
	free(tmpbuffer);
}

size_t serailize_http_response(char *buffer,response_obj *objp){
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
				//printf("HDR:%s\n",buffer);
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

/*
void print_response(response_obj* objp){

	Iterator *iterp;
	header *hdrp;

	printf("STATUS: %s\n",objp->status_line);

	iterp=create_iterator(objp->header_list);
	printf("HEADERS:\n");
	while(iterp->has_next(iterp->currptr)){
		hdrp=(header *)iterp->next(&iterp->currptr);
		printf("%s: %s\n",hdrp->name,hdrp->value);
	}

	printf("\nREQMESSAGE:%s\n",objp->message_body);

	
}
*/


/*
int main(int argc, char *argv[]) {
	
	request_obj *req_objp=create_http_request();
	response_obj *res_objp=create_http_response();

	char raw_req [] ="POST / HTTP/1.1\r\nHost: www.baidu.com\r\nConnection: close\r\nUser-Agent: Paw 2.0.9 (Macintosh; Mac OS X 10.9.5; en_US)\r\nContent-Length: 38\r\n\r\nthis is a test for content correctness";
	char * rdbufptr=raw_req;
	parse_request(req_objp,raw_req,sizeof(raw_req));

	char *buffer=NULL;
	
	root_folder=malloc(20);
	strcpy(root_folder,"/home/qing/www");
	
	build_http_response(res_objp,req_objp);
	buffer=(char*)malloc(strlen(res_objp->status_line)+res_objp->header_list->count*MAXLINESIZE+res_objp->fobjp->file_size);
	serailize_http_response(buffer,res_objp);
	
	printf("RESULT IS ----------\n%s",buffer);
	return 0;
	
}
*/
