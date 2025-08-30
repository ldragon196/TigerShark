/*
 *  ui_utils.c
 *
 *  Created on: Aug 30, 2025
 */

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "lvgl.h"
#include "resource.h"
#include "app_config.h"
#include "ui_utils.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/



/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/



/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

lv_style_t style_dashboard;
lv_style_t style_border;
lv_style_t style_label_font_36;
lv_style_t style_label_font_40;
lv_style_t style_label_font_44;

/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/



/******************************************************************************/

/**
 * @brief  Set text in cernter
 */
void ui_utils_set_text_center(lv_obj_t *obj, char *text, lv_obj_t *parent) {
    lv_label_set_text(obj, text);
    int16_t width = lv_obj_get_width(obj);
    int16_t max_width = lv_obj_get_width(parent);
    int16_t x = ((max_width - width) / 2) + lv_obj_get_x(parent);
    lv_obj_align(obj, NULL, LV_ALIGN_IN_TOP_LEFT, x, lv_obj_get_y(obj));
}

/*!
 * @brief  Initialize common components
 */
void ui_utils_init(void) {
    lv_style_init(&style_dashboard);
    lv_style_set_border_width(&style_dashboard, LV_STATE_DEFAULT, 0);

    lv_style_init(&style_border);
    lv_style_set_border_width(&style_border, LV_STATE_DEFAULT, 2);
    lv_style_set_border_color(&style_border, LV_STATE_FOCUSED, SENSOR_LOW_COLOR);
    lv_style_set_border_color(&style_border, LV_STATE_PRESSED, SENSOR_HIGH_COLOR);

    lv_style_init(&style_label_font_36);
    lv_style_set_text_font(&style_label_font_36, LV_STATE_DEFAULT, &lv_font_abadi_36);
    lv_style_init(&style_label_font_40);
    lv_style_set_text_font(&style_label_font_40, LV_STATE_DEFAULT, &lv_font_abadi_40);
    lv_style_init(&style_label_font_44);
    lv_style_set_text_font(&style_label_font_44, LV_STATE_DEFAULT, &lv_font_abadi_44);

    if (system_config.user_setting.flags.dark_mode) {
        lv_style_set_bg_color(&style_dashboard, LV_STATE_DEFAULT, DARK_BACKGROUND);
        lv_style_set_border_color(&style_dashboard, LV_STATE_DEFAULT, LV_COLOR_WHITE);
        lv_style_set_text_color(&style_dashboard, LV_STATE_DEFAULT, DARK_NORMAL_TEXT);
        lv_style_set_bg_color(&style_border, LV_STATE_DEFAULT, DARK_BACKGROUND);
        lv_style_set_border_color(&style_border, LV_STATE_DEFAULT, LV_COLOR_WHITE);

        lv_style_set_bg_color(&style_border, SENSOR_LOW_STATE, SENSOR_LOW_BG_COLOR);
        lv_style_set_border_color(&style_border, SENSOR_LOW_STATE, PANEL_BORDER_COLOR);
        lv_style_set_text_color(&style_label_font_44, SENSOR_LOW_STATE, SENSOR_LOW_COLOR);
        lv_style_set_text_color(&style_label_font_40, SENSOR_LOW_STATE, SENSOR_LOW_COLOR);

        lv_style_set_bg_color(&style_border, SENSOR_HIGH_STATE, SENSOR_HIGH_BG_COLOR);
        lv_style_set_border_color(&style_border, SENSOR_HIGH_STATE, PANEL_BORDER_COLOR);
        lv_style_set_text_color(&style_label_font_44, SENSOR_HIGH_STATE, SENSOR_HIGH_COLOR);
        lv_style_set_text_color(&style_label_font_40, SENSOR_HIGH_STATE, SENSOR_HIGH_COLOR);

        lv_style_set_text_color(&style_label_font_36, LV_STATE_DEFAULT, DARK_ACTIVE_TEXT);
        lv_style_set_text_color(&style_label_font_40, LV_STATE_DEFAULT, DARK_ACTIVE_TEXT);
        lv_style_set_text_color(&style_label_font_44, LV_STATE_DEFAULT, DARK_ACTIVE_TEXT);
    }
    else {
        lv_style_set_bg_color(&style_dashboard, LV_STATE_DEFAULT, LIGHT_BACKGROUND);
        lv_style_set_border_color(&style_dashboard, LV_STATE_DEFAULT, LV_COLOR_BLACK);
        lv_style_set_text_color(&style_dashboard, LV_STATE_DEFAULT, LIGHT_NORMAL_TEXT);
        lv_style_set_bg_color(&style_border, LV_STATE_DEFAULT, LIGHT_BACKGROUND);
        lv_style_set_border_color(&style_border, LV_STATE_DEFAULT, LV_COLOR_BLACK);

        lv_style_set_bg_color(&style_border, SENSOR_LOW_STATE, SENSOR_LOW_BG_COLOR);
        lv_style_set_border_color(&style_border, SENSOR_LOW_STATE, PANEL_BORDER_COLOR);
        lv_style_set_text_color(&style_label_font_44, SENSOR_LOW_STATE, SENSOR_LOW_COLOR);
        lv_style_set_text_color(&style_label_font_40, SENSOR_LOW_STATE, SENSOR_LOW_COLOR);

        lv_style_set_bg_color(&style_border, SENSOR_HIGH_STATE, SENSOR_HIGH_BG_COLOR);
        lv_style_set_border_color(&style_border, SENSOR_HIGH_STATE, PANEL_BORDER_COLOR);
        lv_style_set_text_color(&style_label_font_44, SENSOR_HIGH_STATE, SENSOR_HIGH_COLOR);
        lv_style_set_text_color(&style_label_font_40, SENSOR_HIGH_STATE, SENSOR_HIGH_COLOR);

        lv_style_set_text_color(&style_label_font_36, LV_STATE_DEFAULT, LIGHT_ACTIVE_TEXT);
        lv_style_set_text_color(&style_label_font_40, LV_STATE_DEFAULT, LIGHT_ACTIVE_TEXT);
        lv_style_set_text_color(&style_label_font_44, LV_STATE_DEFAULT, LIGHT_ACTIVE_TEXT);
    }
}

/*!
 * @brief  Convert sensor to string
 */
void ui_utils_convert_sensor(char *array, uint16_t value) {
    array[0] = '0' + value % 1000 / 100;
    array[1] = '.';
    array[2] = '0' + value % 100 / 10;
    array[3] = '0' + value % 10 / 1;
    array[4] = '\0';
}

/*!
 * @brief  Convert setpoit to string
 */
void ui_utils_convert_setpoint(char *array, uint16_t value, uint16_t mode) {
    if (mode == CLOSED_CIRCUIT) {
        array[0] = '0' + value % 1000 / 100;
        array[1] = '.';
        array[2] = '0' + value % 100 / 10;
        array[3] = '0' + value % 10 / 1;
        array[4] = '\0';
    }
    else {
        array[0] = '-';
        array[1] = 'O';
        array[2] = 'C';
        array[3] = '-';
        array[4] = '\0';
    }
}

/*!
 * @brief  Convert dive time to string
 */
uint8_t ui_utils_convert_dive_time(char *array, int value) {
    uint8_t index = 0;
    uint16_t hour = 0;
    uint16_t minu = 0;
    hour = value / 3600;
    minu = (value / 60) % 60;
    if (value >= 60) {
        if (hour) {
            if (hour / 100) {
                array[index++] = '0' + (hour / 100) % 10;
            }

            if (hour / 10) {
                array[index++] = '0' + (hour / 10) % 10;
            }
            array[index++] = '0' + hour % 10;
            array[index++] = ':';
            array[index++] = '0' + minu / 10;
            array[index++] = '0' + minu % 10;
            array[index] = 0;
        }
        else {
            if (minu / 10) {
                array[index++] = '0' + minu / 10;
            }
            array[index++] = '0' + minu % 10;
        }
    }
    else {
        array[index++] = '0';
    }

    array[index++] = '\0';
    return index;
}

/*!
 * @brief  Convert dive time to string and return unit
 */
uint8_t ui_utils_convert_dive_time_with_unit(char *array, int value) {
    uint8_t hour = value / 3600;
    uint8_t min = (value / 60) % 60;
    uint8_t sec = value % 60;

    if (hour > 99) {
        sprintf(array, "%d", hour);
        return 2;
    }
    else if (hour > 0) {
        sprintf(array, "%02d:%02d", hour, min);
        return 1;
    }
    else {
        sprintf(array, "%02d:%02d", min, sec);
    }
    return 0;
}
