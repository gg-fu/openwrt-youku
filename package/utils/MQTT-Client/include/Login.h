#ifndef __LOGIN_H
#define __LOGIN_H

#include "RSA.h"
#include "cJSON.h"
#include "MQTTClient.h"
#include "HttpsRequest.h"

#define BASE_LEN 1024
#define CIPHER_LEN 257
#define REQUEST_LEN 1024

#define TYPE          "type"
#define MSGID         "msgId"
#define TOKEN         "token"
#define AESKEY        "aeskey"
#define MQTTPASS      "mqttPass"
#define MQTTADDRESS   "mqttaddress"
#define uchar unsigned char
#define uint unsigned int

typedef struct
{
	char *type;
	char *Token;
	char *AESKey;
	char *MQTTPass;
	char *MQTTAddress;
	int msgId;
}DATA;
DATA *parseJSON(char *msg);
int Register(uchar *buffer,uchar *prikey_path,uchar *ReqHead,uint size);
#endif
