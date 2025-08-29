/*
 *  app_main.c
 *
 *  Created on: Aug 28, 2025
 */

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "stm32l4xx_hal.h"
#include "cmsis_os2.h"
#include "app_config.h"
#include "log.h"
#include "power_manager.h"
#include "ui_control.h"
#include "app_main.h"

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

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

static osThreadId_t main_task_handle;
static const osThreadAttr_t main_task_attributes = {
    .name = "main_task",
    .priority = (osPriority_t) osPriorityNormal,
    .stack_size = 4096
};

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

system_config_t system_config;

/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/



/******************************************************************************/

/*!
 * @brief  Task for the main application
 */
static void main_task(void *argument) {
    (void) argument;

    while (1) {
        delay(1000);
    }
}

/*!
 * @brief  Initialize the main application
 */
void app_main_init(void) {
    LOG_INFO("");
    LOG_INFO("================================================");
    LOG_INFO("---------- Application %s Version %s ----------", DEVICE_NAME, APP_VERSION);
    LOG_INFO("================================================");
    LOG_INFO("");

    /* Reload configuration */
    memset(&system_config, 0, sizeof(system_config_t));
    if (system_config.magic_number == MAGIC_NUMBER) {
        LOG_INFO("Valid system configuration found");
    }
    else {
        LOG_WARN("Invalid system configuration. Set default values");
        system_config.magic_number = MAGIC_NUMBER;
        sprintf(system_config.user_name, "Juergensen Marine");
        system_config.manufacturer_id = JUER_MARINE_ID;
        system_config.screen_rotate = 0;
    }

    /* Components intialization */
    power_board_on();
    ui_control_init();

    /* Create task for main process */
    main_task_handle = osThreadNew(main_task, NULL, &main_task_attributes);
}
