/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/time.h>
#include <semaphore.h>
#include <errno.h>
#include <assert.h>
#include <net/if.h>       // struct ifreq
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/reboot.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include "HAL_Linux.h"
#include "HAL_DEBUG.h"
#include "CoAPPlatform.h"
void *HAL_MutexCreate(void)
{
    int err_num;
    pthread_mutex_t *mutex = (pthread_mutex_t *)HAL_Malloc(sizeof(pthread_mutex_t));
    if (NULL == mutex) {
        return NULL;
    }
    if (0 != (err_num = pthread_mutex_init(mutex, NULL))) {
        hal_err("create mutex failed");
        HAL_Free(mutex);
        return NULL;
    }
    return mutex;
}
void HAL_MutexDestroy(void *mutex)
{
    int err_num;
    if (!mutex) {
        hal_warning("mutex want to destroy is NULL!");
        return;
    }
    if (0 != (err_num = pthread_mutex_destroy((pthread_mutex_t *)mutex))) {
        hal_err("destroy mutex failed");
    }
    free(mutex);
}
void HAL_MutexLock(void *mutex)
{
    int err_num;
    if (0 != (err_num = pthread_mutex_lock((pthread_mutex_t *)mutex))) {
        hal_err("lock mutex failed: '%s' (%d)", strerror(err_num), err_num);
    }
}
void HAL_MutexUnlock(void *mutex)
{
    int err_num;
    if (0 != (err_num = pthread_mutex_unlock((pthread_mutex_t *)mutex))) {
        hal_err("unlock mutex failed- '%s' (%d)", strerror(err_num), err_num);
    }
}
void *HAL_Malloc(uint32_t size)
{
    return malloc(size);
}
void HAL_Free(void *ptr)
{
    free(ptr);
}
#ifdef __APPLE__
uint64_t HAL_UptimeMs(void)
{
    struct timeval tv = { 0 };
    uint64_t time_ms;
    gettimeofday(&tv, NULL);
    time_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return time_ms;
}
#else
uint64_t HAL_UptimeMs(void)
{
    uint64_t            time_ms;
    struct timespec     ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    time_ms = ((uint64_t)ts.tv_sec * (uint64_t)1000) + (ts.tv_nsec / 1000 / 1000);
    return time_ms;
}
char *HAL_GetTimeStr(char *buf, int len)
{
    struct timeval tv;
    struct tm      tm;
    int str_len    = 0;
    if (buf == NULL || len < 28) {
        return NULL;
    }
    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &tm);
    strftime(buf, 28, "%m-%d %H:%M:%S", &tm);
    str_len = strlen(buf);
    if (str_len + 3 < len) {
        snprintf(buf + str_len, len, ".%3.3d", (int)(tv.tv_usec) / 1000);
    }
    return buf;
}
#endif
void HAL_SleepMs(uint32_t ms)
{
    usleep(1000 * ms);
}
void HAL_Srandom(uint32_t seed)
{
    srandom(seed);
}
uint32_t HAL_Random(uint32_t region)
{
    return (region > 0) ? (random() % region) : 0;
}

void *HAL_SemaphoreCreate(void)
{
    sem_t *sem = (sem_t *)malloc(sizeof(sem_t));
    if (NULL == sem) {
        return NULL;
    }
    if (0 != sem_init(sem, 0, 0)) {
        free(sem);
        return NULL;
    }
    return sem;
}
void HAL_SemaphoreDestroy(void *sem)
{
    sem_destroy((sem_t *)sem);
    free(sem);
}
void HAL_SemaphorePost(void *sem)
{
    sem_post((sem_t *)sem);
}
int HAL_SemaphoreWait(void *sem, uint32_t timeout_ms)
{
    if (PLATFORM_WAIT_INFINITE == timeout_ms) {
        sem_wait(sem);
        return 0;
    } else {
        struct timespec ts;
        int s;
        /* Restart if interrupted by handler */
        do {
            if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
                return -1;
            }
            s = 0;
            ts.tv_nsec += (timeout_ms % 1000) * 1000000;
            if (ts.tv_nsec >= 1000000000) {
                ts.tv_nsec -= 1000000000;
                s = 1;
            }
            ts.tv_sec += timeout_ms / 1000 + s;
        } while (((s = sem_timedwait(sem, &ts)) != 0) && errno == EINTR);
        return (s == 0) ? 0 : -1;
    }
}
int HAL_ThreadCreate(
            void **thread_handle,
            void *(*work_routine)(void *),
            void *arg,
            hal_os_thread_param_t *hal_os_thread_param,
            int *stack_used)
{
    int ret = -1;
    if (stack_used) {
        *stack_used = 0;
    }
    ret = pthread_create((pthread_t *)thread_handle, NULL, work_routine, arg);
    return ret;
}
void HAL_ThreadDetach(void *thread_handle)
{
    pthread_detach((pthread_t)thread_handle);
}
void HAL_ThreadDelete(void *thread_handle)
{
    if (NULL == thread_handle) {
        pthread_exit(0);
    } else {
        /*main thread delete child thread*/
        pthread_cancel((pthread_t)thread_handle);
        pthread_join((pthread_t)thread_handle, 0);
    }
}

void HAL_Sys_reboot(void)
{
    if (system(REBOOT_CMD)) {
        perror("HAL_Sys_reboot failed");
    }
}

char *_get_default_routing_ifname(char *ifname, int ifname_size)
{
    FILE *fp = NULL;
    char line[ROUTER_RECORD_SIZE] = {0};
    char iface[IFNAMSIZ] = {0};
    char *result = NULL;
    unsigned int destination, gateway, flags, mask;
    unsigned int refCnt, use, metric, mtu, window, irtt;
    fp = fopen(ROUTER_INFO_PATH, "r");
    if (fp == NULL) {
        perror("fopen");
        return result;
    }
    char *buff = fgets(line, sizeof(line), fp);
    if (buff == NULL) {
        perror("fgets");
        goto out;
    }
    while (fgets(line, sizeof(line), fp)) {
        if (11 !=
            sscanf(line, "%s %08x %08x %x %d %d %d %08x %d %d %d",
                   iface, &destination, &gateway, &flags, &refCnt, &use,
                   &metric, &mask, &mtu, &window, &irtt)) {
            perror("sscanf");
            continue;
        }
        /*default route */
        if ((destination == 0) && (mask == 0)) {
            strncpy(ifname, iface, ifname_size - 1);
            result = ifname;
            break;
        }
    }
out:
    if (fp) {
        fclose(fp);
    }
    return result;
}

uint32_t HAL_Wifi_Get_IP(char ip_str[NETWORK_ADDR_LEN], const char *ifname)
{
    struct ifreq ifreq;
    int sock = -1;
    char ifname_buff[IFNAMSIZ] = {0};
    if ((NULL == ifname || strlen(ifname) == 0) &&
        NULL == (ifname = _get_default_routing_ifname(ifname_buff, sizeof(ifname_buff)))) {
        perror("get default routeing ifname");
        return -1;
    }
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return -1;
    }
    ifreq.ifr_addr.sa_family = AF_INET; //ipv4 address
    strncpy(ifreq.ifr_name, ifname, IFNAMSIZ - 1);
    if (ioctl(sock, SIOCGIFADDR, &ifreq) < 0) {
        close(sock);
        perror("ioctl");
        return -1;
    }
    close(sock);
    strncpy(ip_str,
            inet_ntoa(((struct sockaddr_in *)&ifreq.ifr_addr)->sin_addr),
            NETWORK_ADDR_LEN);
    return ((struct sockaddr_in *)&ifreq.ifr_addr)->sin_addr.s_addr;
}

static long long os_time_get(void)
{
    struct timeval tv;
    long long ms;
    gettimeofday(&tv, NULL);
    ms = tv.tv_sec * 1000LL + tv.tv_usec / 1000;
    return ms;
}
static long long delta_time = 0;
void HAL_UTC_Set(long long ms)
{
    delta_time = ms - os_time_get();
}
long long HAL_UTC_Get(void)
{
    return delta_time + os_time_get();
}
void *HAL_Timer_Create(const char *name, void (*func)(void *), void *user_data)
{
    timer_t *timer = NULL;
    struct sigevent ent;
    /* check parameter */
    if (func == NULL) {
        return NULL;
    }
    timer = (timer_t *)malloc(sizeof(time_t));
    /* Init */
    memset(&ent, 0x00, sizeof(struct sigevent));
    /* create a timer */
    ent.sigev_notify = SIGEV_THREAD;
    ent.sigev_notify_function = (void (*)(union sigval))func;
    ent.sigev_value.sival_ptr = user_data;
    printf("HAL_Timer_Create\n");
    if (timer_create(CLOCK_MONOTONIC, &ent, timer) != 0) {
        free(timer);
        return NULL;
    }
    return (void *)timer;
}
int HAL_Timer_Start(void *timer, int ms)
{
    struct itimerspec ts;
    /* check parameter */
    if (timer == NULL) {
        return -1;
    }
    /* it_interval=0: timer run only once */
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;
    /* it_value=0: stop timer */
    ts.it_value.tv_sec = ms / 1000;
    ts.it_value.tv_nsec = (ms % 1000) * 1000;
    return timer_settime(*(timer_t *)timer, 0, &ts, NULL);
}
int HAL_Timer_Stop(void *timer)
{
    struct itimerspec ts;
    /* check parameter */
    if (timer == NULL) {
        return -1;
    }
    /* it_interval=0: timer run only once */
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;
    /* it_value=0: stop timer */
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 0;
    return timer_settime(*(timer_t *)timer, 0, &ts, NULL);
}
int HAL_Timer_Delete(void *timer)
{
    int ret = 0;
    /* check parameter */
    if (timer == NULL) {
        return -1;
    }
    ret = timer_delete(*(timer_t *)timer);
    free(timer);
    return ret;
}
void HAL_Reboot(void)
{
    reboot(0);
}

