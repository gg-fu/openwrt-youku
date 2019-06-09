#include "PubSub.h"
static int finish_flag=0; 
static int NUM = 0;
static char * const *TOPIC=NULL;
static const char *Rpassword=NULL;
static const char *Rusername=NULL;

void onDisconnect(void* context, MQTTAsync_successData* response)
{
	printf("Successful disconnection\n");
}

void ConSuccess(void* context, MQTTAsync_successData* response)
{
	printf("connect success\n");
	finish_flag = 1;
}

void ReconSuccess(void* context, MQTTAsync_successData* response)
{
	MQTTAsync client = (MQTTAsync)context;
	int ret = Subscribe_topic(client,TOPIC,NUM);
	if(ret)
	{
		fprintf(stderr,"Fail to ReSubscribe,return code %d\n",ret);
		return ;
	}
	printf("Reconnect success\n");
}
void onReConnect(void* context, MQTTAsync_failureData* response)
{
	MQTTAsync client = (MQTTAsync)context;
	MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
	int rc;
	printf("Reconnecting\n");

	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	conn_opts.username = Rusername;
	conn_opts.password = Rpassword;
	conn_opts.onSuccess = ReconSuccess;
	conn_opts.onFailure = onReConnect;
	conn_opts.context = client;
	if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
		fprintf(stderr,"Failed to start reconnect, return code %d\n", rc);
	sleep(3);
}

void connlost(void *context, char *cause)
{
	MQTTAsync client = (MQTTAsync)context;
	MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
	int rc;
	printf("Reconnecting\n");

	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	conn_opts.username = Rusername;
	conn_opts.password = Rpassword;
	conn_opts.onFailure = onReConnect;
	conn_opts.context = client;
	if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
		fprintf(stderr,"Failed to reconnect, return code %d\n", rc);
}

int Connect(MQTTAsync *client,const char *MQTTAddress,
const char *CLIENTID,const char *username,const char *password,MQTTAsync_messageArrived* ma)
{
	int rc,i=0;
	char ADDRESS[ADDRESS_LEN]={0};
	sprintf(ADDRESS,"tcp://%s",MQTTAddress);
	MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
	
	MQTTAsync_create(client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
	MQTTAsync_setCallbacks(*client, *client, connlost, ma, NULL);
	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	conn_opts.username = username;
	conn_opts.password = password;
	conn_opts.onSuccess = ConSuccess;
	//conn_opts.onFailure = ConFailure;
	conn_opts.context = *client;
	
	Rpassword = password;
	Rusername = username;
	if ((rc = MQTTAsync_connect(*client, &conn_opts)) != MQTTASYNC_SUCCESS)
	{
		fprintf(stderr,"Failed to start connect, return code %d\n", rc);
		return -1;
	}
	for (;i<300;i++){ 
		if(finish_flag) break;
		else usleep(10000);
		}
	if (i == 300) return -1;
	return 0;
}

int Publish_Message(MQTTAsync client,const char *topic,char buf[],int size)
{
	int rc;
	MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
	MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
	opts.onSuccess = NULL;
	opts.context = client;

	pubmsg.payload = buf;
	pubmsg.payloadlen = size;
	pubmsg.qos = QOS;
	pubmsg.retained = 0;
	if ((rc=MQTTAsync_sendMessage(client, topic, &pubmsg, &opts))!= MQTTASYNC_SUCCESS)
	{
		fprintf(stderr,"Failed to start sendMessage, return code %d\n", rc);
		return -1;
	}
	return 0;
}

int Subscribe_topic(MQTTAsync client,char * const *topics,int num)
{
	MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
	int rc;
	
	NUM = num;
	TOPIC = topics;
	opts.onSuccess = NULL;
	opts.onFailure = NULL;
	opts.context = client;
	int qos[TOPIC_MAXNUM]={(int)QOS};
	if(num > TOPIC_MAXNUM)
	{
		fprintf(stderr,"Sorry the number of topics is less than 1024");
		num = TOPIC_MAXNUM;
	}
	if ((rc = MQTTAsync_subscribeMany(client, num,topics, qos, &opts)) != MQTTASYNC_SUCCESS)
	{
		fprintf(stderr,"Failed to start subscribe, return code %d", rc);
		return rc;
	}
	return 0;
}
void disConnect(MQTTAsync client,char * const *topics,int num)
{
	MQTTAsync_disconnectOptions disc_opts = MQTTAsync_disconnectOptions_initializer;
	disc_opts.onSuccess = onDisconnect;
	MQTTAsync_unsubscribeMany(client,num,topics,NULL);
	MQTTAsync_disconnect(client, &disc_opts);
	MQTTAsync_destroy(&client);
}



