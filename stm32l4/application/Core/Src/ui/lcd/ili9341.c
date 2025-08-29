/*
 *  ili9341.c
 *
 *  Created on: Aug 28, 2025
 */

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "main.h"
#include "app_config.h"
#include "ili9341.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

#define FMC_LCD_BASE ((uint32_t)(0x60000000 | 0x00000000))
#define FMC_LCD ((lcd_fmc_address_t *)FMC_LCD_BASE)

typedef struct lcd_fmc_address_st {
	__IO uint16_t lcd_reg;
	__IO uint16_t lcd_ram;
} lcd_fmc_address_t;

typedef struct lcd_params_st {
	uint16_t lcd_width;
	uint16_t lcd_height;
	uint16_t lcd_id;
	uint8_t lcd_direction;
	uint16_t wram_cmd;
	uint16_t set_x_cmd;
	uint16_t set_y_cmd;
} lcd_params_t;

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

lcd_params_t lcd_params;

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/



/******************************************************************************/

/*!
 * @brief  Write a command to the LCD
 */
static void lcd_write_cmd(volatile uint16_t cmd) {
	FMC_LCD->lcd_reg = cmd;
}

/*!
 * @brief  Read data from the LCD
 */
static uint16_t lcd_read_data(void) {
	volatile uint16_t data;
	data = FMC_LCD->lcd_ram;
	return data;
}

void lcd_write_data(uint8_t x) {
    FMC_LCD->lcd_ram = 0x00FF & x;
}

/*!
 * @brief  Read the LCD ID
 */
static int lcd_read_id(void) {
	/* Trying to perform ILI9341 controller ID Read process of  */
	lcd_write_cmd(0xD3);
	lcd_params.lcd_id = lcd_read_data();
	lcd_params.lcd_id = lcd_read_data();
	lcd_params.lcd_id = lcd_read_data();
	lcd_params.lcd_id <<= 8;
	lcd_params.lcd_id |= lcd_read_data();

	/*  If you read normally , Then return to success  */
	if (lcd_params.lcd_id == 0x9341) {
		return 0;
	}

	/*  Trying to perform NT35310 controller ID Read process of  */
	lcd_write_cmd(0xD4);
	lcd_params.lcd_id = lcd_read_data();
	lcd_params.lcd_id = lcd_read_data();
	lcd_params.lcd_id = lcd_read_data();
	lcd_params.lcd_id <<= 8;
	lcd_params.lcd_id |= lcd_read_data();

	/*  If you read normally , Then return to success  */
	if (lcd_params.lcd_id == 0x5310) {
		return 0;
	}

	/*  Trying to perform NT35510 controller ID Read process of  */
	lcd_write_cmd(0xDA00);
	lcd_params.lcd_id = lcd_read_data();
	lcd_write_cmd(0xDB00);
	lcd_params.lcd_id = lcd_read_data();
	lcd_params.lcd_id <<= 8;
	lcd_write_cmd(0xDC00);
	lcd_params.lcd_id |= lcd_read_data();
	/*  If you read normally , Then return to success  */
	if (lcd_params.lcd_id == 0x8000) {
		lcd_params.lcd_id = 0x5510;
		return 0;
	}

	/*  drive IC I won't support it  */
	lcd_params.lcd_id = 0;
	return -1;
}

/*!
 * @brief  Initialize the ILI9341 display
 */
void ili9341_init(void) {
    lcd_read_id();

    lcd_write_cmd(0x11);
    delay(120);

    lcd_write_cmd(0xCF);
    lcd_write_data(0x00);
    lcd_write_data(0xc3);
    lcd_write_data(0x30);

    lcd_write_cmd(0xED);
    lcd_write_data(0x64);
    lcd_write_data(0x03);
    lcd_write_data(0x12);
    lcd_write_data(0x81);

    lcd_write_cmd(0xE8);
    lcd_write_data(0x85);
    lcd_write_data(0x10);
    lcd_write_data(0x79);

    lcd_write_cmd(0xCB);
    lcd_write_data(0x39);
    lcd_write_data(0x2C);
    lcd_write_data(0x00);
    lcd_write_data(0x34);
    lcd_write_data(0x02);

    lcd_write_cmd(0xF7);
    lcd_write_data(0x20);

    lcd_write_cmd(0xEA);
    lcd_write_data(0x00);
    lcd_write_data(0x00);

    lcd_write_cmd(0xC0);
    lcd_write_data(0x22);

    lcd_write_cmd(0xC1);
    lcd_write_data(0x11);

    lcd_write_cmd(0xC5);
    lcd_write_data(0x3d);
    lcd_write_data(0x20);

    lcd_write_cmd(0xC7);
    lcd_write_data(0xAA);

    lcd_write_cmd(0x36);
    lcd_write_data(0x08);

    lcd_write_cmd(0x3A);
    lcd_write_data(0x55);

    lcd_write_cmd(0xB1);
    lcd_write_data(0x00);
    lcd_write_data(0x13);

    lcd_write_cmd(0xB6);
    lcd_write_data(0x0A);
    lcd_write_data(0xA2);

    lcd_write_cmd(0xF6);
    lcd_write_data(0x01);
    lcd_write_data(0x30);

    lcd_write_cmd(0xF2);
    lcd_write_data(0x00);

    lcd_write_cmd(0x26);
    lcd_write_data(0x01);

    lcd_write_cmd(0xE0);
    lcd_write_data(0x0F);
    lcd_write_data(0x3F);
    lcd_write_data(0x2F);
    lcd_write_data(0x0C);
    lcd_write_data(0x10);
    lcd_write_data(0x0A);
    lcd_write_data(0x53);
    lcd_write_data(0xD5);
    lcd_write_data(0x40);
    lcd_write_data(0x0A);
    lcd_write_data(0x13);
    lcd_write_data(0x03);
    lcd_write_data(0x08);
    lcd_write_data(0x03);
    lcd_write_data(0x00);

    lcd_write_cmd(0xE1);
    lcd_write_data(0x00);
    lcd_write_data(0x00);
    lcd_write_data(0x10);
    lcd_write_data(0x03);
    lcd_write_data(0x0F);
    lcd_write_data(0x05);
    lcd_write_data(0x2C);
    lcd_write_data(0xA2);
    lcd_write_data(0x3F);
    lcd_write_data(0x05);
    lcd_write_data(0x0E);
    lcd_write_data(0x0C);
    lcd_write_data(0x37);
    lcd_write_data(0x3C);
    lcd_write_data(0x0F);

    lcd_write_cmd(0x11);
    delay(120);
    lcd_write_cmd(0x29);
    delay(50);
}

/*!
 * @brief  Set the display window
 */
void ili9341_set_window(unsigned char x0, unsigned char x1, unsigned int y0, unsigned int y1) {
	unsigned char YSH, YSL, YEH, YEL;

	YSH = y0 >> 8;
	YSL = y0;

	YEH = y1 >> 8;
	YEL = y1;

	lcd_write_cmd(0x2A);
	lcd_write_data(0x00);
	lcd_write_data(x0);
	lcd_write_data(0x00);
	lcd_write_data(x1);
	lcd_write_cmd(0x2B);
	lcd_write_data(YSH);
	lcd_write_data(YSL);
	lcd_write_data(YEH);
	lcd_write_data(YEL);
	lcd_write_cmd(0x2C);
}

/*!
 * @brief  Write data to the display
 */
void ili9341_write_data(uint16_t data) {
    FMC_LCD->lcd_ram = data;
}
