/*
 * gps.c
 *
 *  Created on: Feb 9, 2026
 *      Author: illi
 */

#include "FreeRTOS.h"
#include "task.h"
#include "gps.h"
#include "gps_pmtkN.h"
#include "gps_pcasN.h"
#include <stdbool.h>
#include "uartRingBufDMA.h"
#include "string.h"
#include "uart_transport.h"


// see: uartRingBuffer.h
// #define UART huart1

static GPS_Context_t gps_ctx;


static bool GPS_DetectBaud(uint32_t *detected_baud)
{
    const uint32_t bauds[] = {115200, 9600};

    for (int i = 0; i < 2; i++)
    {
        gps_uart_reconfigure(bauds[i]);

        // send question to receiver..
        UART_Send((uint8_t *)"$PCAS03,0,0,0,0,1,0,0,0*03\r\n" , 28);
//        UART_Send((uint8_t *)"$PCAS05,1*19\r\n" , 14);
        Delay_ms(30);
        UART_Send((uint8_t *)"$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n" , 51);
//        UART_Send((uint8_t *)"$PMTK000*32\r\n" , 13);
        Delay_ms(30);

        if (waitFor("$G", 1500))
        {
            *detected_baud = bauds[i];
            return true;
        }
    }

    return false;
}


static bool GPS_DetectProtocol(void)
{
    if (GPS_PMTK_Probe())
    {
        gps_ctx.protocol = GPS_PROTO_PMTK;
        GPS_PMTK_Bind(&gps_ctx);
        return true;
    }
    else if (GPS_PCAS_Probe())
    {
        gps_ctx.protocol = GPS_PROTO_PCAS;
        GPS_PCAS_Bind(&gps_ctx);
        return true;
    }
    else
    {
        gps_ctx.protocol = GPS_PROTO_UNKNOWN;
        return false;
    }

}


bool GPS_Init(void)
{
	memset(&gps_ctx, 0, sizeof(gps_ctx));
	gps_ctx.nmea_mask = 0;

	// 1. Detect Baud-rate
	if (!GPS_DetectBaud(&gps_ctx.baudrate))
		return false;

	// 2. Detect Protocol
	// 3. Bind
	if (!GPS_DetectProtocol())
		return false;

    gps_ctx.is_initialized = true;
	if ( huart1.Init.BaudRate != 115200)
	{
		GPS_Set_Baud(BAUDR_115200);
		gps_uart_reconfigure(115200);
	}

	PCAS_Set_NMEA_Out_X(0);		// Temporary

    return true;
}


//bool GPS_port_TX(const char *cmd, uint8_t len)
//{
//	if (!gps_ctx.is_initialized)
//		return false;
//
//	return gps_ctx.send_cmd(cmd, len);
////	return true;
//}


bool GPS_StartRMC(void)
{
	if (!gps_ctx.is_initialized || !gps_ctx.start_rmc)
		return false;

	return gps_ctx.start_rmc(&gps_ctx);
//	return true;
}


bool GPS_StopRMC(void)
{
	if (!gps_ctx.is_initialized || !gps_ctx.stop_rmc)
		return false;

	return gps_ctx.stop_rmc(&gps_ctx);
//	return true;
}


bool GPS_StartGSV(void)
{
	if (!gps_ctx.is_initialized || !gps_ctx.start_gsv)
		return false;

	return gps_ctx.start_gsv(&gps_ctx);
//	return true;
}


bool GPS_StopGSV(void)
{
	if (!gps_ctx.is_initialized || !gps_ctx.stop_gsv)
		return false;

	return gps_ctx.stop_gsv(&gps_ctx);
//	return true;
}


//================================================


bool GPS_Get_Version(void)
{
	if (!gps_ctx.is_initialized || !gps_ctx.get_version)
		return false;

	return gps_ctx.get_version(&gps_ctx);
//	return true;
}


bool GPS_Hot_Start(void)
{
	if (!gps_ctx.is_initialized || !gps_ctx.hot_start)
		return false;

	return gps_ctx.hot_start(&gps_ctx);
//	return true;
}


bool GPS_Cold_Start(void)
{
	if (!gps_ctx.is_initialized || !gps_ctx.cold_start)
		return false;

	return gps_ctx.cold_start(&gps_ctx);
//	return true;
}


bool GPS_Set_Baud(enum BaudFlag baud)
{
	if (!gps_ctx.is_initialized || !gps_ctx.set_baud)
		return false;

	if (gps_ctx.requested_baud == baud)
		return true;
	else
		gps_ctx.requested_baud = baud;

	HAL_UART_DMAStop(&UART);
	Ringbuf_Init ();
    if (gps_ctx.set_baud(&gps_ctx))
    {
    	vTaskDelay(pdMS_TO_TICKS(200)); // wait for GPS to change rate
    	gps_uart_reconfigure(baud);
    	return true;
    }
    return false;
//	return true;
}


bool GPS_Standby(void)
{
	if (!gps_ctx.is_initialized || !gps_ctx.standby)
		return false;

	return gps_ctx.standby(&gps_ctx);
//	return true;
}


bool GPS_Restart(void)
{
	if (!gps_ctx.is_initialized || !gps_ctx.restart)
		return false;

	return gps_ctx.restart(&gps_ctx);
//	return true;
}




