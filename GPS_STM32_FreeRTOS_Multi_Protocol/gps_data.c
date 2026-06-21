/*
 * gps_data.c
 *
 *  Created on: Jun 13, 2025
 *      Author: illi
 */


#include "gps_data.h"


GPSData_t gGpsData = {0};
GPSfw_t   gGpsFw   = {0};
GPSsats_t gGpsSats = {0};


static SemaphoreHandle_t gpsDataMutex;
static SemaphoreHandle_t gpsFwMutex;

// initialization function
void gps_data_init(void)
{
	gpsDataMutex = xSemaphoreCreateMutex();
	gpsFwMutex   = xSemaphoreCreateMutex();
}

// GPS data protection:
uint8_t gps_data_Lock(void)			// BaseType_t
{
    return (xSemaphoreTake(gpsDataMutex, portMAX_DELAY));
}


void gps_data_Unlock(void)
{
    xSemaphoreGive(gpsDataMutex);
}

// GPS firmware protection:
uint8_t gps_fw_Lock(void)			// BaseType_t
{
    return (xSemaphoreTake(gpsFwMutex, portMAX_DELAY));
}


void gps_fw_Unlock(void)
{
    xSemaphoreGive(gpsFwMutex);
}

// Create copies to these structures:
void gps_data_GetCopy(GPSData_t *copy)
{
	gps_data_Lock();
    *copy = gGpsData; // Copy the structure
    gps_data_Unlock();
}


void gps_fw_GetCopy(GPSfw_t *copy)
{
	gps_fw_Lock();
    *copy = gGpsFw; // Copy the structure
    gps_fw_Unlock();
}

/********** EOF **********/
