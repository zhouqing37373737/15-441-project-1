#include <sys/types.h>
#include "conn_obj.h"

conn_obj *create_connection(int listen_sock,enum protocal proto){
	int conn_fd;
	conn_obj *cobjp;
	cobjp=(conn_obj *)malloc(sizeof(conn_obj));

	if((conn_fd=accept(listen_sock,NULL,NULL))==-1){
		//accept failer
		free(cobjp);
		return NULL;
	};

	cobjp->conn_fd=conn_fd;

	if(proto==HTTPS){
		if(ssl_wrap_socket(cobjp)!=0){
			//wra[ failer
			if(cobjp->ssl_context!=NULL){
				SSL_CTX_free(cobjp->ssl_context)
			}
			free(cobjp);
			return NULL;
		}
	}

	cobjp->req_objp=create_http_request();
	cobjp->res_objp=create_http_response();
	cobjp->conn_fd=conn_fd;
	cobjp->read_size=0;
	cobjp->write_size=0;
	cobjp->is_open=1;
	cobjp->is_pipe=0;
	cobjp->protocal=proto;
	cobjp->environ_list=create_list();
	cobjp->read_buffer=NULL;
	cobjp->write_buffer=NULL;
	cobjp->ssl_context=NULL;

	return cobjp;
	
}

void refresh_connection(conn_obj *cobjp){
	cobjp->req_objp=create_http_request();
	cobjp->res_objp=create_http_response();
	cobjp->environ_list=create_list();
	cobjp->read_buffer=NULL;
	cobjp->write_buffer=NULL;
	cobjp->read_size=0;
	cobjp->write_size=0;
	cobjp->is_open=1;
	cobjp->is_pipe=0;
}

conn_obj *create_dummy_connection(){
	int conn_fd=100;
	conn_obj *cobjp;
		
	cobjp=(conn_obj *)malloc(sizeof(conn_obj));
	cobjp->req_objp=create_http_request();
	cobjp->res_objp=create_http_response();
	cobjp->conn_fd=conn_fd;
	cobjp->read_size=0;
	cobjp->write_size=0;
	cobjp->is_open=1;
	cobjp->is_pipe=0;
	cobjp->protocal=HTTP;
	cobjp->environ_list=create_list();

	return cobjp;
	
}


void free_connection(conn_obj *cobjp){
	
	//add NULL checking
	Iterator *iterp;
	header *hdrp;
	iterp=create_iterator(cobjp->environ_list);

	while(iterp->has_next(iterp->currptr)){
		hdrp=(header *)iterp->next(&iterp->currptr);
		free_header(hdrp);
	}	
	free(iterp);

	printf("free reqres\n");
	free_http_request(cobjp->req_objp);
	printf("free res\n");
	free_http_response(cobjp->res_objp);

	printf("free buffer\n");
	if(cobjp->read_buffer!=NULL){
		free(cobjp->read_buffer);
	}

	if(cobjp->write_buffer!=NULL){
		free(cobjp->write_buffer);
	}
	printf("free list\n");
	free_list(cobjp->environ_list);
	
	cobjp->read_size=0;
	cobjp->write_size=0;

	if(cobjp->ssl_context!=NULL){
		SSL_CTX_free(cobjp->ssl_context);
		cobjp->ssl_context=NULL;
	}

}


int read_connection(conn_obj *cobjp){
	//int bufpos;
	//int bufsize;
	int readret;
	char *buf;
	
	buf = (char*) malloc (DEFAULT_BUFSIZE*sizeof(char));
	//bufpos=0;
	//bufsize=0;
	
	if(cobjp->protocal==HTTPS){
		readret=SSL_read(cobjp->ssl_context,buf,DEFAULT_BUFSIZE);
	}
	else if(cobjp->protocal==HTTP){
		readret =recv(cobjp->conn_fd, buf, DEFAULT_BUFSIZE, 0);
	}
	else{
		readret=-1;
	}
	
	if(readret<0){
		fprintf(stderr, "READ ERROR!");
		//free(buf);
		//cobj->is_open==0;
		return -1;
	}
	
	else if(readret==0){
		free(buf);
		//cobj->is_open==0;
		return -1;
	}
	
	else{
		cobjp->read_size=readret;
		cobjp->read_buffer=buf;
		cobjp->read_buffer[readret]='\0';
		return 0;
	}
	/*
	while((readret =recv(cobj->conn_fd, buf+bufpos, DEFAULT_BUFSIZE, MSG_DONTWAIT))>0){
		bufpos+=DEFAULT_BUFSIZE;
		if((buf=realloc(buf,bufpos+DEFAULT_BUFSIZE))==NULL){
		     fprintf(stderr, "REALLOC ERROR!");
		     return -1;
		}
		if(bufsize>MAX_BUFSIZE){
			fprintf(stderr, "REQUEST TOO LARGE!");
			return -2;
		}
		if(readret>0){
		     bufsize+=readret;
		}
		
		if(errno==EAGAIN || errno == EWOULDBLOCK){
			if(bufpos>0){
				return 0;
			}
			else if(bufpos==0){
				return 1;
			}
		}
			
		
	}
	*/
}

int process_connection(conn_obj *cobjp){
	
	parse_request(cobjp->req_objp,cobjp->read_buffer,cobjp->read_size);

	print_request(cobjp->req_objp);
	
	if(cobjp->req_objp->is_CGI){
		cobjp->is_pipe=1;
		build_environ_header(cobjp);
		build_CGI_pipe(cobjp);
		allocate_write_buffer(cobjp);
		//serailize_cgi_response(cobjp->write_buffer,cobjp->write_size,cobjp->res_obj);
	}
	else{
		build_http_response(cobjp->res_objp,cobjp->req_objp);
		allocate_write_buffer(cobjp);
		serailize_http_response(cobjp->write_buffer,cobjp->res_objp);

		printf("RESPOSE IS ***********\n\n%s\n",cobjp->write_buffer);
	}
	
	/*
	if(!cobjp->res_objp->is_CGI){
		cobjp->write_buffer=(char*)malloc(strlen(res_objp->status_line)+res_objp->header_list->count*MAXLINESIZE+res_objp->fobjp->file_size);
		
	}
	*/
	return 0;
}

void allocate_write_buffer(conn_obj *cobjp){
	printf("ALLOCATE WRITEBUF\n");
	response_obj *res_objp;
	
	if(cobjp->is_pipe){
		cobjp->write_buffer=(char *)malloc(BUF_SIZE);
	}
	else{
		res_objp=cobjp->res_objp;
		cobjp->write_size=strlen(res_objp->status_line)+res_objp->header_list->count*MAXLINESIZE+res_objp->fobjp->file_size;
		cobjp->write_buffer=(char*)malloc(cobjp->write_size);
	}
	printf("ALLOCATE ENDED,size is %zu(%zu + %zu + %zu)\n",cobjp->write_size,strlen(res_objp->status_line),res_objp->header_list->count*MAXLINESIZE,res_objp->fobjp->file_size);	
}

int write_connection(conn_obj *cobjp){
	int writeret;
	
	if(cobjp->protocal==HTTP){
		writeret=send(cobjp->conn_fd, cobjp->write_buffer, cobjp->write_size, MSG_NOSIGNAL);
	}
	else if(cobjp->protocal==HTTPS){
		writeret=SSL_write(cobjp->ssl_context, cobjp->write_buffer, cobjp->write_size);
	}
	else{
		writeret=-1;
	}
	
	if(writeret==-1 || cobjp->res_objp->is_open==0){
		//write error
		cobjp->is_open=0;
	}
}


/*int main(int argc, char *argv[]) {

	conn_obj *cobjp=create_dummy_connection();

	
}*/
