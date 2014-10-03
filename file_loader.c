#include"file_loader.h"


void free_file_wrapper(file_obj *fobjp){
	if(fobjp->content_type!=NULL){
	     free(fobjp->content_type);
	}

	if(fobjp->file_path!=NULL){
	   free(fobjp->file_path);
	}
//	free(fobjp->content_type);
	printf("CK1\n");
//	free(fobjp->file_path);

	        printf("CK2\n");
	if(fobjp->content!=NULL){
		free(fobjp->content);
	}

	free(fobjp);
		
}

file_obj *create_file_wrapper(){
	file_obj *fobjp;
	
	fobjp=(file_obj *)malloc(sizeof(file_obj));
	fobjp->content_type=NULL;
	fobjp->content=NULL;
	fobjp->file_path=NULL;
	fobjp->file_size=0;
	return fobjp;
}


int check_file(char* file_path,struct stat *file_statp){
	
	if(file_path[strlen(file_path)-1]=='/'){
		strcat(file_path, "index.html");
	}
		
	if(stat(file_path,file_statp)!=0){
		//printf("ERROR OPEN FILE");
		return 1;
	}
		
	if(S_ISDIR(file_statp->st_mode)){
		strcat(file_path, "/index.html");
		
		if(stat(file_path,file_statp)!=0){
			//printf("ERROR OPEN FILE");
			return 1;
		}
	}
	return 0;
	
}

int access_file(request_obj *req_objp,file_obj *fobjp){
	char *uri;
	char *file_path;
	struct stat file_stat;
	FILE *fd;
	
	uri=req_objp->uri;
	file_path=(char*)malloc(strlen(uri)+strlen(root_folder)+20);
	strcpy(file_path, root_folder);
	strcat(file_path, uri);
	//
	if(check_file(file_path,&file_stat)!=0){
		req_objp->stucode=NOT_FOUND;
		printf("404\n");
		printf("PATH: %s",uri);
		return 1;
	}
	
	
	if((fd=fopen(file_path,"r"))==NULL){
		req_objp->stucode=SERVER_ERROR;
		printf("500");
		return 1;
	}
	
	
	fobjp->fd=fd;
	fobjp->file_path=file_path;
	fobjp->last_modified=file_stat.st_mtime;
	fobjp->content_type=get_content_type(file_path);
	
	fseek(fd, 0, SEEK_END);
	fobjp->file_size=ftell(fd);
	rewind(fd);
	
	if(req_objp->mtdcode==GET||req_objp->mtdcode==POST){
		fobjp->content=load_file(fobjp->file_size,fd);
	}
	fclose(fd);
	return 0;
	
}

char *load_file(size_t file_size,FILE* fd){
	char *file_buffer;
	//size_t read_size;
	
	file_buffer=(char *)malloc(file_size+1);
	//add error checking
	//read_size=fread(file_buffer,file_size,1,fd);
	fread(file_buffer,file_size,1,fd);
	
	//file_buffer[read_size+1]='\0';
	//printf("SIZE IS : %lu\nFILE IS : %s",read_size,file_buffer);
	
	return file_buffer;
}

char *get_content_type(char * file_path){
	
	char *content_type;
	char *tmp_type;
	char *str_tailer;
	content_type=(char *)malloc(50);
	str_tailer=strrchr(file_path,'.');
	
	if(str_tailer!=NULL){
		if(strcmp(str_tailer,".html")==0|| strcmp(str_tailer,".htm")==0){
			tmp_type="text/html";
		}
		else if(strcmp(str_tailer,".css")==0){
			tmp_type="text/css";
		}
		else if(strcmp(str_tailer,".js")==0){
			tmp_type="application/x-javascript";
		}
		else if(strcmp(str_tailer,".jpeg")==0|| strcmp(str_tailer,".jpg")==0){
			tmp_type="image/jpeg";
		}
		else if(strcmp(str_tailer,".png")==0){
			tmp_type="image/png";
		}
		else if (strcmp(str_tailer,".gif")==0) {
			tmp_type="image/gif";
		}
		else{
			tmp_type="application/octet-stream";
		}
	}
	else{
		tmp_type="application/octet-stream";
	}
	
	strcpy(content_type, tmp_type);
	return content_type;	
}

/*
int main(int argc, char *argv[]) {
	
	file_obj fobj;
	file_obj *fobjp;
	
	fobjp=&fobj;
	char * uri=malloc(50);
	root_folder=malloc(10);
	
	strcpy(uri, "projects/web/blog/blogger/static/css/bootstrap.css");
	//printf("PATH: %s",uri);
	strcpy(root_folder,"/Users/qing/");
	read_file(uri,fobjp);
	
	printf("PATH: %s\nTYPE: %s\nSIZE: %lu\nLAST_MOD: %lu\nCONTENT:\n%s",fobjp->file_path,fobjp->content_type,fobjp->file_size,fobjp->last_modified,fobjp->content);
	free(root_folder);
	free(uri);
	free_fileobj(fobjp);
	
	
}
*/
