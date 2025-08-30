/*
 *  ui_big_number.h
 *
 *  Created on: Aug 30, 2025
 */

#ifndef _UI_BIG_NUMBER_H_
#define _UI_BIG_NUMBER_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/



/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/



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
 * @brief  Initialize the big number mode screen
 * @param  None
 * @retval None
 */
void ui_big_number_init(void);

/*!
 * @brief  Set the big number mode screen
 * @param  None
 * @retval None
 */
void ui_big_number_loadscreen(void);

/**
 * @brief  Set menu text
 * @param  text: Text menu to display
 * @retval None
 */
void ui_big_number_set_menu_text(char *text);

/**
 * @brief  Update big number screen
 * @param  None
 * @retval None
 */
void ui_big_number_update(void);

/**
 * @brief  Update blink (should call every 500ms)
 * @param  None
 * @retval None
 */
void ui_big_number_blink(void);

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _UI_BIG_NUMBER_H_ */
