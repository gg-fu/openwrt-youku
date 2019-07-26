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

#include <stdio.h>
#include "CCoAPExport.h"
#include "CoAPSerialize.h"
#include "CoAPDeserialize.h"
#include "lite-list.h"

int CCoAPStrOption_add(CCoAPMessage *message, unsigned short optnum, unsigned char *data, unsigned short datalen)
{
    unsigned char *ptr = NULL;
    if (COAP_MSG_MAX_OPTION_NUM <= message->optnum) {
        return COAP_ERROR_INVALID_PARAM;
    }
    message->options[message->optnum].num = optnum - message->optdelta;
    message->options[message->optnum].len = datalen;
    ptr = (unsigned char *)coap_malloc(datalen);
    if (NULL != ptr) {
        memcpy(ptr, data, datalen);
    }
    message->options[message->optnum].val = ptr;
    message->optdelta = optnum;
    message->optnum ++;
    return COAP_SUCCESS;
}
int CCoAPUintOption_add(CCoAPMessage *message, unsigned short  optnum, unsigned int data)
{
    unsigned char *ptr = NULL;
    if (COAP_MSG_MAX_OPTION_NUM <= message->optnum) {
        return COAP_ERROR_INVALID_PARAM;
    }
    message->options[message->optnum].num = optnum - message->optdelta;
    if (0 == data) {
        message->options[message->optnum].len = 0;
    } else if (256 >= data) {
        message->options[message->optnum].len = 1;
        ptr = (unsigned char *)coap_malloc(1);
        if (NULL != ptr) {
            *ptr = (unsigned char)data;
        }
    } else if (65535 >= data) {
        message->options[message->optnum].len = 2;
        ptr  = (unsigned char *)coap_malloc(2);
        if (NULL != ptr) {
            *ptr     = (unsigned char)((data & 0xFF00) >> 8);
            *(ptr + 1) = (unsigned char)(data & 0x00FF);
        }
    } else {
        message->options[message->optnum].len = 4;
        ptr   = (unsigned char *)coap_malloc(4);
        if (NULL != ptr) {
            *ptr     = (unsigned char)((data & 0xFF000000) >> 24);
            *(ptr + 1) = (unsigned char)((data & 0x00FF0000) >> 16);
            *(ptr + 2) = (unsigned char)((data & 0x0000FF00) >> 8);
            *(ptr + 3) = (unsigned char)(data & 0x000000FF);
        }
    }
    message->options[message->optnum].val = ptr;
    message->optdelta = optnum;
    message->optnum   += 1;
    return COAP_SUCCESS;
}
unsigned short CCoAPMessageId_gen(CCoAPContext *context)
{
    unsigned short msg_id = 0;
    msg_id = ((COAP_MAX_MESSAGE_ID == context->message_id)  ? 1 : context->message_id++);
    return msg_id;
}

int CCoAPMessageId_set(CCoAPMessage *message, unsigned short msgid)
{
    if (NULL == message) {
        return COAP_ERROR_NULL;
    }
    message->header.msgid = msgid;
    return COAP_SUCCESS;
}
int CCoAPMessageType_set(CCoAPMessage *message, unsigned char type)
{
    if (NULL == message) {
        return COAP_ERROR_NULL;
    }
    if (COAP_MESSAGE_TYPE_CON != type && COAP_MESSAGE_TYPE_NON != type
        && COAP_MESSAGE_TYPE_ACK != type && COAP_MESSAGE_TYPE_RST != type) {
        return COAP_ERROR_INVALID_PARAM;
    }
    message->header.type = type;
    return COAP_SUCCESS;
}
int CCoAPMessageCode_set(CCoAPMessage *message, CoAPMessageCode code)
{
    if (NULL == message) {
        return COAP_ERROR_NULL;
    }
    message->header.code  = code;
    return COAP_SUCCESS;
}
int CCoAPMessageToken_set(CCoAPMessage *message, unsigned char *token,
                         unsigned char tokenlen)
{
    if (NULL == message || NULL == token) {
        return COAP_ERROR_NULL;
    }
    if (COAP_MSG_MAX_TOKEN_LEN < tokenlen) {
        return COAP_ERROR_INVALID_LENGTH;
    }
    memcpy(message->token, token, tokenlen);
    message->header.tokenlen = tokenlen;
    return COAP_SUCCESS;
}
int CCoAPMessageUserData_set(CCoAPMessage *message, void *userdata)
{
    if (NULL == message || NULL == userdata) {
        return COAP_ERROR_NULL;
    }
    message->user = userdata;
    return COAP_SUCCESS;
}
int CCoAPMessagePayload_set(CCoAPMessage *message, unsigned char *payload,
                           unsigned short payloadlen)
{
    if (NULL == message || (0 < payloadlen && NULL == payload)) {
        return COAP_ERROR_NULL;
    }
    message->payload = payload;
    message->payloadlen = payloadlen;
    return COAP_SUCCESS;
}
int CCoAPMessageHandler_set(CCoAPMessage *message, CCoAPRespMsgHandler handler)
{
    if (NULL == message) {
        return COAP_ERROR_NULL;
    }
    message->handler = handler;
    return COAP_SUCCESS;
}
int CCoAPMessage_init(CCoAPMessage *message)
{
    if (NULL == message) {
        return COAP_ERROR_NULL;
    }
    memset(message, 0x00, sizeof(CoAPMessage));
    message->header.version    = COAP_CUR_VERSION;
    message->header.type       = COAP_MESSAGE_TYPE_ACK;
    message->header.tokenlen   = 0;
    message->header.code       = COAP_MSG_CODE_EMPTY_MESSAGE;
    message->header.msgid      = 0;
    message->payload           = NULL;
    message->payloadlen        = 0;
    message->optnum            = 0;
    message->optdelta          = 0;
    message->handler           = NULL;
    return COAP_SUCCESS;
}
int CCoAPMessage_destory(CCoAPMessage *message)
{
    int count = 0;
    if (NULL == message) {
        return COAP_ERROR_NULL;
    }
    for (count = 0; count < COAP_MSG_MAX_TOKEN_LEN; count++) {
        if (NULL != message->options[count].val) {
            coap_free(message->options[count].val);
            message->options[count].val = NULL;
        }
    }
    return COAP_SUCCESS;
}
static int CCoAPMessageList_add(CCoAPContext *context, CCoAPMessage *message, int len)
{
    CCoAPSendNode *node = NULL;
    node = coap_malloc(sizeof(CCoAPSendNode));
    if (NULL != node) {
        node->acked        = 0;
        node->user         = message->user;
        node->msgid        = message->header.msgid;
        node->handler      = message->handler;
        node->msglen       = len;
        node->timeout_val   = COAP_ACK_TIMEOUT * COAP_ACK_RANDOM_FACTOR;
        if (COAP_MESSAGE_TYPE_CON == message->header.type) {
            node->timeout       = node->timeout_val;
            node->retrans_count = 0;
        } else {
            node->timeout       = COAP_MAX_TRANSMISSION_SPAN;
            node->retrans_count = COAP_MAX_RETRY_COUNT;
        }
        node->tokenlen     = message->header.tokenlen;
        memcpy(node->token, message->token, message->header.tokenlen);
        node->message      = (unsigned char *)coap_malloc(len);
        if (NULL != node->message) {
            memcpy(node->message, context->sendbuf, len);
        }
        if (&context->list.count >= &context->list.maxcount) {
            coap_free(node);
            return -1;
        } else {
            list_add_tail(&node->sendlist, &context->list.sendlist);
            context->list.count ++;
            return 0;
        }
    } else {
        return -1;
    }
}
int CCoAPMessage_send(CCoAPContext *context, CCoAPMessage *message)
{
    unsigned int   ret            = COAP_SUCCESS;
    unsigned short msglen         = 0;
    if (NULL == message || NULL == context) {
        return (COAP_ERROR_INVALID_PARAM);
    }
    /* TODO: get the message length */
    msglen = CoAPSerialize_MessageLength(message);
    if (COAP_MSG_MAX_PDU_LEN < msglen) {
        COAP_INFO("The message length %d is too loog", msglen);
        return COAP_ERROR_DATA_SIZE;
    }
    memset(context->sendbuf, 0x00, COAP_MSG_MAX_PDU_LEN);
    msglen = CoAPSerialize_Message(message, context->sendbuf, COAP_MSG_MAX_PDU_LEN);
    COAP_DEBUG("----The message length %d-----", msglen);

    ret = CCoAPNetwork_write(&context->network, context->sendbuf, (unsigned int)msglen);
    if (COAP_SUCCESS == ret) {
        if (CoAPReqMsg(message->header) || CoAPCONRespMsg(message->header)) {
            COAP_DEBUG("Add message id %d len %d to the list",
                       message->header.msgid, msglen);
            CCoAPMessageList_add(context, message, msglen);
        } else {
            COAP_DEBUG("The message doesn't need to be retransmitted");
        }
    } else {
        COAP_ERR("CoAP transoprt write failed, return %d", ret);
    }
    return ret;
}

static int CCoAPAckMessage_handle(CCoAPContext *context, CCoAPMessage *message)
{
    CCoAPSendNode *node = NULL;
    list_for_each_entry(node, &context->list.sendlist, sendlist, CCoAPSendNode) {
        if (node->msgid == message->header.msgid) {
            node->acked = 1;
            return COAP_SUCCESS;
        }
    }
    return COAP_SUCCESS;
}
static int CCoAPAckMessage_send(CCoAPContext *context, unsigned short msgid)
{
    CCoAPMessage message;
    CCoAPMessage_init(&message);
    CCoAPMessageId_set(&message, msgid);
    return CCoAPMessage_send(context, &message);
}
static int CCoAPRespMessage_handle(CCoAPContext *context, CCoAPMessage *message)
{
    CCoAPSendNode *node = NULL;
    if (COAP_MESSAGE_TYPE_CON == message->header.type) {
        CCoAPAckMessage_send(context, message->header.msgid);
    }

    list_for_each_entry(node, &context->list.sendlist, sendlist, CCoAPSendNode) {
        if (0 != node->tokenlen && node->tokenlen == message->header.tokenlen
            && 0 == memcmp(node->token, message->token, message->header.tokenlen)) {
            COAP_DEBUG("Find the node by token");
            message->user  = node->user;
            if (COAP_MSG_CODE_400_BAD_REQUEST <= message->header.code) {
                /* TODO:i */
                if (NULL != context->notifier) {
                    //context->notifier(message->header.code, message);
                }
            }
            if (NULL != node->handler) {
                node->handler(node->user, message);
            }
            COAP_DEBUG("Remove the message id %d from list", node->msgid);
            list_del_init(&node->sendlist);
            context->list.count--;
            if (NULL != node->message) {
                coap_free(node->message);
            }
            coap_free(node);
            node = NULL;
            return COAP_SUCCESS;
        }
    }
    return COAP_ERROR_NOT_FOUND;
}
static void CCoAPMessage_handle(CCoAPContext *context,
                               unsigned char     *buf,
                               unsigned short      datalen)
{
    int    ret  = COAP_SUCCESS;
    CCoAPMessage     message;
    unsigned char code, msgclass, detail;
    memset(&message, 0x00, sizeof(CCoAPMessage));
    ret = CoAPDeserialize_Message(&message, buf, datalen);
    code = (unsigned char)message.header.code;
    msgclass = code >> 5;
    detail = code & 0x1F;
    COAP_DEBUG("Version     : %d", message.header.version);
    COAP_DEBUG("Code        : %d.%02d(0x%x)", msgclass, detail, code);
    COAP_DEBUG("Type        : 0x%x", message.header.type);
    COAP_DEBUG("Msgid       : %d", message.header.msgid);
    COAP_DEBUG("Option      : %d", message.optnum);
    COAP_DEBUG("Payload Len : %d", message.payloadlen);
    if (COAP_SUCCESS != ret) {
        if (NULL != context->notifier) {
            /* TODO: */
            /* context->notifier(context, event); */
        }
    }
    if (COAPAckMsg(message.header)) {
        COAP_DEBUG("Receive CoAP ACK Message,ID %d", message.header.msgid);
        CCoAPAckMessage_handle(context, &message);
    } else if (CoAPRespMsg(message.header)) {
        COAP_DEBUG("Receive CoAP Response Message,ID %d", message.header.msgid);
        CCoAPRespMessage_handle(context, &message);
    }
}
int CCoAPMessage_recv(CCoAPContext *context, unsigned int timeout, int readcount)
{
    int len = 0;
    int count = readcount;
    while (1) {
        len = CCoAPNetwork_read(&context->network, context->recvbuf,
                               COAP_MSG_MAX_PDU_LEN, timeout);
        if (len > 0) {
            if(0 == readcount){
                CCoAPMessage_handle(context, context->recvbuf, len);
            }
            else{
                count--;
                CCoAPMessage_handle(context, context->recvbuf, len);
                if(0 == count){
                    return len;
                }
            }
        } else {
            return 0;
        }
    }
}
int CCoAPMessage_cycle(CCoAPContext *context)
{
    unsigned int ret = 0;
    CCoAPMessage_recv(context, context->waittime, 0);
    CCoAPSendNode *node = NULL, *next = NULL;
    list_for_each_entry_safe(node, next, &context->list.sendlist, sendlist, CCoAPSendNode) {
        if (NULL != node) {
            if (node->timeout == 0) {
                if (node->retrans_count < COAP_MAX_RETRY_COUNT && (0 == node->acked)) {
                    node->timeout     = node->timeout_val * 2;
                    node->timeout_val = node->timeout;
                    node->retrans_count++;
                    COAP_DEBUG("Retansmit the message id %d len %d", node->msgid, node->msglen);
                    ret = CCoAPNetwork_write(&context->network, node->message, node->msglen);
                    if (ret != COAP_SUCCESS) {
                        if (NULL != context->notifier) {
                            /* TODO: */
                            /* context->notifier(context, event); */
                        }
                    }
                }
                if ((node->timeout > COAP_MAX_TRANSMISSION_SPAN) ||
                    (node->retrans_count >= COAP_MAX_RETRY_COUNT)) {
                    if (NULL != context->notifier) {
                        /* TODO: */
                        /* context->notifier(context, event); */
                    }
                    /*Remove the node from the list*/
                    list_del_init(&node->sendlist);
                    context->list.count--;
                    COAP_INFO("Retransmit timeout,remove the message id %d count %d",
                              node->msgid, context->list.count);
                    coap_free(node->message);
                    coap_free(node);
                }
            }
             else {
                node->timeout--;
            }
        }
    }
    return COAP_SUCCESS;
}

