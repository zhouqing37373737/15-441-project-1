/******************************************************************************
 * echo_server.c                                                               *
 *                                                                             *
 * Description: This file contains the C source code for an echo server.  The  *
 *              server runs on a hard-coded port and simply write back anything*
 *              sent to it by connected clients.  It does not support          *
 *              concurrent clients.                                            *
 *                                                                             *
 * Authors: Athula Balachandran <abalacha@cs.cmu.edu>,                         *
 *          Wolf Richter <wolf@cs.cmu.edu>                                     *
 *                                                                             *
 *******************************************************************************/

#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include<errno.h>

#define ECHO_PORT 9999
#define BUF_SIZE 4096


int close_socket(int sock)
{
    if (close(sock))
    {
        fprintf(stderr, "Failed closing socket.\n");
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[])
{
    int sock, client_sock;
    ssize_t readret;
    socklen_t cli_size;
    struct sockaddr_in addr, cli_addr;
    //char buf[BUF_SIZE];
    char *buf;

    fd_set readfds,waitfds;//at this version read&sent together
    struct timeval tv;
    int i;
    int dsize;

    dsize = getdtablesize();
    
    fprintf(stdout, "----- Echo Server -----\n");
    
    /* all networked programs must create a socket */
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Failed creating socket.\n");
        return EXIT_FAILURE;
    }
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(ECHO_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    
    
    /* servers bind sockets to ports---notify the OS they accept connections */
    if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)))
    {
        close_socket(sock);
        fprintf(stderr, "Failed binding socket.\n");
        return EXIT_FAILURE;
    }
    
    
    if (listen(sock, 5))
    {
        close_socket(sock);
        fprintf(stderr, "Error listening on socket.\n");
        return EXIT_FAILURE;
    }
    

    
    // wait until either socket has data ready to be recv()d (timeout 10.5 secs)
    tv.tv_sec = 0;
    tv.tv_usec = 500000;
    
    FD_ZERO(&waitfds);
    FD_SET(sock,&waitfds);
    
    /* finally, loop waiting for input and then write it back */
    while (1)
    {
        readfds=waitfds;
        
        //fprintf(stderr,"selecting...\n");
        //SELECT()
        if(select(dsize,&readfds,NULL,NULL,&tv)<0){
            fprintf(stderr, "Error selecting.\n");
            return EXIT_FAILURE;
            
        }

       /* for(i=0;i<dsize;i++)
        {
            if(FD_ISSET(i,&readfds)){
                fprintf(stderr, "PORT_READY :-->%d\n",i);
            }
        }
        */

        
        //if(listener is in readfdset)
        if(FD_ISSET(sock,&readfds)){
            cli_size = sizeof(cli_addr);
            if ((client_sock = accept(sock, (struct sockaddr *) &cli_addr,
                                      &cli_size)) == -1)
            {
                close(sock);
                fprintf(stderr, "Error accepting connection.\n");
                return EXIT_FAILURE;
            }
            
            //set to non-blocking
            //fcntl(client_sock, F_SETFL, O_NONBLOCK);

            //add fd to waitlist
            FD_SET(client_sock,&waitfds);
            //fprintf(stderr," accepting connection.\n");
            
            
        }
 
        
        //fprintf(stderr, "anotherround\n");
        for(i=0;i<dsize;i++)
        {
            //printf("yougottakiddingme? %d  %d\n",FD_ISSET(active_connection[i],&readfds),i);
            if(FD_ISSET(i,&readfds)&&i!=sock)
            {
                //fprintf(stderr, "reading connection.\n");

                buf = (char*) malloc (BUF_SIZE*sizeof(char));
                int bufpos=0;
                int bufsize=0;

                while((readret =recv(i, buf+bufpos, BUF_SIZE, MSG_DONTWAIT))>0){
                    
                    buf=realloc(buf,sizeof(buf)+BUF_SIZE);

                    if((buf=realloc(buf,sizeof(buf)+BUF_SIZE))==NULL){
                        fprintf(stderr, "REALLOC ERROR!");
                        return EXIT_FAILURE;
                    }

                    if(readret>0){
                        bufsize+=readret;
                    }

                    bufpos+=BUF_SIZE;
                }
                
                
                if(errno==EAGAIN || errno == EWOULDBLOCK){                
                    //got data
                    if(bufpos>0){
                        send(i, buf, bufsize, 0);
                        free(buf);
                        //FD_CLR(i,&readfds);
                        //close_socket(i);
                        //fprintf(stderr, "sending %s(%d) :->%d\n", buf, bufsize,i);
                        //fprintf(stderr, "socket closed!.\n");
                    }

                    // socket closed from client
                    else if(bufpos==0){
                      FD_CLR(i,&waitfds); 
                      close_socket(i); 
                      //fprintf(stderr, "socket closed :->%d\n",i);
                    }
                }
                

                else{
                    close_socket(i);
                    close_socket(sock);
                    fprintf(stderr, "Error reading from client socket : -> %s-->%d-->%d.\n",strerror(errno),i,bufpos);
                    return EXIT_FAILURE;
                }

                
                /*
                //read&send
                if(readret>1)
                {
                    send(i, buf, readret, 0);
                    memset(buf, 0, BUF_SIZE);
                    FD_CLR(i,&waitfds);
                    close_socket(i);
                    fprintf(stderr, "socket closed!.\n");
                    
                }
                */
                              
            }
            
            //close_socket(sock);
            
        }//check ends
        
      //  if(connection_counter>=MAXCONNECTIONSIZE)
          
        
    }//while(1)ends
    
    
    
}