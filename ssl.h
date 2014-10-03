
#ifndef SSL_H

#define SSL_H

#include <stdio.h>
#include <unistd.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include "conn_obj.h"
int init_ssl_context(SSL_CTX **ssl_context);
int ssl_wrap_socket(conn_obj *cobjp,SSL_CTX *liso_ssl_context);
#endif
