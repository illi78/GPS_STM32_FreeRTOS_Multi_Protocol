/*
 * gps_pcasN.c
 *
 *  Created on: Feb 9, 2026
 *      Author: illi
 */


#include "gps.h"
#include "gps_pcasN.h"
#include "gps_pmtkN.h"
#include "main.h"
#include "uartRingBufDMA.h"
#include "uart_transport.h"
#include "string.h"

#include "task_debug.h"      // for printing notification codes

static bool PCAS_StartRMC (GPS_Context_t *ctx);
static bool PCAS_StopRMC  (GPS_Context_t *ctx);
static bool PCAS_StartGSV (GPS_Context_t *ctx);
static bool PCAS_StopGSV  (GPS_Context_t *ctx);

char TxDataPCAS_X[32] = {0};

bool GPS_PCAS_Probe(void)
{
//    GPS_UART_FlushRx();

    UART_Send((uint8_t *)"$PCAS06,0*1B\r\n" , 14); // Firmware version request		// GPS_UART_Send
	Ringbuf_Init ();

    if (waitFor("$GPTXT", 5000))
        return true;

    return false;
}


static bool PCAS_Send(const char * str , uint8_t len)
{
    return UART_Send((const uint8_t *)str, len);
}


static bool PCAS_StartRMC(GPS_Context_t *ctx)
{
    /* PCAS command */
	ctx->nmea_mask |= NMEA_SEN_RMC;
	PCAS_Set_NMEA_Out_X(ctx->nmea_mask);
	logger (PCAS_TX_RMC_START_COMMAND_SENT);
    return true;
}


static bool PCAS_StopRMC(GPS_Context_t *ctx)
{
    /* PCAS command */
	ctx->nmea_mask &= ~NMEA_SEN_RMC;
	PCAS_Set_NMEA_Out_X(ctx->nmea_mask);
	logger (PCAS_TX_RMC_STOP_COMMAND_SENT);
	return true;
}


static bool PCAS_StartGSV(GPS_Context_t *ctx)
{
    /* PCAS command */
	ctx->nmea_mask |= NMEA_SEN_GSV;
	PCAS_Set_NMEA_Out_X(ctx->nmea_mask);
	logger (PCAS_TX_GSV_START_COMMAND_SENT);
	return true;
}


static bool PCAS_StopGSV(GPS_Context_t *ctx)
{
    /* PCAS command */
	ctx->nmea_mask &= ~NMEA_SEN_GSV;
	PCAS_Set_NMEA_Out_X(ctx->nmea_mask);
	logger (PCAS_TX_GSV_SOPT_COMMAND_SENT);
	return true;
}

//---------
static bool PCAS_Get_Version(GPS_Context_t *ctx)
{
    /* PCAS command */
	sprintf (TxDataPCAS_X , "$PCAS06,0*1B\r\n");	// version number
	UART_Send ((uint8_t *)TxDataPCAS_X, strlen(TxDataPCAS_X));
	logger (PCAS_TX_RELEASE0_COMMAND_SENT);

	sprintf (TxDataPCAS_X , "$PCAS06,1*1A\r\n");	// HW model & s/n
	UART_Send ((uint8_t *)TxDataPCAS_X, strlen(TxDataPCAS_X));
	logger (PCAS_TX_RELEASE2_COMMAND_SENT);

	sprintf (TxDataPCAS_X , "$PCAS06,2*19\r\n");	// system mode
	UART_Send ((uint8_t *)TxDataPCAS_X, strlen(TxDataPCAS_X));
	logger (PCAS_TX_RELEASE2_COMMAND_SENT);

	sprintf (TxDataPCAS_X , "$PCAS06,3*18\r\n");	// product number
	UART_Send ((uint8_t *)TxDataPCAS_X, strlen(TxDataPCAS_X));
	logger (PCAS_TX_RELEASE3_COMMAND_SENT);

	sprintf (TxDataPCAS_X , "$PCAS06,6*1D\r\n");	// HW model & s/n
	UART_Send ((uint8_t *)TxDataPCAS_X, strlen(TxDataPCAS_X));
	logger (PCAS_TX_RELEASE3_COMMAND_SENT);

	return true;
}


static bool PCAS_Hot_Start(GPS_Context_t *ctx)
{
    /* PCAS command */
	sprintf (TxDataPCAS_X , "$PCAS10,0*1C\r\n");	// hot start
	UART_Send ((uint8_t *)TxDataPCAS_X, strlen(TxDataPCAS_X));
	logger (PCAS_TX_HOTSTART_COMMAND_SENT);
	return true;
}


static bool PCAS_Cold_Start(GPS_Context_t *ctx)
{
    /* PCAS command */
	sprintf (TxDataPCAS_X , "$PCAS10,2*1E\r\n");	// cold start
	UART_Send ((uint8_t *)TxDataPCAS_X, strlen(TxDataPCAS_X));
	logger (PCAS_TX_COLDSTART_COMMAND_SENT);
	return true;
}


//static bool PCAS_Set_Baud(GPS_Context_t *ctx, enum BaudRateFlag code)
static bool PCAS_Set_Baud(GPS_Context_t *ctx)
{
    if (!ctx)
        return false;

    /* PCAS command */
    memset(TxDataPCAS_X , 0 , sizeof(TxDataPCAS_X) );
    sprintf(TxDataPCAS_X, "$PCAS01,%c", ctx->requested_baud);

    // calculate checksum
    uint8_t chksum = checksum_X(TxDataPCAS_X);

    // append checksum and ending
    sprintf(TxDataPCAS_X + strlen(TxDataPCAS_X), "*%02X\r\n", chksum);

    // TxData now contains the full command ready to send
    // e.g., "$PCAS01,5*19\r\n"    // for 115200

    // send TxData to the GPS module
    if (!UART_Send ((uint8_t *)TxDataPCAS_X, strlen(TxDataPCAS_X)))
    	return false;

    logger (PCAS_BAUDRATE_CHANGE_REQUEST_SENT);
    return true;
}


static bool PCAS_Standby(GPS_Context_t *ctx)
{
    /* PCAS command */
	sprintf (TxDataPCAS_X , "$PCAS12,0*1E\r\n");	// standby mode
	UART_Send ((uint8_t *)TxDataPCAS_X, strlen(TxDataPCAS_X));
	logger (PCAS_TX_STANDBY_COMMAND_SENT);
	return true;
}


static bool PCAS_Restart(GPS_Context_t *ctx)
{
    /* PCAS command */
	sprintf (TxDataPCAS_X , "$PCAS10,0*1C\r\n");	// restart
	UART_Send ((uint8_t *)TxDataPCAS_X, strlen(TxDataPCAS_X));
	logger (PCAS_TX_RESTART_COMMAND_SENT);
	return true;
}



void GPS_PCAS_Bind(GPS_Context_t *ctx)
{
    ctx->send_cmd  = PCAS_Send;

    ctx->start_rmc = PCAS_StartRMC;
    ctx->stop_rmc  = PCAS_StopRMC;
    ctx->start_gsv = PCAS_StartGSV;
    ctx->stop_gsv  = PCAS_StopGSV;

    ctx->get_version  = PCAS_Get_Version;
    ctx->hot_start    = PCAS_Hot_Start;
    ctx->cold_start   = PCAS_Cold_Start;
    ctx->set_baud     = PCAS_Set_Baud;
    ctx->standby      = PCAS_Standby;
    ctx->restart      = PCAS_Restart;
}


void PCAS_Set_NMEA_Out_X(uint8_t sentence)
{
    // Start with a default "all off" sentence configuration
    // Indexes refer to each NMEA sentence position in the CAS03 command
    sprintf(TxDataPCAS_X, "$PCAS03,0,0,0,0,0,0,0,0");

    // If STOP bit is set, disable all NMEA output
    if (sentence & NMEA_SEN_STOP)
    {
        uint8_t chksum = checksum_X(TxDataPCAS_X);
        sprintf(TxDataPCAS_X + strlen(TxDataPCAS_X), "*%02X\r\n", chksum);

        UART_Send ((uint8_t *)TxDataPCAS_X, strlen(TxDataPCAS_X));

        logger(PCAS_NMEA_OUT_STOPPED);
        return;
    }

    // Enable selected NMEA sentences based on bitmask
    if (sentence & NMEA_SEN_GGA) TxDataPCAS_X[PCAS_IDXs_GGA] = '1';  ///< Global Positioning System Fix Data
    if (sentence & NMEA_SEN_GLL) TxDataPCAS_X[PCAS_IDXs_GLL] = '1';  ///< Geographic Position - Latitude/Longitude
    if (sentence & NMEA_SEN_GSA) TxDataPCAS_X[PCAS_IDXs_GSA] = '5';  ///< GNSS DOP and Active Satellites
    if (sentence & NMEA_SEN_GSV) TxDataPCAS_X[PCAS_IDXs_GSV] = '5';  ///< GNSS Satellites in View
    if (sentence & NMEA_SEN_RMC) TxDataPCAS_X[PCAS_IDXs_RMC] = '1';  ///< Recommended Minimum Specific GNSS Data
    if (sentence & NMEA_SEN_VTG) TxDataPCAS_X[PCAS_IDXs_VTG] = '1';  ///< Track Made Good and Ground Speed
    if (sentence & NMEA_SEN_ZDA) TxDataPCAS_X[PCAS_IDXs_ZDA] = '1';  ///< Time & Date
//    if (sentence & NMEA_SEN_TXT) TxDataPCAS_X[IDXs_TXT] = '1';  ///< Time & Date

    // Finalize the sentence with checksum and send
    uint8_t chksum = checksum_X(TxDataPCAS_X);
    sprintf(TxDataPCAS_X + strlen(TxDataPCAS_X), "*%02X\r\n", chksum);
    UART_Send ((uint8_t *)TxDataPCAS_X, strlen(TxDataPCAS_X));
    logger(PCAS_NMEA_OUT_UPDATED_SUCCESSFULLY);
}

















