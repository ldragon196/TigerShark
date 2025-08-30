/*
 *  ui_big_number.c
 *
 *  Created on: Aug 30, 2025
 */

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "main.h"
#include "lvgl.h"
#include "app_config.h"
#include "log.h"
#include "ui_utils.h"
#include "ui_big_number.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

#define LINE2_LABEL_Y_IDLE 84
#define LINE2_VALUE_Y_IDLE (LINE2_LABEL_Y_IDLE + 24)
#define LINE3_LABEL_Y_IDLE 160
#define LINE3_VALUE_Y_IDLE (LINE3_LABEL_Y_IDLE + 24)

#define LINE2_LABEL_Y_MENU 64
#define LINE2_VALUE_Y_MENU (LINE2_LABEL_Y_MENU + 24)
#define LINE3_LABEL_Y_MENU 178
#define LINE3_VALUE_Y_MENU (LINE3_LABEL_Y_MENU + 24)

enum {
    SENSOR_NORMAL = 0,
    SENSOR_BELOW_SETPOINT,
    SENSOR_ABOVE_SETPOINT,
};

enum {
    BIG_NUMBER_S1 = 0,
    BIG_NUMBER_S2,
    BIG_NUMBER_S3,
    BIG_NUMBER_DEPTH,             /* Current Depth from pressure transducer */
    BIG_NUMBER_DEPTH_LABEL,
    BIG_NUMBER_SETPOINT,          /* Current Set Point selected */
    BIG_NUMBER_SETPOINT_LABEL,
    BIG_NUMBER_D_TIME,            /* Dive time */
    BIG_NUMBER_D_TIME_LABEL,
    BIG_NUMBER_02_HE,             /* Oxygen/Helium fraction of Gas Mix selected */
    BIG_NUMBER_02_HE_LABEL,
    BIG_NUMBER_TTS,               /* Total Time to Surface from calculation in deco program */
    BIG_NUMBER_TTS_LABEL,
    BIG_NUMBER_NDL,               /* Next Stop Depth from deco calculation */
    BIG_NUMBER_NDL_LABEL,
    BIG_NUMBER_STOP,
    BIG_NUMBER_STOP_LABEL,
    BIG_NUMBER_STIME,
    BIG_NUMBER_STIME_LABEL,
    BIG_NUMBER_MENU,              /* Display menu state */
    BIG_NUMBER_COUNT,
};

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

static lv_obj_t *screen_dashboard;
static lv_obj_t *dashboard_labels[BIG_NUMBER_COUNT];
static lv_obj_t *panel_sensor[O2_SENSOR_NUM];
static lv_obj_t *panel_stop;

static bool is_idle = true;
static bool is_warning = false;
static bool dashboard_blink = false;
static uint8_t stop_time_blink = 0;
static uint8_t warining_timeout = 0;

static uint8_t sensor_warn_mask = 0;
static uint8_t valid_sensor_count = 0;
static uint32_t sum_valid_sensor = 0;

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/



/******************************************************************************/

/**
 * @brief  Initialize static components
 */
static void ui_dashboard_static_init(void)
{
    panel_sensor[BIG_NUMBER_S1] = lv_obj_create(screen_dashboard, NULL);
    lv_obj_set_width(panel_sensor[BIG_NUMBER_S1], 98);
    lv_obj_set_height(panel_sensor[BIG_NUMBER_S1], 48);
    lv_obj_align(panel_sensor[BIG_NUMBER_S1], NULL, LV_ALIGN_IN_TOP_MID, -102, 6);
    lv_obj_add_style(panel_sensor[BIG_NUMBER_S1], LV_STATE_DEFAULT, &style_border);

    panel_sensor[BIG_NUMBER_S2] = lv_obj_create(screen_dashboard, NULL);
    lv_obj_set_width(panel_sensor[BIG_NUMBER_S2], 98);
    lv_obj_set_height(panel_sensor[BIG_NUMBER_S2], 48);
    lv_obj_align(panel_sensor[BIG_NUMBER_S2], NULL, LV_ALIGN_IN_TOP_MID, 0, 6);
    lv_obj_add_style(panel_sensor[BIG_NUMBER_S2], LV_STATE_DEFAULT, &style_border);

    panel_sensor[BIG_NUMBER_S3] = lv_obj_create(screen_dashboard, NULL);
    lv_obj_set_width(panel_sensor[BIG_NUMBER_S3], 98);
    lv_obj_set_height(panel_sensor[BIG_NUMBER_S3], 48);
    lv_obj_align(panel_sensor[BIG_NUMBER_S3], NULL, LV_ALIGN_IN_TOP_MID, 102, 6);
    lv_obj_add_style(panel_sensor[BIG_NUMBER_S3], LV_STATE_DEFAULT, &style_border);

    panel_stop = lv_obj_create(screen_dashboard, NULL);
    lv_obj_set_width(panel_stop, 14);
    lv_obj_set_height(panel_stop, 10);
    lv_obj_align(panel_stop, NULL, LV_ALIGN_IN_TOP_MID, 40, 178);
    lv_obj_add_style(panel_stop, LV_STATE_DEFAULT, &style_border);
    lv_obj_set_hidden(panel_stop, true);

    dashboard_labels[BIG_NUMBER_DEPTH_LABEL] = lv_label_create(screen_dashboard, NULL);
    lv_obj_align(dashboard_labels[BIG_NUMBER_DEPTH_LABEL], NULL, LV_ALIGN_IN_TOP_MID, -128, LINE2_LABEL_Y_IDLE);
    lv_label_set_text(dashboard_labels[BIG_NUMBER_DEPTH_LABEL], "DEPTH (ft)");

    dashboard_labels[BIG_NUMBER_SETPOINT_LABEL] = lv_label_create(screen_dashboard, NULL);
    lv_obj_align(dashboard_labels[BIG_NUMBER_SETPOINT_LABEL], NULL, LV_ALIGN_IN_TOP_MID, -24, LINE2_LABEL_Y_IDLE);
    lv_label_set_text(dashboard_labels[BIG_NUMBER_SETPOINT_LABEL], "SETPOINT");

    dashboard_labels[BIG_NUMBER_D_TIME_LABEL] = lv_label_create(screen_dashboard, NULL);
    lv_obj_align(dashboard_labels[BIG_NUMBER_D_TIME_LABEL], NULL, LV_ALIGN_IN_TOP_MID, 78, LINE2_LABEL_Y_IDLE);
    lv_label_set_text(dashboard_labels[BIG_NUMBER_D_TIME_LABEL], "D TIME");

    dashboard_labels[BIG_NUMBER_02_HE_LABEL] = lv_label_create(screen_dashboard, NULL);
    lv_obj_align(dashboard_labels[BIG_NUMBER_02_HE_LABEL], NULL, LV_ALIGN_IN_TOP_MID, -126, LINE3_LABEL_Y_IDLE);
    lv_label_set_text(dashboard_labels[BIG_NUMBER_02_HE_LABEL], "O2/HE");

    dashboard_labels[BIG_NUMBER_TTS_LABEL] = lv_label_create(screen_dashboard, NULL);
    lv_obj_align(dashboard_labels[BIG_NUMBER_TTS_LABEL], NULL, LV_ALIGN_IN_TOP_MID, -36, LINE3_LABEL_Y_IDLE);
    lv_label_set_text(dashboard_labels[BIG_NUMBER_TTS_LABEL], "TTS");

    dashboard_labels[BIG_NUMBER_NDL_LABEL] = lv_label_create(screen_dashboard, NULL);
    lv_obj_align(dashboard_labels[BIG_NUMBER_NDL_LABEL], NULL, LV_ALIGN_IN_TOP_MID, 102, LINE3_LABEL_Y_MENU);
    lv_label_set_text(dashboard_labels[BIG_NUMBER_NDL_LABEL], "NDL");
    lv_obj_set_hidden(dashboard_labels[BIG_NUMBER_NDL_LABEL], true);

    dashboard_labels[BIG_NUMBER_STOP_LABEL] = lv_label_create(screen_dashboard, NULL);
    lv_obj_align(dashboard_labels[BIG_NUMBER_STOP_LABEL], NULL, LV_ALIGN_IN_TOP_MID, 22, LINE3_LABEL_Y_IDLE);
    lv_label_set_text(dashboard_labels[BIG_NUMBER_STOP_LABEL], "STOP");

    dashboard_labels[BIG_NUMBER_STIME_LABEL] = lv_label_create(screen_dashboard, NULL);
    lv_obj_align(dashboard_labels[BIG_NUMBER_STIME_LABEL], NULL, LV_ALIGN_IN_TOP_MID, 90, LINE3_LABEL_Y_IDLE);
    lv_label_set_text(dashboard_labels[BIG_NUMBER_STIME_LABEL], "TIME");
}

/**
 * @brief  Initialize static components
 */
static void ui_dashboard_param_init(void)
{
    dashboard_labels[BIG_NUMBER_SETPOINT] = lv_label_create(screen_dashboard, NULL);
    lv_obj_align(dashboard_labels[BIG_NUMBER_SETPOINT], NULL, LV_ALIGN_IN_TOP_MID, -12, LINE2_VALUE_Y_MENU);
    lv_label_set_text(dashboard_labels[BIG_NUMBER_SETPOINT], "1.00");
    lv_obj_add_style(dashboard_labels[BIG_NUMBER_SETPOINT], LV_STATE_DEFAULT, &style_label_font_36);

    dashboard_labels[BIG_NUMBER_D_TIME] = lv_label_create(screen_dashboard, NULL);
    lv_obj_align(dashboard_labels[BIG_NUMBER_D_TIME], NULL, LV_ALIGN_IN_TOP_MID, 78, LINE2_VALUE_Y_MENU);
    lv_label_set_text(dashboard_labels[BIG_NUMBER_D_TIME], "00:00");
    lv_obj_add_style(dashboard_labels[BIG_NUMBER_D_TIME], LV_STATE_DEFAULT, &style_label_font_36);

    dashboard_labels[BIG_NUMBER_DEPTH] = lv_label_create(screen_dashboard, NULL);
    lv_obj_align(dashboard_labels[BIG_NUMBER_DEPTH], NULL, LV_ALIGN_IN_TOP_MID, -120, LINE2_VALUE_Y_MENU);
    lv_label_set_text(dashboard_labels[BIG_NUMBER_DEPTH], "0");
    lv_obj_add_style(dashboard_labels[BIG_NUMBER_DEPTH], LV_STATE_DEFAULT, &style_label_font_44);

    dashboard_labels[BIG_NUMBER_MENU] = lv_label_create(screen_dashboard, NULL);
    lv_obj_align(dashboard_labels[BIG_NUMBER_MENU], NULL, LV_ALIGN_IN_TOP_LEFT, 4, 132);
    lv_label_set_text(dashboard_labels[BIG_NUMBER_MENU], " ");
    lv_obj_add_style(dashboard_labels[BIG_NUMBER_MENU], LV_STATE_DEFAULT, &style_label_font_40);

    dashboard_labels[BIG_NUMBER_02_HE] = lv_label_create(screen_dashboard, NULL);
    lv_obj_align(dashboard_labels[BIG_NUMBER_02_HE], NULL, LV_ALIGN_IN_TOP_MID, -126, LINE3_VALUE_Y_IDLE);
    lv_label_set_text(dashboard_labels[BIG_NUMBER_02_HE], "00/00");
    lv_obj_add_style(dashboard_labels[BIG_NUMBER_02_HE], LV_STATE_DEFAULT, &style_label_font_36);

    dashboard_labels[BIG_NUMBER_TTS] = lv_label_create(screen_dashboard, NULL);
    lv_obj_align(dashboard_labels[BIG_NUMBER_TTS], NULL, LV_ALIGN_IN_TOP_MID, -36, LINE3_VALUE_Y_IDLE);
    lv_label_set_text(dashboard_labels[BIG_NUMBER_TTS], "0");
    lv_obj_add_style(dashboard_labels[BIG_NUMBER_TTS], LV_STATE_DEFAULT, &style_label_font_36);

    dashboard_labels[BIG_NUMBER_NDL] = lv_label_create(screen_dashboard, NULL);
    lv_obj_align(dashboard_labels[BIG_NUMBER_NDL], NULL, LV_ALIGN_IN_TOP_MID, 96, LINE3_VALUE_Y_MENU);
    lv_label_set_text(dashboard_labels[BIG_NUMBER_NDL], "0");
    lv_obj_add_style(dashboard_labels[BIG_NUMBER_NDL], LV_STATE_DEFAULT, &style_label_font_36);
    lv_obj_set_hidden(dashboard_labels[BIG_NUMBER_NDL], true);

    dashboard_labels[BIG_NUMBER_S1] = lv_label_create(screen_dashboard, NULL);
    lv_obj_align(dashboard_labels[BIG_NUMBER_S1], NULL, LV_ALIGN_IN_TOP_MID, -124, 10);
    lv_label_set_text(dashboard_labels[BIG_NUMBER_S1], "1.00");
    lv_obj_add_style(dashboard_labels[BIG_NUMBER_S1], LV_STATE_DEFAULT, &style_label_font_44);

    dashboard_labels[BIG_NUMBER_S2] = lv_label_create(screen_dashboard, NULL);
    lv_obj_align(dashboard_labels[BIG_NUMBER_S2], NULL, LV_ALIGN_IN_TOP_MID, -22, 10);
    lv_label_set_text(dashboard_labels[BIG_NUMBER_S2], "1.00");
    lv_obj_add_style(dashboard_labels[BIG_NUMBER_S2], LV_STATE_DEFAULT, &style_label_font_44);

    dashboard_labels[BIG_NUMBER_S3] = lv_label_create(screen_dashboard, NULL);
    lv_obj_align(dashboard_labels[BIG_NUMBER_S3], NULL, LV_ALIGN_IN_TOP_MID, 80, 10);
    lv_label_set_text(dashboard_labels[BIG_NUMBER_S3], "1.00");
    lv_obj_add_style(dashboard_labels[BIG_NUMBER_S3], LV_STATE_DEFAULT, &style_label_font_44);

    /* STOP and TIME */
    dashboard_labels[BIG_NUMBER_STOP] = lv_label_create(screen_dashboard, NULL);
    lv_obj_align(dashboard_labels[BIG_NUMBER_STOP], NULL, LV_ALIGN_IN_TOP_MID, 30, LINE3_VALUE_Y_IDLE);
    lv_label_set_text(dashboard_labels[BIG_NUMBER_STOP], "0");
    lv_obj_add_style(dashboard_labels[BIG_NUMBER_STOP], LV_STATE_DEFAULT, &style_label_font_36);

    dashboard_labels[BIG_NUMBER_STIME] = lv_label_create(screen_dashboard, NULL);
    lv_obj_align(dashboard_labels[BIG_NUMBER_STIME], NULL, LV_ALIGN_IN_TOP_MID, 108, LINE3_VALUE_Y_IDLE);
    lv_label_set_text(dashboard_labels[BIG_NUMBER_STIME], "0");
    lv_obj_add_style(dashboard_labels[BIG_NUMBER_STIME], LV_STATE_DEFAULT, &style_label_font_36);
}

/**
 * @brief  Check sensor value and return status
 */
static int ui_check_sensor_state(uint32_t s1, uint16_t setpoint)
{
    if (warining_timeout > 6)
    {
        valid_sensor_count = 3;
        return SENSOR_NORMAL;
    }

    /* >= 15% lower */
    if ((s1 < setpoint) && ((setpoint - s1) * 100 > (setpoint * 15)))
    {
        return SENSOR_BELOW_SETPOINT;
    }

    /* >= 15% higher */
    if ((s1 > setpoint) && ((s1 - setpoint) * 100 > (setpoint * 15)))
    {
        return SENSOR_ABOVE_SETPOINT;
    }

    valid_sensor_count++;
    sum_valid_sensor += s1;
    warining_timeout = 0;

    return SENSOR_NORMAL;
}

/**
 * @brief  Update main idle screen
 */
static void ui_big_number_set_idle(bool idle) {
    is_idle = idle;

    if (is_idle) {
        lv_obj_set_y(dashboard_labels[BIG_NUMBER_DEPTH_LABEL], LINE2_LABEL_Y_IDLE);
        lv_obj_set_y(dashboard_labels[BIG_NUMBER_SETPOINT_LABEL], LINE2_LABEL_Y_IDLE);
        lv_obj_set_y(dashboard_labels[BIG_NUMBER_D_TIME_LABEL], LINE2_LABEL_Y_IDLE);
        lv_obj_set_y(dashboard_labels[BIG_NUMBER_02_HE_LABEL], LINE3_LABEL_Y_IDLE);
        lv_obj_set_y(dashboard_labels[BIG_NUMBER_TTS_LABEL], LINE3_LABEL_Y_IDLE);
        lv_obj_set_y(dashboard_labels[BIG_NUMBER_STOP_LABEL], LINE3_LABEL_Y_IDLE);

        lv_obj_set_hidden(dashboard_labels[BIG_NUMBER_NDL_LABEL], true);
        lv_obj_set_hidden(dashboard_labels[BIG_NUMBER_NDL], true);
        lv_obj_set_hidden(dashboard_labels[BIG_NUMBER_STIME], false);
        lv_obj_set_hidden(dashboard_labels[BIG_NUMBER_STIME_LABEL], false);

        lv_obj_set_y(dashboard_labels[BIG_NUMBER_DEPTH], LINE2_VALUE_Y_IDLE);
        lv_obj_set_y(dashboard_labels[BIG_NUMBER_SETPOINT], LINE2_VALUE_Y_IDLE);
        lv_obj_set_y(dashboard_labels[BIG_NUMBER_D_TIME], LINE2_VALUE_Y_IDLE);
        lv_obj_set_y(dashboard_labels[BIG_NUMBER_02_HE], LINE3_VALUE_Y_IDLE);
        lv_obj_set_y(dashboard_labels[BIG_NUMBER_TTS], LINE3_VALUE_Y_IDLE);
        lv_obj_set_y(dashboard_labels[BIG_NUMBER_STOP], LINE3_VALUE_Y_IDLE);
    }
    else {
        lv_obj_set_y(dashboard_labels[BIG_NUMBER_DEPTH_LABEL], LINE2_LABEL_Y_MENU);
        lv_obj_set_y(dashboard_labels[BIG_NUMBER_SETPOINT_LABEL], LINE2_LABEL_Y_MENU);
        lv_obj_set_y(dashboard_labels[BIG_NUMBER_D_TIME_LABEL], LINE2_LABEL_Y_MENU);
        lv_obj_set_y(dashboard_labels[BIG_NUMBER_02_HE_LABEL], LINE3_LABEL_Y_MENU);
        lv_obj_set_y(dashboard_labels[BIG_NUMBER_TTS_LABEL], LINE3_LABEL_Y_MENU);
        lv_obj_set_y(dashboard_labels[BIG_NUMBER_STOP_LABEL], LINE3_LABEL_Y_MENU);

        lv_obj_set_hidden(dashboard_labels[BIG_NUMBER_NDL], false);
        lv_obj_set_hidden(dashboard_labels[BIG_NUMBER_NDL_LABEL], false);
        lv_obj_set_hidden(dashboard_labels[BIG_NUMBER_STIME_LABEL], true);
        lv_obj_set_hidden(dashboard_labels[BIG_NUMBER_STIME], true);

        lv_obj_set_y(dashboard_labels[BIG_NUMBER_DEPTH], LINE2_VALUE_Y_MENU);
        lv_obj_set_y(dashboard_labels[BIG_NUMBER_SETPOINT], LINE2_VALUE_Y_MENU);
        lv_obj_set_y(dashboard_labels[BIG_NUMBER_D_TIME], LINE2_VALUE_Y_MENU);
        lv_obj_set_y(dashboard_labels[BIG_NUMBER_02_HE], LINE3_VALUE_Y_MENU);
        lv_obj_set_y(dashboard_labels[BIG_NUMBER_TTS], LINE3_VALUE_Y_MENU);
        lv_obj_set_y(dashboard_labels[BIG_NUMBER_STOP], LINE3_VALUE_Y_MENU);
    }
}

/******************************************************************************/

/*!
 * @brief  Initialize the big number mode screen
 */
void ui_big_number_init(void) {
    screen_dashboard = lv_obj_create(NULL, NULL);
    lv_obj_add_style(screen_dashboard, LV_STATE_DEFAULT, &style_dashboard);

    ui_dashboard_static_init();
    ui_dashboard_param_init();
}

/*!
 * @brief  Set the big number mode screen
 */
void ui_big_number_loadscreen(void) {
    lv_scr_load(screen_dashboard);
    ui_big_number_set_idle(true);
}

/**
 * @brief  Set menu text
 */
void ui_big_number_set_menu_text(char *text) {
    int16_t width = lv_obj_get_width(dashboard_labels[BIG_NUMBER_MENU]);
    int16_t x = (316 - width) / 2;
    
    lv_label_set_text(dashboard_labels[BIG_NUMBER_MENU], text);
    lv_obj_align(dashboard_labels[BIG_NUMBER_MENU], NULL, LV_ALIGN_IN_TOP_LEFT, x, lv_obj_get_y(dashboard_labels[BIG_NUMBER_MENU]));
    lv_obj_set_state(dashboard_labels[BIG_NUMBER_MENU], LV_STATE_DEFAULT);
        
    ui_big_number_set_idle(false);
}

/**
 * @brief  Update big number screen
 */
void ui_big_number_update(void) {
    int i;
    static char char_value[32];
    static uint16_t last_depth_deco = 0;

    /* Sensors 1, 2, 3 */
    for (i = 0; i < O2_SENSOR_NUM; i++) {
        ui_utils_convert_sensor(char_value, system_status.sensor[i].data);
        ui_utils_set_text_center(dashboard_labels[i], char_value, panel_sensor[i]);
    }

    /* Current Depth from pressure */
    if ((system_status.dive_state != DIVE_CONTROL_STATE) || (system_status.current_distance < DIVETHRESH_DECIMETERS)) {
        /* Check DIVETHRESH_DECIMETERS to avoid displaying small depth */
        system_status.current_distance = 0;
    }

    if (system_config.user_setting.flags.depth_units == 1) {
        sprintf(char_value, "%d.%d", system_status.current_distance / 10, system_status.current_distance % 10);
        lv_label_set_text(dashboard_labels[BIG_NUMBER_DEPTH_LABEL], "DEPTH (m)");
    }
    else {
        sprintf(char_value, "%d", system_status.current_distance);
        lv_label_set_text(dashboard_labels[BIG_NUMBER_DEPTH_LABEL], "DEPTH (ft)");
    }
    ui_utils_set_text_center(dashboard_labels[BIG_NUMBER_DEPTH], char_value, dashboard_labels[BIG_NUMBER_DEPTH_LABEL]);

    /* Display in minutes or in seconds */
    int time_s = (system_status.dive_state == DIVE_CONTROL_STATE) ? system_status.dive_time_sec : system_status.surface_time_sec;
    uint8_t unit = ui_utils_convert_dive_time_with_unit(char_value, time_s);
    if (unit == 2) {
        lv_label_set_text_fmt(dashboard_labels[BIG_NUMBER_D_TIME_LABEL], "%cTIME (h)", system_status.dive_state == DIVE_CONTROL_STATE ? 'D' : 'S');
    }
    else if (unit == 1) {
        lv_label_set_text_fmt(dashboard_labels[BIG_NUMBER_D_TIME_LABEL], "%cTIME (m)", system_status.dive_state == DIVE_CONTROL_STATE ? 'D' : 'S');
    }
    else {
        lv_label_set_text_fmt(dashboard_labels[BIG_NUMBER_D_TIME_LABEL], "%cTIME (s)", system_status.dive_state == DIVE_CONTROL_STATE ? 'D' : 'S');
    }
    ui_utils_set_text_center(dashboard_labels[BIG_NUMBER_D_TIME], char_value, dashboard_labels[BIG_NUMBER_D_TIME_LABEL]);

    if (system_status.dive_state == SURFACE_CONTROL_STATE) {
        uint8_t unit = ui_utils_convert_dive_time_with_unit(char_value, system_status.last_dive_info.total_time_last_dive);
        lv_label_set_text_fmt(dashboard_labels[BIG_NUMBER_STIME_LABEL], "TIME (%c)", unit ? 'm' : 's');
        ui_utils_set_text_center(dashboard_labels[BIG_NUMBER_STIME], char_value, dashboard_labels[BIG_NUMBER_STIME_LABEL]);
    }
    else {
        if (system_status.deco_mode) {
            sprintf(char_value, "%d@%d", system_status.deco_time, system_status.deco_depth);
            lv_label_set_text(dashboard_labels[BIG_NUMBER_STIME_LABEL], "DECO");
            ui_utils_set_text_center(dashboard_labels[BIG_NUMBER_STIME], char_value, dashboard_labels[BIG_NUMBER_STIME_LABEL]);
        }
        else {
            lv_label_set_text(dashboard_labels[BIG_NUMBER_STIME_LABEL], "NSTOP");
            ui_utils_set_text_center(dashboard_labels[BIG_NUMBER_STIME], "NS", dashboard_labels[BIG_NUMBER_STIME_LABEL]);
        }
    }

    /* Current Set Point selected */
    ui_utils_convert_setpoint(char_value, system_status.set_point.data, system_status.cc_mode);
    ui_utils_set_text_center(dashboard_labels[BIG_NUMBER_SETPOINT], char_value, dashboard_labels[BIG_NUMBER_SETPOINT_LABEL]);

    /* Total Time to Surface from calculation in deco program */
    if (system_status.time_to_surface.blink) {
        ui_utils_set_text_center(dashboard_labels[BIG_NUMBER_TTS], "!!!", dashboard_labels[BIG_NUMBER_TTS_LABEL]);
    }
    else {
        sprintf(char_value, "%d", system_status.time_to_surface.data);
        ui_utils_set_text_center(dashboard_labels[BIG_NUMBER_TTS], char_value, dashboard_labels[BIG_NUMBER_TTS_LABEL]);
    }

    /* Oxygen/Helium fraction of Gas Mix selected */
    sprintf(char_value, "%d/%d", system_status.gas_mix.O2, system_status.gas_mix.he);
    ui_utils_set_text_center(dashboard_labels[BIG_NUMBER_02_HE], char_value, dashboard_labels[BIG_NUMBER_02_HE_LABEL]);

    /* Deco depth */
    if (system_status.deco_mode) {
        if (last_depth_deco != system_status.deco_depth) {
            lv_state_t state = last_depth_deco < system_status.deco_depth ? LV_STATE_PRESSED : LV_STATE_FOCUSED;
            stop_time_blink = 6;
            last_depth_deco = system_status.deco_depth;
            lv_obj_set_state(panel_stop, state);
        }

        sprintf(char_value, "%d", system_status.deco_depth);
        ui_utils_set_text_center(dashboard_labels[BIG_NUMBER_STOP], char_value, dashboard_labels[BIG_NUMBER_STOP_LABEL]);

        lv_obj_set_width(panel_stop, lv_obj_get_width(dashboard_labels[BIG_NUMBER_STOP]) + 12);
        lv_obj_set_height(panel_stop, lv_obj_get_height(dashboard_labels[BIG_NUMBER_STOP]));
        lv_obj_align(panel_stop, NULL, LV_ALIGN_IN_TOP_LEFT, lv_obj_get_x(dashboard_labels[BIG_NUMBER_STOP]) - 6, lv_obj_get_y(dashboard_labels[BIG_NUMBER_STOP]) - 2);

        sprintf(char_value, "%d", system_status.deco_time);
        ui_utils_set_text_center(dashboard_labels[BIG_NUMBER_NDL], char_value, dashboard_labels[BIG_NUMBER_NDL_LABEL]);
    }
    else {
        stop_time_blink = 0;
        ui_utils_set_text_center(dashboard_labels[BIG_NUMBER_NDL], "0", dashboard_labels[BIG_NUMBER_NDL_LABEL]);
        ui_utils_set_text_center(dashboard_labels[BIG_NUMBER_STOP], "ND", dashboard_labels[BIG_NUMBER_STOP_LABEL]);
    }

    /* Check menu state */
    valid_sensor_count = 0;
    sum_valid_sensor = 0;
    for (i = 0; i < O2_SENSOR_NUM; i++) {
        int state = ui_check_sensor_state(system_status.sensor[i].data, system_status.set_point.data);
        if (state == SENSOR_BELOW_SETPOINT) {
            system_status.sensor[i].blink = 1;
        }
        else if (state == SENSOR_ABOVE_SETPOINT) {
            system_status.sensor[i].blink = 1;
        }
        else {
            system_status.sensor[i].blink = 0;
            lv_obj_set_state(panel_sensor[i], LV_STATE_DEFAULT);
            lv_obj_set_state(dashboard_labels[i], LV_STATE_DEFAULT);
        }
        sensor_warn_mask |= 1 << state;
    }


    /* Update menu label if in the menu state */
    if (is_idle) {
        /* We calculate all valid sensor data */
        if (valid_sensor_count > 0) {
            uint32_t val = sum_valid_sensor / valid_sensor_count;
            int state = ui_check_sensor_state(val, system_status.set_point.data);
            sensor_warn_mask = 1 << state;
        }

        if ((sensor_warn_mask & (1 << SENSOR_BELOW_SETPOINT)) && (sensor_warn_mask & (1 << SENSOR_ABOVE_SETPOINT))) {
            is_warning = true;
            ui_big_number_set_menu_text("CELL IMBALANCE");
            lv_obj_set_state(dashboard_labels[BIG_NUMBER_MENU], SENSOR_HIGH_STATE);
        }
        else if (sensor_warn_mask & (1 << SENSOR_BELOW_SETPOINT)) {
            is_warning = true;
            ui_big_number_set_menu_text("BELOW SETPOINT");
            lv_obj_set_state(dashboard_labels[BIG_NUMBER_MENU], SENSOR_LOW_STATE);
        }
        else if (sensor_warn_mask & (1 << SENSOR_ABOVE_SETPOINT)) {
            is_warning = true;
            ui_big_number_set_menu_text("ABOVE SETPOINT");
            lv_obj_set_state(dashboard_labels[BIG_NUMBER_MENU], SENSOR_HIGH_STATE);
        }
        else {
            is_warning = false;
            ui_big_number_set_menu_text(" ");
            lv_obj_set_state(dashboard_labels[BIG_NUMBER_MENU], LV_STATE_DEFAULT);
        }
        is_idle = true;    /* ui_big_number_set_menu_text change is_idle so we change it back */
    }
}

/**
 * @brief  Update blink (should call every 500ms)
 */
void ui_big_number_blink(void) {
    int i;

    warining_timeout++;
    if (warining_timeout > 12) {
        warining_timeout = 0;
    }

    for (i = 0; i < O2_SENSOR_NUM; i++) {
        if (system_status.sensor[i].blink) {
            if (dashboard_blink) {
                if (system_status.sensor[i].data < system_status.set_point.data) {
                    lv_obj_set_state(panel_sensor[i], SENSOR_LOW_STATE);
                    lv_obj_set_state(dashboard_labels[i], SENSOR_LOW_STATE);
                }
                else {
                    lv_obj_set_state(panel_sensor[i], SENSOR_HIGH_STATE);
                    lv_obj_set_state(dashboard_labels[i], SENSOR_HIGH_STATE);
                }
            }
            else {
                if (system_status.sensor[i].data < system_status.set_point.data) {
                    lv_obj_set_state(dashboard_labels[i], SENSOR_LOW_STATE);
                }
                else {
                    lv_obj_set_state(dashboard_labels[i], SENSOR_HIGH_STATE);
                }
                lv_obj_set_state(panel_sensor[i], LV_STATE_DEFAULT);
            }
        }
    }

    /* Only blink when warning */
    if ((stop_time_blink > 0) && is_idle && (!is_warning)) {
        stop_time_blink--;
        lv_obj_set_hidden(panel_stop, dashboard_blink);
        dashboard_blink = !dashboard_blink;
    }
}
