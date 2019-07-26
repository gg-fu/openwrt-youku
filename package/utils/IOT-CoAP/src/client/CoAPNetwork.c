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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "CCoAPExport.h"
#include "CCoAPNetwork.h"
#include "HAL_UDP_Linux.h"

unsigned int CCoAPNetwork_write(coap_network_t *p_network,
                               const unsigned char   *p_data,
                               unsigned int           datalen)
{
    int rc = COAP_ERROR_WRITE_FAILED;
#ifdef COAP_DTLS_SUPPORT
#endif
        rc = HAL_UDP_write((intptr_t)p_network->context, p_data, datalen);
        COAP_DEBUG("[CoAP-NWK]: Network write return %d", rc);
        if (-1 == rc) {
            rc = COAP_ERROR_WRITE_FAILED;
        } else {
            rc = COAP_SUCCESS;
        }
#ifdef COAP_DTLS_SUPPORT
#endif
    return (unsigned int)rc;
}
int CCoAPNetwork_read(coap_network_t *network, unsigned char  *data,
                     unsigned int datalen, unsigned int timeout)
{
    int len = 0;
#ifdef COAP_DTLS_SUPPORT
#endif
        memset(data, 0x00, datalen);
        len = HAL_UDP_readTimeout((intptr_t)network->context,
                                  data, COAP_MSG_MAX_PDU_LEN, timeout);
#ifdef COAP_DTLS_SUPPORT
#endif
    if(len > 0)
        COAP_TRC("<< CoAP recv %d bytes data", len);
    return len;
}
unsigned int CCoAPNetwork_init(const coap_network_init_t *p_param, coap_network_t *p_network)
{
    unsigned int    err_code = COAP_SUCCESS;
    if (NULL == p_param || NULL == p_network) {
        return COAP_ERROR_INVALID_PARAM;
    }
    /* TODO : Parse the url here */
    p_network->ep_type = p_param->ep_type;
#ifdef COAP_DTLS_SUPPORT
#endif
    if (COAP_ENDPOINT_NOSEC == p_param->ep_type
            || COAP_ENDPOINT_PSK == p_param->ep_type) {
        /*Create udp socket*/
        //p_network->context = (void *)HAL_UDP_create_without_connect(p_param->p_host, p_param->port);
	p_network->context = (void *)HAL_UDP_create(p_param->p_host,p_param->port);
        if ((void *) - 1 == p_network->context) {
            return COAP_ERROR_NET_INIT_FAILED;
        }
    }
    return err_code;
}

unsigned int CCoAPNetwork_deinit(coap_network_t *p_network)
{
    unsigned int    err_code = COAP_SUCCESS;
#ifdef COAP_DTLS_SUPPORT
#endif
    if (COAP_ENDPOINT_NOSEC == p_network->ep_type
            || COAP_ENDPOINT_PSK == p_network->ep_type) {
        HAL_UDP_close_without_connect((intptr_t)p_network->context);
    }
    return err_code;
}

