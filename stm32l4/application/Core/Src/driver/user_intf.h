/*
 *  user_intf.h
 *
 *  Created on: Aug 29, 2025
 */

#ifndef _USER_INTF_H_
#define _USER_INTF_H_

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

#define BUTTON_DEBOUNCE_MS       50

#define BUTTON_IDLE_LEVEL        0
#define WET_IDLE_LEVEL           1

enum {
    LED_IDLE = 0,
    LED_CAN_ONLINE,
    LED_WARNING_LOW,
    LED_WARNING_HIGH,
    LED_TRANSITION,
    LED_ASCENT_HIGH,
};

enum {
    BUTTON_TAP = 0,
    BUTTON_MAIN,
    BUTTON_COUNT,
};

typedef void (*button_callback_t)(void);

typedef struct {
    int state;
    int last_state;
    uint32_t last_time;
    button_callback_t callback;
} button_state_t;

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
 * @brief  Set leds state
 * @param  state: LED state to set
 * @retval None
 */
void user_intf_set_led(uint8_t state);

/*!
 * @brief  Register button callback
 * @param  None
 * @retval None
 */
void user_intf_register_button_callback(uint8_t button_id, button_callback_t callback);

/*!
 * @brief  Initialize buttons/leds
 * @param  None
 * @retval None
 */
void user_intf_init(void);

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _USER_INTF_H_ */
