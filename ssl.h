
#ifndef SSL_H

#define SSL_H

#include <stdio.h>
#include <unistd.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>

#include <openssl/ssl.h>

int ssl_wrap_socket(conn_obj *cobjp);
int init_ssl_context(SSL_CTX *ssl_context);

#endif