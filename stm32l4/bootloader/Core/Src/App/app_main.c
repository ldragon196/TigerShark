/*
 *  app_main.c
 *
 *  Created on: Aug 28, 2025
 */

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "stm32l4xx_hal.h"
#include "app_config.h"
#include "log.h"
#include "W25Qx.h"
#include "app_main.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

#define FLASH_SECTOR_SIZE     (0x800)

#define ETX_APP_FLASH_ADDR    (0x0800A000)
#define ETX_CONFIG_FLASH_ADDR (ETX_APP_FLASH_ADDR - FLASH_SECTOR_SIZE)
#define ETX_APP_NPAGE         ((ETX_APP_FLASH_ADDR - 0x08000000) / FLASH_SECTOR_SIZE)

/* Reboot reason */
#define ETX_FIRST_TIME_BOOT   (0xFFFFFFFF)     /* First time boot */
#define ETX_NORMAL_BOOT       (0xBEEFFEED)     /* Normal Boot */
#define ETX_OTA_DONE_BOOT     (0xBEEFFDDE)     /* OTA done */
#define ETX_OTA_REQUEST       (0xDEADBEEF)     /* OTA request by application */
#define ETX_LOAD_PREV_APP     (0xFACEFADE)     /* App requests to load the previous version */

#define OTA_FIRMWARE_ADDRESS  (0x10000)
#define OTA_PART_LENGTH       (1024)
#define MAGIC_NUMBER          (0xAA555AA5)

typedef void (*application_func_t)(void);

typedef struct {
    int32_t fw_size;
    uint32_t fw_crc;
    uint32_t reserved1;
} __attribute__((packed)) ext_slot_t;

typedef struct {
    uint32_t reboot_cause;
    ext_slot_t slot_table;
} __attribute__((packed)) ext_general_cfg_t;

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

static uint8_t ota_buf[OTA_PART_LENGTH];
static uint32_t checksum_file = 0;

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/



/******************************************************************************/

/*!
 * @brief  Calculate checksum
 */
static uint32_t cal_checksum_file(uint32_t now, uint8_t *data, int length)
{
    uint32_t ret = now;
    for (int i = 0; i < length; i++) {
        ret += data[i];
    }
    return ret;
}

/*!
 * @brief  Perform a jump to the application if it's available
 */
static void jump_to_application(void) {
    /* Local variables */
    uint32_t app_ddress;
    application_func_t jump_func;

    /* Check whether App is available */
    if((*(uint32_t*) ETX_APP_FLASH_ADDR) != 0xFFFFFFFF) {
        LOG_INFO("Jump to Application");
        __disable_irq();

        /* Set the Clock to default state */
        HAL_RCC_DeInit();
        HAL_DeInit();

        /* Disable SysTick */
        SysTick->CTRL = 0;
        SysTick->LOAD = 0;
        SysTick->VAL  = 0;
        __set_CONTROL(0);

        /* Jump to user application */
        app_ddress = *(__IO uint32_t*) (ETX_APP_FLASH_ADDR + 4);
        jump_func = (application_func_t) app_ddress;

        /* Initialize use application's Stack Pointer */
        __set_MSP(*(__IO uint32_t*) ETX_APP_FLASH_ADDR);
        jump_func();
    }
    else {
        LOG_ERR("Invalid application firmware");
        NVIC_SystemReset();
    }
}

/*!
 * @brief Write the configuration to flash
 */
static HAL_StatusTypeDef write_cfg_to_flash(ext_general_cfg_t *cfg)
{
    HAL_StatusTypeDef ret;
    HAL_FLASH_Unlock();

    do {
        FLASH_WaitForLastOperation(HAL_MAX_DELAY);

        /* Erase the Flash */
        FLASH_EraseInitTypeDef EraseInitStruct;
        uint32_t SectorError;

        EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
        EraseInitStruct.Banks = FLASH_BANK_1;
        EraseInitStruct.Page = ETX_APP_NPAGE - 1;
        EraseInitStruct.NbPages = 1;

        /* Clear all flags before you write it to flash */
        __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR |
                            FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR);

        ret = HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);
        if (ret != HAL_OK) {
            LOG_INFO("Config Flash Write Error");
            break;
        }

        /* Write the configuration */
        uint8_t *data = (uint8_t *)cfg;
        uint8_t mem_offest = 0;

        for (int i = 0; i < sizeof(ext_general_cfg_t); i += 8) {
            uint64_t u64_data;
            memcpy(&u64_data, &data[i], sizeof(uint64_t));

            ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (ETX_CONFIG_FLASH_ADDR + mem_offest), u64_data);
            if (ret == HAL_OK) {
                mem_offest += 8;
            }
            else {
                LOG_INFO("Config Flash Write Error");
                break;
            }
        }

        /* Check if the FLASH_FLAG_BSY */
        FLASH_WaitForLastOperation(HAL_MAX_DELAY);

        if (ret != HAL_OK) {
            break;
        }

    } while (0);

    HAL_FLASH_Lock();
    return ret;
}

/*!
 * @brief  Check if a reboot ota is required
 */
static void app_main_check_reboot(void) {
    /* Check firmware configuration */
    ext_general_cfg_t *cfg = (ext_general_cfg_t *)(ETX_CONFIG_FLASH_ADDR);
    if (cfg->reboot_cause != ETX_OTA_REQUEST) {
        LOG_INFO("Normal reset");
    }
    else {
        int file_length = cfg->slot_table.fw_size;
        int offset = 0;
        int file_valid = 1;
        HAL_StatusTypeDef ret;

        /* Check crc */
        while (offset < file_length) {
            if (w25qx_read(ota_buf, OTA_FIRMWARE_ADDRESS + offset, OTA_PART_LENGTH) == W25Qx_OK) {
                checksum_file = cal_checksum_file(checksum_file, ota_buf,
                                    offset + OTA_PART_LENGTH < file_length ? OTA_PART_LENGTH : (file_length - offset));
                offset += OTA_PART_LENGTH;
            }
            else {
                file_valid = 0;
                break;
            }
        }

        if ((file_valid) && (checksum_file == cfg->slot_table.fw_crc)) {
            offset = 0;
            checksum_file = 0;
            if ((cfg->reboot_cause == ETX_OTA_REQUEST) && (file_length > 0) && (cfg->slot_table.reserved1 == MAGIC_NUMBER)) {
                LOG_INFO("Start download file %d bytes", file_length);
                HAL_FLASH_Unlock();

                while (offset < file_length) {
                    if (w25qx_read(ota_buf, OTA_FIRMWARE_ADDRESS + offset, OTA_PART_LENGTH) == W25Qx_OK) {
                        if (offset == 0) {
                            /* No need to erase every time. Erase only the first time */
                            FLASH_EraseInitTypeDef EraseInitStruct;
                            uint32_t SectorError;
                            EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
                            EraseInitStruct.Banks = FLASH_BANK_1;
                            EraseInitStruct.Page = ETX_APP_NPAGE;
                            EraseInitStruct.NbPages = 256 - ETX_APP_NPAGE;

                            if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK) {
                                break;
                            }

                            EraseInitStruct.TypeErase = FLASH_TYPEERASE_MASSERASE;
                            EraseInitStruct.Banks = FLASH_BANK_2;
                            EraseInitStruct.Page = 0;
                            EraseInitStruct.NbPages = 256;

                            ret = HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);
                            if (ret != HAL_OK) {
                                break;
                            }
                        }

                        uint32_t temp = cal_checksum_file(checksum_file, ota_buf,
                                                offset + OTA_PART_LENGTH < file_length ? OTA_PART_LENGTH : (file_length - offset));
                        checksum_file = temp;
                        for (int i = 0; i < OTA_PART_LENGTH; i += 8) {
                            uint64_t u64_data;
                            memcpy(&u64_data, &ota_buf[i], sizeof(uint64_t));
                            ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, ETX_APP_FLASH_ADDR + offset, u64_data);
                            offset += 8;
                            if (ret != HAL_OK) {
                                LOG_INFO("Write flash failed");
                                break;
                            }
                        }
                    }
                    else {
                        LOG_INFO("Read flash failed");
                        break;
                    }
                }
                HAL_FLASH_Lock();
            }
        }
        else {
            LOG_INFO("Firmware is invalid");
        }

        if ((offset >= file_length) && (checksum_file == cfg->slot_table.fw_crc)) {
            LOG_INFO("Received %d bytes, Checksum %u", offset, checksum_file);
            ext_general_cfg_t _cfg;
            _cfg.reboot_cause = ETX_OTA_DONE_BOOT;
            _cfg.slot_table.fw_size = 0;
            write_cfg_to_flash(&_cfg);
        }
        else {
            LOG_ERR("Bootloader OTA failed");
        }
    }

    jump_to_application();
}

/*!
 * @brief  Initialize the main application
 */
void app_main_init(void) {
    LOG_INFO("");
    LOG_INFO("================================================");
    LOG_INFO("---------- BOOTLOADER %s Version %s ----------", DEVICE_NAME, BOOTLOADER_VERSION);
    LOG_INFO("================================================");
    LOG_INFO("");

    w25qx_init();
    app_main_check_reboot();
}
