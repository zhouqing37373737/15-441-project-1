#include "ssl.h"



int init_ssl_context(SSL_CTX **ssl_context){
/************ SSL INIT ************/
    char *private_key_path;
	char *public_cert_path;

	private_key_path=key_file;
	public_cert_path=crt_file;


    SSL_load_error_strings();
    SSL_library_init();

    /* we want to use TLSv1 only */
    if ((*ssl_context = SSL_CTX_new(TLSv1_server_method())) == NULL)
    {
        logger(INFO, "Error creating SSL context.\n");
        return EXIT_FAILURE;
    }

    /* register private key */
    if (SSL_CTX_use_PrivateKey_file(*ssl_context, private_key_path,SSL_FILETYPE_PEM) == 0)
    {
        //SSL_CTX_free(ssl_context);
        logger(INFO,"Error associating private key.\n");
        return EXIT_FAILURE;
    }

    /* register public key (certificate) */
    if (SSL_CTX_use_certificate_file(*ssl_context,public_cert_path,SSL_FILETYPE_PEM) == 0)
    {
        //SSL_CTX_free(ssl_context);
        logger(INFO, "Error associating certificate.\n");
        return EXIT_FAILURE;
    }


    return 0;
}

int ssl_wrap_socket(conn_obj *cobjp,SSL_CTX *liso_ssl_context){
 /************ WRAP SOCKET WITH SSL ************/
logger(INFO,"LISO_SSL NULL? %d",liso_ssl_context==NULL?1:0);
	SSL *ssl_context;
    if ((ssl_context = SSL_new(liso_ssl_context)) == NULL){
        logger(INFO, "Error creating client SSL context.\n");
        //cobjp->is_open=0;
	ERR_print_errors_fp (stderr);
        return EXIT_FAILURE;
    }

    if (SSL_set_fd(ssl_context, cobjp->conn_fd) == 0){
        logger(INFO, "Error creating client SSL context.\n");
       // cobjp->is_open=0;
        return EXIT_FAILURE;
    }  

    if (SSL_accept(ssl_context) <= 0){

        logger(INFO, "Error accepting (handshake) client SSL context.\n");
       // cobjp->is_open=0;
        return EXIT_FAILURE;
    }

    cobjp->ssl_context=ssl_context;
    return 0;

/************ END WRAP SOCKET WITH SSL ************/
}

