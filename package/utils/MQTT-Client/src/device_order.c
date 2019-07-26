/**************************
*	title:device api
*	autor:bruvin
*	time:2019-6-30
**************************/
#include <sys/types.h>
#include <stdio.h>
#include "DaemonLog.h"
#include "device_order.h"
#include "cJSON.h"
#include "UbusTransform.h"
#include "PubSub.h"
#include "DeviceList.h"
int get_msgId(const char *msg)
{
	cJSON *pJSON  = cJSON_Parse(msg);
        if(!pJSON)	return -1;
        cJSON *msgId = cJSON_GetObjectItem(pJSON,"msgId");
	if(msgId == NULL)
	{
		DBG_vPrintf(DBG_MQTTC,"get msgId is NULL\n");
		return -1;
	}
	return msgId->valueint;
}


int set_auto()
{
	return 0;
}

int set_scene()
{
	return 0;
}

int multicontrol()
{
	return 0;
}

int Device_Control(const char *msg)
{
	cJSON *pJSON  = cJSON_Parse(msg);
        if(!pJSON)	return -1;
        cJSON *deviceId = cJSON_GetObjectItem(pJSON,"deviceId");
	if(deviceId == NULL)
	{
		DBG_vPrintf(DBG_MQTTC,"get deviceId is NULL\n");
		return -1;
	}
        cJSON *meteId = cJSON_GetObjectItem(pJSON,"meteId");
	if(meteId == NULL)
	{
		DBG_vPrintf(DBG_MQTTC,"get meteId is NULL\n");
		return -1;
	}
        cJSON *value = cJSON_GetObjectItem(pJSON,"value");
	if(value == NULL)
	{
		DBG_vPrintf(DBG_MQTTC,"get value is NULL\n");
		return -1;
	}
	int ret = Ubus_send_control(deviceId->valuestring,meteId->valuestring,value->valueint);
	return ret;
}

int Device_Delete(const char *msg)
{	
	cJSON *pJSON  = cJSON_Parse(msg);
        if(!pJSON)	return -1;
        cJSON *deviceId = cJSON_GetObjectItem(pJSON,"deviceId");
	if(deviceId == NULL)
	{
		DBG_vPrintf(DBG_MQTTC,"get deviceId is NULL\n");
		return -1;
	}
	char *devId = deviceId->valuestring;
	int ret = Ubus_send_delete(devId);
	if(!ret)
	{
		DeleteNode_By_DeviceId(devId);
	}	
	return ret;
}

int Device_Report(IotFormat data)
{
	char buf[128]={0};
	if(data.deviceId == NULL && data.meteId == NULL)	
	{
		DBG_vPrintf(DBG_MQTTC,"data is NULL\n");
		return -1;
	}
	sprintf(buf,"{\"deviceId\":%s,\"meteId\":%s,\"value\":%d}",data.deviceId,data.meteId,data.value);
	DBG_vPrintf(DBG_MQTTC,"device status report:%s\n",buf);
	Update_ListNode(data);
	Publish_Message(REPORT,buf,strlen(buf));
	return 0;	
}
int Device_Leave(IotFormat data)
{
	char buf[128]={0};
	if(data.deviceId == NULL)	
	{
		DBG_vPrintf(DBG_MQTTC,"data  is  NULL\n");
		return -1;
	}
	sprintf(buf,"{\"deviceId\":%s}",data.deviceId);
	DBG_vPrintf(DBG_MQTTC,"device leave:%s\n",buf);
	Publish_Message(LEAVE,buf,strlen(buf));
	DeleteNode_By_DeviceId(data.deviceId);
	return 0;	
}
int Device_Join(IotFormat data)
{
	char buf[128]={0};
	if(data.deviceId == NULL || data.meteId == NULL || data.channel < 1)	
	{
		DBG_vPrintf(DBG_MQTTC,"data report has NULL\n");
		return -1;
	}
	sprintf(buf,"{\"deviceId\":%s,\"meteId\":%s,\"channel\":%d}",data.deviceId,data.meteId,data.channel);
	DBG_vPrintf(DBG_MQTTC,"device join:%s\n",buf);
	Publish_Message(JOIN,buf,strlen(buf));
	Add_ListNode(data);
	return 0;	
}
