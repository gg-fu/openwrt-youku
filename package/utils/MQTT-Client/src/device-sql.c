#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "device-sql.h"
#include "DaemonLog.h"
#include "DeviceList.h"

static sqlite3 *pDevicedb = NULL;
int SQL_Open()
{
	int ret = sqlite3_open(DEVICEDB,&pDevicedb);
	if(ret){
		DBG_vPrintf(DBG_MQTTC, "sqlite3_open:%s failed!!!\n",DEVICEDB);
		sqlite3_close(pDevicedb);
		return -1;
	}
	return 0;
}

int SQL_Create()
{
	int ret;
	char *zMsgErr = 0;
	const char *device_sql = "CREATE TABLE IF NOT EXISTS device_base(deviceId VARCHAR(16) PRIMARY KEY,deviceType INTERGER,deviceKind INTERGER,channel INTERGER);";
	ret = sqlite3_exec(pDevicedb,device_sql,0,0,&zMsgErr);
	if(ret != SQLITE_OK){
		DBG_vPrintf(DBG_MQTTC, "sqlite3_create device_base failed:%s\n",zMsgErr);
		return -1;
	}
	return 0;
}

int SQL_deleteBy_deviceId(const char *deviceId)
{
	int ret;
	char *zMsgErr = 0;
	char buf[128] = {0};
	sprintf(buf,"DELETE FROM device_base WHERE deviceId='%s'",deviceId);
	
	ret = sqlite3_exec(pDevicedb,buf,0,0,&zMsgErr);
	if(ret != SQLITE_OK){
		DBG_vPrintf(DBG_MQTTC, "sql delete %s failed:%s\n",deviceId,zMsgErr);
		return -1;
	}
	
	return 0;
}

int SQL_Insert(IotFormat *data)
{
	int ret;
	char *zMsgErr = 0;
	char buf[128] = {0};
	sprintf(buf,"INSERT INTO device_base VALUES('%s',%d,%d,%d);",data->deviceId,data->deviceType,data->deviceKind,data->channel);
	
	ret = sqlite3_exec(pDevicedb,buf,0,0,&zMsgErr);
	if(ret != SQLITE_OK){
		DBG_vPrintf(DBG_MQTTC, "sql insert %s failed:%s\n",data->deviceId,zMsgErr);
		return -1;
	}
	return 0;
}

int SQL_Select()
{
	int i;
	int nrows = 0;
	int ncolumns = 0;
	char *zMsgErr = 0;
	char **pResult;
	char *sql = "SELECT * FROM device_base";
	IotFormat data = {0};
	
	int ret = sqlite3_get_table( pDevicedb , sql , &pResult , &nrows , &ncolumns , &zMsgErr ); 
	if( ret != SQLITE_OK){
		DBG_vPrintf(DBG_MQTTC, "sql select failed:%s\n",zMsgErr);
		return -1;
	}
	for(i = 0; i < nrows; i++){
		memset(&data,0,sizeof(IotFormat));
		strcpy(data.deviceId,pResult[i][0]);
		data.deviceType = pResult[i][1];
		data.deviceKind = pResult[i][2];
		data.channel = pResult[i][3];
		Add_ListNode(data);	
	}
	sqlite3_free_table( pResult ); 
	return 0;
}

int SQL_Init()
{
	int ret;
	ret = SQL_Open();
	ret = SQL_Create();
	ret = SQL_Select();
	return ret;
}
