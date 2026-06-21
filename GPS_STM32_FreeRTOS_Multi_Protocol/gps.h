/*
 * gps.h
 *
 *  Created on: Feb 9, 2026
 *      Author: illi
 */

#ifndef GPS_H_
#define GPS_H_

#include <stdint.h>
#include <stdbool.h>
#include "gps_types.h"
#include "gps_nmea.h"



//#define MAX_SATELLITES 12
//#define MAX_FIELD_LEN 32

// for RMC sentences:
#define ISLATITUDE           0x01
#define GPS_DATA_VALID       A
#define GPS_DATA_NOT_VALID   V




bool GPS_Init       (void);

bool GPS_StartRMC   (void);
bool GPS_StopRMC    (void);

bool GPS_StartGSV   (void);
bool GPS_StopGSV    (void);

bool GPS_Get_Version(void);
bool GPS_Hot_Start  (void);
bool GPS_Cold_Start (void);
bool GPS_Set_Baud   (enum BaudFlag baud);
bool GPS_Standby    (void);
bool GPS_Restart    (void);


#endif /* GPS_H_ */
