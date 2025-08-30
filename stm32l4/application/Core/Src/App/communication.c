/*
 *  communication.c
 *
 *  Created on: Aug 30, 2025
 */

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "stm32l4xx_hal.h"
#include "main.h"
#include "app_config.h"
#include "log.h"
#include "ui_control.h"
#include "communication.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

#define SOF  0xAA
#define SOF2 0x55

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

static osThreadId_t communication_task_handle;
static const osThreadAttr_t communication_task_attributes = {
    .name = "communication_task",
    .priority = (osPriority_t) osPriorityLow,
    .stack_size = 2048
};

static uint8_t rx_temp;    /* To read uart data from uart */
static uart_fifo_t uart_fifo;
static communication_rx_t comm_rx;

#if SIMULATOR
#define SIMULATOR_MAX_LEN 16
static uint8_t simulator_rx = 0;
static bool received_simulator = false;
static char simulator_buf[SIMULATOR_MAX_LEN + 1];
static uint8_t simulator_length = 0;
#endif

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/



/******************************************************************************/


/**
  * @brief  Rx Transfer completed callback.
  * @param  huart UART handle.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
#if SIMULATOR
    if (huart->Instance == USART1) {
        if ((simulator_rx == '\r') || (simulator_rx == '\n')) {
            if (simulator_length > 0) {
                simulator_buf[simulator_length] = '\0';
                received_simulator = true;
                simulator_length = 0;
            }
        }
        else if (!received_simulator) {
            if (simulator_length < SIMULATOR_MAX_LEN) {
                simulator_buf[simulator_length++] = simulator_rx;
            }
        }
        HAL_UART_Receive_IT(&huart1, (uint8_t *)&simulator_rx, 1);
    }
#endif

    if (huart->Instance == USART2) {
        if (uart_fifo.count < FIFO_SIZE) {
            uart_fifo.data[uart_fifo.head] = rx_temp;
            uart_fifo.head = (uart_fifo.head + 1) % FIFO_SIZE;
            uart_fifo.count++;
        }
        HAL_UART_Receive_IT(&huart2, (uint8_t *)&rx_temp, 1);
    }
}

/*!
 * @brief  Calculate checksum
 */
static uint8_t calculate_checksum(uint8_t *data, int size) {
    uint8_t checksum = 0;
    for (int i = 0; i < size; i++) {
        checksum += data[i];
    }

    return checksum;
}

/*!
 * @brief  Check and handle rx data
 */
static bool communication_process(uint8_t c) {
    switch (comm_rx.state) {
        case COMM_IDLE_STATE:
            if (c == SOF) {
                comm_rx.state = COMM_SOF_STATE;
            }
            break;

        case COMM_SOF_STATE:
            if (c == SOF2) {
                comm_rx.state = COMM_LENGTH_STATE;
            }
            else {
                comm_rx.state = COMM_IDLE_STATE;
                LOG_ERR("Invalid SOF2");
            }
            break;

        case COMM_LENGTH_STATE:
            if (c == 0) {
                comm_rx.length = OTA_PACKET_LENGTH;
            }
            else {
                comm_rx.length = c;
            }
            comm_rx.index = 0;
            comm_rx.state = COMM_DATA_STATE;
            break;

        case COMM_DATA_STATE:
            comm_rx.data[comm_rx.index++] = c;
            if (comm_rx.index >= comm_rx.length) {
                comm_rx.state = COMM_CHECKSUM_STATE;
            }
            break;

        case COMM_CHECKSUM_STATE:
            comm_rx.state = COMM_IDLE_STATE;
            if (calculate_checksum(comm_rx.data, comm_rx.length) == c) {
                LOG_DBG("Received a packet %d bytes", comm_rx.length);
            }
            else {
                LOG_ERR("Invalid checksum");
            }
            return true;

        default:
            comm_rx.state = COMM_IDLE_STATE;
            break;
    }

    return false;
}

/*!
 * @brief  Task for communication handling uart from user/esp32
 */
static void communication_task(void *argument) {
    (void) argument;
    int available;

    while (1) {
        available = uart_fifo.count;
        if (available > 0) {
            for (int i = 0; i < available; i++) {
                /* Pop data from FIFO */
                uint8_t byte = uart_fifo.data[uart_fifo.tail];
                uart_fifo.tail = (uart_fifo.tail + 1) % FIFO_SIZE;
                uart_fifo.count--;

                if (communication_process(byte)) {
                    /* A complete packet received, process it */
                    delay(1);
                }
            }
        }
        else {
            delay(10);
        }
        
    #if SIMULATOR
        if (received_simulator) {
            received_simulator = false;
            LOG_DBG("Simulator command: %s", simulator_buf);

            if (!strncmp((char *)simulator_buf, (char *)"MAIN", 4))
            {
                ui_control_button_main_pressed();
            }
            else if (!strncmp((char *)simulator_buf, (char *)"TAP", 3))
            {
                ui_control_button_tap_pressed();
            }
            else if (!strncmp((char *)simulator_buf, (char *)"PRES", 4)) {
                int pressure = atoi((char *)&simulator_buf[5]);
                LOG_DBG("Received simulator pressure %d", pressure);
            }
            else if (!strncmp((char *)simulator_buf, (char *)"TEMP", 4)) {
                int temp = atoi((char *)&simulator_buf[5]);
                LOG_DBG("Received simulator temperature %d", temp);
            }

            else if (!strncmp((char *)simulator_buf, (char *)"SENS", 4)) {
                int sens = atoi((char *)&simulator_buf[5]);
                LOG_DBG("Received simulator sensitivity %d", sens);
            }
        }
    #endif
    }
}

/*!
 * @brief  Initialize communication api
 */
void communication_init(void) {
    memset(&uart_fifo, 0, sizeof(uart_fifo));
    memset(&comm_rx, 0, sizeof(comm_rx));
    communication_task_handle = osThreadNew(communication_task, NULL, &communication_task_attributes);

    /* Start receive data from uart */
#if SIMULATOR
    HAL_UART_Receive_IT(&huart1, (uint8_t *)&simulator_rx, 1);
#endif
    HAL_UART_Receive_IT(&huart2, (uint8_t *)&rx_temp, 1);
}
