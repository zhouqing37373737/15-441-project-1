
#include "server.h"

int close_socket(int sock)
{
    if (close(sock))
    {
        fprintf(stderr, "Failed closing socket.\n");
        return 1;
    }
    return 0;
}

void remove_connection(conn_obj *cobjp,List *connection_pool,fd_set *waitfdsp){
	remove_node_content(connection_pool,cobjp);
	FD_CLR(cobjp->conn_fd,waitfdsp);
	close_socket(cobjp->conn_fd);
}

liso_server *create_liso(int HTTP_port){
	liso_server *lserverp;
	lserverp=malloc(sizeof(liso_server));
	
	lserverp->close=0;
	lserverp->connection_pool=create_list();
	lserverp->HTTP_port=HTTP_port;
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
	
	int sock,port;
	int max_fd;
	fd_set readfds,waitfds;//at this version read&sent together
    size_t readret;
	Iterator *iterp;
	conn_obj *cobjp;

	sock=0;
	port=lserverp->HTTP_port;

	iterp=create_iterator(lserverp->connection_pool);
	max_fd=-1;

 
    create_bind_listen_socket(&sock,port);
    
    FD_ZERO(&waitfds);
    FD_SET(sock,&waitfds);
	max_fd=sock>max_fd?sock:max_fd;
    //ADD HTTPS
	
	
    while (!lserverp->close){
        readfds=waitfds;

        if(select(max_fd+1,&readfds,NULL,NULL,NULL)<0){
            fprintf(stderr, "Error selecting.\n");
            return EXIT_FAILURE;
            
        }

        if(FD_ISSET(sock,&readfds)){
            
            if ((cobjp=create_connection(sock,HTTP))==NULL){
                close_socket(sock);
                return EXIT_FAILURE;
            }

			max_fd=cobjp->conn_fd>max_fd?cobjp->conn_fd:max_fd;
            
			add_head(lserverp->connection_pool,cobjp);
			
            FD_SET(cobjp->conn_fd,&waitfds);
        }

 		reset_iterator(lserverp->connection_pool,iterp);

        while(iterp->has_next(iterp->currptr)){
			cobjp=(conn_obj*)iterp->next(&iterp->currptr);
            
            if(FD_ISSET(cobjp->conn_fd,&readfds)){
	
				if(read_connection(cobjp)<0){
					cobjp->is_open=0;
				}
				else{
					process_connection(cobjp);
					
					if(!cobjp->is_pipe){
				
						write_connection(cobjp);
					}
					else{
						FD_SET(cobjp->pipe_fd,&waitfds);
						max_fd=cobjp->pipe_fd>max_fd?cobjp->pipe_fd:max_fd;
					}
				}                          
            }
			
			if(cobjp->is_open && cobjp->is_pipe && FD_ISSET(cobjp->pipe_fd,&readfds)){
				
				if(read_CGI_response(cobjp)!=0){
					//error
					cobjp->is_open=0;
				}
				else{
					write_connection(cobjp);
				}
				
			}
			
			//close not opened connections
			
			if(!cobjp->is_open){
				remove_connection(cobjp,lserverp->connection_pool,&waitfds);
				free_connection(cobjp);
			}
			
            
        }//check ends
        
     
    
    }//while(1)ends
    
    return EXIT_SUCCESS;
    
}

int main(int argc, char* argv[]){
    
	int port;
	liso_server *lserverp;
	
	if(argc!=9){
        fprintf(stderr, "Incorrect arg list\n");
        return EXIT_FAILURE;
    }

    port=strtol(argv[1],NULL,10);

	if(errno==EINVAL||errno==ERANGE){
		fprintf(stderr, "Port number invalid.\n");
		return EXIT_FAILURE;
	}
	
	lserverp=create_liso(port);
	run_liso(lserverp);
}

