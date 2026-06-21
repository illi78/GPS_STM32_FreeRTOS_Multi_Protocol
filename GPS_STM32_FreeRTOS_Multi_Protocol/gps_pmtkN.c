/*
 * gps_pmtkN.c
 *
 *  Created on: Feb 9, 2026
 *      Author: illi
 */


#include "gps.h"
#include "gps_pmtkN.h"
#include "main.h"
#include "uartRingBufDMA.h"
#include "uart_transport.h"
#include "string.h"


#include "task_debug.h"      // for printing notification codes

static bool PMTK_StartRMC (GPS_Context_t *ctx);
static bool PMTK_StopRMC  (GPS_Context_t *ctx);
static bool PMTK_StartGSV (GPS_Context_t *ctx);
static bool PMTK_StopGSV  (GPS_Context_t *ctx);

char TxDataMPTK[257] = {0};

bool GPS_PMTK_Probe(void)
{
//    GPS_UART_FlushRx();

    UART_Send((uint8_t *)"$PMTK605*31\r\n" , 13); // Firmware version request		// GPS_UART_Send

    if (waitFor("$PMTK", 5000))
        return true;

    return false;
}


static bool PMTK_Send(const char * str , uint8_t len)
{
    return UART_Send((const uint8_t *)str, len);
}


static bool PMTK_StartRMC(GPS_Context_t *ctx)
{
    /* PMTK command */
    ctx->nmea_mask |= NMEA_SEN_RMC;
    PMTK_Set_NMEA_Out_X(ctx->nmea_mask);
	logger (PMTK_TX_RMC_START_COMMAND_SENT);
    return true;
}


static bool PMTK_StopRMC(GPS_Context_t *ctx)
{
    /* PMTK command */
	ctx->nmea_mask &= ~NMEA_SEN_RMC;
	PMTK_Set_NMEA_Out_X(ctx->nmea_mask);
	logger (PMTK_TX_RMC_STOP_COMMAND_SENT);
   return true;
}


static bool PMTK_StartGSV(GPS_Context_t *ctx)
{
    /* PMTK command */
    ctx->nmea_mask |= NMEA_SEN_GSV;
    PMTK_Set_NMEA_Out_X(ctx->nmea_mask);
	logger (PMTK_TX_GSV_START_COMMAND_SENT);
   return true;
}


static bool PMTK_StopGSV(GPS_Context_t *ctx)
{
    /* PMTK command */
    ctx->nmea_mask &= ~NMEA_SEN_GSV;
    PMTK_Set_NMEA_Out_X(ctx->nmea_mask);
	logger (PMTK_TX_GSV_SOPT_COMMAND_SENT);
    return true;
}


static bool PMTK_Get_Version(GPS_Context_t *ctx)
{
    /* PMTK command */
	sprintf (TxDataMPTK , "$PMTK605*31\r\n");
	UART_Send ((uint8_t *)TxDataMPTK, strlen(TxDataMPTK));
	logger (PMTK_TX_RELEASE_COMMAND_SENT);
    return true;
}


static bool PMTK_Hot_Start(GPS_Context_t *ctx)
{
    /* PMTK command */
	sprintf (TxDataMPTK , "$PMTK101*32\r\n");
	UART_Send ((uint8_t *)TxDataMPTK, strlen(TxDataMPTK));
	logger (PMTK_TX_HOTSTART_COMMAND_SENT);
    return true;
}


static bool PMTK_Cold_Start(GPS_Context_t *ctx)
{
    /* PMTK command */
	sprintf (TxDataMPTK , "$PMTK103*30\r\n");
	UART_Send ((uint8_t *)TxDataMPTK, strlen(TxDataMPTK));
	logger (PMTK_TX_COLDSTART_COMMAND_SENT);
    return true;
}


static bool PMTK_Set_Baud(GPS_Context_t *ctx)
{
    /* PMTK command */
    if (!isValidBaudRate_X(ctx->baudrate))
    {
        logger (PMTK_BAUDRATE_INVALID_INPUT);
        return false;  // Stop execution if baud rate is not valid
    }

    char TxDataMPTK[100];
    sprintf(TxDataMPTK, "$PMTK%s,%lu", TOSTRING(PMTK_SET_NMEA_BAUDRATE), ctx->baudrate);

    // calculate checksum
    uint8_t chksum = checksum_X(TxDataMPTK);

    // append checksum and ending
    sprintf(TxDataMPTK + strlen(TxDataMPTK), "*%02X\r\n", chksum);

    // TxDataMPTK now contains the full command ready to send
    // e.g., "$PMTK251,38400*27\r\n"

    // send TxDataMPTK to the GPS module
    UART_Send ((uint8_t *)TxDataMPTK, strlen(TxDataMPTK));

    logger (PMTK_BAUDRATE_CHANGE_REQUEST_SENT);

    return true;
}


static bool PMTK_Standby(GPS_Context_t *ctx)
{
    /* PMTK command */
	sprintf (TxDataMPTK , "$PMTK161,0*28\r\n");
	UART_Send ((uint8_t *)TxDataMPTK, strlen(TxDataMPTK));
	logger (PMTK_TX_STANDBY_COMMAND_SENT);
    return true;
}


static bool PMTK_Restart(GPS_Context_t *ctx)
{
    /* PMTK command */
	logger (PMTK_TX_NO_RESTART_COMMAND);
    return false;
}



void GPS_PMTK_Bind(GPS_Context_t *ctx)
{
    ctx->send_cmd  = PMTK_Send;

    ctx->start_rmc = PMTK_StartRMC;
    ctx->stop_rmc  = PMTK_StopRMC;
    ctx->start_gsv = PMTK_StartGSV;
    ctx->stop_gsv  = PMTK_StopGSV;

    ctx->get_version  = PMTK_Get_Version;
    ctx->hot_start    = PMTK_Hot_Start;
    ctx->cold_start   = PMTK_Cold_Start;
    ctx->set_baud     = PMTK_Set_Baud;
    ctx->standby      = PMTK_Standby;
    ctx->restart      = PMTK_Restart;
}


void PMTK_Set_NMEA_Out_X(uint8_t sentence)
{
    // Start with a default "all off" sentence configuration
    // Indices refer to each NMEA sentence position in the PMTK314 command
    sprintf(TxDataMPTK, "$PMTK%s,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0", TOSTRING(PMTK_API_SET_NMEA_OUTPUT));
    sprintf(TxDataMPTK, "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0");

    // If STOP bit is set, disable all NMEA output
    if (sentence & NMEA_SEN_STOP)
    {
        uint8_t chksum = checksum_X(TxDataMPTK);
        sprintf(TxDataMPTK + strlen(TxDataMPTK), "*%02X\r\n", chksum);

        UART_Send ((uint8_t *)TxDataMPTK, strlen(TxDataMPTK));

        logger(PMTK_NMEA_OUT_STOPPED);
        return;
    }

    // Enable selected NMEA sentences based on bitmask
    if (sentence & NMEA_SEN_GLL) TxDataMPTK[PMTK_IDX_GLL] = '1';  ///< Geographic Position - Latitude/Longitude
    if (sentence & NMEA_SEN_RMC) TxDataMPTK[PMTK_IDX_RMC] = '1';  ///< Recommended Minimum Specific GNSS Data
    if (sentence & NMEA_SEN_VTG) TxDataMPTK[PMTK_IDX_VTG] = '1';  ///< Track Made Good and Ground Speed
    if (sentence & NMEA_SEN_GGA) TxDataMPTK[PMTK_IDX_GGA] = '1';  ///< Global Positioning System Fix Data
    if (sentence & NMEA_SEN_GSA) TxDataMPTK[PMTK_IDX_GSA] = '1';  ///< GNSS DOP and Active Satellites
    if (sentence & NMEA_SEN_GSV) TxDataMPTK[PMTK_IDX_GSV] = '1';  ///< GNSS Satellites in View
    if (sentence & NMEA_SEN_ZDA) TxDataMPTK[PMTK_IDX_ZDA] = '1';  ///< Time & Date


    // Finalize the sentence with checksum and send
    uint8_t chksum = checksum_X(TxDataMPTK);
    sprintf(TxDataMPTK + strlen(TxDataMPTK), "*%02X\r\n", chksum);
    UART_Send ((uint8_t *)TxDataMPTK, strlen(TxDataMPTK));
    logger(PMTK_NMEA_OUT_UPDATED_SUCCESSFULLY);
}


uint8_t checksum_X(const char *str)
{
    uint8_t chksm = 0;
    // Find the start character '$'
    const char *p = strchr(str, '$');
    if (!p) return 0;  // If no '$', return 0
    p++;  // Move past '$'

    // XOR all characters until '*' or end of string
    while (*p && *p != '*')
    {
        chksm ^= *p++;
    }
    return chksm;
}



bool isValidBaudRate_X(uint32_t baudRate)
{
    // List of allowed baud rates
    const uint32_t allowedRates[] = {4800, 9600, 14400, 19200, 38400, 57600, 115200, 230400};
    size_t numRates = sizeof(allowedRates) / sizeof(allowedRates[0]);
    for (size_t i = 0; i < numRates; i++)
    {
        if (baudRate == allowedRates[i])
        {
            return true;
        }
    }
    return false;
}




