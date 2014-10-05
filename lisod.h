
#ifndef LISOD_H
#define LISOD_H

#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "server.h"
char *root_folder;
char *HTTP_port_str;
char *HTTPS_port_str;

char *CGI_file;
char *key_file;
char *crt_file;

int liso_state;


void signal_handler(int sig);
int daemonize(char* lock_file);

#endif
