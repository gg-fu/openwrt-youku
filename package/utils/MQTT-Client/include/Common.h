#ifndef _COMMON_H
#define _COMMON_H
#include <libubox/list.h>
#include <stdbool.h>
#define REPORT  "server/report"
#define RESPOND "server/respond" 
#define LEAVE   "server/leave" 
#define JOIN    "server/join" 
#define REPORT_FIFO "/var/run/report_fifo"

	
enum MdevOrder{
        DEV_CONTROL             =101,
        DEV_CONTROL_REP         =102,
        DEV_DELETE              =103,
        DEV_DELETE_REP          =104,
        DEV_SET_AUTO            =105,
        DEV_SET_AUTO_REP        =106,
        DEV_SET_SCENE           =107,
        DEV_SET_SCENE_REP       =108,
        DEV_MULTICONTROL        =109,
        DEV_MULTICONTROL_REP    =110,

        DEV_REPORT              =201,
        DEV_LEAVE               =202,
        DEV_JOIN                =203
};


#define ID_LEN 17
typedef struct
{
        int meteType;
        char value;
        char meteId[ID_LEN-2];
}meteNode;
typedef struct
{
        bool online;
        int channel;
        int deviceType;
        int deviceKind;
        char deviceId[ID_LEN];
        meteNode *metes;
}devNode;

typedef struct
{
        devNode   dev;
        struct list_head list;
}devList;

typedef struct{
        int msgId;
	int deviceType;
	int deviceKind;
        int channel;
        char value;
        char deviceId[ID_LEN];
        char meteId[ID_LEN-2];
}IotFormat;

#endif
