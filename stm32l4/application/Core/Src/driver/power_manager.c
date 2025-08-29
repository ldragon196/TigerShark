/*
 *  power_manager.c
 *
 *  Created on: Aug 29, 2025
 */

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "main.h"
#include "log.h"
#include "power_manager.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

/* System */
#define WAKE_UP()           HAL_GPIO_WritePin(WAKE_PW_EN_GPIO_Port, WAKE_PW_EN_Pin, GPIO_PIN_SET)
#define SLEEP_MODE()        HAL_GPIO_WritePin(WAKE_PW_EN_GPIO_Port, WAKE_PW_EN_Pin, GPIO_PIN_RESET)

/* CAN and RS485 */
#define COMM_POWER_ON()     do { \
                                HAL_GPIO_WritePin(COM_PW_EN_GPIO_Port, COM_PW_EN_Pin, GPIO_PIN_SET); \
                                HAL_GPIO_WritePin(CAN_SHDN_GPIO_Port, CAN_SHDN_Pin, GPIO_PIN_RESET); \
                            } while(0)

#define COMM_POWER_OFF()    do { \
                                HAL_GPIO_WritePin(COM_PW_EN_GPIO_Port, COM_PW_EN_Pin, GPIO_PIN_RESET); \
                                HAL_GPIO_WritePin(CAN_SHDN_GPIO_Port, CAN_SHDN_Pin, GPIO_PIN_SET); \
                            } while(0)

/* RF */
#define RF_POWER_ON()       HAL_GPIO_WritePin(RF_PW_EN_GPIO_Port, RF_PW_EN_Pin, GPIO_PIN_SET)
#define RF_POWER_OFF()      HAL_GPIO_WritePin(RF_PW_EN_GPIO_Port, RF_PW_EN_Pin, GPIO_PIN_RESET)

/* Light Sensor */
#define LIGHT_SENSOR_POWER_ON()  HAL_GPIO_WritePin(LIGHT_SENSOR_EN_GPIO_Port, LIGHT_SENSOR_EN_Pin, GPIO_PIN_SET)
#define LIGHT_SENSOR_POWER_OFF() HAL_GPIO_WritePin(LIGHT_SENSOR_EN_GPIO_Port, LIGHT_SENSOR_EN_Pin, GPIO_PIN_RESET)

/* O2 Sensor */
#define O2_SENSOR_POWER_ON()     HAL_GPIO_WritePin(O2_SENSOR_EN_GPIO_Port, O2_SENSOR_EN_Pin, GPIO_PIN_SET)
#define O2_SENSOR_POWER_OFF()    HAL_GPIO_WritePin(O2_SENSOR_EN_GPIO_Port, O2_SENSOR_EN_Pin, GPIO_PIN_RESET)

/* DIVA */
#define DIVA_POWER_ON()     HAL_GPIO_WritePin(DIVA_PW_EN_GPIO_Port, DIVA_PW_EN_Pin, GPIO_PIN_SET)
#define DIVA_POWER_OFF()    HAL_GPIO_WritePin(DIVA_PW_EN_GPIO_Port, DIVA_PW_EN_Pin, GPIO_PIN_RESET)

/* WiFi */
#define WIFI_ON()           do { \
                                HAL_GPIO_WritePin(WIFI_PW_EN_GPIO_Port, WIFI_PW_EN_Pin, GPIO_PIN_SET); \
                                HAL_GPIO_WritePin(WIFI_EN_GPIO_Port, WIFI_EN_Pin, GPIO_PIN_SET); \
                            } while (0)

#define WIFI_OFF()          do { \
                                HAL_GPIO_WritePin(WIFI_PW_EN_GPIO_Port, WIFI_PW_EN_Pin, GPIO_PIN_RESET); \
                                HAL_GPIO_WritePin(WIFI_EN_GPIO_Port, WIFI_EN_Pin, GPIO_PIN_RESET); \
                            } while (0)

/* LCD Power */
#define LCD_ON()            HAL_GPIO_WritePin(LCD_PW_EN_GPIO_Port, LCD_PW_EN_Pin, GPIO_PIN_SET)
#define LCD_OFF()           HAL_GPIO_WritePin(LCD_PW_EN_GPIO_Port, LCD_PW_EN_Pin, GPIO_PIN_RESET)

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/



/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/



/******************************************************************************/

/*!
 * @brief  Change lcd brightness
 */
void power_change_lcd_brightness(uint8_t percentage) {
    int brightness = (percentage * 499) / 100;    /* Timer count max 499 */
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, brightness);
}

/*!
 * @brief  Turn esp32 wifi on/off
 */
void power_wifi_control(bool on) {
    if (on) {
        WIFI_ON();
    }
    else {
        WIFI_OFF();
    }
}

/*!
 * @brief  Turn board and components on
 */
void power_board_on(void) {
    WAKE_UP();
    LIGHT_SENSOR_POWER_OFF();
    DIVA_POWER_OFF();
    O2_SENSOR_POWER_OFF();
    RF_POWER_OFF();
    COMM_POWER_OFF();
    WIFI_OFF();
    LED_RED_OFF();
    LED_GREEN_OFF();
    LED_BLUE_OFF();

    /* LEDs timer */
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Start(&htim3, TIM_CHANNEL_3);

    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
    HAL_TIMEx_PWMN_Start(&htim3, TIM_CHANNEL_4);

    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
    HAL_TIMEx_PWMN_Start(&htim2, TIM_CHANNEL_4);

    /* LCD brightness */
    LCD_ON();
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
    power_change_lcd_brightness(100);    /* Max brightness at start */
}

/*!
 * @brief  Turn board and components off
 */
void power_board_off(void) {
    LCD_OFF();
    LIGHT_SENSOR_POWER_OFF();
    DIVA_POWER_OFF();
    O2_SENSOR_POWER_OFF();
    RF_POWER_OFF();
    COMM_POWER_OFF();
    WIFI_OFF();
    LED_RED_OFF();
    LED_GREEN_OFF();
    LED_BLUE_OFF();

    SLEEP_MODE();
}
