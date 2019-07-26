#ifndef _DEVICELIST_H
#define _DEVICELIST_H

#include "Common.h"
int show();
int test();
int DeviceList_Init();
int Update_ListNode(IotFormat data);
int Add_ListNode(IotFormat data);
int DeleteNode_By_DeviceId(const char *deviceId);
void  List_Add_Tail(struct list_head *list);
#endif

