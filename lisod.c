
#include "lisod.h"
#include "common.h"
void signal_handler(int sig)
{
        switch(sig)
        {
                case SIGHUP:
                        liso_state=1;
                        break;          
                case SIGTERM:
                        liso_state=2;
                        break;    
                default:
                        break;
                        /* unhandled signal */      
        }       
}


int daemonize(char* lock_file)
{
        int i, lfp, pid = fork();
        char str[256] = {0};
        if (pid < 0) exit(EXIT_FAILURE);
        if (pid > 0) exit(EXIT_SUCCESS);

        setsid();

        for (i = getdtablesize(); i>=0; i--)
                close(i);

        i = open("/dev/null", O_RDWR);
       if( dup(i)==-1){
		return EXIT_FAILURE;
	}; /* stdout */
	if( dup(i)==-1){
             return EXIT_FAILURE;
        }; /* stdout */
        umask(027);



        lfp = open(lock_file, O_RDWR|O_CREAT, 0640);
        
        if (lfp < 0)
                exit(EXIT_FAILURE);

        if (lockf(lfp, F_TLOCK, 0) < 0)
                exit(EXIT_SUCCESS);
        
        /* only first instance continues */
        sprintf(str, "%d\n", getpid());
	
       if( write(lfp, str, strlen(str)==-1))
{ return EXIT_FAILURE;
};

        signal(SIGCHLD, SIG_IGN); 

        signal(SIGHUP, signal_handler); 
        signal(SIGTERM, signal_handler);


        return EXIT_SUCCESS;
}



int main(int argc, char* argv[]){
    
    int HTTP_port,HTTPS_port;
    char *logfile;
    char *lock_file;
    char *www_folder;
    char *cgi_script_path;
    char *key;
    char *crt;

    liso_server *lserverp;

    HTTP_port_str=malloc(MAXLINESIZE);
    HTTPS_port_str=malloc(MAXLINESIZE); 

    if(argc!=9){
        fprintf(stderr, "Incorrect arg list\n");
        return EXIT_FAILURE;
    }

    HTTP_port=strtol(argv[1],NULL,10);

    if(errno==EINVAL||errno==ERANGE){
        fprintf(stderr, "Port number invalid.\n");
        return EXIT_FAILURE;
    }
    strcpy(HTTP_port_str,argv[1]);


    HTTPS_port=strtol(argv[2],NULL,10);
    if(errno==EINVAL||errno==ERANGE){
                fprintf(stderr, "Port number invalid.\n");
                return EXIT_FAILURE;
    }

    strcpy(HTTPS_port_str,argv[2]);

    logfile=argv[3];

    lock_file=argv[4];
    if(daemonize(lock_file)==EXIT_FAILURE){
        return EXIT_FAILURE;
    }
	    init_logger(FILEIO,INFO,logfile);
         logger(INFO,"%s\n","START LOGGING...");
	  logger(INFO,"%s\n","DEMONIZE SUCCESS...");

    www_folder=argv[5];
    root_folder=malloc(strlen(argv[5]));
    strcpy(root_folder,www_folder);
	logger(INFO,"%s\n","www SUCCESS...");


    cgi_script_path=argv[6];
    CGI_file=malloc(strlen(argv[6]));
    strcpy(CGI_file,cgi_script_path);


    key=argv[7];
    key_file=malloc(strlen(argv[7]));
    strcpy(key_file,key);

    crt=argv[8];
    crt_file=malloc(strlen(argv[8]));
    strcpy(crt_file,crt);

    liso_state=0;

    while(liso_state!=2){
        liso_state=0;
        lserverp=create_liso(HTTP_port,HTTPS_port);
        
        logger(INFO,"%s\n","STARTING LISO...");
        run_liso(lserverp);
        //add free liso
    }

    clear_logger();

    return 0;
}
