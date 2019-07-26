#include "UbusTransform.h"
#include "DaemonLog.h"
#include "device_order.h"
#include "PubSub.h"
#include "cJSON.h"
#include <signal.h>
static int id;
static struct ubus_context *ctx;
 
void Ubus_create_pthread(void *(*func)(void *),void *arg)
{
        pthread_t thread;
        pthread_attr_t attr;
        int ret;
        pthread_attr_init(&attr);
 
        ret = pthread_create(&thread,&attr,func,arg);
        if (ret!=0)
        {
                DBG_vPrintf(DBG_MQTTC,"pthread_create failed\n");
                exit(1);
        }
}
 
enum {
        deviceId,
        meteId,
        value,
        __CONTROL_MAX
};
 
static const struct blobmsg_policy control_policy[__CONTROL_MAX] = {
  [deviceId] = { .name = "deviceId", .type = BLOBMSG_TYPE_STRING},
  [meteId] = { .name = "meteId", .type = BLOBMSG_TYPE_STRING},
  [value] = { .name = "value", .type = BLOBMSG_TYPE_INT8},
};
 
enum{
	ID,
	result,
	__REQ_MAX
}; 
static const struct blobmsg_policy request_policy[__CONTROL_MAX] = {
  [ID] = { .name = "ID", .type = BLOBMSG_TYPE_INT8},
  [result] = { .name = "result", .type = BLOBMSG_TYPE_INT8},
};
static int Ubus_Recv(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req,
                const char *method, struct blob_attr *msg)
{
	struct blob_attr *tb[__REQ_MAX];
	int msgID =blobmsg_get_u32(blob_data(msg));
	switch(msgID)
	{
		case DEV_CONTROL_REP:
        		DBG_vPrintf(DBG_MQTTC,"DEV_CONTROL_REP\n");
			break;
		case DEV_DELETE_REP:
        		DBG_vPrintf(DBG_MQTTC,"DEV_DELETE_REP\n");
			break;
		case DEV_SET_AUTO_REP:
        		DBG_vPrintf(DBG_MQTTC,"DEV_SET_AUTO_REP\n");
			break;
		case DEV_SET_SCENE_REP:
        		DBG_vPrintf(DBG_MQTTC,"DEV_SET_SCENE_REP\n");
			break;
		case DEV_MULTICONTROL_REP:
        		DBG_vPrintf(DBG_MQTTC,"DEV_MULTICONTROL_REP\n");
			break;
			
		default:
			break;
	}
        /*int ID,result;
 	char buf[64] = {0};
        blobmsg_parse(request_policy, __REQ_MAX, tb, blob_data(msg), blob_len(msg));
 
        if (!tb[ID] || !tb[result])
                return UBUS_STATUS_INVALID_ARGUMENT;
 
        ID = blobmsg_get_u8(tb[ID]);
        result = blobmsg_get_u8(tb[result]);
	sprintf(buf,"{\"ID\":%d,\"result\":%d}",ID,result);
	Publish_Message(RESPOND,buf,strlen(buf));*/
 	return 0;
}
 
void Ubus_handle_remove(struct ubus_context *ctx,
                          struct ubus_subscriber *obj, uint32_t id)
{
        DBG_vPrintf(DBG_MQTTC,"remove .......\n");
	exit(0);
}
 
void *Ubus_thread(void *arg)
{
        int ret;
	struct ubus_subscriber event;
	event.cb = Ubus_Recv;
        event.remove_cb = Ubus_handle_remove;
        ret = ubus_register_subscriber(ctx,&event);
        if (ret){
        	DBG_vPrintf(DBG_MQTTC,"ubus_register_subscriber failed\n");	
        }
        if (ubus_lookup_id(ctx, "CoAP", &id)) {
                DBG_vPrintf(DBG_MQTTC,"ubus_loojup_id failed \n");
        }
        ret = ubus_subscribe(ctx, &event, id);
        if(ret)
                DBG_vPrintf(DBG_MQTTC,"Failed to subscribe: %s\n", ubus_strerror(ret));
        uloop_run();
	ubus_free(ctx);
        uloop_done();
}
 
static void Ubus_report_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
        DBG_vPrintf(DBG_MQTTC,"report_cb\n");
}
int Ubus_send_control(const char *deviceId,const char *meteId,int value)
{
 
	struct blob_buf b;
        blobmsg_buf_init(&b);
        blobmsg_add_string(&b, "deviceId",deviceId);
        blobmsg_add_string(&b, "meteId",meteId);
        blobmsg_add_u8(&b,"value",value);
 
        ubus_invoke(ctx, id, "control", b.head, Ubus_report_cb, 0, 500);
}

int Ubus_send_delete(const char *deviceId)
{
 
	struct blob_buf b;
        blobmsg_buf_init(&b);
        blobmsg_add_string(&b, "deviceId",deviceId);
 
        ubus_invoke(ctx, id, "delete", b.head, Ubus_report_cb, 0, 500);
}

void Ubus_thread_main(void)
{
        const char *socket = "/var/run/ubus.sock";
        uloop_init();
 
        signal(SIGPIPE,SIG_IGN);
        ctx = ubus_connect(socket);
        if(!ctx)
        {
               DBG_vPrintf(DBG_MQTTC,"ubus connect failed\n");
               return;
        }
        ubus_add_uloop(ctx);
        Ubus_create_pthread(Ubus_thread,NULL);
}
