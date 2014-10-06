#include "logger.h"

void init_logger(enum output out,enum level lev,char *log_file){
		default_out=out;
		default_level=lev;
		if(default_out==FILEIO){
			default_logfd=fopen(log_file, "a+");
			dup2(fileno(default_logfd),fileno(stderr));
		}
		else{
			default_logfd=NULL;
		}
}

void logger(enum level lev,const char * format, ...){
	va_list args;
	va_start(args,format);
	
	if(lev>=default_level){
		if(default_out==TERMINAL){
			vprintf(format,args);
		}
		else if(default_out==FILEIO){
			vfprintf(default_logfd,format,args);
			fflush(default_logfd);
		}
	}
	va_end(args);
}

void clear_logger(){
	
	if(default_logfd!=NULL){
		fclose(default_logfd);
	}
}
/*
int main(int argc, char *argv[]) {
	init_logger(FILEIO,DEBUG,"/Users/qing/projects/network/ck2/log");
	char teststr[]="haha";
	int testint=100;
	logger(DEBUG,"TEST:++>%s\n",teststr);
	logger(DEBUG,"TEST:++>%d\n",testint);
	clear_logger();
	
	return 0;
}
*/
