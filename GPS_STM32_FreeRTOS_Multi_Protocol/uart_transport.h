/*
 * uart_transport.h
 *
 *  Created on: Feb 10, 2026
 *      Author: illi
 */

#ifndef UART_TRANSPORT_H_
#define UART_TRANSPORT_H_

#include "stdbool.h"
//#include <stdint.h>
#include "FreeRTOS.h"


void       uart_mutex_init(void);
BaseType_t UART_Lock      (void);
void       UART_Unlock    (void);

bool       UART_Send      (const uint8_t *data, uint16_t len);
void gps_uart_reconfigure (uint32_t baud);

#endif /* UART_TRANSPORT_H_ */
