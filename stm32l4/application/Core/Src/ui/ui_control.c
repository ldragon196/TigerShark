/*
 *  ui_control.c
 *
 *  Created on: Aug 28, 2025
 */

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "main.h"
#include "lvgl.h"
#include "app_config.h"
#include "log.h"
#include "lv_port_disp.h"
#include "power_manager.h"
#include "user_intf.h"

#include "ui_utils.h"
#include "ui_splash.h"
#include "ui_big_number.h"

#include "ui_control.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/



/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

static osThreadId_t lvgl_task_handle;
static const osThreadAttr_t lvgl_task_attributes = {
    .name = "lvgl_task",
    .priority = (osPriority_t) osPriorityHigh,
    .stack_size = 8192
};

static osThreadId_t ui_control_task_handle;
static const osThreadAttr_t ui_control_task_attributes = {
    .name = "ui_control_task",
    .priority = (osPriority_t) osPriorityAboveNormal,
    .stack_size = 4096
};

static osMutexId_t lvgl_mutex;

static int ui_current_screen = SCREEN_INIT_ID;
static uint8_t current_mode = SCREEN_MODE_COUNT;

static const screen_mode_t screen_modes[SCREEN_MODE_COUNT] = {
    /* BIG_NUMBER_MODE */ {ui_big_number_init, ui_big_number_loadscreen, ui_big_number_update},
    /* OLD_MAN_MODE */
    /* OLD_MAN_ADV_MODE */
    /* FIXED_SET_POINT_MODE */
    /* LEGACY_MODE */
};

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/



/******************************************************************************/

/*!
 * @brief  Task for LVGL handle
 */
static void lvgl_task(void *argument) {
    (void) argument;

    /* Initialize LVGL */
    lv_init();
    lv_port_disp_init();

    if (system_config.screen_rotate == 1) {
        lv_disp_set_rotation(NULL, LV_DISP_ROT_90);
    }
    else {
        lv_disp_set_rotation(NULL, LV_DISP_ROT_270);
    }

    /* Initialize screens */
    ui_utils_init();
    ui_splash_init();
    for (int i = 0; i < SCREEN_MODE_COUNT; i++) {
        if (screen_modes[i].init) {
            screen_modes[i].init();
        }
    }

    ui_current_screen = SCREEN_SPLASH_ID;
    while (1) {
        osMutexAcquire(lvgl_mutex, osWaitForever);
        lv_task_handler();    /* Let the GUI do its work */
        osMutexRelease(lvgl_mutex);
        delay(LVGL_TICK_HANDLER);
    }
}

/*!
 * @brief  Task for ui control
 */
static void ui_control_task(void *argument) {
    int delay_time_ms = 100;
    (void) argument;
    LOG_INFO("UI control task started");

    while (1) {
        osMutexAcquire(lvgl_mutex, osWaitForever);
        switch (ui_current_screen) {
            case SCREEN_SPLASH_ID:
                LOG_INFO("Loading splash screen...");
                ui_splash_loadscreen();
                ui_current_screen = SCREEN_HOME_ID;
                delay_time_ms = 2000;    /* Wait for splash bar */
                break;

            case SCREEN_HOME_ID:
                /* When we change screen mode, we need to load it */
                if ((current_mode != system_config.screen_mode) && (system_config.screen_mode < SCREEN_MODE_COUNT)) {
                    current_mode = system_config.screen_mode;
                    LOG_INFO("Loading home screen mode %d...", current_mode);
                    if (screen_modes[current_mode].load) {
                        screen_modes[current_mode].load();
                    }
                }

                /* Update screens */
                if (screen_modes[current_mode].update) {
                    screen_modes[current_mode].update();
                }
                delay_time_ms = 500;
                break;

            default:
                delay_time_ms = 100;
                break;
        }
        osMutexRelease(lvgl_mutex);
        
        delay(delay_time_ms);
    }
}

/*!
 * @brief  Handle button main pressed event
 */
void ui_control_button_main_pressed(void) {
    ui_big_number_set_menu_text("SETPOINT 123");
}

/*!
 * @brief  Handle button tap pressed event
 */
void ui_control_button_tap_pressed(void) {
    ui_big_number_set_menu_text("LCD 50%");
}

/*!
 * @brief  Initialize UI control
 */
void ui_control_init(void) {
    /* Create task for lvgl handle */
    ui_control_task_handle = osThreadNew(ui_control_task, NULL, &ui_control_task_attributes);
    lvgl_task_handle = osThreadNew(lvgl_task, NULL, &lvgl_task_attributes);
    lvgl_mutex = osMutexNew(NULL);

    user_intf_register_button_callback(BUTTON_MAIN, ui_control_button_main_pressed);
    user_intf_register_button_callback(BUTTON_TAP, ui_control_button_tap_pressed);
}
