#ifndef __DEVICE_SQL_H
#define __DEVICE_SQL_H

#include "Common.h"
#define DEVICEDB "/etc/IoT/device.db"
int SQL_Open();
int SQL_Create();
int SQL_Select();
int SQL_Init();
int SQL_Insert(IotFormat *data);
int SQL_deleteBy_deviceId(const char *deviceId);


#endif
