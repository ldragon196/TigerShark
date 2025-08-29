/*
 *  user_intf.c
 *
 *  Created on: Aug 29, 2025
 */

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "main.h"
#include "cmsis_os2.h"
#include "app_config.h"
#include "log.h"
#include "power_manager.h"
#include "user_intf.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

#define USER_INTF_TICK  20  /* in ms */

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

static osThreadId_t user_intf_task_handle;
static const osThreadAttr_t user_intf_task_attributes = {
    .name = "user_intf_task",
    .priority = (osPriority_t) osPriorityNormal,
    .stack_size = 2048
};

static int led_state = LED_IDLE;
static button_state_t button_state[BUTTON_COUNT];

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/



/******************************************************************************/


/*!
 * @brief  Loop for check button state
 */
static void user_intf_button_process(void) {
    int reading[BUTTON_COUNT];
    uint32_t now = current_ms();

    reading[BUTTON_TAP] = HAL_GPIO_ReadPin(MAIN_SW_GPIO_Port, MAIN_SW_Pin);
    reading[BUTTON_MAIN] = HAL_GPIO_ReadPin(TAP_SW_GPIO_Port, TAP_SW_Pin);

    for (int i = 0; i < BUTTON_COUNT; i++) {
        /* Check if state is changed */
        if (reading[i] != button_state[i].last_state) {
            button_state[i].last_time = now;
        }

        /* Handle after noise */
        if (((now - button_state[i].last_time) & 0xFFFFFFFF) > BUTTON_DEBOUNCE_MS) {
            if (reading[i] != button_state[i].state) {
                button_state[i].state = reading[i];

                /* Button is pressed. Call handler */
                if (button_state[i].state != BUTTON_IDLE_LEVEL) {
                    if (button_state[i].callback != NULL) {
                        button_state[i].callback();
                    }
                }
            }
        }

        button_state[i].last_state = reading[i];
    }
}

/*!
 * @brief  Process user interface events
 */
static void user_intf_task(void *argument) {
    int led_counter = 0;
    int blink_led = 0;

    /* Blink leds after power up */
    for (uint8_t i = 0; i < 3; i++) {
        LED_RED_ON(); LED_GREEN_ON(); LED_BLUE_ON();
        delay(100);
        LED_RED_OFF(); LED_GREEN_OFF(); LED_BLUE_OFF();
        delay(400);
    }

    /* Handle user input and control leds */
    while (1) {
        delay(USER_INTF_TICK);
        user_intf_button_process();

        /* Leds process */
        led_counter++;
        if (led_counter == 10) {
            blink_led = 0;    /* Turn led on 200 ms to save power */
        }
        else if (led_counter == 40) {
            led_counter = 0;
            blink_led = 1;    /* Turn led off 800ms */
        }
        else {
            continue;
        }

        LED_RED_OFF(); LED_GREEN_OFF(); LED_BLUE_OFF();

        switch (led_state) {
            case LED_CAN_ONLINE:
                if (blink_led) {
                    LED_RED_ON(); LED_BLUE_ON();
                }
                break;

            case LED_WARNING_LOW:
                if (blink_led) {
                    LED_RED_ON();
                }
                break;

            case LED_WARNING_HIGH:
                if (blink_led) {
                    LED_GREEN_ON();
                }
                break;

            case LED_TRANSITION:
                if (blink_led) {
                    LED_RED_ON();
                }
                else {
                    LED_GREEN_ON();
                }
                break;

            case LED_ASCENT_HIGH:
                if (blink_led) {
                    LED_BLUE_ON();
                }
                break;

            case LED_IDLE:
            default:
                break;
        }
    }
}

/*!
 * @brief  Register button callback
 */
void user_intf_register_button_callback(uint8_t button_id, button_callback_t callback) {
    if (button_id < BUTTON_COUNT) {
        button_state[button_id].callback = callback;
    }
}

/*!
 * @brief  Set leds state
 */
void user_intf_set_led(uint8_t state) {
    led_state = state;
}

/*!
 * @brief  Initialize buttons/leds
 */
void user_intf_init(void) {
    user_intf_task_handle = osThreadNew(user_intf_task, NULL, &user_intf_task_attributes);
    memset(&button_state, 0, sizeof(button_state));
}
