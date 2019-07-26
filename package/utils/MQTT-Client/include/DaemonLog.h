#ifndef _DAEMONLOG_H
#define _DAEMONLOG_H

#include <libdaemon/daemon.h>
#define DBG_vPrintf(a,b,ARG...)  do{ if (a) daemon_log(LOG_INFO, "%s:"b,__FUNCTION__,##ARG);}while(0)
#define DBG_MQTTC 1

#endif
