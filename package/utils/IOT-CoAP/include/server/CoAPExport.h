/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef _COAPEXPORT_H
#define _COAPEXPORT_H

#include "CoAPNetwork.h"
#include "lite-list.h"
#include "CoAPCommon.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
typedef enum {
    COAP_REQUEST_SUCCESS,
    COAP_RECV_RESP_TIMEOUT,
} CoAPReqResult;
typedef struct {
    int len;
    unsigned char *data;
} CoAPLenString;
typedef struct {
    unsigned char                  version   : 2;
    unsigned char                  type      : 2;
    unsigned char                  tokenlen  : 4;
    unsigned char                  code;
    unsigned short                 msgid;
} CoAPMsgHeader;

typedef struct {
    unsigned short num;
    unsigned short len;
    unsigned char *val;
} CoAPMsgOption;
typedef void  CoAPContext;
typedef struct CoAPMessage  CoAPMessage;
typedef void (*CoAPSendMsgHandler)(CoAPContext *context, CoAPReqResult result, void *userdata, NetworkAddr *remote,
                                   CoAPMessage *message);
typedef void (*CoAPEventNotifier)(unsigned int event, NetworkAddr *remote, void *message);
typedef void (*CoAPRecvMsgHandler) (CoAPContext *context, const char *paths, NetworkAddr *remote, CoAPMessage *message);
typedef int (*CoAPDataEncrypt)(CoAPContext *context, const char *paths, NetworkAddr *addr, CoAPMessage *message,
                               CoAPLenString *src, CoAPLenString *dest);
struct CoAPMessage {
    CoAPMsgHeader   header;
    unsigned char   token[COAP_MSG_MAX_TOKEN_LEN];
    CoAPMsgOption   options[COAP_MSG_MAX_OPTION_NUM];
    unsigned char   optcount;
    unsigned char   optdelta;
    unsigned short  payloadlen;
    unsigned char  *payload;
    CoAPSendMsgHandler handler;
    void           *user;
    int             keep;
};
typedef struct {
    unsigned char        send_maxcount;  /*list maximal count*/
    unsigned char        obs_maxcount;   /*observe maximal count*/
    unsigned short       port;           /* Local port */
    char                 *group;         /* Multicast address */
    unsigned int         waittime;
    CoAPEventNotifier    notifier;
    void                 *appdata;
    unsigned char        res_maxcount;
} CoAPInitParam;

CoAPContext *CoAPContext_create(CoAPInitParam *param);
void CoAPContext_free(CoAPContext *context);
void *CoAPContextAppdata_get(CoAPContext *context);
/* CoAP message options APIs*/
extern int CoAPStrOption_add(CoAPMessage *message, unsigned short optnum,
                             unsigned char *data, unsigned short datalen);
extern int CoAPStrOption_get(CoAPMessage *message, unsigned short optnum,
                             unsigned char *data, unsigned short *datalen);
extern int CoAPUintOption_add(CoAPMessage *message, unsigned short  optnum,
                              unsigned int data);
extern int CoAPUintOption_get(CoAPMessage *message,
                              unsigned short  optnum,
                              unsigned int *data);
extern int CoAPOption_present(CoAPMessage *message, unsigned short option);

/*CoAP Message APIs*/
extern unsigned short CoAPMessageId_gen(CoAPContext *context);
extern int CoAPMessageId_set(CoAPMessage *message, unsigned short msgid);
extern int CoAPMessageType_set(CoAPMessage *message, unsigned char type);
extern int CoAPMessageCode_set(CoAPMessage *message, CoAPMessageCode code);
extern int CoAPMessageCode_get(CoAPMessage *message, CoAPMessageCode *code);
extern int CoAPMessageToken_set(CoAPMessage *message, unsigned char *token,
                                 unsigned char tokenlen);
extern int CoAPMessageUserData_set(CoAPMessage *message, void *userdata);
extern int CoAPMessageKeep_Set(CoAPMessage *message, int keep);
extern int CoAPMessagePayload_set(CoAPMessage *message, unsigned char *payload,
                                  unsigned short payloadlen);
extern int CoAPMessageHandler_set(CoAPMessage *message, CoAPSendMsgHandler handler);
extern int CoAPMessage_init(CoAPMessage *message);
extern int CoAPMessage_destory(CoAPMessage *message);
extern int CoAPMessage_send(CoAPContext *context, NetworkAddr *remote, CoAPMessage *message);
extern int CoAPMessage_process(CoAPContext *context, unsigned int timeout);
extern int CoAPMessage_retransmit(CoAPContext *context);
extern int CoAPMessage_cycle(CoAPContext *context);
extern int CoAPMessage_cancel(CoAPContext *context, CoAPMessage *message);
extern int CoAPMessageId_cancel(CoAPContext *context, unsigned short msgid);
extern void CoAPMessage_dump(NetworkAddr *remote, CoAPMessage *message);
/*CoAP Resource APIs*/
extern int CoAPResource_register(CoAPContext *context, const char *path,
                                 unsigned short permission, unsigned int ctype,
                                 unsigned int maxage, CoAPRecvMsgHandler callback);
/*CoAP observe APIs*/
extern int CoAPObsServer_add(CoAPContext *context, const char *path, NetworkAddr *remote, CoAPMessage *request);
extern int CoAPObsServer_notify(CoAPContext *context,
                                const char *path, unsigned char *payload,
                                unsigned short payloadlen, CoAPDataEncrypt handler);


CoAPContext *CoAPContext_create(CoAPInitParam *param);
void CoAPContext_free(CoAPContext *p_ctx);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
