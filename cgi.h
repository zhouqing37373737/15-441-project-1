
#ifndef CGI_H

#define CGI_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "http_parse_request.h"
#include "http_generate_response.h"
#include "conn_obj.h"

#define FILENAME "/home/qing/Learn/Network/15-441-project-1/cgi/cgi_script.py"

void build_environ_header(conn_obj* cobjp);
void free_environ_array(char ** env_array);
char **create_environ_array(conn_obj *cobjp);
int build_CGI_pipe(conn_obj *cobjp);
int read_CGI_response(conn_obj *cobj);

#endif
