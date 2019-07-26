#ifndef _PUB_SUB_H
#define _PUB_SUB_H
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "MQTTAsync.h"
#define  QOS       1
#define TIMEOUT     10000L
#define TOPIC_MAXNUM  1024
#define ADDRESS_LEN 24

void connlost(void *context, char *cause);
void disConnect(char * const *topics,int num);
int Subscribe_topic(char * const *topics,int num);
int Publish_Message(const char *TOPIC,char buf[],int size);
int Connect(const char *MQTTAddress,const char *CLIENTID,
const char *username,const char *password,MQTTAsync_messageArrived* ma);


#endif
