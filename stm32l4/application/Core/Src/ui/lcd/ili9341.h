/*
 *  ili9341.h
 *
 *  Created on: Aug 28, 2025
 */

#ifndef _ILI9341_H_
#define _ILI9341_H_

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
 * @brief  Initialize the ILI9341 display
 * @param  None
 * @retval None
 */
void ili9341_init(void);

/*!
 * @brief  Set the display window
 * @param  x0: Start X coordinate
 * @param  x1: End X coordinate
 * @param  y0: Start Y coordinate
 * @param  y1: End Y coordinate
 * @retval None
 */
void ili9341_set_window(unsigned char x0, unsigned char x1, unsigned int y0, unsigned int y1);

/*!
 * @brief  Write data to the display
 * @param  data: Data to be written
 * @retval None
 */
void ili9341_write_data(uint16_t data);

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _ILI9341_H_ */
