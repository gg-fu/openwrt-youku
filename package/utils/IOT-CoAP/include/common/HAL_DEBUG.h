#ifndef _HAL_DEBUG_H
#define _HAL_DEBUG_H
#include "log.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define hal_emerg(...)    log_emerg("hal", __VA_ARGS__)
#define hal_crit(...)     log_crit("hal", __VA_ARGS__)
#define hal_err(...)      log_err("hal", __VA_ARGS__)
#define hal_warning(...)  log_warning("hal", __VA_ARGS__)
#define hal_info(...)     log_info("hal", __VA_ARGS__)
#define hal_debug(...)    log_debug("hal", __VA_ARGS__)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

