#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "IotFifo.h"
#include "DaemonLog.h"

int init_report_fifo()
{
	if(access(REPORT_FIFO,F_OK))
	{
		mkfifo(REPORT_FIFO,0644);
	}
	int fd=open(REPORT_FIFO,O_RDWR);
	fcntl( fd, F_SETFL,fcntl(fd,F_GETFL)|O_NONBLOCK);	
	return fd;	
}

int write_fifo(int fd,IotFormat *data)
{
	int n;
again:
	n = write(fd,data,sizeof(IotFormat));
	if(n==0) return -1;
	else if(n==-1)
	{
		if(errno==EINTR)
			goto again;
		else
		{
			DBG_vPrintf(DBG_MQTTC,"write failed\n");	
			return -1;									
		}
	}
	return 0;
}

int read_fifo(int fd,IotFormat *data)
{
	int n;
again:
	n =read(fd,data,sizeof(IotFormat));
	if(n==0) return -1;
	else if(n==-1)
	{
		if(errno==EAGAIN||errno==EINTR)
			goto again;
		else
		{
			DBG_vPrintf(DBG_MQTTC,"read failed\n");	
			return -1;
		}
	}
	return n;
}
