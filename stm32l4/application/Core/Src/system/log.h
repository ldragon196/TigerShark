/*
 *  log.h
 *
 *  Created on: Nov 5, 2025
 */

/******************************************************************************/

#ifndef _LOG_H_
#define _LOG_H_

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "app_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

#define LLOG_LEVEL_NONE                            0 /*< No log */
#define LLOG_LEVEL_ERR                             1 /*< Errors */
#define LLOG_LEVEL_WARN                            2 /*< Warnings */
#define LLOG_LEVEL_INFO                            3 /*< Basic info */
#define LLOG_LEVEL_DBG                             4 /*< Detailed debug */

#define LOG_LINE_MAX                               512
#define SS_FILE_NAME                               (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOC_SIZE                                   24


#define LOG(newline, level, levelstr, ...)                         \
    do {                                                           \
        if (level <= (LOG_LEVEL)) {                                \
            if (newline) {                                         \
                char loc_str[LOC_SIZE];                            \
                int loc_sz;                                        \
                loc_sz = snprintf(loc_str, sizeof(loc_str), "%s:%d", SS_FILE_NAME, __LINE__); \
                for (int i = loc_sz; i < LOC_SIZE - 1; i++) {      \
                     loc_str[i] = ' ';                             \
                }                                                  \
                loc_str[LOC_SIZE - 1] = '\0';                      \
                log_printf("[%c][%s] ", levelstr, loc_str);        \
            }                                                      \
            log_printf(__VA_ARGS__);                               \
            log_printf("\r\n");                                    \
        }                                                          \
    } while (0)

#define LOG_PRINT                                  log_printf
#define LOG_LEVEL                                  LLOG_LEVEL_INFO
#define LOG_ERR(...)                               LOG(1, LLOG_LEVEL_ERR,  'E', __VA_ARGS__)
#define LOG_WARN(...)                              LOG(1, LLOG_LEVEL_WARN, 'W', __VA_ARGS__)
#define LOG_INFO(...)                              LOG(1, LLOG_LEVEL_INFO, 'I', __VA_ARGS__)
#define LOG_DBG(...)                               LOG(1, LLOG_LEVEL_DBG,  'D', __VA_ARGS__)
#define LOG_RAW(...)                               log_printf(__VA_ARGS__)
#define LOG_HEX(a,b)                               log_printf_hex((a), (b))

/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

void log_printf(const char *format, ...);
void log_printf_hex(uint8_t *buffs, int length);

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _LOG_H_ */
