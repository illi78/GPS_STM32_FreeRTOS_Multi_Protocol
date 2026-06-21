/*
 * gps_nmea.c
 *
 *  Created on: Feb 17, 2026
 *      Author: illi
 */


#include "FreeRTOS.h"
#include <main.h>
#include "gps_nmea.h"
#include "uartRingBufDMA.h"
#include "gps.h"
#include "gps_data.h"

#include "task_debug.h"
#include "gps_pmtkN.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#include "taskSensors.h"		// for GSV


GPSData_t gGpsData_X = {0};
GPSfw_t   gGpsFw_X   = {0};
GPSsats_t gGpsSats_X = {0};


/*--------------- PMTK and NMEA answers from GPS --------------*/

// $PMTK001,314,3*36
uint8_t Gps_ParsePMTK_ACK_X(const char* nmea)
{

    if (nmea == NULL)
        return -1;  // Error: null input

    // Make a local copy to safely tokenize
    char buf[20];
    strncpy(buf, nmea, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';  // Ensure null-termination

    // Tokenize input string to reach the fix type (3rd token)
    char* token = strtok(buf, ",");	// $PMTK001
    token = strtok(NULL, ",");		// Cmd

    if (token == NULL)
        return -2;  // Error: missing fix field

//    int ACK = (uint16_t)atoi(token);

    token = strtok(NULL, ",");		// Cmd

//    uint8_t Flag = (uint8_t)atoi(token);

    return 0;  // Success

	;
}


uint8_t Gps_ParsePMTK_Release_X(const char* nmea, GPSfw_t* firmware)
{
	// THE SENTENCE IS VALID!!!

    if (!nmea || !firmware)
    {
        return 1; // Null pointer error
    }

    // Make a local copy to safely tokenize
    char buf[83];
    strncpy(buf , nmea , sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';  // Ensure null-termination

    /*
     * PMTK705,
     * ReleaseStr,
     * Build_ID,
     * Internal_USE_1,
     * ( Internal_USE_2)
     *
     */
    // Tokenize input string to reach the fix type (3rd token)
    char* token = strtok(buf, ",");  // $PMTK705

    // Firmware release information

    // clean before refreshing data
    memset(firmware, 0, sizeof(GPSfw_t));

    token = strtok(NULL, ",");       // ReleaseStr
    strncpy(firmware->ReleaseStr, token, MAX_FIELD_LEN - 1);

    token = strtok(NULL, ",");       // Build_ID
    strncpy(firmware->Build_ID, token, MAX_FIELD_LEN - 1);

    token = strtok(NULL, ",");       // Internal_USE_1
    strncpy(firmware->Internal_USE_1, token, MAX_FIELD_LEN - 1);

    // sometimes there is a 4th field
    token = strtok(NULL, ",");
    if (token) {
        // check for asterisk and cut it
        char *asterisk = strchr(token, '*');
        if (asterisk) {
            *asterisk = '\0';
        }

        strncpy(firmware->Internal_USE_2, token, MAX_FIELD_LEN - 1);
//        firmware->Internal_USE_2[MAX_FIELD_LEN - 1] = '\0';
    }
    return 0;  // Success
}


/**
 * for date\time digits from RMC
 */
/*static inline*/ uint8_t parse_two_digits_X(const char* str)
{
    return (uint8_t)((str[0] - '0') * 10 + (str[1] - '0'));
}


/**
 * @brief Convert ASCII digits to integer
 *
 * Stops when a non-digit character is found.
 * Returns parsed value.
 */
uint16_t parse_uint_X(const char *value)
{
    uint16_t result = 0;

    if (value == NULL)
        return 0;

    while (*value)
    {
        if (*value >= '0' && *value <= '9')
        {
            result = result * 10 + (*value - '0');
        }
        else
        {
            break;
        }

        value++;
    }

    return result;
}




/**
 * @brief Convert DMM (ddmm.mmmm or dddmm.mmmm) to decimal degrees × 1e6
 * @param dmm        String coordinate from RMC
 * @param direction  'N','S','E','W'
 * @param isLatitude 1 = latitude, 0 = longitude
 * @return int32_t coordinate in microdegrees (×1e6)
 */
uint32_t convertDMMtoMicroDeg_Y(const char *dmm, char direction, int isLatitude)
{
    if (dmm == NULL || *dmm == '\0')
        return 0;

    int32_t deg      = 0;
    int32_t min_int  = 0;
    int32_t min_frac = 0;

    // Degrees part length
    int deg_len = isLatitude ? 2 : 3;

    // Extract degrees
    for (int i = 0 ; i < deg_len ; i++)
    {
        if (dmm[i] < '0' || dmm[i] > '9')
            return 0;
        deg = deg * 10 + (dmm[i] - '0');
    }

    // Extract minutes integer part (always 2 digits)
    min_int = (dmm[deg_len    ] - '0') * 10 +
              (dmm[deg_len + 1] - '0');

    // Extract minutes fractional part (up to 4 digits typical)
    const char *p = &dmm[deg_len + 3]; // skip "ddmm."
    int digits = 0;

    while (*p >= '0' && *p <= '9' && digits < 6)
    {
        min_frac = min_frac * 10 + (*p - '0');
        p++;
        digits++;
    }

    // Scale fractional minutes to micro precision
    while (digits < 6)
    {
        min_frac *= 10;
        digits++;
    }

    // Total minutes in micro (minutes × 1e6)
    int32_t total_min_micro = (min_int * 1000000) + min_frac;

    // Convert minutes to degrees:
    // degrees + minutes/60
    int32_t microdegrees = (deg * 1000000) + (total_min_micro / 60);

    if (direction == 'S' || direction == 'W')
        microdegrees = -microdegrees;

    return microdegrees;
}


 /**
  * @brief Convert microdegrees to DMS
  * USEFUL FOR LCD printing
  */
void microdeg_to_DMS(int32_t  coord,
                     uint8_t *deg,
                     uint8_t  *min,
                     uint8_t  *sec,
                     char     *hemisphere,
                     uint8_t  isLatitude)
{
    int32_t value = coord;

    if (value < 0)
    {
        *hemisphere = isLatitude ? 'S' : 'W';
        value = -value;
    }
    else
    {
        *hemisphere = isLatitude ? 'N' : 'E';
    }

    *deg = value / 1000000;  // e.g. 31958613 --> 31

    int32_t remainder = value % 1000000;  // e.g. 31958613 --> 958613

    uint32_t total_seconds =
        (remainder * 3600) / 1000000;  // e.g. 3451

    *min = total_seconds / 60;  // e.g. 57
    *sec = total_seconds % 60;  // e.g. 31
}


uint16_t parse_value_x10_Y(const char *value, uint8_t knots)
{
    if (value == NULL || *value == '\0')
        return 0;

    // Parse value with one decimal digit manually
    uint16_t value_x10         = 0;
    uint8_t decimal_seen   = 0;
    uint8_t  decimal_count = 0;

    while (*value)
    {
    	if (*value == '.')
    	{
    		decimal_seen = 1;
    	}
    	else if (*value >= '0' && *value <= '9')
    	{
    		value_x10 = value_x10 * 10 + (*value - '0');

            if (decimal_seen)
                decimal_count++;

            if (decimal_count >= 1)
                break;  // keep one decimal only
    	}
    	else
    	{
    		break;
    	}

    	value++;
    }

    if (knots)
    {
        // km/h x 10 = knots × 18.52
        value_x10 = (value_x10 * 1852) / 1000;
    }
    else
    {
        // value of 183 -> 1830 || value of 257.6 -> 2756
        if (!decimal_seen)
            value_x10 *= 10;
    }
    return (uint16_t)value_x10;
}


int Gps_ParseRMC_Y(const char* nmea, GPSData_t* GPS_DB)
{
    if (nmea == NULL || GPS_DB == NULL)
    {
        return -1;  // Error: invalid pointers
    }

    // Example input: "$GPRMC,200517.206,V,,,,,0.00,153.83,150625,,,N*41"
    char buf[83];
    strncpy(buf, nmea, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';  // Ensure null-termination

    // Tokenize the string using ',' as separator

    char* ptr = buf;
    char* token;

    token = strsep(&ptr, ","); // $GPRMC
    token = strsep(&ptr, ","); // time

    if (token && strlen(token) >= 6)
    {
		GPS_DB->gTime.Hours   = parse_two_digits_X(&token[0]);
		GPS_DB->gTime.Minutes = parse_two_digits_X(&token[2]);
		GPS_DB->gTime.Seconds = parse_two_digits_X(&token[4]);
    }

    token = strsep(&ptr, ",");		// Status: A = valid, V = invalid
    GPS_DB->status = (token && token[0] == 'A');

    if (GPS_DB->status)
    {
        char *latlon_dir;
        token      = strsep(&ptr, ",");		// Latitude - RAW
        latlon_dir = strsep(&ptr, ",");		//  N/S Indicator

        GPS_DB->latitude_1e6  =
                    convertDMMtoMicroDeg_Y(token, latlon_dir ? latlon_dir[0] : 'N', 1);

        token      = strsep(&ptr, ",");		// Longitude - RAW
        latlon_dir = strsep(&ptr, ",");		//  E/W Indicator

        GPS_DB->longitude_1e6 =
                    convertDMMtoMicroDeg_Y(token, latlon_dir ? latlon_dir[0] : 'E', 0);

        token = strsep(&ptr, ","); // speed
        GPS_DB->speed_x10 = parse_value_x10_Y(token , 1);
//        GPS_DB->speed_x10 = knots_to_kmh_x10(token);

        token = strsep(&ptr, ","); // course
        GPS_DB->course_x10 = parse_value_x10_Y(token , 0);
//        GPS_DB->course_x10 = parse_course_x10(token);
    }
    else
    {
    	// Skip fields (6 total): lat, N/S, lon, E/W, speed, course
    	for (int i = 0; i < 6; i++)
    	{
    		strsep(&ptr, ",");
    	}

    	GPS_DB->latitude_1e6  = 0;
    	GPS_DB->longitude_1e6 = 0;
    	GPS_DB->speed_x10     = 0;
    	GPS_DB->course_x10    = 0;
    }

    token = strsep(&ptr, ",");		// Date(UTC)
    if (token && strlen(token) >= 6)
    {
		GPS_DB->gDate.Date  = parse_two_digits_X(&token[0]);
		GPS_DB->gDate.Month = parse_two_digits_X(&token[2]);
		GPS_DB->gDate.Year  = parse_two_digits_X(&token[4]);	// add 2000
    }

    Sensor_GPS_RMC_get_data();

    return 0;
}

/**
 * @brief Parse a GSV (GNSS Satellites in View) NMEA sentence and populate satellite data.
 */
int Gps_ParseGSV_X(const char* nmea, GPSsats_t* GPS_DB)
{
    if (nmea == NULL || GPS_DB == NULL)
        return -1;  // Error: null input

//    uint8_t numOfMessages = 0;	//  3
    uint8_t        messageIndex     = 0;	//  1
    uint8_t        satellitesInView = 0;	// 10
    static uint8_t idx              = 0;

    // initialize satellite power DB
    if (idx == 0)
    {
    	memset(GPS_DB , 0 , sizeof(GPSsats_t));
    }

    // Make a local copy to safely tokenize
    char buf[83];
    strncpy(buf, nmea, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';  // Ensure null-termination

    char* ptr = buf;
    // Tokenize input string to reach the fix type (3rd token)
    char* token;
	token = strsep(&ptr, ",");	// $GPGSV

//	numOfMessages = atoi(strsep(&ptr, ","));	// Total number of GSV sentences
	token = strsep(&ptr, ",");
	messageIndex =    atoi(strsep(&ptr, ","));	// Sentence number of the total
	if (messageIndex > 3)
	{
		idx = 0;
		return 2;	// overflow
	}


	satellitesInView = atoi(strsep(&ptr, ","));	// Number of satellites in view

	GPS_DB->NumOfSats = satellitesInView;

//	// if NO satellites in view, turn LED off (tests only):
//	if (satellitesInView == 0)
//	{
//		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, SET);
//		return 3;	// no channels
//	}

	do
	{
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, RESET);

		token = strsep(&ptr, ",");				// Satellite ID
		GPS_DB->SatChann[idx].sat_ID = (token && *token) ? atoi(token) : 0;

		token = strsep(&ptr, ",");				// elevation - ignored
		token = strsep(&ptr, ",");				// azimuth   - ignored

		token = strsep(&ptr, ",");				// SNR(C/NO)
		GPS_DB->SatChann[idx].sat_SNR =  (token && *token) ? atoi(token) : 0;
		idx++;
	}
	while ((idx != satellitesInView) && (idx % 4 ) );

	if (idx == satellitesInView)
	{
		idx = 0;
        Sensor_GPS_GSV_get_data();
        return 1;
	}

	return 0;
}


/**
 * @brief Parses a GPGSA NMEA sentence and extracts the fix type.
 */
int Gps_ParseGSA_X(const char* nmea, GPSData_t* GPS_DB)
{
    if (nmea == NULL || GPS_DB == NULL)
        return -1;  // Error: null input

    // Make a local copy to safely tokenize
    char buf[83];
    strncpy(buf, nmea, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';  // Ensure null-termination

    // Tokenize input string to reach the fix type (3rd token)
    char* token = strtok(buf, ",");	// $GPGSA
    token = strtok(NULL, ",");		// Mode (A/M)

    token = strtok(NULL, ",");		// Fix type

    if (token == NULL)
        return -2;  // Error: missing fix field

    GPS_DB->fix = (uint8_t)atoi(token);

    return 0;  // Success
}


uint8_t Gps_ParseTXT_X(const char* nmea, GPSfw_t* firmware)
{
	// THE SENTENCE IS VALID!!!

    if (!nmea || !firmware)
    {
        return 1; // Null pointer error
    }

    // Make a local copy to safely tokenize
    char buf[83];
    strncpy(buf , nmea , sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';  // Ensure null-termination

    char *p = strstr(buf , "AXN");
    if (p)
    {
    	strncpy(firmware->Manufacturer, "SKYLAB (AXN)\0", MAX_FIELD_LEN - 1);
    }

    char* ptr = buf;
    // Tokenize input string to reach the fix type (3rd token)
    char* token;
    token = strsep(&ptr, ",");  // GPTXT
    token = strsep(&ptr, ",");  // Total number of sentences
    token = strsep(&ptr, ",");  // Sentence number
    token = strsep(&ptr, ",");  // Text identifier

    // Firmware release information

    // every piece of information comes in separate message so comment next line:
//    // clean before refreshing data
//    memset(firmware, 0, sizeof(GPSfw_t));

    token = strsep(&ptr, ",");  // Text message

    // ex: GPTXT,01,01,02,SW=URANUS5,V5.3.0.0*1D\r\n
	if (strncmp(token, "SW=", 3) == 0)	// Manufacturer
	{
		char *value = strchr(token, '=');	// remove "SW="
		if (value)
		{
			value++;
			strncpy(firmware->ReleaseStr, value, MAX_FIELD_LEN - 1);
			firmware->ReleaseStr[MAX_FIELD_LEN - 1] = '\0';
		}

		token = strsep(&ptr, ",");  // 2nd Text message
		if (token && strncmp(token, "V", 1) == 0)	// Version
		{
	        // check for asterisk and cut it
	        char *asterisk = strchr(token, '*');
	        if (asterisk)
	        {
	            *asterisk = '\0';
	        }

			strncpy(firmware->Build_ID, token, MAX_FIELD_LEN - 1);
			firmware->Build_ID[MAX_FIELD_LEN - 1] = '\0';
		}
	}

	else
		// ex: GPTXT,01,01,02,TB=2020-03-26,13:25:12*4B\r\n
		if (strncmp(token, "TB=", 3) == 0)	// Time (date only) Built
		{
			char *value = strchr(token, '=');	// remove "TB="
			if (value)
			{
				value++;
				strncpy(firmware->Internal_USE_1, value, MAX_FIELD_LEN - 1);
				firmware->Internal_USE_1[MAX_FIELD_LEN - 1] = '\0';
			}

			// No need to copy time. ONLY date.
		}

		else
			// ex: GPTXT,01,01,02,IC=AT6558R-5N-32-1C580901*13\r\n
			if (strncmp(token, "IC=", 3) == 0)	// chip
			{
				// check for minus and end string
				char *minus = strchr(token, '-');
				if (minus)
				{
					*minus = '\0';
				}

				char *value = strchr(token, '=');	// remove "IC="
				if (value)
				{
					value++;
					strncpy(firmware->Internal_USE_2, value, MAX_FIELD_LEN - 1);
					firmware->Internal_USE_2[MAX_FIELD_LEN - 1] = '\0';
				}
			}
			else
				// $GPTXT, 01,01,02, MA=CASIC*27\r\n
				if (strncmp(token, "MA=", 3) == 0)	// manufacturer
				{
			        // check for asterisk and cut it
			        char *asterisk = strchr(token, '*');
			        if (asterisk)
			        {
			            *asterisk = '\0';
			        }

					char *value = strchr(token, '=');	// remove "IC="
					if (value)
					{
						value++;
						strncpy(firmware->Manufacturer, value, MAX_FIELD_LEN - 1);
						firmware->Build_ID[MAX_FIELD_LEN - 1] = '\0';
					}
				}

    return 0;  // Success
}



/*---------------------------- LOGIC --------------------------*/


uint8_t GpsParser_IsValidStructure_X(const char *s , uint8_t length)
{
	if (length < 6)
		return 0;

	if (s[0] != '$')
		return 0;

	if (!( s[length - 2] == '\r' && s[length - 1] == '\n') )
		return 0;

	return 1;
}


uint8_t hex2nibble_X(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;  // fallback
}


uint8_t hex2byte_X(const char *str)
{
    return (hex2nibble_X(str[0]) << 4) | hex2nibble_X(str[1]);
}


uint8_t GpsParser_IsValidChecksum_X(const char *s , uint8_t length)
{
	// calculate...
	uint8_t calculated = checksum_X(s);

	// extract...
	char *star = strchr(s, '*');

	if (star && isxdigit((unsigned char)star[1]) && isxdigit((unsigned char)star[2]))
//    if (star && strlen(star) >= 3)
	{
        uint8_t extracted = hex2byte_X(&star[1]);
        return (extracted == calculated);
    }

    return 0;
}


void GpsParser_RouteSentence_X(const char *s , uint8_t length)
{
	const char *p = s;

	// make sure there is at least a sentence name (6 characters after $)
	if (length < 7)
		return;

//	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	// Skip '$'
	p++;

	// sentences (314)
	if (strncmp(p, "GP", 2) == 0 || strncmp(p, "GA", 2) == 0 || strncmp(p, "GN", 2) == 0)	// GPS , GALILEO , GNSS
	{
		const char *type = p + 2;

		if (strncmp(type, "RMC", 3) == 0)	// extracts LAT-LONG date time status
		{
			if (gps_data_Lock() == pdTRUE)
			{
//				// test function:
////				char test[] = "GNRMC,203359.000,A,3157.50417,N,03447.89738,E,3.71,183.03,080326,,,A,V*09\r\n";
//				char test[] = "GNRMC,203359.000,A,3157.50417,N,03447.89738,E,57,92,080326,,,A,V*09\r\n";
//				const char *t = test;
//				Gps_ParseRMC_Y(t , &gGpsData_X);

				// clear old data before entering new one:
				memset(&gGpsData_X, 0, sizeof(GPSData_t));
				Gps_ParseRMC_Y(p , &gGpsData_X);
				gps_data_Unlock();
			}
		}

		else
		if (strncmp(type, "GSA", 3) == 0)	// fix type
		{
			if (gps_data_Lock() == pdTRUE)
			{
				Gps_ParseGSA_X(p , &gGpsData_X);
				gps_data_Unlock();
			}
		}

		else
		if (strncmp(type, "GSV", 3) == 0)	// Satellites in View
		{
			if (gps_data_Lock() == pdTRUE)		// CONSIDER USING A NEW MUTEX...
			{
				Gps_ParseGSV_X(p , &gGpsSats_X);
				gps_data_Unlock();
			}

		}

		else
		if (strncmp(type, "TXT", 3) == 0)	// product information
		{
			if (gps_fw_Lock() == pdTRUE)		// CONSIDER USING A NEW MUTEX...
			{
				Gps_ParseTXT_X(p , &gGpsFw_X);
				gps_fw_Unlock();
			}
		}

		else
		{
			// Unknown GP sentence
			logger(NMEA_UNKNOWN_SENTENCE);
		}


	}

	else
	//
	if (strncmp(p, "PM", 2) == 0)
	{
		const char *id = p + 4;

		// PMTK001 PMTK_ACK
		if (strncmp(id, "001", 3) == 0)
		{
			Gps_ParsePMTK_ACK_X(p);
		}

		else
		// PMTK705 PMTK_DT_RELEASE
		if (strncmp(id, "705", 3) == 0)
		{
			if (gps_fw_Lock() == pdTRUE)
			{
				Gps_ParsePMTK_Release_X(p , &gGpsFw_X);
				gps_fw_Unlock();
			}
		}

		else
		{
			// Unknown PMTK sentence
			logger(NMEA_UNKNOWN_SENTENCE);
		}
	}

	else
	{
		// Unknown prefix
	}

}



void get_respond_from_GPS_X(void)
{
    char sentence[92] = {0};
    uint8_t len = 0;

    while (num_OK > 0)
    {
        copynUpto("\r\n", sentence, 100, sizeof(sentence));
        num_OK--;

        // process buffer...

        len = strlen(sentence);
        // check sentence is valid
        if ( GpsParser_IsValidStructure_X(sentence , len) )
        {
            // checksum...
            if (GpsParser_IsValidChecksum_X(sentence , len) )
            {
                // route...
                GpsParser_RouteSentence_X(sentence , len);
            }
        }
    }
}

