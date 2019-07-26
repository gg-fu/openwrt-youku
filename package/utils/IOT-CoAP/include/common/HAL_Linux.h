#ifndef _HAL_LINUX_H
#define _HAL_LINUX_H
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <inttypes.h>

#include "log.h"
#include "HAL_UDP_Linux.h"
#define REBOOT_CMD "reboot"
#define ROUTER_INFO_PATH        "/proc/net/route"
#define ROUTER_RECORD_SIZE      256

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    os_thread_priority_idle = -3,        /* priority: idle (lowest) */
    os_thread_priority_low = -2,         /* priority: low */
    os_thread_priority_belowNormal = -1, /* priority: below normal */
    os_thread_priority_normal = 0,       /* priority: normal (default) */
    os_thread_priority_aboveNormal = 1,  /* priority: above normal */
    os_thread_priority_high = 2,         /* priority: high */
    os_thread_priority_realtime = 3,     /* priority: realtime (highest) */
    os_thread_priority_error = 0x84,     /* system cannot determine priority or thread has illegal priority */
} hal_os_thread_priority_t;
typedef struct _hal_os_thread {
    hal_os_thread_priority_t priority;     /*initial thread priority */
    void                    *stack_addr;   /* thread stack address malloced by caller, use system stack by . */
    size_t                   stack_size;   /* stack size requirements in bytes; 0 is default stack size */
    int                      detach_state; /* 0: not detached state; otherwise: detached state. */
    char                    *name;         /* thread name. */
} hal_os_thread_param_t;

void HAL_Free(void *ptr);
void HAL_SleepMs(uint32_t ms);
uint64_t HAL_UptimeMs(void);
void *HAL_SemaphoreCreate(void);
void HAL_SemaphoreDestroy(void *sem);
void HAL_SemaphorePost(void *sem);
void HAL_Srandom(uint32_t seed);
uint32_t HAL_Random(uint32_t region);
void *HAL_Malloc(uint32_t size);
void *HAL_MutexCreate(void);
void HAL_MutexLock(void *mutex);
void HAL_MutexLock(void *mutex);
void HAL_MutexUnlock(void *mutex);
char *HAL_GetTimeStr(char *buf, int len);
int HAL_SemaphoreWait(void *sem, uint32_t timeout_ms);


int HAL_ThreadCreate(
            void **thread_handle,
            void *(*work_routine)(void *),
            void *arg,
            hal_os_thread_param_t *hal_os_thread_param,
            int *stack_used);
void HAL_ThreadDetach(void *thread_handle);
void HAL_ThreadDetach(void *thread_handle);
void HAL_ThreadDelete(void *thread_handle);
char *_get_default_routing_ifname(char *ifname, int ifname_size);
uint32_t HAL_Wifi_Get_IP(char ip_str[NETWORK_ADDR_LEN], const char *ifname);

void HAL_Sys_reboot(void);
void HAL_Reboot(void);
static long long os_time_get(void);
void HAL_UTC_Set(long long ms);
long long HAL_UTC_Get(void);
int HAL_Timer_Start(void *timer, int ms);
int HAL_Timer_Stop(void *timer);
int HAL_Timer_Delete(void *timer);
void *HAL_Timer_Create(const char *name, void (*func)(void *), void *user_data);
void HAL_MutexDestroy(void *mutex);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
