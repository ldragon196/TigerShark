/*
 *  lv_port_disp.c
 *
 *  Created on: Aug 28, 2025
 */

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "lvgl.h"
#include "stm32l4xx_hal.h"
#include "ili9341.h"
#include "lv_port_disp.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/



/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

/* LVGL requires a buffer where it internally draws the widgets.
 * Later this buffer will passed your display drivers `flush_cb` to copy its content to your display.
 * The buffer has to be greater than 1 display row
 *
 * There are three buffering configurations:
 * 1. Create ONE buffer with some rows:
 *      LVGL will draw the display's content here and writes it to your display
 *
 * 2. Create TWO buffer with some rows:
 *      LVGL will draw the display's content to a buffer and writes it your display.
 *      You should use DMA to write the buffer's content to the display.
 *      It will enable LVGL to draw the next part of the screen to the other buffer while
 *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
 *
 * 3. Create TWO screen-sized buffer:
 *      Similar to 2) but the buffer have to be screen sized. When LVGL is ready it will give the
 *      whole frame to display. This way you only need to change the frame buffer's address instead of
 *      copying the pixels.
 */
#define draw_buf_2_1      (lv_color_t*)(SRAM2_BASE)
#define draw_buf_2_2      (lv_color_t*)(SRAM2_BASE +0x4000)
static lv_disp_buf_t draw_buf_dsc_2;

static lv_disp_drv_t disp_drv;

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

static void disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);

/******************************************************************************/

/**
 * @brief  Display flushing
 */
static void disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    int32_t x;
    int32_t y;

    if (area->x2 < 0) return;
    if (area->y2 < 0) return;
    if (area->x1 > LV_HOR_RES_MAX - 1) return;
    if (area->y1 > LV_VER_RES_MAX - 1) return;

    ili9341_set_window(area->x1, area->x2, area->y1, area->y2);
    for (y = area->y1; y <= area->y2; y++) {
        for (x = area->x1; x <= area->x2; x++) {
            ili9341_write_data(color_p->full);
            color_p++;
        }
    }

    /* Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp);
}

/*!
 * @brief  Initialize the display driver
 */
void lv_port_disp_init(void) {
    ili9341_init();
    lv_disp_buf_init(&draw_buf_dsc_2, draw_buf_2_1, draw_buf_2_2, LV_HOR_RES_MAX * LV_VER_RES_MAX / 10);

    /* Initialize the display */
    lv_disp_drv_init(&disp_drv);

    /* Set the resolution of the display */
    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
    disp_drv.rotated = LV_DISP_ROT_NONE;
    disp_drv.sw_rotate = 1;

    /* Used to copy the buffer's content to the display */
    disp_drv.flush_cb = disp_flush;
    disp_drv.set_px_cb = 0;

    /* Set a display buffer */
    disp_drv.buffer = &draw_buf_dsc_2;

    /* Finally register the driver */
    lv_disp_drv_register(&disp_drv);

    /* Initialize the (dummy) input device driver */
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_NONE;
    lv_indev_drv_register(&indev_drv);
}
