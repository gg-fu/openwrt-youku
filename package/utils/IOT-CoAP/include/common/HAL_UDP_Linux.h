#ifndef _HAL_UDP_LINUX_H
#define _HAL_UDP_LINUX_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <inttypes.h>

#define NETWORK_ADDR_LEN        (16)    /* UDP网络地址的长度 */
typedef struct _network_addr_t {
    unsigned char
    addr[NETWORK_ADDR_LEN];         /* 目标UDP主机地址, 点分十进制IP地址 */
    unsigned short  port;           /* 目标UDP端口, 范围是0-65535 */
} NetworkAddr;


/**
 * @brief   创建一个本地的UDP socket, 但并不发起任何网络交互
 *
 * @param   host : UDP的源地址, 如果不指定地址，设为 NULL
 *          port : UDP的源端口
 *
 * @retval  -1 : 创建失败
 * @retval  其它 : 创建成功, 返回值是UDP socket的句柄
 */
intptr_t HAL_UDP_create_without_connect(const char *host, unsigned short port);
/**
 * @brief   设置UDP socket的目的地址和目的端口
 *
 * @param   host :  UDP的目的地址
 *          port : UDP的目的端口
 *
 * @retval  -1 : 失败
 * @retval  0 : 设置成功
 */
int HAL_UDP_connect(intptr_t sockfd,
                    const char *host,
                    unsigned short port);

/**
 * @brief   在指定的UDP socket上发送指定缓冲区的指定长度, 阻塞时间不超过指定时长, 且指定长度若发送完需提前返回
 *
 * @param   sockfd : UDP socket的句柄
 * @param   p_remote : 目标网络地址结构体的首地址
 * @param   p_data : 被发送的缓冲区起始地址
 * @param   datalen: 被发送的数据长度, 单位是字节(Byte)
 * @param   timeout_ms : 可能阻塞的最大时间长度, 单位是毫秒
 *
 * @retval  < 0 : 发送过程中出现错误或异常
 * @retval  0 : 在指定的'timeout_ms'时间间隔内, 没有任何数据被成功发送
 * @retval  (0, len] : 在指定的'timeout_ms'时间间隔内, 被成功发送的数据长度, 单位是字节(Byte)
 */
int HAL_UDP_sendto(intptr_t          sockfd,
                   const NetworkAddr *p_remote,
                   const unsigned char *p_data,
                   unsigned int datalen,
                   unsigned int timeout_ms);
/**
 * @brief   在指定的UDP socket上发送指定缓冲区的指定长度, 阻塞时间不超过指定时长, 且指定长度若发送完需提前返回
 * @param   sockfd : UDP socket的句柄
 * @param   p_data : 被发送的缓冲区起始地址
 * @param   datalen: 被发送的数据长度, 单位是字节(Byte)
 * @param   timeout_ms : 可能阻塞的最大时间长度, 单位是毫秒
 *
 * @retval  < 0 : 发送过程中出现错误或异常
 * @retval  0 : 在指定的'timeout_ms'时间间隔内, 没有任何数据被成功发送
 * @retval  (0, len] : 在指定的'timeout_ms'时间间隔内, 被成功发送的数据长度, 单位是字节(Byte)
 *
 * @note    调用该接口之前需要调用HAL_UDP_connect设置好目的地址和端口。
 */
int HAL_UDP_send(intptr_t sockfd,
                 const unsigned char *p_data,
                 unsigned int datalen,
                 unsigned int timeout_ms);
/**
 * @brief   从指定的UDP句柄接收指定长度数据到缓冲区, 阻塞时间不超过指定时长, 且指定长度若接收完需提前返回, 源地址保存在出参中
 *
 * @param   fd : UDP socket的句柄
 * @param   p_remote : 存放源网络地址的结构体首地址
 * @param   p_data : 存放被接收数据的缓冲区起始地址
 * @param   datalen : 接收并存放到缓冲区中数据的最大长度, 单位是字节(Byte)
 * @param   timeout_ms : 可能阻塞的最大时间长度, 单位是毫秒
 *
 * @retval  < 0 : 接收过程中出现错误或异常
 * @retval  0 : 在指定的'timeout_ms'时间间隔内, 没有任何数据被成功接收
 * @retval  (0, len] : 在指定的'timeout_ms'时间间隔内, 被成功接收的数据长度, 单位是字节(Byte)
 */
int HAL_UDP_recvfrom(intptr_t sockfd,
                     NetworkAddr *p_remote,
                     unsigned char *p_data,
                     unsigned int datalen,
                     unsigned int timeout_ms);
/**
* @brief   从指定的UDP句柄接收指定长度数据到缓冲区, 阻塞时间不超过指定时长, 且指定长度若接收完需提前返回, 源地址保存在出参中
*          调用该接口之前需要调用HAL_UDP_connect设置好目的地址和端口。
* @param   fd : UDP socket的句柄
* @param   p_data : 存放被接收数据的缓冲区起始地址
* @param   datalen : 接收并存放到缓冲区中数据的最大长度, 单位是字节(Byte)
* @param   timeout_ms : 可能阻塞的最大时间长度, 单位是毫秒
*
* @retval  < 0 : 接收过程中出现错误或异常
* @retval  0 : 在指定的'timeout_ms'时间间隔内, 没有任何数据被成功接收
* @retval  (0, len] : 在指定的'timeout_ms'时间间隔内, 被成功接收的数据长度, 单位是字节(Byte)
*/
int HAL_UDP_recv(intptr_t sockfd,
                 unsigned char *p_data,
                 unsigned int datalen,
                 unsigned int timeout_ms);

/**
 * @brief   在指定的UDP socket上发送加入组播组的请求
 *
 * @param   sockfd : 指定用来发送组播请求的UDP socket
 * @param   p_group : 指定需要加入的组播组名字
 * @retval  < 0 : 发送过程中出现异常或失败
 * @retval  0 : 发送成功
 */
int HAL_UDP_joinmulticast(intptr_t sockfd,
                          		char *p_group);
/**
 * @brief   绑定UDP socket到指定接口，只接收来自该接口的数据包
 *
 * @param   fd : 指定用来绑定的UDP socket
 * @param   ifname : 指定用来绑定socket的网络接口名字
 *
 * @retval  < 0 : 绑定异常或失败
 * @retval  0 : 发送成功
 */
int HAL_UDP_bindtodevice(intptr_t fd,
                              const char *ifname);
/**
 * @brief   销毁指定的UDP socket, 回收资源
 *
 * @param   sockfd : 将要关闭并销毁的UDP socket
 *
 * @return  操作的结果
 * @retval  < 0 : 操作失败
 * @retval  0 : 操作成功
 */
int HAL_UDP_close_without_connect(intptr_t sockfd);
int HAL_UDP_write(intptr_t p_socket,
                  const unsigned char *p_data,
                  unsigned int datalen);
intptr_t   HAL_UDP_create(char *host, unsigned short port);
int HAL_UDP_readTimeout(intptr_t p_socket,

                        unsigned char *p_data,

                        unsigned int datalen,

                        unsigned int timeout);
#if defined(__cplusplus)
}
#endif
#endif
