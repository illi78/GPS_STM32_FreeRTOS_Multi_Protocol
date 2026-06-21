/*
 * gps_pmtkN.h
 *
 *  Created on: Feb 9, 2026
 *      Author: illi
 */

#ifndef GPS_PMTKN_H_
#define GPS_PMTKN_H_


#define PMTK_IDX_GLL            9
#define PMTK_IDX_RMC           11
#define PMTK_IDX_VTG           13
#define PMTK_IDX_GGA           15
#define PMTK_IDX_GSA           17
#define PMTK_IDX_GSV           19
#define PMTK_IDX_ZDA           43


#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define PMTK_ok                            0
#define Talker_ID                          PMTK

#define NMEA_SEN_GLL    0x01	// [0]
#define NMEA_SEN_RMC    0x02	// [1]		// date time LAT LON
#define NMEA_SEN_VTG    0x04	// [2]
#define NMEA_SEN_GGA    0x08	// [3]
#define NMEA_SEN_GSA    0x10	// [4]		// fix type
#define NMEA_SEN_GSV    0x20	// [5]		// list of satellites and SNR (12 channels MAX)
#define NMEA_SEN_ZDA    0x40	// [17]		// date time

#define NMEA_SEN_STOP   0x80                // clears everything


#define PMTK_TX_RMC_START_COMMAND_SENT      0x70
#define PMTK_TX_RMC_STOP_COMMAND_SENT       0x71
#define PMTK_TX_GSV_START_COMMAND_SENT      0x72
#define PMTK_TX_GSV_SOPT_COMMAND_SENT       0x73
#define PMTK_TX_RELEASE_COMMAND_SENT        0x74
#define PMTK_TX_HOTSTART_COMMAND_SENT       0x75
#define PMTK_TX_COLDSTART_COMMAND_SENT      0x76
#define PMTK_BAUDRATE_INVALID_INPUT         0x77
#define PMTK_BAUDRATE_CHANGE_REQUEST_SENT   0x78
#define PMTK_TX_STANDBY_COMMAND_SENT        0x79
#define PMTK_TX_NO_RESTART_COMMAND          0x7A
#define PMTK_NMEA_OUT_STOPPED               0x7B
#define PMTK_NMEA_OUT_UPDATED_SUCCESSFULLY  0x7C


/*
// Pkt Type
// From "000" to "999", an identifier used to tell the
// decoder how to decode the packet

#define PMTK_TEST                          000	// * Test Packet
#define PMTK_ACK                           001	// Acknowledge of PMTK command

#define PMTK_SYS_MSG                       010	// Output system message
#define PMTK_TXT_MSG                       011	// Output system message
#define PMTK_CMD_HOT_START                 101	// * Hot Restart: Use all available data in the NV Store.
#define PMTK_CMD_WARM_START                102	// * Warm Restart: Don't use Ephemeris at re-start.
#define PMTK_CMD_COLD_START                103	// * Cold Restart: Don't use Time, Position, Almanacs and Ephemeris data at re-start.
#define PMTK_CMD_FULL_COLD_START           104	// * Full Cold Restart: It's essentially a Cold Restart, but additionally clear system/user configurations at re-start. That is, reset the receiver to the factory status.

#define PMTK_CMD_SET_NMEA_UPDATERATE       220	// * Set NMEA port update rate
#define PMTK_API_Q_FIX_CTL                 400	// Query update rate
#define PMTK_DT_FIX_CTL                    500	// The parameter means which update is set currently

#define PMTK_SET_NMEA_BAUDRATE             251	// Set NMEA port baud rate
#define PMTK_API_SET_DGPS_MODE             301	// API_Set_Dgps_Mode
#define PMTK_API_Q_DGPS_MODE               401	// API_Query_Dgps_Mode
#define PMTK_API_DT_DGPS_MODE              501	// DGPS data source mode
#define PMTK_API_SET_SBAS_ENABLED          313	// API_Set_Sbas_Enabled
#define PMTK_API_Q_SBAS_ENABLED            413	// API_Query_Sbas_Enabled

#define PMTK_DT_SBAS_ENABLED               513	// Acknowledge for SBAS function is enable or disable.
#define PMTK_API_SET_NMEA_OUTPUT           314	// API_Set_NMEA_Out. Set NMEA sentence output frequencies.
#define  PMTK_API_Q_NMEA_OUTPUT            414	// API_Query_NMEA_Out. Query current NMEA sentence output frequencies
#define PMTK_API_DT_NMEA_OUTPUT            514	// NMEA sentence output frequency setting

#define PMTK_Q_RELEASE                     605	// Query the firmware release information.
#define PMTK_DT_RELEASE                    705	// Firmware release information.

#define PMTK_Q_EPO_INFO                    607	// Query the EPO data status stored in the GPS chip
#define PMTK_DT_EPO_INFO                   707	// EPO data status stored in the GPS chip

#define PMTK_CMD_CLEAR_EPO                 127	// Clear the EPO data stored in the GPS chip
#define PMTK_SET_Nav Speed threshold       386	// Set the speed threshold for static navigation. If the actual speed is below the threshold, output position. will keep the same and output speed will be zero. If threshold value is set to 0, this function is disabled.

#define PMTK_Q_Nav_Threshold               447	// Query current Nav Speed threshold setting.
#define PMTK_DT_Nav_Threshold              527	// Current Nav Speed Threshold setting

#define PMTK_CMD_STANDBY_MODE              161	// * Enter standby mode for power saving.
#define PMTK_SET_AL_DEE_CFG                223	// It means the module needs to extend the time for ephemeris data receiving under what situation.
#define PMTK_CMD_PERIODIC_MODE             225	// Enter Standby or Backup mode for power saving.
#define PMTK_SET_SYNC_PPS_NMEA             255  // Enable or disable fix NMEA output time behind PPS function. (Default off)
#define PMTK_CMD_AIC_MODE                  286	// Active Interference Cancellation (AIC) feature provides effective narrow-band interference and jamming elimination.
#define PMTK_CMD_EASY_ENABLE               869	// Enable or disable EASY function. Query if EASY is enabled or disabled
#define PMTK_FR_MODE                       886	// Set navigation mode. ( '0': Vehicle mode: For general purpose , '1': Pedestrian: For running and walking purpose that the low-speed (< 5m/s) movement )
#define PMTK_API_SET_DATUM                 330	// Configure Datum
#define PMTK_API_SET_DATUM_ADVANCE         331	// Set user defined datum
#define PMTK_API_Q_DATUM_ADVANCE           431	// Query user defined datum
#define PMTK_API_SET_GNSS_SEARCH_MODE      353	// s used to configure the receive to start searching of which satellite system The setting will be kept available when NVRAM data is valid
#define PMTK_API_Q_DATUM                   438	// Query default Datum
#define PMTK_API_DT_DATUM                  530	// Current datum used
#define PMTK_LOCUS_QUERY_STATUS            183	// Query Logging status
#define PMTK_LOCUS_ERASE_FLASH             184	// Erase Logging flash
#define PMTK_LOCUS_STOP_LOGGER             185	// Stop/Start Logging flash
#define PMTK_LOG_NOW                       186	// Snapshot write log
#define PMTK_LOCUS_CONFIG                  187	// Configure LOCUS setting by command
#define PMTK_Q_LOCUS_DATA                  622	// Dump LOCUS flash data

#define PMTK_Q_DATA_PORT                   602	// Read data port input/output
#define PMTK_DT_DATA_PORT                  702	// Display data port input/output data type and baud rate

#define PMTK_SET_PPS_CONFIG_CMD            285	// Configure PPS setting
#define PMTK_SET_OUTPUT_DEBUG              299	// Enable or disable Debug log output
#define PMTK_API_QUERY_GNSS_SEARCH_MODE    355	// This command is to get GLONASS, BEIDOU and GALILEO search setting
#define PMTK_API_SET_HDOP_THRESHOLD        356	// set the HDOP threshold. If the HDOP value is larger than this threshold value, the position will not be fixed.
#define PMTK_API_GET_HDOP_THRESHOLD        357	// This command is to get the HDOP threshold.

#define PMTK_API_Q_RTC_TIME                435	// This command is to get current RTC UTC Time
#define PMTK_API_DT_RTC_TIME               535	// This packet includes current RTC UTC Time

#define PMTK_SET_DATA_PORT                 258	// Set data port (UART1) input/output data type and baud rate
*/


bool    GPS_PMTK_Probe(void);
void    GPS_PMTK_Bind (GPS_Context_t *ctx);

void    PMTK_Set_NMEA_Out_X(uint8_t sentence);
uint8_t checksum_X         (const char *str);
bool    isValidBaudRate_X  (uint32_t baudRate);

#endif /* GPS_PMTKN_H_ */
