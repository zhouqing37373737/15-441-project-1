#include "ssl.h"



int init_ssl_context(SSL_CTX *ssl_context){
/************ SSL INIT ************/
    char private_key_path[] ="qzhou.key";
    char public_cert_path[] ="qzhou.cert";

    SSL_load_error_strings();
    SSL_library_init();

    /* we want to use TLSv1 only */
    if ((ssl_context = SSL_CTX_new(TLSv1_server_method())) == NULL)
    {
        fprintf(stderr, "Error creating SSL context.\n");
        return EXIT_FAILURE;
    }

    /* register private key */
    if (SSL_CTX_use_PrivateKey_file(ssl_context, private_key_path,SSL_FILETYPE_PEM) == 0)
    {
        //SSL_CTX_free(ssl_context);
        fprintf(stderr, "Error associating private key.\n");
        return EXIT_FAILURE;
    }

    /* register public key (certificate) */
    if (SSL_CTX_use_certificate_file(ssl_context,public_cert_path,SSL_FILETYPE_PEM) == 0)
    {
        //SSL_CTX_free(ssl_context);
        fprintf(stderr, "Error associating certificate.\n");
        return EXIT_FAILURE;
    }

    return 0;
}

int ssl_wrap_socket(conn_obj *cobjp){
 /************ WRAP SOCKET WITH SSL ************/
    if ((client_context = SSL_new(ssl_context)) == NULL){
        fprintf(stderr, "Error creating client SSL context.\n");
        //cobjp->is_open=0;
        return EXIT_FAILURE;
    }

    if (SSL_set_fd(client_context, cobjp->conn_fd) == 0){
        fprintf(stderr, "Error creating client SSL context.\n");
       // cobjp->is_open=0;
        return EXIT_FAILURE;
    }  

    if (SSL_accept(client_context) <= 0){

        fprintf(stderr, "Error accepting (handshake) client SSL context.\n");
       // cobjp->is_open=0;
        return EXIT_FAILURE;
    }

    cobjp->ssl_context=client_context;
    return 0;

/************ END WRAP SOCKET WITH SSL ************/
}

