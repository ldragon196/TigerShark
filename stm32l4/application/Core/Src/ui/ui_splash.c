/*
 *  ui_splash.c
 *
 *  Created on: Aug 29, 2025
 */

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "lvgl.h"
#include "resource.h"
#include "app_config.h"
#include "ui_splash.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/



/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

static lv_obj_t *screen_splash;
static lv_obj_t *progress_bar;

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/



/******************************************************************************/

/*!
 * @brief  Initialize header
 */
void ui_splash_text(void) {
    static lv_style_t style_splash_header;
    static lv_style_t style_splash_context;
    lv_style_init(&style_splash_header);
    lv_style_init(&style_splash_context);
    lv_style_set_text_font(&style_splash_header, LV_STATE_DEFAULT, &lv_font_abadi_36);
    lv_style_set_text_color(&style_splash_header, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0x00, 0xff));
    lv_style_set_text_font(&style_splash_context, LV_STATE_DEFAULT, &lv_font_montserrat_18);
    lv_style_set_text_color(&style_splash_context, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0x00, 0x80));

    lv_obj_t *lb_splash_header = lv_label_create(screen_splash, NULL);
    lv_label_set_long_mode(lb_splash_header, LV_LABEL_LONG_BREAK); /* Break the long lines */
    lv_label_set_recolor(lb_splash_header, true);                  /* Enable re-coloring by commands in the text */
    lv_label_set_align(lb_splash_header, LV_LABEL_ALIGN_CENTER);   /* Center aligned lines */
    lv_label_set_text(lb_splash_header, "TIGERSHARK");
    lv_obj_set_width(lb_splash_header, 298);
    lv_obj_add_style(lb_splash_header, LV_LABEL_PART_MAIN, &style_splash_header);
    lv_obj_align(lb_splash_header, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 5);

    lv_obj_t *lb_splash_context = lv_label_create(screen_splash, NULL);
    lv_label_set_long_mode(lb_splash_context, LV_LABEL_LONG_BREAK); /* Break the long lines */
    lv_label_set_recolor(lb_splash_context, true);                  /* Enable re-coloring by commands in the text */
    lv_label_set_align(lb_splash_context, LV_LABEL_ALIGN_CENTER);   /* Center aligned lines */
    lv_label_set_text_fmt(lb_splash_context, "%s", system_config.user_name);
    lv_obj_set_width(lb_splash_context, 298);
    lv_obj_add_style(lb_splash_context, LV_LABEL_PART_MAIN, &style_splash_context);
    lv_obj_align(lb_splash_context, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 38);
}

/*!
 * @brief  Initialize splash image
 */
void ui_splash_image(void) {
    static lv_img_dsc_t img_logo = {
        .header.always_zero = 0,
        .header.w = 120,
        .header.h = 120,
        .data_size = 120 * 120 * LV_COLOR_DEPTH / 8,
        .header.cf = LV_IMG_CF_TRUE_COLOR, /*Set the color format*/
        .data = img_logo_marine_120x120,
    };

    lv_obj_t *img_splash_logo = lv_img_create(screen_splash, NULL);
    if (system_config.manufacturer_id == REB_TEK_ID) {
        img_logo.header.w = 310;
        img_logo.header.h = 62;
        img_logo.data_size = 310 * 62 * LV_COLOR_DEPTH / 8;
        img_logo.data = img_logo_rebtek_310_62;
    }

    lv_img_set_src(img_splash_logo, &img_logo);
    lv_obj_align(img_splash_logo, NULL, LV_ALIGN_CENTER, 0, 8);
}

/*!
 * @brief  Initialize version label
 */
void ui_splash_version(char *text_version) {
    static lv_style_t style_splash_version;
    lv_style_init(&style_splash_version);
    lv_style_set_text_font(&style_splash_version, LV_STATE_DEFAULT, &lv_font_montserrat_18);
    lv_style_set_text_color(&style_splash_version, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0x00, 0x80));

    lv_obj_t *lb_splash_version = lv_label_create(screen_splash, NULL);
    lv_label_set_long_mode(lb_splash_version, LV_LABEL_LONG_BREAK); /* Break the long lines */
    lv_label_set_recolor(lb_splash_version, true);                  /* Enable re-coloring by commands in the text */
    lv_label_set_align(lb_splash_version, LV_LABEL_ALIGN_CENTER);   /* Center aligned lines */
    lv_label_set_text_fmt(lb_splash_version, "Version %s", text_version);
    lv_obj_set_width(lb_splash_version, 298);
    lv_obj_add_style(lb_splash_version, LV_LABEL_PART_MAIN, &style_splash_version);
    lv_obj_align(lb_splash_version, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 192);
}

/*!
 * @brief  Initialize Progress Bar
 */
void ui_splash_progress_bar(void) {
    progress_bar = lv_bar_create(screen_splash, NULL);
    lv_obj_set_size(progress_bar, 200, 10);
    lv_obj_align(progress_bar, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_bar_set_anim_time(progress_bar, 2000);
    lv_obj_align(progress_bar, NULL, LV_ALIGN_IN_TOP_LEFT, 60, 218);
}

/*!
 * @brief  Initialize the splash screen
 */
void ui_splash_init(void) {
    screen_splash = lv_obj_create(NULL, NULL);

    ui_splash_text();
    ui_splash_image();
    ui_splash_version(APP_VERSION);
    ui_splash_progress_bar();
}

/*!
 * @brief  Set the splash screen
 */
void ui_splash_loadscreen(void) {
    lv_scr_load(screen_splash);
    lv_bar_set_value(progress_bar, 100, LV_ANIM_ON);
}
