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
#ifndef __CCOAP_MESSAGE_H__
#define __CCOAP_MESSAGE_H__
#include "CCoAPExport.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int CCoAPStrOption_add(CCoAPMessage *message, unsigned short optnum,
            unsigned char *data, unsigned short datalen);

int CCoAPUintOption_add(CCoAPMessage *message, unsigned short  optnum,
            unsigned int data);
unsigned short CCoAPMessageId_gen(CCoAPContext *context);
int CCoAPMessageId_set(CCoAPMessage *message, unsigned short msgid);
int CCoAPMessageType_set(CCoAPMessage *message, unsigned char type);
int CCoAPMessageCode_set(CCoAPMessage *message, CoAPMessageCode code);
int CCoAPMessageToken_set(CCoAPMessage *message, unsigned char *token,
        unsigned char tokenlen);
int CCoAPMessageUserData_set(CCoAPMessage *message, void *userdata);
int CCoAPMessagePayload_set(CCoAPMessage *message, unsigned char *payload,
        unsigned short payloadlen);
int CCoAPMessageHandler_set(CCoAPMessage *message, CCoAPRespMsgHandler handler);
int CCoAPMessage_init(CCoAPMessage *message);
int CCoAPMessage_destory(CCoAPMessage *message);
int CCoAPMessage_send(CCoAPContext *context, CCoAPMessage *message);
int CCoAPMessage_recv(CCoAPContext *context, unsigned int timeout, int readcount);
int CCoAPMessage_cycle(CCoAPContext *context);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
