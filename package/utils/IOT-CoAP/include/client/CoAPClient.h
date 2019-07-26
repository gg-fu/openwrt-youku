#ifndef _COAPCLIENT_H
#define _COAPCLIENT_H
#include "CCoAPExport.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int CoAPClient_Request(const char *uri,
                        int port,
                        const char *path,
                        unsigned char *buff,
                        unsigned int len,
			CCoAPRespMsgHandler callback);



#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
