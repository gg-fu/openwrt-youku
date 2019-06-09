#include "HttpsRequest.h"

SSL *Connect_https(SSL *ssl,SSL_CTX *ctx,int *sockfd,char *IP,unsigned int portnumber)
{
	/* SSL初始化 */
	SSL_library_init();
	SSL_load_error_strings();
	
	struct in_addr addr;
	struct hostent *host;
	struct sockaddr_in  servaddr;
	/* 建立 sockfd套接字 */
	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{ 	   
		printf("socket fail..............\n");
		return NULL;
	}
	/* 服务器地址信息 */
	/*int i,j,count=0;
	char ip[16]={0};
	char portnumber[6]={0};
	for(i=0;i<strlen(IP);i++)
	{
		if(IP[i] != ':')
			ip[i] = IP[i];
		else
			break;
	}
	for(j=i+1;j<strlen(IP);j++)
	{
		portnumber[count++] = IP[j];
	}
*/
	memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(portnumber);
   	servaddr.sin_addr.s_addr = inet_addr(IP);
	
	/* 客户程序发起连接请求 */
	if (connect(*sockfd, (struct sockaddr *)&servaddr,
						sizeof(struct sockaddr)) == -1)
	{		 
		printf( "Connect Error:%s\a\n", strerror(errno));
		close(*sockfd);
		return NULL;
	}
	
	ctx = SSL_CTX_new(SSLv23_client_method());
	if (ctx == NULL)
	{
		close(*sockfd);
		return NULL;
	}
	/* 基于ctx新建一个ssl套接字 */
	ssl = SSL_new(ctx);
	if (ssl == NULL)
	{
		close(*sockfd);
		return NULL;
	}
	/* 把sockfd加入到SSL中 */
	int ret = SSL_set_fd(ssl, *sockfd);
	if (ret == 0)
	{
		close(*sockfd);
		return NULL;
	}
	/* 建立ssl连接 */
	ret = SSL_connect(ssl);
	if (ret != 1)
	{
		close(*sockfd);
		return NULL;
	}
	
	return ssl;
	
}
int Request(SSL *ssl,char buffer[],int size,char *request,int nbytes)
{
	/*发送https请求request */
	int send = 0;
	int totalsend = 0;
	if(ssl==NULL)
	{
		printf("ssl is NULL\n");
		return -1;
	}
	while (totalsend < nbytes) 
	{
		send = SSL_write(ssl, request + totalsend, nbytes - totalsend);
		if (send == -1) 
			return -1;
		totalsend += send;
	}
	/* 读取服务器的返回信息 */
	int nread=SSL_read(ssl,buffer,size);
	if(nread == -1)
		return -1;
	
	return 0;
	
}
int disConnect_https(SSL *ssl,SSL_CTX *ctx,int sockfd)
{
	SSL_shutdown(ssl);
	SSL_free(ssl);
	close(sockfd);
	SSL_CTX_free(ctx);
	ERR_free_strings();
	return 0;
}
