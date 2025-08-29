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

#include "ui_splash.h"
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

static int ui_current_screen = SCREEN_INIT_ID;

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
    ui_splash_init();

    ui_current_screen = SCREEN_SPLASH_ID;
    while (1) {
        lv_task_handler();    /* Let the GUI do its work */
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
                ui_splash_loadscreen();
                delay(2000);    /* Wait for splash bar */
                ui_current_screen = SCREEN_HOME_ID;
                break;

            case SCREEN_HOME_ID:
                delay(1000);
                break;

            default:
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
}
