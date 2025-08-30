/*
 *  main_process.c
 *
 *  Created on: Aug 29, 2025
 */

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "main.h"
#include "app_config.h"
#include "main_process.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/



/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/


/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

system_status_t system_status;
extern TIM_HandleTypeDef htim6;

/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/



/******************************************************************************/

/*!
 * @brief  Timer 1 second callback
 */
void main_process_1sec_event(void) {
    if (system_status.dive_state == SURFACE_CONTROL_STATE) {
        system_status.surface_time_sec++;
    }
    else {
        system_status.dive_time_sec++;
    }
}

/*!
 * @brief  Initialize components for main process
 */
void main_process_init(void) {
    memset(&system_status, 0, sizeof(system_status_t));
    HAL_TIM_Base_Start_IT(&htim6);

    system_status.set_point.data = system_config.set_point;
    system_status.set_point.blink = false;
}
