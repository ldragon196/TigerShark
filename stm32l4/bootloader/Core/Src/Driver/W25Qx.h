/*
 *  W25Qx.h
 *
 *  Created on: Aug 28, 2025
 */

#ifndef _W25QX_H_
#define _W25QX_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "main.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

#define W25Q128FV_FLASH_SIZE                 0x1000000 /* 128 MBits => 16MBytes */
#define W25Q128FV_BLOCK_SIZE                 0x10000   /* 256 block sectors of 64KBytes */
#define W25Q128FV_SECTOR_SIZE                0x1000    /* 4096 sectors of 4kBytes */
#define W25Q128FV_PAGE_SIZE                  0x100     /* 65536 pages of 256 bytes */

#define W25Q128FV_DUMMY_CYCLES_READ          4
#define W25Q128FV_DUMMY_CYCLES_READ_QUAD     10

#define W25Q128FV_BULK_ERASE_MAX_TIME        250000
#define W25Q128FV_SECTOR_ERASE_MAX_TIME      3000
#define W25Q128FV_SUBSECTOR_ERASE_MAX_TIME   800
#define W25Qx_TIMEOUT_VALUE                  1000

/* Reset Operations */
#define RESET_ENABLE_CMD                     0x66
#define RESET_MEMORY_CMD                     0x99

#define ENTER_QPI_MODE_CMD                   0x38
#define EXIT_QPI_MODE_CMD                    0xFF

/* Identification Operations */
#define READ_ID_CMD                          0x90
#define DUAL_READ_ID_CMD                     0x92
#define QUAD_READ_ID_CMD                     0x94
#define READ_JEDEC_ID_CMD                    0x9F

/* Read Operations */
#define READ_CMD                             0x03
#define FAST_READ_CMD                        0x0B
#define DUAL_OUT_FAST_READ_CMD               0x3B
#define DUAL_INOUT_FAST_READ_CMD             0xBB
#define QUAD_OUT_FAST_READ_CMD               0x6B
#define QUAD_INOUT_FAST_READ_CMD             0xEB

/* Write Operations */
#define WRITE_ENABLE_CMD                     0x06
#define WRITE_DISABLE_CMD                    0x04

/* Register Operations */
#define READ_STATUS_REG1_CMD                 0x05
#define READ_STATUS_REG2_CMD                 0x35
#define READ_STATUS_REG3_CMD                 0x15

#define WRITE_STATUS_REG1_CMD                0x01
#define WRITE_STATUS_REG2_CMD                0x31
#define WRITE_STATUS_REG3_CMD                0x11


/* Program Operations */
#define PAGE_PROG_CMD                        0x02
#define QUAD_INPUT_PAGE_PROG_CMD             0x32


/* Erase Operations */
#define SECTOR_ERASE_CMD                     0x20
#define CHIP_ERASE_CMD                       0xC7

#define PROG_ERASE_RESUME_CMD                0x7A
#define PROG_ERASE_SUSPEND_CMD               0x75


/* Flag Status Register */
#define W25Q128FV_FSR_BUSY                   ((uint8_t)0x01)    /* busy */
#define W25Q128FV_FSR_WREN                   ((uint8_t)0x02)    /* write enable */
#define W25Q128FV_FSR_QE                     ((uint8_t)0x02)    /* quad enable */

enum {
    W25Qx_OK = 0,
    W25Qx_ERROR,
    W25Qx_BUSY,
    W25Qx_TIMEOUT
};

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
 * @brief  Initialize the W25Qx flash memory device
 * @param  None
 * @retval uint8_t: Status (0 = OK, other = Error)
 */
uint8_t w25qx_init(void);

/*!
 * @brief  Get the current status of W25Qx flash
 * @param  None
 * @retval uint8_t: Status register value
 */
uint8_t w25qx_get_status(void);

/*!
 * @brief  Enable write operations on W25Qx flash
 * @param  None
 * @retval uint8_t: Status (0 = OK, other = Error)
 */
uint8_t w25qx_write_enable(void);

/*!
 * @brief  Read flash ID (Manufacturer ID, Device ID)
 * @param  id: Pointer to buffer (at least 2 bytes)
 * @retval None
 */
void w25qx_read_id(uint8_t *id);

/*!
 * @brief  Read data from W25Qx flash memory
 * @param  data: Pointer to buffer to store data
 * @param  read_addr: Start address to read from
 * @param  size: Number of bytes to read
 * @retval uint8_t: Status (0 = OK, other = Error)
 */
uint8_t w25qx_read(uint8_t *data, uint32_t read_addr, uint32_t size);

/*!
 * @brief  Write data to W25Qx flash memory
 * @param  data: Pointer to data buffer
 * @param  write_addr: Start address to write to
 * @param  size: Number of bytes to write
 * @retval uint8_t: Status (0 = OK, other = Error)
 */
uint8_t w25qx_write(uint8_t *data, uint32_t write_addr, uint32_t size);

/*!
 * @brief  Erase one block of W25Qx flash
 * @param  address: Any address inside the block to erase
 * @retval uint8_t: Status (0 = OK, other = Error)
 */
uint8_t w25qx_erase_block(uint32_t address);

/*!
 * @brief  Erase entire W25Qx flash memory chip
 * @param  None
 * @retval uint8_t: Status (0 = OK, other = Error)
 */
uint8_t w25qx_erase_chip(void);

/*!
 * @brief  Read status register 1 of W25Qx flash
 * @param  sr1: Pointer to variable to store status
 * @retval None
 */
void w25qx_read_status_register1(uint8_t *sr1);

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _W25QX_H_ */
