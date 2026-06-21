/*
 * gps_types.h
 *
 *  Created on: Feb 9, 2026
 *      Author: illi
 */

#ifndef GPS_TYPES_H_
#define GPS_TYPES_H_


#pragma once
#include <stdint.h>
#include <stdbool.h>


/* Forward declaration */
typedef struct GPS_Context GPS_Context_t;

/* Function pointer types */
typedef bool (*gps_cmd_f)(GPS_Context_t *ctx);
typedef bool (*gps_send_f)(const char *str, uint8_t len);


/* Protocol enum */
typedef enum
{
    GPS_PROTO_UNKNOWN = 0,
    GPS_PROTO_PMTK,
    GPS_PROTO_PCAS,
} GPS_Protocol_t;


enum BaudFlag
{
	BAUDR_4800   = '0',
	BAUDR_9600   = '1',
	BAUDR_19200  = '2',
	BAUDR_38400  = '3',
	BAUDR_57600  = '4',
	BAUDR_115200 = '5'
};


/* Context definition */
struct GPS_Context
{
    GPS_Protocol_t protocol;
    bool           is_initialized;
    bool           has_fix;
    uint32_t       baudrate;            // current runtime baud
    enum BaudFlag  requested_baud;      // desired baud-rate
    uint8_t        nmea_mask;           // shadow state

    /* function table */
    bool (*send_cmd) (const char *cmd, uint8_t len);
    bool (*start_rmc)  (GPS_Context_t *ctx);
    bool (*stop_rmc)   (GPS_Context_t *ctx);
    bool (*start_gsv)  (GPS_Context_t *ctx);
    bool (*stop_gsv)   (GPS_Context_t *ctx);

    bool (*get_version)(GPS_Context_t *ctx);
    bool (*hot_start)  (GPS_Context_t *ctx);
    bool (*cold_start) (GPS_Context_t *ctx);
    bool (*set_baud)   (GPS_Context_t *ctx);
    bool (*standby)    (GPS_Context_t *ctx);
    bool (*restart)    (GPS_Context_t *ctx);

};


typedef enum
{
    GPS_CMD_RMC,
    GPS_CMD_GSV
} GPS_Command_t;


typedef struct
{
    bool     has_fix;
    double   latitude;
    double   longitude;
} GPS_Data_t;


#endif /* GPS_TYPES_H_ */
