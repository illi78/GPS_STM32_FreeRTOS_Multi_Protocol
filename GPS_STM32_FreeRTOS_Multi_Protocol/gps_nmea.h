/*
 * gps_nmea.h
 *
 *  Created on: Feb 17, 2026
 *      Author: illi
 */

#ifndef GPS_NMEA_H_
#define GPS_NMEA_H_

#include "gps.h"
#include "rtc.h"

#define MAX_SATELLITES 12
#define MAX_FIELD_LEN 32


typedef struct {
    bool            status;		// A=data status or V=data not valid

    uint32_t        latitude_1e6;   // in DMM format		/ 8-byte
    uint32_t        longitude_1e6;  // in DMM format
//    uint8_t         direction_flags;// bit0: N/S, bit1: E/W

	uint16_t        speed_x10;      // knots (convert to kph or mph)
    uint16_t        course_x10;	    // 0 ~ 3599 ( Deg / 10 )

    uint16_t        altitude_p500;  // altitude + 500m {786 -> 286m}

    uint8_t         fix;            // 0=no fix, 1=1D, 2=2D, 3=3D

	RTC_DateTypeDef gDate;          // date
	RTC_TimeTypeDef gTime;          // time (Coordinated Universal Time (UTC) )
	int             timeZone;       // time zone
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


#define NMEA_UNKNOWN_SENTENCE               0x1E


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



// External declaration of the global routing table
extern GPSData_t gGpsData_X;
extern GPSfw_t   gGpsFw_X;
extern GPSsats_t gGpsSats_X;

// for debug ONLY
// #define pdTICKS_TO_MS(xTicks) ( ( (TickType_t)(xTicks) * 1000u ) / configTICK_RATE_HZ )

uint8_t Gps_ParsePMTK_ACK_X    (const char* nmea);
uint8_t Gps_ParsePMTK_Release_X(const char* nmea, GPSfw_t* firmware);
//uint8_t parse_two_digits_X          (const char* str);
//double  convertDMMtoDecimalDegrees_X(const char *dmm, char direction, int isLatitude);
//void    convertDecimalDegreesToDMS_X(double value, char* str);
//double  knots_to_kph                (double knots);


int  Gps_ParseRMC_Y(const char* nmea, GPSData_t* GPS_DB);
int  Gps_ParseGSV_X(const char* nmea, GPSsats_t* GPS_DB);
int  Gps_ParseGSA_X(const char* nmea, GPSData_t* GPS_DB);

void get_respond_from_GPS_X    (void);

void microdeg_to_DMS(int32_t  coord,
                     uint8_t  *deg,
                     uint8_t  *min,
                     uint8_t  *sec,
                     char     *hemisphere,
                     uint8_t  isLatitude);




#endif /* GPS_NMEA_H_ */
