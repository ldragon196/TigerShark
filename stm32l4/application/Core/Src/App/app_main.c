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
#include "communication.h"
#include "main_process.h"
#include "app_main.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/



/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/



/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

system_config_t system_config;

/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/



/******************************************************************************/

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
        system_config.screen_mode = BIG_NUMBER_MODE;
        system_config.set_point = 100;

        system_config.menu_timeout_sec = 30;
        system_config.power_off_timeout_sec = 120;

        system_config.user_setting.flags.depth_units              = 0;        /* 0 = FSW,                   1 = MSW */
        system_config.user_setting.flags.pp_o2_units              = 1;        /* 0 = atm_percent,           1 = bars */
        system_config.user_setting.flags.diva_enabled             = 1;        /* 0 = diva_disabled,         1 = diva_enabled */
        system_config.user_setting.flags.diva_mode                = 0;        /* 0 = ppO2 Mode,             1 = Setpoint Mode */
        system_config.user_setting.flags.signal_opt_show          = 1;        /* 0 = hide signal opt,       1 = show signal opt */
        system_config.user_setting.flags.flash_light_opt_show     = 0;        /* 0 = hide flashlight opt,   1 = show flashlight opt */
        system_config.user_setting.flags.enable_buddy_leds        = 1;        /* 0 = buddy leds disabled,   1 = buddy leds enabled */
        system_config.user_setting.flags.enable_signal_leds       = 0;        /* 0 = signal leds off,       1 = signal leds on */
        system_config.user_setting.flags.enable_flash_light       = 1;        /* 0 = flashlight off,        1 = flashlight on */
        system_config.user_setting.flags.enable_deco_leds         = 0;        /* 0 = divaDecoLeds off,      1 = divaDecoLeds on */
        system_config.user_setting.flags.show_deco                = 1;        /* 0 = Don't Show,            1 = Show Deco Info */
        system_config.user_setting.flags.deco_mode                = 0;        /* 0 = Deco Off,              1 = Deco Mode On */
        system_config.user_setting.flags.diva_setpoint_blink_repeat_sec = 3;  /* blink repeat: 0–7 sec */
        system_config.user_setting.flags.screen_rotation          = 0;        /* 0 = Landscape,             1 = Portrait */
        system_config.user_setting.flags.dark_mode                = 1;        /* 0 = Light Mode,            1 = Dark Mode */
        system_config.user_setting.flags.light_time               = 2;        /* Auto light-off setting: 0–3 */
        system_config.user_setting.flags.enable_led               = 1;        /* 0 = LED TS off,            1 = LED TS on */
        system_config.user_setting.flags.fresh_water              = 0;        /* 0 = Saltwater,             1 = Freshwater */
        system_config.user_setting.flags.can_enabled              = 0;        /* 0 = Disabled,              1 = Enabled */
    }

    /* Components initialization */
    power_board_on();
    user_intf_init();
    communication_init();
    ui_control_init();
    main_process_init();
}
