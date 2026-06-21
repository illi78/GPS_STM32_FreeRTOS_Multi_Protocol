/*
 * task_debug.h
 *
 *  Created on: Mar 24, 2025
 *      Author: illi
 */

#ifndef INC_TASK_DEBUG_H_
#define INC_TASK_DEBUG_H_

#include <stdio.h>
#include <stdint.h>


/************* Definitions ************/

#define PING_FAILED					0x00
#define PING_SUCCESS				0x01

#define SRATING_QUEUE				0x02
#define QUEUE_CREATE_FAILED			0x03
#define QUEUE_CREATE_SUCCESSFUL		0x04
#define SEND_PING_TO_QUEUE_SUCCESS	0x05
#define SEND_TO_QUEUE_FAILED		0x06
#define QUEUE_FILLED_SUCCESSFUL		0x07

//#define MENU_PRINT_PAGE_1                   0x08
//#define MENU_PRINT_PAGE_2                   0x09
//#define MENU_INVALID_PAGE                   0x0A
#define DYNAMIC_ALLOCATION_FAILED           0x0B


#define BAUDRATE_INVALID_INPUT              0x0C
#define BAUDRATE_CHANGE_REQUEST_SENT        0x0D
#define UPDATERATE_INVALID_INPUT            0x0E
#define UPDATERATE_CHANGED_SUCCESSFULLY     0x0F
#define NMEA_OUT_STOPPED                    0x10
#define NMEA_OUT_UPDATED_SUCCESSFULLY       0x11


// GPS:
#define NMEA_REQUEST_RELEASE                0x12
#define NMEA_REQUEST_TEST_PACKET            0x13
#define NMEA_REQUEST_HOT_START              0x14
#define NMEA_REQUEST_COLD_START             0x15
#define NMEA_REQUEST_UPDATE_RATE            0x16
#define NMEA_REQUEST_STANDBY                0x17
#define NMEA_REQUEST_FIX_TYPE               0x18
#define NMEA_REQUEST_GNSS_DATA              0x19
#define NMEA_REQUEST_SATELLITES_IN_VIEW     0x1A
#define NMEA_REQUEST_STOP_UPDATE            0x1B
#define NMEA_WRONG_REQUEST                  0x1C
#define PMTK_TX_COMMAND_SENT                0x1D

//#define NMEA_UNKNOWN_SENTENCE               0x1E
#define NMEA_REQUEST_CHANGE_BAUDRATE_9600   0x1F
#define NMEA_REQUEST_CHANGE_BAUDRATE_115200 0x20

#define PMTK_REQUEST_PORT_CHANGE_RATE_115200        0x21
#define PMTK_REQUEST_PORT_CHANGE_RATE_115200_FAILED 0x22

#define PMTK_REQUEST_HANDSHAKE_FAILED         0x23
#define PMTK_REQUEST_HANDSHAKE_SUCCESS        0x24
#define PMTK_REQUEST_STOP_DATA_UPDATE_FAILED  0x25
#define PMTK_REQUEST_STOP_DATA_UPDATE_SUCCESS 0x26

#define PCAS_TX_COMMAND_SENT                  0x27





/************* structures *************/


/******** functions prototypes ********/
void Debug(void);
void logger_boot (uint8_t code);
void logger (uint8_t code);
//void loggerStr (char *STR , int len);
//
//void send_to_GPS (char * str , uint8_t len);

#endif /* INC_TASK_DEBUG_H_ */
