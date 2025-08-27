/*
 *  W25Qx.c
 *
 *  Created on: Aug 28, 2025
 */

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "main.h"
#include "app_config.h"
#include "log.h"
#include "W25Qx.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

#define CS_Pin             GPIO_PIN_9
#define CS_GPIO_Port       GPIOB
#define W25Q_SPI           hspi2

#define w25qx_enable()     HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET)
#define w25qx_disable()    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET)

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/



/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

extern SPI_HandleTypeDef hspi2;

/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/



/******************************************************************************/

/**
  * @brief  This function reset the W25Qx
  */
static void w25qx_reset(void)
{
    uint8_t cmd[2] = {RESET_ENABLE_CMD,RESET_MEMORY_CMD};

    w25qx_enable();
    /* Send the reset command */
    HAL_SPI_Transmit(&W25Q_SPI, cmd, 2, W25Qx_TIMEOUT_VALUE);
    w25qx_disable();
}

/*!
 * @brief  Initialize the W25Qx flash memory device
 */
uint8_t w25qx_init(void) {
    /* Reset W25Qxxx */
    w25qx_reset();

    return w25qx_get_status();
}

/*!
 * @brief  Get the current status of W25Qx flash
 */
uint8_t w25qx_get_status(void) {
    uint8_t cmd[] = {READ_STATUS_REG1_CMD};
    uint8_t status;
    
    w25qx_enable();

    /* Send the read status command */
    HAL_SPI_Transmit(&W25Q_SPI, cmd, 1, W25Qx_TIMEOUT_VALUE);
    /* Reception of the data */
    HAL_SPI_Receive(&W25Q_SPI,&status, 1, W25Qx_TIMEOUT_VALUE);

    w25qx_disable();
    
    /* Check the value of the register */
    if((status & W25Q128FV_FSR_BUSY) != 0) {
        return W25Qx_BUSY;
    }

    return W25Qx_OK;
}

/*!
 * @brief  Enable write operations on W25Qx flash
 */
uint8_t w25qx_write_enable(void) {
    uint8_t cmd[] = {WRITE_ENABLE_CMD};
    uint32_t tickstart = HAL_GetTick();

    /*Select the FLASH: Chip Select low */
    w25qx_enable();
    /* Send the read ID command */
    HAL_SPI_Transmit(&W25Q_SPI, cmd, 1, W25Qx_TIMEOUT_VALUE);
    /*Deselect the FLASH: Chip Select high */
    w25qx_disable();
    
    /* Wait the end of Flash writing */
    while (w25qx_get_status() == W25Qx_BUSY) {
        /* Check for the Timeout */
        if ((HAL_GetTick() - tickstart) > W25Qx_TIMEOUT_VALUE) {
            return W25Qx_TIMEOUT;
        }
    }
    
    return W25Qx_OK;
}

/*!
 * @brief  Read flash ID (Manufacturer ID, Device ID)
 */
void w25qx_read_id(uint8_t *id) {
    uint8_t cmd[4] = {READ_ID_CMD,0x00,0x00,0x00};
    
    w25qx_enable();
    /* Send the read ID command */
    HAL_SPI_Transmit(&W25Q_SPI, cmd, 4, W25Qx_TIMEOUT_VALUE);
    /* Reception of the data */
    HAL_SPI_Receive(&W25Q_SPI, id, 2, W25Qx_TIMEOUT_VALUE);
    w25qx_disable();
}

/*!
 * @brief  Read data from W25Qx flash memory
 */
uint8_t w25qx_read(uint8_t *data, uint32_t read_addr, uint32_t size) {
    uint8_t cmd[4];

    /* Configure the command */
    cmd[0] = READ_CMD;
    cmd[1] = (uint8_t)(read_addr >> 16);
    cmd[2] = (uint8_t)(read_addr >> 8);
    cmd[3] = (uint8_t)(read_addr);

    w25qx_enable();
    /* Send the read ID command */
    HAL_SPI_Transmit(&W25Q_SPI, cmd, 4, W25Qx_TIMEOUT_VALUE);
    /* Reception of the data */
    if (HAL_SPI_Receive(&W25Q_SPI, data, size, W25Qx_TIMEOUT_VALUE) != HAL_OK) {
        return W25Qx_ERROR;
    }
    w25qx_disable();

    return W25Qx_OK;
}

/*!
 * @brief  Write data to W25Qx flash memory
 */
uint8_t w25qx_write(uint8_t *data, uint32_t write_addr, uint32_t size) {
    uint8_t cmd[4];
    uint32_t end_addr, current_size, current_addr;
    uint32_t tickstart = HAL_GetTick();
    
    /* Calculation of the size between the write address and the end of the page */
    current_addr = 0;

    while (current_addr <= write_addr) {
        current_addr += W25Q128FV_PAGE_SIZE;
    }
    current_size = current_addr - write_addr;

    /* Check if the size of the data is less than the remaining place in the page */
    if (current_size > size) {
        current_size = size;
    }

    /* Initialize the adress variables */
    current_addr = write_addr;
    end_addr = write_addr + size;
      
    /* Perform the write page by page */
    do {
        /* Configure the command */
        cmd[0] = PAGE_PROG_CMD;
        cmd[1] = (uint8_t)(current_addr >> 16);
        cmd[2] = (uint8_t)(current_addr >> 8);
        cmd[3] = (uint8_t)(current_addr);

        /* Enable write operations */
        w25qx_write_enable();
    
        w25qx_enable();
        /* Send the command */
        if (HAL_SPI_Transmit(&W25Q_SPI,cmd, 4, W25Qx_TIMEOUT_VALUE) != HAL_OK)
        {
            return W25Qx_ERROR;
        }
        
        /* Transmission of the data */
        if (HAL_SPI_Transmit(&W25Q_SPI, data, current_size, W25Qx_TIMEOUT_VALUE) != HAL_OK)
        {
            return W25Qx_ERROR;
        }
        w25qx_disable();

        /* Wait the end of Flash writing */
        while(w25qx_get_status() == W25Qx_BUSY) {
            /* Check for the Timeout */
            if((HAL_GetTick() - tickstart) > W25Qx_TIMEOUT_VALUE) {        
                return W25Qx_TIMEOUT;
            }
        }
    
        /* Update the address and size variables for next page programming */
        current_addr += current_size;
        data += current_size;
        current_size = ((current_addr + W25Q128FV_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : W25Q128FV_PAGE_SIZE;
    } while (current_addr < end_addr);

    
    return W25Qx_OK;
}

/*!
 * @brief  Erase one block of W25Qx flash
 */
uint8_t w25qx_erase_block(uint32_t address) {
    uint8_t cmd[4];
    uint32_t tickstart = HAL_GetTick();
    cmd[0] = SECTOR_ERASE_CMD;
    cmd[1] = (uint8_t)(address >> 16);
    cmd[2] = (uint8_t)(address >> 8);
    cmd[3] = (uint8_t)(address);

    /* Enable write operations */
    w25qx_write_enable();
    
    /*Select the FLASH: Chip Select low */
    w25qx_enable();
    /* Send the read ID command */
    HAL_SPI_Transmit(&W25Q_SPI, cmd, 4, W25Qx_TIMEOUT_VALUE);
    /*Deselect the FLASH: Chip Select high */
    w25qx_disable();
    
    /* Wait the end of Flash writing */
    while(w25qx_get_status() == W25Qx_BUSY) {
        /* Check for the Timeout */
        if((HAL_GetTick() - tickstart) > W25Q128FV_SECTOR_ERASE_MAX_TIME) {
            return W25Qx_TIMEOUT;
        }
    }
    return W25Qx_OK;
}

/*!
 * @brief  Erase entire W25Qx flash memory chip
 */
uint8_t w25qx_erase_chip(void) {
    uint8_t cmd[4];
    uint32_t tickstart = HAL_GetTick();
    cmd[0] = CHIP_ERASE_CMD;
    
    /* Enable write operations */
    w25qx_write_enable();
    
    /*Select the FLASH: Chip Select low */
    w25qx_enable();
    /* Send the read ID command */
    HAL_SPI_Transmit(&W25Q_SPI, cmd, 1, W25Qx_TIMEOUT_VALUE);
    /*Deselect the FLASH: Chip Select high */
    w25qx_disable();
    
    /* Wait the end of Flash writing */
    while(w25qx_get_status() == W25Qx_BUSY) {
        /* Check for the Timeout */
        if((HAL_GetTick() - tickstart) > W25Q128FV_BULK_ERASE_MAX_TIME) {
            return W25Qx_TIMEOUT;
        }
    }

    return W25Qx_OK;
}

/*!
 * @brief  Read status register 1 of W25Qx flash
 */
void w25qx_read_status_register1(uint8_t *sr1) {
    uint8_t cmd = READ_STATUS_REG1_CMD;

    w25qx_enable();
    /* Send the read ID command */
    HAL_SPI_Transmit(&W25Q_SPI, &cmd, 1, W25Qx_TIMEOUT_VALUE);
    /* Reception of the data */
    HAL_SPI_Receive(&W25Q_SPI, sr1, 1, W25Qx_TIMEOUT_VALUE);
    w25qx_disable();
}
