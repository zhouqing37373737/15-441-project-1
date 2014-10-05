
#include "server.h"
#include "lisod.h"

int close_socket(int sock)
{
    if (close(sock))
    {
        fprintf(stderr, "Failed closing socket.\n");
        return 1;
    }
    return 0;
}


liso_server *create_liso(int HTTP_port,int HTTPS_port){
	liso_server *lserverp;
	lserverp=(liso_server *)malloc(sizeof(liso_server));
	
	lserverp->close=0;
	lserverp->connection_pool=create_list();
	lserverp->HTTP_port=HTTP_port;
	lserverp->HTTPS_port=HTTPS_port;
	lserverp->ssl_context=NULL;

	return lserverp;
}

int create_bind_listen_socket(int *sockp,int port){
	
	struct sockaddr_in addr;
	
	if ((*sockp = socket(PF_INET, SOCK_STREAM, 0)) == -1){
		
	    fprintf(stderr, "Failed creating socket.\n");
	    return EXIT_FAILURE;
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	    
	    
	/* servers bind sockets to ports---notify the OS they accept connections */
	if (bind(*sockp, (struct sockaddr *) &addr, sizeof(addr))){
		close_socket(*sockp);
		fprintf(stderr, "Failed binding socket.\n");
		return EXIT_FAILURE;
	}
	    
	    
	if (listen(*sockp, 120)){
		close_socket(*sockp);
		fprintf(stderr, "Error listening on socket.\n");
		return EXIT_FAILURE;
	}	    
	
	return 0;
}


int run_liso(liso_server *lserverp){
	
	int HTTP_sock,HTTP_port;
	int HTTPS_sock,HTTPS_port;

	int max_fd;
	fd_set readfds,waitfds;//at this version read&sent together
    size_t readret;
	Iterator *iterp;
	conn_obj *cobjp;
	//int liso_state;



	HTTP_sock=0;
	HTTP_port=lserverp->HTTP_port;

	HTTPS_sock=0;
	HTTPS_port=lserverp->HTTPS_port;

	iterp=create_iterator(lserverp->connection_pool);
	max_fd=-1;
	logger(INFO,"%s\n","HERE?");
	if(init_ssl_context(&(lserverp->ssl_context))!=0){
		return EXIT_FAILURE;
	}
 
	logger(INFO,"%s\n","SSL?");
    if(create_bind_listen_socket(&HTTP_sock,HTTP_port)!=0){
    	return EXIT_FAILURE;
    }

    if(create_bind_listen_socket(&HTTPS_sock,HTTPS_port)!=0){
    	return EXIT_FAILURE;
    }
    logger(INFO,"%s\n","OPENSOCK?"); 
    FD_ZERO(&waitfds);
    FD_SET(HTTP_sock,&waitfds);
    FD_SET(HTTPS_sock,&waitfds);
	max_fd=HTTP_sock>HTTPS_sock?HTTP_sock:HTTPS_sock;
    //ADD HTTPS
	
	
    while (liso_state==0){
        readfds=waitfds;

        if(select(max_fd+1,&readfds,NULL,NULL,NULL)<0){
            fprintf(stderr, "Error selecting.\n");
            return EXIT_FAILURE;
            
        }

        if(FD_ISSET(HTTP_sock,&readfds)){
            
            if ((cobjp=create_connection(HTTP_sock,HTTP,NULL))==NULL){
                close_socket(HTTP_sock);
                //return EXIT_FAILURE;
                continue;
            }

			max_fd=cobjp->conn_fd>max_fd?cobjp->conn_fd:max_fd;
           	printf("NEW HTTP CONN\n"); 
			add_head(lserverp->connection_pool,cobjp);
			FD_SET(cobjp->conn_fd,&waitfds);
			continue;
        }

        if(FD_ISSET(HTTPS_sock,&readfds)){
            
            if ((cobjp=create_connection(HTTPS_sock,HTTPS,lserverp->ssl_context))==NULL){
                close_socket(HTTPS_sock);
                //return EXIT_FAILURE;
                continue;
            }

			max_fd=cobjp->conn_fd>max_fd?cobjp->conn_fd:max_fd;
           	printf("NEW HTTPS CONN\n"); 
			add_head(lserverp->connection_pool,cobjp);
			FD_SET(cobjp->conn_fd,&waitfds);
			continue;
        }

 		reset_iterator(lserverp->connection_pool,iterp);

        while(iterp->has_next(iterp->currptr)){
			cobjp=(conn_obj*)iterp->next(&iterp->currptr);
            
            if(FD_ISSET(cobjp->conn_fd,&readfds)){
				printf("READ CONN\n");	
				if(read_connection(cobjp)<0){
					cobjp->state=CLOSED;
				}     
				else{
					cobjp->state=PARSING;
				}                    
            }
		if(cobjp->state==PARSING){
			printf("PROCESS CONN\n");
			process_connection(cobjp);
		

			if(cobjp->is_pipe){
				FD_SET(cobjp->pipe_fd,&waitfds);
				max_fd=cobjp->pipe_fd>max_fd?cobjp->pipe_fd:max_fd;
				continue;
			}
			
		}
		 if(cobjp->state==PARSED && cobjp->is_pipe && FD_ISSET(cobjp->pipe_fd,&readfds)){
			printf("READ CGI\n");	
				if(read_CGI_response(cobjp)!=0){
					//error
					cobjp->state=CLOSED;
				}				
			}

			write_connection(cobjp);
			
			//close not opened connections
			if(cobjp->state==CLOSED){
				printf("CLOSE CONN\n");
				FD_CLR(cobjp->conn_fd,&waitfds);
				free_connection(cobjp,lserverp->connection_pool);
			}

			else if(cobjp->state==SENT){
				printf("REFRESH CONN\n");
				refresh_connection(cobjp);
				cobjp->state=VACANT;
			}
            
        }//check ends
            
    }//while(1)ends
    
    return EXIT_SUCCESS;
    
}



