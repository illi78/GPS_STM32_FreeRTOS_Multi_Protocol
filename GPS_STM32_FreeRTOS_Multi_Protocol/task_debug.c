/*
 * task_debug.c
 *
 *  Created on: Mar 24, 2025
 *      Author: illi
 */


#include "main.h"
#include "FreeRTOS.h"
#include "freertos_queues.h"
#include "task_debug.h"
#include "usart.h"
#include "gps_data.h"

#include "uart_transport.h"

/************* functions **************/



void logger (uint8_t N_code)
{
	char str[8];
	int size = sprintf(str, "#0x%02X\r\n", N_code);

	UART_Send((uint8_t *)str, size);
}

