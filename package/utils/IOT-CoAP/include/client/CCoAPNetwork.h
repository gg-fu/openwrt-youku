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
#ifndef __CCoAPNETWORK_H__
#define __CCoAPNETWORK_H__

#include <stdint.h>
#include "HAL_UDP_Linux.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void DTLSContext;
typedef enum
{
    COAP_ENDPOINT_NOSEC = 0,
    COAP_ENDPOINT_DTLS,
    COAP_ENDPOINT_PSK,
}coap_endpoint_type;

typedef struct
{
    DTLSContext         *context;
} coap_remote_session_t;

typedef struct
{
    int                      socket_id;
    coap_endpoint_type       ep_type;
    void                    *context;
}coap_network_t;

typedef struct
{
    coap_endpoint_type       ep_type;
    unsigned char           *p_ca_cert_pem;
    char                    *p_host;
    unsigned short           port;
} coap_network_init_t;

unsigned int CCoAPNetwork_init (const coap_network_init_t   * p_param, coap_network_t *p_network);

unsigned int CCoAPNetwork_write(coap_network_t *p_network,
                                  const unsigned char  * p_data,
                                  unsigned int           datalen);
int CCoAPNetwork_read(coap_network_t *network, unsigned char  *data,
                      unsigned int datalen, unsigned int timeout);
unsigned int CCoAPNetwork_deinit(coap_network_t *p_network);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
