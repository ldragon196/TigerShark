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
#include "user_intf.h"
#include "ui_control.h"
#include "app_main.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/



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
    int led_state = 0;

    while (1) {
        delay(5000);
        user_intf_set_led(led_state++);
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

        system_config.menu_timeout_sec = 30;
        system_config.power_off_timeout_sec = 120;
    }

    /* Components initialization */
    power_board_on();
    user_intf_init();
    ui_control_init();

    /* Create task for main process */
    main_task_handle = osThreadNew(main_task, NULL, &main_task_attributes);
}
