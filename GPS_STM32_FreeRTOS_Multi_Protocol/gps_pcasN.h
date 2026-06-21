/*
 * gps_pcasN.h
 *
 *  Created on: Feb 9, 2026
 *      Author: illi
 */

#ifndef GPS_PCASN_H_
#define GPS_PCASN_H_

#define PCAS_IDXs_GGA           8
#define PCAS_IDXs_GLL          10
#define PCAS_IDXs_GSA          12
#define PCAS_IDXs_GSV          14
#define PCAS_IDXs_RMC          16
#define PCAS_IDXs_VTG          18
#define PCAS_IDXs_ZDA          20
#define PCAS_IDXs_TXT          22





/*
#define NMEA_SEN_GLL    0x01	// [0]
#define NMEA_SEN_RMC    0x02	// [1]		// date time LAT LON
#define NMEA_SEN_VTG    0x04	// [2]
#define NMEA_SEN_GGA    0x08	// [3]
#define NMEA_SEN_GSA    0x10	// [4]		// fix type
#define NMEA_SEN_GSV    0x20	// [5]		// list of satellites and SNR (12 channels MAX)
#define NMEA_SEN_ZDA    0x40	// [17]		// date time

#define NMEA_SEN_STOP   0x80
*/



#define PCAS_TX_RMC_START_COMMAND_SENT      0x60
#define PCAS_TX_RMC_STOP_COMMAND_SENT       0x61
#define PCAS_TX_GSV_START_COMMAND_SENT      0x62
#define PCAS_TX_GSV_SOPT_COMMAND_SENT       0x63
#define PCAS_TX_RELEASE0_COMMAND_SENT       0x64
#define PCAS_TX_RELEASE2_COMMAND_SENT       0x65
#define PCAS_TX_RELEASE3_COMMAND_SENT       0x66
#define PCAS_TX_HOTSTART_COMMAND_SENT       0x67
#define PCAS_TX_COLDSTART_COMMAND_SENT      0x68
#define PCAS_BAUDRATE_CHANGE_REQUEST_SENT   0x69
#define PCAS_TX_STANDBY_COMMAND_SENT        0x6A
#define PCAS_TX_RESTART_COMMAND_SENT        0x6B
#define PCAS_NMEA_OUT_STOPPED               0x6C
#define PCAS_NMEA_OUT_UPDATED_SUCCESSFULLY  0x6D


bool GPS_PCAS_Probe(void);
void GPS_PCAS_Bind (GPS_Context_t *ctx);

void PCAS_Set_NMEA_Out_X(uint8_t sentence);

#endif /* GPS_PCASN_H_ */
