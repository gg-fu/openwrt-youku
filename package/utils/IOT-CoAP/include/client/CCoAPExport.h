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
#ifndef _CCOAPEXPORT_H
#define _CCOAPEXPORT_H

#include "CCoAPNetwork.h"
#include "log.h"
#include "HAL_Linux.h"
#include "lite-list.h"
#include "CoAPCommon.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct
{
    unsigned char                  version   :2;
    unsigned char                  type      :2;
    unsigned char                  tokenlen  :4;
    unsigned char                  code;
    unsigned short                 msgid;
} CCoAPMsgHeader;

typedef struct
{
    unsigned short num;
    unsigned short len;
    unsigned char *val;
}CCoAPMsgOption;
typedef void (*CCoAPRespMsgHandler)(void *data, void *message);
typedef void (*CCoAPEventNotifier)(unsigned int event, void *p_message);
typedef struct
{
    void                    *user;
    unsigned short           msgid;
    char                     acked;
    unsigned char            tokenlen;
    unsigned char            token[8];
    unsigned char            retrans_count;
    unsigned short           timeout;
    unsigned short           timeout_val;
    unsigned char           *message;
    unsigned int             msglen;
    CCoAPRespMsgHandler       handler;
    struct list_head         sendlist;
} CCoAPSendNode;
typedef struct
{
    unsigned char            count;
    unsigned char            maxcount;
    struct list_head         sendlist;
}CCoAPSendList;

typedef struct
{
    CCoAPMsgHeader   header;
    unsigned char   token[COAP_MSG_MAX_TOKEN_LEN];
    CCoAPMsgOption   options[COAP_MSG_MAX_OPTION_NUM];
    unsigned char   optnum;
    unsigned short  optdelta;
    unsigned char  *payload;
    unsigned short  payloadlen;
    CCoAPRespMsgHandler handler;
    void           *user;
}CCoAPMessage;
typedef struct
{
             char       *url;
    unsigned char        maxcount;  /*list maximal count*/
    unsigned int         waittime;
    CCoAPEventNotifier    notifier;
}CCoAPInitParam;
typedef struct
{
    unsigned short           message_id;
    coap_network_t           network;
    CCoAPEventNotifier        notifier;
    unsigned char            *sendbuf;
    unsigned char            *recvbuf;
    CCoAPSendList             list;
    unsigned int             waittime;
}CCoAPContext;
CCoAPContext *CCoAPContext_create(CCoAPInitParam *param);
void CCoAPContext_free(CCoAPContext *p_ctx);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
