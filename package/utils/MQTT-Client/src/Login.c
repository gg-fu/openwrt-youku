#include "Login.h"

DATA *parseJSON(char *msg)
{	
	assert(msg);
	static DATA data;
	cJSON *pJSON  =	cJSON_Parse(msg);
	if(!pJSON)
		return NULL;
	cJSON *type = cJSON_GetObjectItem(pJSON,TYPE);
	cJSON *msgId = cJSON_GetObjectItem(pJSON,MSGID);
	cJSON *Token = cJSON_GetObjectItem(pJSON,TOKEN);	
	cJSON *AESKey = cJSON_GetObjectItem(pJSON,AESKEY);	
	cJSON *MQTTPass = cJSON_GetObjectItem(pJSON,MQTTPASS);	
	cJSON *MQTTAddress = cJSON_GetObjectItem(pJSON,MQTTADDRESS);
	
	if(AESKey==NULL)
	{
		printf("AESKey is NULL\n");
		return NULL;
	}
	if(MQTTPass==NULL)
	{
		printf("MQTTPass is NULL\n");
		return NULL;
	}
	if(MQTTAddress==NULL)
	{
		printf("MQTTAddress is NULL\n");
		return NULL;
	}
	
	data.type = type->valuestring;
	data.Token = Token->valuestring;
	data.AESKey = AESKey->valuestring;
	data.MQTTPass = MQTTPass->valuestring;
	data.MQTTAddress = MQTTAddress->valuestring;
	data.msgId = msgId->valueint;
	//cJSON_Delete(pJSON);
	return &data;
}
static void CutString(uchar *ReqHead,uchar *IP,uchar *Post,uchar *Sign)
{
	int i=0;
	char *p = strstr(ReqHead,"://")+3;
	while(*p!=':'){
		IP[i]=*p;
		p++,i++;
	}
	i=0;p++;
	while(*p!='/'){
		Post[i] = *p;
		p++,i++;
	}
	i=0;
	char *q = strstr(ReqHead,"mac=");
	char *r = strstr(ReqHead,"&sign=");
	while(q!=r){
		Sign[i]=*q;
		q++,i++;
	}
	return ;
}
int Register(uchar *buffer,uchar *prikey_path,uchar *ReqHead,uint size)
{
	SSL *ssl;
	SSL_CTX *ctx;
	int sockfd=0,i,j;
	
	uchar request[REQUEST_LEN];
	uchar cipher[CIPHER_LEN];
	uchar base[BASE_LEN];
	uchar IP[16]={0};
    uchar Post[6]={0};
    uchar Sign[1024]={0};
	uint  cipher_len;
	/* 获取签名后数据信息 */
	if(prikey_path==NULL||ReqHead==NULL)
	{
		printf("the privateKey or ReqHead is NULL\n");
		return -1;
	}
	CutString(ReqHead,IP,Post,Sign);
	if(GetSign(prikey_path,Sign,cipher,&cipher_len)==NULL)
	{
		printf("sign error\n");
		return -1;
	}
	/* 对签名进行base64编码 */
	base64(cipher,cipher_len,base,sizeof(base));
	/*base64编码后自带回车符，所以要去掉回车符*/
	int len=strlen(base);
	uchar S[BASE_LEN]={0};
	for(i=0,j=0;i<len;i++)
	{
		if(base[i]!='\n')
		{
			S[j]=base[i];
			j++;
		}
	}
	/* https 请求头 */
	sprintf(request, "GET %s%s HTTP/1.1\r\n"
					 "Host: %s:%s\r\n"
					 "Connection: keep-alive\r\n\r\n",ReqHead,S,IP,Post);
	
	printf("request:%s\n",request);
	/* 连接 */
	ssl = Connect_https(ssl,ctx,&sockfd,IP,atoi(Post));
	if(ssl == NULL)
	{
		close(sockfd);
		return -1;
	}
	/* 接收服务器返回的消息 */
	int nbytes = strlen(request);
	int ret = Request(ssl,buffer,size,request,nbytes);
	if(ret == -1)
	{
		disConnect_https(ssl,ctx,sockfd);
		return -1;
	}
	disConnect_https(ssl,ctx,sockfd);
	return 0;
}
