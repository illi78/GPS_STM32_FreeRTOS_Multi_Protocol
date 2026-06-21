/*
 * uart_transport.c
 *
 *  Created on: Feb 10, 2026
 *      Author: illi
 */

#include "uart_transport.h"
#include "FreeRTOS.h"
#include "main.h"
#include "semphr.h"
#include "uartRingBufDMA.h"


static SemaphoreHandle_t uartMutex;


// initialization function
void uart_mutex_init(void)
{
	uartMutex = xSemaphoreCreateMutex();
	configASSERT(uartMutex != NULL);
}


// GPS data protection:
BaseType_t UART_Lock(void)			// BaseType_t
{
    return (xSemaphoreTake(uartMutex, portMAX_DELAY));
}


void UART_Unlock(void)
{
    xSemaphoreGive(uartMutex);
}


bool UART_Send(const uint8_t *data, uint16_t len)
{
    if (UART_Lock() != pdTRUE)
        return false;

    HAL_UART_Transmit(&UART, (uint8_t *)data, len, HAL_MAX_DELAY);

    // release resource
    UART_Unlock();
    return true;
}


void gps_uart_reconfigure(uint32_t baud)
{
	HAL_UART_DMAStop(&UART);
	HAL_UART_DeInit(&UART);

	UART.Init.BaudRate = baud;

	HAL_UART_Init(&UART);
	Ringbuf_Init();		// contains: HAL_UARTEx_ReceiveToIdle_DMA()
	Delay_ms(100);
}


