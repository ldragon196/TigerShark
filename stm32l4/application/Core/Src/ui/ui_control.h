/*
 *  ui_control.h
 *
 *  Created on: Aug 28, 2025
 */

#ifndef _UI_CONTROL_H_
#define _UI_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "ui_utils.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

#define LVGL_TICK_HANDLER    (10)   /* in ms */

enum {
    SCREEN_INIT_ID = 0,
    SCREEN_SPLASH_ID,
    SCREEN_HOME_ID,
    SCREEN_SETTINGS_ID,
    SCREEN_MAX_ID
};

typedef void (*screen_init_func_t)(void);
typedef void (*screen_load_func_t)(void);
typedef void (*screen_update_func_t)(void);

typedef struct {
    screen_init_func_t init;
    screen_load_func_t load;
    screen_update_func_t update;
} screen_mode_t;

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/



/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

/*!
 * @brief  Handle button main pressed event
 * @param  None
 * @retval None
 */
void ui_control_button_main_pressed(void);

/*!
 * @brief  Handle button tap pressed event
 * @param  None
 * @retval None
 */
void ui_control_button_tap_pressed(void);

/*!
 * @brief  Initialize UI control
 * @param  None
 * @retval None
 */
void ui_control_init(void);

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _UI_CONTROL_H_ */
