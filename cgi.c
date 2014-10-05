
#include "cgi.h"


void build_environ_header(conn_obj* objp){
	char *hdr_value;
	char *path_info;
	char *query_str;
	char *tmp;
	request_obj *req_objp;

	req_objp=objp->req_objp;
	path_info=req_objp->uri+strlen("/cgi");
	query_str=strchr(req_objp->uri,'?');
	query_str=query_str==NULL?query_str:query_str+1;

	hdr_value=get_header_value(req_objp->header_list,"Content-Length");
	add_tail(objp->environ_list,create_environ_header("CONTENT_LENGTH",hdr_value));

	hdr_value=get_header_value(req_objp->header_list,"Content-Type");
	add_tail(objp->environ_list,create_environ_header("CONTENT_TYPE",hdr_value));

	add_tail(objp->environ_list,create_environ_header("GATEWAY_INTERFACE","CGI/1.1"));

	add_tail(objp->environ_list,create_environ_header("REQUEST_URI",req_objp->uri));

	add_tail(objp->environ_list,create_environ_header("PATH_INFO",path_info));

	add_tail(objp->environ_list,create_environ_header("QUERY_STRING",query_str));

	add_tail(objp->environ_list,create_environ_header("SCRIPT_NAME","/cgi"));

	switch(req_objp->mtdcode){
		case GET:
			tmp="GET";break;
		case POST:
			tmp="POST";break;
		case HEAD:
			tmp="HEAD";break;
		default:
			break;
	}

	add_tail(objp->environ_list,create_environ_header("REQUEST_METHOD",tmp));

	tmp=(char *)malloc(20);
	snprintf(tmp,20,"%d",objp->listen_sock);
	add_tail(objp->environ_list,create_environ_header("SERVER_PORT",tmp));

	add_tail(objp->environ_list,create_environ_header("SERVER_PROTOCOL","HTTP/1.1"));

	add_tail(objp->environ_list,create_environ_header("SERVER_SOFTWARE","Liso/1.0"));

	hdr_value=get_header_value(req_objp->header_list,"Accept");
	add_tail(objp->environ_list,create_environ_header("HTTP_ACCEPT",hdr_value));

	hdr_value=get_header_value(req_objp->header_list,"Referer");
	add_tail(objp->environ_list,create_environ_header("HTTP_REFERER",hdr_value));

	hdr_value=get_header_value(req_objp->header_list,"Accept-Encoding");
	add_tail(objp->environ_list,create_environ_header("HTTP_ACCEPT_ENCODING",hdr_value));

	hdr_value=get_header_value(req_objp->header_list,"Accept-Language");
	add_tail(objp->environ_list,create_environ_header("HTTP_ACCEPT_LANGUAGE",hdr_value));

	hdr_value=get_header_value(req_objp->header_list,"Accept-Charset");
	add_tail(objp->environ_list,create_environ_header("HTTP_ACCEPT_CHARSET",hdr_value));

	hdr_value=get_header_value(req_objp->header_list,"Host");
	add_tail(objp->environ_list,create_environ_header("HTTP_HOST",hdr_value));

	hdr_value=get_header_value(req_objp->header_list,"Cookie");
	add_tail(objp->environ_list,create_environ_header("HTTP_COOKIE",hdr_value));

	hdr_value=get_header_value(req_objp->header_list,"User-Agent");
	add_tail(objp->environ_list,create_environ_header("HTTP_USER_AGENT",hdr_value));

	hdr_value=get_header_value(req_objp->header_list,"Connection");
	add_tail(objp->environ_list,create_environ_header("HTTP_CONNECTION",hdr_value));	


}

char **create_environ_array(conn_obj *cobjp){
	char ** env_array;
	Iterator *iterp;
	header *hdrp;
	int i;

	i=0;
	env_array=(char **)malloc(cobjp->environ_list->count*sizeof(char *)+sizeof(char *));
	iterp=create_iterator(cobjp->environ_list);

	while(iterp->has_next(iterp->currptr)){

		hdrp=(header*)iterp->next(&iterp->currptr);
		env_array[i]=(char *)malloc(strlen(hdrp->name)+strlen(hdrp->value)+10);
		sprintf(env_array[i],"%s=%s",hdrp->name,hdrp->value);
		i++;
	}
	env_array[i]=NULL;

	free(iterp);
	return env_array;

}

void free_environ_array(char ** env_array){

}

int build_CGI_pipe(conn_obj *cobjp){
	
	    int stdin_pipe[2];
	    int stdout_pipe[2];
	    int readret;
		char* ARGV[] = {FILENAME,NULL};
		char** ENVP;
		int pid;
		
		
		ENVP = create_environ_array(cobjp);

	    /* 0 can be read from, 1 can be written to */
	    if (pipe(stdin_pipe) < 0)
	    {
	        fprintf(stderr, "Error piping for stdin.\n");
	        return EXIT_FAILURE;
	    }

	    if (pipe(stdout_pipe) < 0)
	    {
	        fprintf(stderr, "Error piping for stdout.\n");
	        return EXIT_FAILURE;
	    }

	    pid = fork();
	
	    /* not good */
	    if (pid < 0)
	    {
	        fprintf(stderr, "Something really bad happened when fork()ing.\n");
	        return EXIT_FAILURE;
	    }

	    /* child, setup environment, execve */
	    if (pid == 0)
	    {
	        /*************** BEGIN EXECVE ****************/
	        close(stdout_pipe[0]);
	        close(stdin_pipe[1]);
	        dup2(stdout_pipe[1], fileno(stdout));
	        dup2(stdin_pipe[0], fileno(stdin));
	        /* you should probably do something with stderr */

	        /* pretty much no matter what, if it returns bad things happened... */
	        if (execve(FILENAME, ARGV, ENVP))
	        {
	            //execve_error_handler();
	            fprintf(stderr, "Error executing execve syscall.\n");
	            return EXIT_FAILURE;
	        }
	        /*************** END EXECVE ****************/ 
	    }

	    if (pid > 0)
	    {
	        fprintf(stdout, "Parent: Heading to select() loop.\n");
	        close(stdout_pipe[1]);
	        close(stdin_pipe[0]);

	        if (write(stdin_pipe[1],cobjp->req_objp->message_body,cobjp->req_objp->content_length)< 0)
	        {
	            fprintf(stderr, "Error writing to spawned CGI program.\n");
	            return EXIT_FAILURE;
	        }

	        close(stdin_pipe[1]); /* finished writing to spawn */

			//res->CGI_read_pid=stdout_pipe[0];
			//res->is_CGI=1;
			cobjp->pipe_fd=stdout_pipe[0];;
	}
	
	return 0;
			

}


int read_CGI_response(conn_obj *cobjp){
	
	int readret;
	size_t buffer_size;
	char *buffer;
	
	buffer=cobjp->write_buffer;
	buffer_size=0;
	
	while((readret=read(cobjp->pipe_fd, buffer+buffer_size, DEFAULT_BUFSIZE))> 0){
		
		if((buffer=realloc(buffer,buffer_size+DEFAULT_BUFSIZE))==NULL){
			return 1;
		}
		
		buffer_size+=readret;
	}
	
	if(readret==0){
		//finished reading
		cobjp->write_size=buffer_size;
		
		return 0;
	}
	else {
		
		//error
		return 1;
	}
	
}
