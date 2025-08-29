/*
 *  log.c
 *
 *  Created on: Aug 28, 2025
 */

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "main.h"
#include "app_config.h"
#include "log.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/



/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/



/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

extern UART_HandleTypeDef huart1;

/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/



/******************************************************************************/

void log_printf(const char *format, ...)
{
    uint8_t log_line[LOG_LINE_MAX];
    int index = 0;
    va_list args;

    va_start(args, format);
    index += vsprintf((char *)&log_line[0], format, args);
    va_end(args);

    HAL_UART_Transmit(&huart1, log_line, index, 1000);
}

void log_printf_hex(uint8_t *buffs, int length)
{
    uint8_t *d;
    int r;
    int idx;
    char line[128];

    for (int i = 0; i < length; i += 16)
    {
        d = &buffs[i];
        r = length - i;
        if (r > 16)
        {
            r = 16;
        }

        idx = 0;
        memset(line, 0, sizeof(line));
        for (int j = 0; j < 16; j++)
        {
            if (j < r)
            {
                idx += sprintf(&line[idx], "%02x ", d[j]);
            }
            else
            {
                line[idx++] = ' ';
                line[idx++] = ' ';
                line[idx++] = ' ';
            }
        }
        line[idx++] = ' ';
        line[idx++] = ' ';
        for (int j = 0; j < r; j++)
        {
            if (d[j] < ' ' || d[j] > '~')
            {
                line[idx++] = '.';
            }
            else
            {
                idx += sprintf(&line[idx], "%c", d[j]);
            }
        }
        log_printf("%s\r\n", line);
    }
    log_printf("%s", "\r\n");
}
