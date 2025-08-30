/*
 *  communication.h
 *
 *  Created on: Aug 30, 2025
 */

#ifndef _COMMUNICATON_H_
#define _COMMUNICATON_H_

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

#define FIFO_SIZE          (2048 + 256)            /* More than 2 command packets */
#define OTA_PART_LENGTH    1024
#define OTA_PACKET_LENGTH  (OTA_PART_LENGTH + 5)   /* 1 byte CMD + 4 bytes offset + 1024 bytes data */
#define COMM_MAX_LENGTH    (OTA_PART_LENGTH + 16)  /* Max length of communication packet */

enum {
    COMM_IDLE_STATE = 0,
    COMM_SOF_STATE,
    COMM_LENGTH_STATE,
    COMM_DATA_STATE,
    COMM_CHECKSUM_STATE,
};

typedef struct {
    uint16_t head;
    uint16_t tail;
    uint16_t count;
    uint8_t data[FIFO_SIZE];
} uart_fifo_t;

typedef struct {
    int state;
    int index;
    int length;
    uint8_t data[COMM_MAX_LENGTH];
} communication_rx_t;

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
 * @brief  Initialize communication api
 * @param  None
 * @retval None
 */
void communication_init(void);

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _COMMUNICATON_H_ */
