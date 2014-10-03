#ifndef FLOADER_H

#define FLOADER_H

#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"

#include "http_parse_request.h"

typedef struct file_wrapper{
	FILE *fd;
	char *content_type;
	char *content;
	char *file_path;
	size_t file_size;
	time_t last_modified;
	
} file_obj;

char *root_folder;


int check_file(char* file_path,struct stat *file_statp);
int access_file(request_obj *req_objp,file_obj *fobjp);
char *load_file(size_t file_size,FILE* fd);
char *get_content_type(char * file_path);
void free_file_wrapper(file_obj *fobjp);
file_obj *create_file_wrapper();


#endif