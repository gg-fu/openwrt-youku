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
#include "device_order.h"
#include "DaemonLog.h"
#include "DeviceList.h"
#include "UbusTransform.h"
#include "IotFifo.h"
#include "device-sql.h"

#define CLIENTID    "GW"
char *const topic[]={"gw/device/#"};
enum daemon_log_flags daemon_log_use = DAEMON_LOG_SYSLOG;
const char* daemon_log_ident;
/**********************************
*	1.control
*	2.delete
*	3.set_auto
*	4.set_scene
*	5.multicontrol 
**********************************/				 

typedef struct {
	int port;
	char *ip;
	char *filename;
	int type;
}Address;

int msgarrvd(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{

    	char payloadptr[1024] = {0};
	MQTTAsync client = (MQTTAsync)context;
	
	memcpy(payloadptr, message->payload,message->payloadlen);
	DBG_vPrintf(DBG_MQTTC, "topic_name:%s,message:%s\n",topicName,payloadptr);
	int msgId = get_msgId(payloadptr);
	switch(msgId)
	{
		case DEV_CONTROL:
			Device_Control(payloadptr);
			break;
		case DEV_DELETE:
			Device_Delete(payloadptr);
			break;
		case DEV_SET_AUTO:
			DBG_vPrintf(DBG_MQTTC, "set device auto mode\n");
			break;
		case DEV_SET_SCENE:
			DBG_vPrintf(DBG_MQTTC, "set device scene mode\n");
			break;
		case DEV_MULTICONTROL:
			DBG_vPrintf(DBG_MQTTC, "set device multicontrol mode\n");
			break;
		default:
			DBG_vPrintf(DBG_MQTTC, "msgId is unkonw:%d\n",msgId);
			break;
	}	
	//Publish_Message(client,topicName,message->payload,message->payloadlen);
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
				addr->ip = optarg;	
                                break;
 
                        case 'p':
				addr->port = atoi(optarg);
                                break;
 
                        case 'f':
                                break;
 
                        case 't':
                                //g_cfg.type = optarg;
                                break;
 
                        case 'h':
				fprintf(stderr, "-i mqtt server ip\n \
						      -p mqtt server port\n\
						      -f filename\n");
                        default:
                                break;
                }
        }
 
        return;
}

int main(int argc, char* argv[])
{
	Address addr = {1883,"127.0.0.1","./",123};
	daemon_log_ident = daemon_ident_from_argv0(argv[0]);

	get_option(&addr,argc,argv);

	SQL_Init();
	DeviceList_Init();
	DBG_vPrintf(DBG_MQTTC, "*************************start mqtt client**********************\n");
	int ret = Connect(addr.ip,CLIENTID,"username","password",msgarrvd);
	if(ret == -1)
	{
		DBG_vPrintf(DBG_MQTTC, "*****************连接MQTT服务器失败*******************\n");
		return -1;
	}
	DBG_vPrintf(DBG_MQTTC, "*****************Success cnnect to MQTT Server*******************\n");
	ret = Subscribe_topic(topic,1);
	if(ret)
	{
		DBG_vPrintf(DBG_MQTTC,"************************订阅主题失败********************\n");
		return -1;
	}
	Ubus_thread_main();	
	int fd = init_report_fifo();
	while(1){
		IotFormat data ={0};
		ret = read_fifo(fd,&data);
		if(ret > 0){
			switch(data.msgId)
			{
				case DEV_REPORT:
					Device_Report(data);
					break;
				case DEV_LEAVE:
					Device_Leave(data);
					break;
				case DEV_JOIN:
					Device_Join(data);
					break;
				default:
					DBG_vPrintf(DBG_MQTTC,"the msgId is unknow:%d\n",data.msgId);
					break;
			}
		}	
	};
	disConnect(topic,1);
	return 0;
}
