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
static bool load_home_screen = false;

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
    ui_big_number_init();

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
    (void) argument;
    LOG_INFO("UI control task started");

    while (1) {
        switch (ui_current_screen) {
            case SCREEN_INIT_ID:
                delay(100);    /* Wait for initialization */
                break;

            case SCREEN_SPLASH_ID:
                LOG_INFO("Loading splash screen...");
                osMutexRelease(lvgl_mutex);
                ui_splash_loadscreen();
                delay(2000);    /* Wait for splash bar */

                ui_current_screen = SCREEN_HOME_ID;
                load_home_screen = true;
                break;

            case SCREEN_HOME_ID:
                osMutexAcquire(lvgl_mutex, osWaitForever);
                if (load_home_screen) {
                    LOG_INFO("Loading home screen mode %d...", system_config.screen_mode);
                    ui_big_number_loadscreen();
                    load_home_screen = false;
                }
                ui_big_number_update();
                ui_big_number_blink();
                osMutexRelease(lvgl_mutex);
                delay(500);
                break;

            default:
                delay(1000);
                break;
        }
    }
}

/*!
 * @brief  Initialize UI control
 */
void ui_control_init(void) {
    /* Create task for lvgl handle */
    ui_control_task_handle = osThreadNew(ui_control_task, NULL, &ui_control_task_attributes);
    lvgl_task_handle = osThreadNew(lvgl_task, NULL, &lvgl_task_attributes);
    lvgl_mutex = osMutexNew(NULL);
}
