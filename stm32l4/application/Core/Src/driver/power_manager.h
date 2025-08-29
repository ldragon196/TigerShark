/*
 *  power_manager.h
 *
 *  Created on: Aug 29, 2025
 */

#ifndef _POWER_MANAGER_H_
#define _POWER_MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include <stdbool.h>

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

#define SET_LCD_BRIGHTNESS(a)    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, a)

#define LED_RED_ON()             __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 500)
#define LED_RED_OFF()            __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0)

#define LED_GREEN_ON()           __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 500)
#define LED_GREEN_OFF()          __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0)

#define LED_BLUE_ON()            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 500)
#define LED_BLUE_OFF()           __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 0)

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

/*!
 * @brief  Turn esp32 wifi on/off
 * @param  on: true to turn on, false to turn off
 * @retval None
 */
void power_wifi_control(bool on);

/*!
 * @brief  Turn board and components on/off
 * @param  None
 * @retval None
 */
void power_board_on(void);

/*!
 * @brief  Turn board and components off
 * @param  None
 * @retval None
 */
void power_board_off(void);

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _POWER_MANAGER_H_ */
