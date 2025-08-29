/*
 *  system.h
 *
 *  Created on: Aug 29, 2025
 */

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

/******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

#define FLASH_SECTOR_SIZE     (0x800)

#define ETX_APP_FLASH_ADDR    (0x0800A000)
#define ETX_CONFIG_FLASH_ADDR (ETX_APP_FLASH_ADDR - FLASH_SECTOR_SIZE)
#define ETX_APP_NPAGE         ((ETX_APP_FLASH_ADDR - 0x08000000) / FLASH_SECTOR_SIZE)

/* Reboot reason */
#define ETX_FIRST_TIME_BOOT   (0xFFFFFFFF)     /* First time boot */
#define ETX_NORMAL_BOOT       (0xBEEFFEED)     /* Normal Boot */
#define ETX_OTA_DONE_BOOT     (0xBEEFFDDE)     /* OTA done */
#define ETX_OTA_REQUEST       (0xDEADBEEF)     /* OTA request by application */
#define ETX_LOAD_PREV_APP     (0xFACEFADE)     /* App requests to load the previous version */

#define OTA_FIRMWARE_ADDRESS  (0x10000)
#define OTA_PART_LENGTH       (1024)
#define MAGIC_NUMBER          (0xAA555AA5)

typedef void (*application_func_t)(void);

typedef struct {
    int32_t fw_size;
    uint32_t fw_crc;
    uint32_t reserved1;
} __attribute__((packed)) ext_slot_t;

typedef struct {
    uint32_t reboot_cause;
    ext_slot_t slot_table;
} __attribute__((packed)) ext_general_cfg_t;

typedef uint8_t manufacture_id_t;
enum {
    JUER_MARINE_ID = 0,
    REB_TEK_ID,
    MANUFACTURER_COUNT
};

typedef struct {
    uint32_t magic_number;
    char user_name[64];

    uint8_t screen_rotate;
    manufacture_id_t manufacturer_id;

    uint16_t menu_timeout_sec;
    uint16_t power_off_timeout_sec;
} system_config_t;

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/



/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

extern system_config_t system_config;

/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/



/******************************************************************************/

#endif /* _SYSTEM_H_ */
