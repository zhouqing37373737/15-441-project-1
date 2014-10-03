#ifndef SERVER_H

#define SERVER_H
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <errno.h>

#include "common.h"
#include "D_linked_list.h"
#include "http_parse_request.h"
#include "http_generate_response.h"
#include "cgi.h"
#include "conn_obj.h"
#include "ssl.h"


typedef struct liso_server{
	int close;
	int HTTP_port;
	int HTTPS_port;
	SSL_CTX *ssl_context;
	List *connection_pool;
	
} liso_server;

int close_socket(int sock);
int create_bind_listen_socket(int *sockp,int port);
int run_liso(liso_server *lserverp);
void remove_connection(conn_obj *cobjp,List *connection_pool,fd_set *waitfdsp);
liso_server *create_liso(int HTTP_port);




#endif