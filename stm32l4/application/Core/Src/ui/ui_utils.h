/*
 *  ui_utils.h
 *
 *  Created on: Aug 30, 2025
 */

#ifndef _UI_UTILS_H_
#define _UI_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "lvgl.h"
#include "app_config.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

/* Dark Mode */
#define DARK_BACKGROUND             LV_COLOR_MAKE(0x00, 0x00, 0x00)
#define DARK_ACTIVE_TEXT            LV_COLOR_MAKE(0x00, 0xFF, 0x00)
#define DARK_NORMAL_TEXT            LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)

/* Light Mode */
#define LIGHT_BACKGROUND            LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)
#define LIGHT_ACTIVE_TEXT           LV_COLOR_NAVY
#define LIGHT_NORMAL_TEXT           LV_COLOR_MAKE(0x00, 0x00, 0x00)

/* Sensor panel */
#define PANEL_BORDER_COLOR          LV_COLOR_RED

#define SENSOR_LOW_COLOR            LV_COLOR_MAKE(0x50, 0xF3, 0xFC)     /* Text of sensor when below set point */
#define SENSOR_LOW_BG_COLOR         LV_COLOR_WHITE    /* Back ground of sensor when below set point */

#define SENSOR_HIGH_COLOR           LV_COLOR_RED      /* Back ground of sensor when above set point */
#define SENSOR_HIGH_BG_COLOR        LV_COLOR_WHITE    /* Back ground of sensor when above set point */

#define LINE_BLUE_COLOR             LV_COLOR_MAKE(0x1F, 0xE0, 0xF7)

#define BATTERY_HIGH_COLOR          LV_COLOR_MAKE(0x93, 0xFF, 0x05)
#define BATTERY_LOW_COLOR           LV_COLOR_RED

#define SENSOR_LOW_STATE            LV_STATE_FOCUSED
#define SENSOR_HIGH_STATE           LV_STATE_HOVERED

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/



/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

extern lv_style_t style_dashboard;
extern lv_style_t style_border;
extern lv_style_t style_label_font_36;
extern lv_style_t style_label_font_40;
extern lv_style_t style_label_font_44;

/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

/**
 * @brief  Set text in center of parent
 * @param  None
 * @retval None
 */
void ui_utils_set_text_center(lv_obj_t *obj, char *text, lv_obj_t *parent);

/*!
 * @brief  Initialize common components
 * @param  None
 * @retval None
 */
void ui_utils_init(void);

/*!
 * @brief  Convert sensor to string
 * @param  array Pointer to string array
 * @param  value Sensor value
 * @retval None
 */
void ui_utils_convert_sensor(char *array, uint16_t value);

/*!
 * @brief  Convert setpoit to string
 * @param  array Pointer to string array
 * @param  value Set point value
 * @param  mode OC or CCO
 * @retval None
 */
void ui_utils_convert_setpoint(char *array, uint16_t value, uint16_t mode);

/*!
 * @brief  Convert dive time to string
 * @param  array Pointer to string array
 * @param  value Dive time in seconds
 * @retval index Length of string
 */
uint8_t ui_utils_convert_dive_time(char *array, int value);

/*!
 * @brief  Convert dive time to string and return unit
 * @param  array Pointer to string array
 * @param  value Dive time in seconds
 * @retval uint 0: seconds, 1: minutes, 2: hours
 */
uint8_t ui_utils_convert_dive_time_with_unit(char *array, int value);

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _UI_UTILS_H_ */
