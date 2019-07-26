#ifndef _IOTFIFO_H
#define _IOTFIFO_H

#include "Common.h"
int init_report_fifo();
int write_fifo(int fd,IotFormat *data);
int read_fifo(int fd,IotFormat *data);

#endif
