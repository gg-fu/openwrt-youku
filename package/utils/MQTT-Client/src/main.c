/*******************************************************************************
 * Copyright (c) 2012, 2017 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution. 
 *
 * The Eclipse Public License is available at 
 *   http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at 
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial contribution
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <OsWrapper.h>
#include <getopt.h>
#include "Login.h"
#include "PubSub.h"
#include "AES.h"
#define SIZE 1024
#define CLIENTID    "APP_Client1"

#define prikey "prikey.pem"
#define Head "https://127.0.0.1:8888/cloud/Register?mac=00:0c:29:db:07:3f&version=1&versionName=123&sign="
#define PAYLOAD "where am I from?who am I?where am I going?"
			
char *const topic[]={"gw/device/#"};				 
DATA *data = NULL;
int count =0;

typedef struct {
	int port;
	char *ip;
	char *filename;
	int type;
}Address;
int msgarrvd(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{

    	char* payloadptr;
	char decrypt_result[2*SIZE+1]={0};
	MQTTAsync client = (MQTTAsync)context;
	
	char iv[16];
	memset(iv,1,sizeof(iv));
    	printf("Message arrived\n");
    	printf("     topic: %s\n", topicName);
    	printf("   message: ");

	/************************此处注意！！！***************************
	 *strlen(message->payload)不等于message->payloadlen **************
	 *原因是：云端传过来的数据后面添加了尾巴，导致比真实数据要长一些 */
    	payloadptr = message->payload;
	//aes_decrypt_cbc(payloadptr,message->payloadlen,"12345678",iv,decrypt_result,sizeof(decrypt_result));
	//printf("dec:%lu,%s\n",strlen(decrypt_result),decrypt_result);
	printf("recv:%s\n",payloadptr);
	Publish_Message(client,topicName,message->payload,message->payloadlen);
    	MQTTAsync_freeMessage(&message);
    	MQTTAsync_free(topicName);
   	return 1;
}                                            

void get_option(Address *addr,int argc, char **argv)
{
        char *cmd = argv[0];
 
        while (1) {
                int option_index = 0;
                struct option long_options[] =
                {
                        {"ip"      , 1, 0, 'i'},
                        {"port"    , 1, 0, 'p'},
                        {"file"    , 1, 0, 'f'},
                        {"type"    , 1, 0, 't'}, //1代表有参数 
                        {"help"    , 0, 0, 'h'}  //0代表没有参数
                };
                int c;
 
                c = getopt_long(argc, argv, "i:p:f:t:h",long_options, &option_index);  
		//注意这里的冒号，有冒号就需要加参数值，没有冒号就不用加参数值
                if (c == -1)
                        break;
 
                switch (c)
                {
                        case 'i':
                                printf("Input arg IP \n");
				addr->ip = optarg;	
                                break;
 
                        case 'p':
                                printf("option p with value '%s'\n", optarg);
				addr->port = atoi(optarg);
                                break;
 
                        case 'f':
                                printf("option f with value '%s'\n", optarg);
                                break;
 
                        case 't':
                                printf("option t with value '%s'\n", optarg);
                                //g_cfg.type = optarg;
                                break;
 
                        case 'h':
                        default:
                                printf("this is default!\n");
                                break;
                }
        }
 
        return;
}

int main(int argc, char* argv[])
{
	MQTTAsync client;
	uchar buf[2048]={0};
	Address addr = {1883,"127.0.0.1","./",123};
	get_option(&addr,argc,argv);
	int ret = Connect(&client,addr.ip,CLIENTID,"username","password",msgarrvd);
	if(ret == -1)
	{
		printf("连接MQTT服务器失败\n");
		return -1;
	}
	ret = Subscribe_topic(client,topic,1);
	if(ret)
	{
		printf("订阅主题失败\n");
		return -1;
	}
	
	while(1){};
	disConnect(client,topic,1);
	return 0;
}
