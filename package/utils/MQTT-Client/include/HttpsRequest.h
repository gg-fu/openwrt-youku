#ifndef _HTTPS_REQUEST__
#define _HTTPS_REQUEST__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <netdb.h>
#include <errno.h>
#include <time.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

int disConnect_https(SSL *ssl,SSL_CTX *ctx,int sockfd);
int Request(SSL *ssl,char buffer[],int size,char *request,int nbytes);
SSL *Connect_https(SSL *ssl,SSL_CTX *ctx,int *sockfd,char *IP,unsigned int portnumber);

#endif
