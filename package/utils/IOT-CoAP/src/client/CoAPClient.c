#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "CCoAPExport.h"
#include "CCoAPNetwork.h"
#include "CCoAPMessage.h"
#include "HAL_UDP_Linux.h"
#include "CoAPClient.h"
#include "CoAPSerialize.h"

void iotx_event_notifyer(unsigned int code, CoAPMessage *message)
{

    if (NULL == message) {
        COAP_ERR("Invalid paramter, message %p", message);
        return ;
    }

    COAP_DEBUG("Error code: 0x%x, payload: %s", code, message->payload);
    switch (code) {
        case COAP_MSG_CODE_402_BAD_OPTION:
        case COAP_MSG_CODE_401_UNAUTHORIZED: {
            /* TODO: call event handle to notify application */
            /* p_context->event_handle(); */
            break;
        }
        default:
            break;
    }
}

CCoAPContext *CCoAPclient_init(char* url)
{
	int wait_time_ms = 2000;
	CCoAPContext *g_Ccontext = NULL;
	CCoAPInitParam param = {0};
	
	param.url = url;
	param.maxcount = IOTX_LIST_MAX_ITEM;
    	param.notifier = (CCoAPEventNotifier)iotx_event_notifyer;
    	param.waittime = wait_time_ms;
		
	g_Ccontext = CCoAPContext_create(&param);
        if(NULL == g_Ccontext){
            COAP_ERR("CoAPClient Context Create failed");
            return NULL;
        }
	return g_Ccontext;
}

void CCoAPClient_deinit(CCoAPContext *context)
{
    if(NULL != context){
        CoAPContext_free(context);
	context = NULL;
    }
   
    HAL_SleepMs(1000);
}


static unsigned int iotx_get_coap_token( unsigned char *p_encoded_data)
{

    static unsigned int value = COAP_INIT_TOKEN;
    p_encoded_data[0] = (unsigned char)((value & 0x00FF) >> 0);

    p_encoded_data[1] = (unsigned char)((value & 0xFF00) >> 8);

    p_encoded_data[2] = (unsigned char)((value & 0xFF0000) >> 16);

    p_encoded_data[3] = (unsigned char)((value & 0xFF000000) >> 24);

    value++;

    return sizeof(unsigned int);
}

static int iotx_split_path_2_option(char *uri, CCoAPMessage *message)

{

    char *ptr     = NULL;

    char *pstr    = NULL;

    char  path[COAP_MSG_MAX_PATH_LEN]  = {0};

    if (NULL == uri || NULL == message) {

        COAP_ERR("Invalid paramter p_path %p, p_message %p", uri, message);

        return IOTX_ERR_INVALID_PARAM;

    }

    if (IOTX_URI_MAX_LEN < strlen(uri)) {

        COAP_ERR("The uri length is too loog,len = %d", (int)strlen(uri));

        return IOTX_ERR_URI_TOO_LOOG;

    }

    COAP_DEBUG("The uri is %s", uri);

    ptr = pstr = uri;

    while ('\0' != *ptr) {

        if ('/' == *ptr) {

            if (ptr != pstr) {

                memset(path, 0x00, sizeof(path));

                strncpy(path, pstr, ptr - pstr);

                COAP_DEBUG("path: %s,len=%d", path, (int)(ptr - pstr));

                CCoAPStrOption_add(message, COAP_OPTION_URI_PATH,

                                  (unsigned char *)path, (int)strlen(path));

            }

            pstr = ptr + 1;

        }

        if ('\0' == *(ptr + 1) && '\0' != *pstr) {

            memset(path, 0x00, sizeof(path));

            strncpy(path, pstr, sizeof(path) - 1);

            COAP_DEBUG("path: %s,len=%d", path, (int)strlen(path));

            CCoAPStrOption_add(message , COAP_OPTION_URI_PATH  , (unsigned char *)path , (int)strlen(path));

        }

        ptr ++;

    }

    return IOTX_SUCCESS;

}

int CCoAPClient_send(CCoAPContext *context, CCoAPRespMsgHandler callback,const char *p_path, unsigned char *buff,unsigned int len)
{
        int ret = COAP_SUCCESS;
        CCoAPMessage message;
	CCoAPContext      *p_coap_ctx = NULL;
	
        unsigned char tokenlen;
        unsigned char token[COAP_MSG_MAX_TOKEN_LEN] = {0};
        if (NULL == context || NULL == p_path || NULL == buff ) {
         	return COAP_ERROR_INVALID_PARAM;
        }
	
	if (len >= COAP_MSG_MAX_PDU_LEN) {
        	COAP_ERR("The payload length %d is too loog", len);
        	return IOTX_ERR_URI_TOO_LOOG;
    	}
        CCoAPMessage_init(&message);
        CCoAPMessageType_set(&message, COAP_MESSAGE_TYPE_CON);
        CCoAPMessageCode_set(&message, COAP_MSG_CODE_POST);
        CCoAPMessageId_set(&message, CCoAPMessageId_gen(context));
        tokenlen = iotx_get_coap_token(token);
        CCoAPMessageToken_set(&message, token, tokenlen);
	CCoAPMessageHandler_set(&message, callback);
	ret = iotx_split_path_2_option(p_path, &message);

        if (IOTX_SUCCESS != ret) {

            return ret;

        }

        CCoAPUintOption_add(&message, COAP_OPTION_CONTENT_FORMAT, COAP_CT_APP_JSON);
	CCoAPUintOption_add(&message, COAP_OPTION_ACCEPT, COAP_CT_APP_JSON);
        CCoAPMessagePayload_set(&message, buff, len);
        ret = CCoAPMessage_send(context, &message);
        CCoAPMessage_destory(&message);
	
	ret = CCoAPMessage_recv(context, CONFIG_COAP_AUTH_TIMEOUT, 2);
        return ret;
}

int CCoAPClient_Request(const char *uri, 
			int port,
			const char *path,
			unsigned char *buff,
			unsigned int len,
			CCoAPRespMsgHandler callback)
{
	int ret;
	char URI[128] = {0};
	sprintf(URI,"coap://%s:%d",uri,port);
	CCoAPContext *context = CCoAPclient_init(URI);
	
	ret = CCoAPClient_send(context,callback,path,buff,len);
	
	CCoAPClient_deinit(context);

	return ret;
}
