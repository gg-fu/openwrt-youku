#include "DeviceList.h"
#include "DaemonLog.h"
#include <stdlib.h>
#include <string.h>
static struct list_head *DEVICE_LIST;

int DeviceList_Init()
{
	DEVICE_LIST =(struct list_head *)malloc(sizeof(struct list_head));
	if(DEVICE_LIST==NULL)
	{
		DBG_vPrintf(DBG_MQTTC, "DEVICE_LIST malloc  failed\n");
		return -1;
	}
	INIT_LIST_HEAD(DEVICE_LIST);

	return 0;
}

int DeleteNode_By_DeviceId(const char *deviceId)
{
	devList *pDev,*tmp;
	
	if(list_empty(DEVICE_LIST))
	{
		DBG_vPrintf(DBG_MQTTC, "list empty\n");
		return -1;
	}
	list_for_each_entry_safe(pDev, tmp, DEVICE_LIST,list){
		if(!strncmp(pDev->dev.deviceId,deviceId,ID_LEN)){
                          list_del(&(pDev->list));
                          free(pDev->dev.metes);
                          free(pDev);
		}
	}
	return 0;
}

void  List_Add_Tail(struct list_head *list)
{
	list_add_tail(list,DEVICE_LIST);
}

int Update_ListNode(IotFormat data)
{
	int i;
	devList *DES;
	if(list_empty(DEVICE_LIST)){
		DBG_vPrintf(DBG_MQTTC, "list empty\n");
		return -1;
	}
	list_for_each_entry(DES, DEVICE_LIST, list){
		if(strncmp(DES->dev.deviceId,data.deviceId,ID_LEN)){
		for(i = 0; i < data.channel; i++){
			if(!strncmp(DES->dev.metes[i].meteId,data.meteId,ID_LEN-2)){
			DES->dev.metes[i].value = data.value;
			return 0;		
			}
		}
		}
	}
	return -1;	
}

int Add_ListNode(IotFormat data)
{
	int i;
	devList *devs = (devList *)malloc(sizeof(devList));
	if(devs == NULL)
	{
		DBG_vPrintf(DBG_MQTTC, "devList malloc failed\n");
		return -1;
	}
	devs->dev.metes = (meteNode *)malloc(data.channel*sizeof(meteNode));
	if(devs->dev.metes == NULL)
	{
		DBG_vPrintf(DBG_MQTTC, "metes malloc failed\n");
		return -1;
	}
	devs->dev.channel = data.channel;
	strcpy(devs->dev.deviceId,data.deviceId);
	for(i = 0; i < data.channel; i++)
	{
		strcpy(devs->dev.metes[i].meteId,data.meteId);
		devs->dev.metes[i].value = 0;
	}
	
	List_Add_Tail(&devs->list);
	return 0;	
}

int test()
{
	devList *devs = (devList *)malloc(sizeof(devList));
	if(devs == NULL)
	{
		DBG_vPrintf(DBG_MQTTC, "devLIst malloc  failed\n");
		return -1;
	}

	devs->dev.metes = (meteNode *)malloc(2*sizeof(meteNode));
	if(devs->dev.metes == NULL)
	{
		DBG_vPrintf(DBG_MQTTC, "meteNode malloc  failed\n");
		return -1;
	}
	devs->dev.online =true;
	devs->dev.channel = 2;
	devs->dev.deviceType = 1234;
	
	strcpy(devs->dev.deviceId,"12345678");
	strcpy(devs->dev.metes[0].meteId,"00000001");	
	strcpy(devs->dev.metes[1].meteId,"00000002");	
	devs->dev.metes[0].value = 0;
	devs->dev.metes[0].meteType =1110;
	devs->dev.metes[1].value = 1;
	devs->dev.metes[1].meteType = 1111;
	
	List_Add_Tail(&devs->list);
}

int show()
{
	devList *DES;
	if(list_empty(DEVICE_LIST))
	{
		DBG_vPrintf(DBG_MQTTC, "list empty\n");
		return -1;
	}
	list_for_each_entry(DES, DEVICE_LIST, list){
		DBG_vPrintf(DBG_MQTTC, "deviceId:%s,meteida:%s,meteid2:%s\n",DES->dev.deviceId,DES->dev.metes[0].meteId,DES->dev.metes[1].meteId);
        }
	return 0;
}
