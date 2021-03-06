
#ifndef CONN_H

#define CONN_H

#define DEFAULT_BUFSIZE 4096
#define MAZ_BUFSIZE 65536

#include <stdio.h>
#include <openssl/ssl.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "common.h"
#include "http_parse_request.h"
#include "http_generate_response.h"
#include "D_linked_list.h"

enum protocal {
	HTTP,
	HTTPS,
};

enum state {
	VACANT,
	PARSING,
	PARSED,
	RESPONSE_READY,
	SENT,
	CLOSED,
};


typedef struct connection{
	enum state state;
	int conn_fd;
	int pipe_fd;
	int listen_sock;
	enum protocal protocal;
	int is_open;
	int is_pipe;
	List *environ_list;
	request_obj *req_objp;
	response_obj *res_objp;
	char *read_buffer;
	char *write_buffer;
	size_t read_size;
	size_t write_size;
	SSL *ssl_context;
	char *remote_addr;	
} conn_obj;


int read_connection(conn_obj *cobjp);
int write_connection(conn_obj *cobjp);
int process_connection(conn_obj *cobjp);

conn_obj *create_connection(int listen_sock,enum protocal proto,SSL_CTX *liso_ssl_context);
void free_connection(conn_obj *cobjp,List *connection_pool);
void refresh_connection(conn_obj *cobjp);
void allocate_write_buffer(conn_obj *cobjp);

#include "cgi.h"



#endif
