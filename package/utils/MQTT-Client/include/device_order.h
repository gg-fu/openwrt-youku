#ifndef _DEVICE_ORDER_H
#define _DEVICE_ORDER_H
#include "Common.h"
int get_msgId(const char *msg);
int set_auto();
int set_scene();
int multicontrol();
int Device_Control(const char *msg);
int Device_Delete(const char *msg);
int Device_Report(IotFormat data);
int Device_Leave(IotFormat data);
int Device_Join(IotFormat data);
#endif
