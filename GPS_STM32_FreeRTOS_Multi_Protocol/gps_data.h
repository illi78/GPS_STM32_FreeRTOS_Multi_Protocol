/*
 * gps_data.h
 *
 *  Created on: Jun 13, 2025
 *      Author: illi
 */

#ifndef INC_GPS_DATA_H_
#define INC_GPS_DATA_H_

#include "FreeRTOS.h"
#include "semphr.h"
#include <stdint.h>
#include <stdbool.h>

#include "rtc.h"


#define MAX_SATELLITES 12
#define MAX_FIELD_LEN 32

typedef struct {
    bool            status;		// A=data status or V=data not valid

    double          latitude;	// in DMM format
    double          longitude;	// in DMM format

	RTC_DateTypeDef gDate;		// date
	RTC_TimeTypeDef gTime;		// time (Coordinated Universal Time (UTC) )
	int             timeZone;	// time zone

    double          speed;		// knots (convert to kph or mph)
    uint16_t        course;		// 0 ~ 359

    uint8_t         fix;		// 1D , 2D , 3D



//    char timestamp[20];         // e.g. "HHMMSS.SSS"
//    char date[10];              // e.g. "DDMMYY"

//    double altitude;
//    int num_satellites;
//    float snr[MAX_SATELLITES];  // Signal to Noise Ratio for each satellite

} GPSData_t;


typedef struct {
	uint8_t sat_ID;
	uint8_t sat_SNR;
//	uint8_t sat_elevation;
//	uint8_t sat_azinmuth;
} GPSchannel_t;


typedef struct {
	GPSchannel_t SatChann[12];
	uint8_t      NumOfSats;
} GPSsats_t;


typedef struct {
//    char manufacturer[MAX_FIELD_LEN];   // e.g. "MediaTek" or "Airoha"
//    char chip_model[MAX_FIELD_LEN];     // e.g. "M130"
//    char firmware_date[MAX_FIELD_LEN];  // e.g. "29/11/2011"
//    char firmware_version[MAX_FIELD_LEN]; // e.g. "1"
//    char hardware_version[MAX_FIELD_LEN]; // e.g. "0130"

    char ReleaseStr[MAX_FIELD_LEN];
    char Build_ID[MAX_FIELD_LEN];
    char Internal_USE_1[MAX_FIELD_LEN];
    char Internal_USE_2[MAX_FIELD_LEN];
    char Manufacturer[MAX_FIELD_LEN];
} GPSfw_t;



// Initialize the GPS data module and MUTEX
void    gps_data_init(void);


// Access functions
uint8_t gps_data_Lock(void);
void    gps_data_Unlock(void);

uint8_t gps_fw_Lock(void);
void    gps_fw_Unlock(void);



// Optional: Safe copy of the routing table
void    gps_data_GetCopy(GPSData_t *copy);
void    gps_fw_GetCopy(GPSfw_t *copy);


// External declaration of the global routing table
extern GPSData_t gGpsData;
extern GPSfw_t   gGpsFw;
extern GPSsats_t gGpsSats;


#endif /* INC_GPS_DATA_H_ */
