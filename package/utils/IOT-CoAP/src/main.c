#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "CoAPExport.h"
#include "CoAPNetwork.h"
#include "CoAPObserve.h"
#include "CoAPMessage.h"
#include "CoAPServer.h"
#include "CoAPClient.h"
#define COAP_DEFAULT_PORT 5683
#define DEVICE_IP  "127.0.0.1"
#define DEVICE_REGISTER "/device/register"
#define DEVICE_CONTROL "/device/control"
static CoAPContext *pContext;

void device_register(CoAPContext *context, const char *paths, NetworkAddr *remote, CoAPMessage *message)
{
	char buff[] = "{\"result\":0}";
	
	COAP_INFO("from ip:%s,port:%d\n",remote->addr,remote->port);
	COAP_INFO("message_len:%d,message:%s",message->payloadlen,message->payload);
	CoAPServerResp_send(context, remote, buff, strlen(buff), message,paths);
}

static void CoAPRequest_callback(void *user, void *p_message)
{

        int ret_code = IOTX_SUCCESS;
        CCoAPMessage *message = (CCoAPMessage *)p_message;

        if (NULL == message) {
                COAP_ERR("Invalid paramter, message %p",  message);
                return;
        }

        COAP_DEBUG("Receive response message:");
        COAP_DEBUG("* Response Code : 0x%x", message->header.code);
        COAP_DEBUG("* Payload: %s", message->payload);
}

void device_control(const char *deviceid,const char *meteid,int value)
{
	char buf[128] = {0};
	
	sprintf(buf,"{\"deviceid\":\"%s\",\"meteid\":\"%s\",\"value\":%d}",deviceid,meteid,value);
	CCoAPClient_Request(DEVICE_IP,COAP_DEFAULT_PORT,DEVICE_CONTROL, buf,sizeof(buf),CoAPRequest_callback);
}

int main(int argc,char *argv[])
{
	LITE_openlog("clent");
	LITE_set_loglevel(LOG_DEBUG_LEVEL);
	
	pContext = CoAPServer_init();
        COAP_INFO("running  coap_server\n");
        if(pContext == NULL)
        {
                COAP_ERR("CoAPServer_init failed\n");
                return -1;
        }
        CoAPServer_register(pContext,DEVICE_REGISTER,device_register);
        while(1){
                sleep(5);
        }       
        CoAPServer_deinit(pContext);
	LITE_closelog();
	return 0;
}



