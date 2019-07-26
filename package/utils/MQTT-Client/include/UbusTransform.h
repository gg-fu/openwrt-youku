#ifndef _UBUSTRANSFORM_H
#define _UBUSTRANSFORM_H


#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <libubus.h>
#include <libubox/blobmsg_json.h>

void Ubus_create_pthread(void *(*func)(void *),void *arg);
int Ubus_send_control(const char *deviceId,const char *meteId,int value);
int Ubus_send_delete(const char *deviceId);
void Ubus_thread_main(void);

#endif
