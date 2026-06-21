// call example:

#include "uart_transport.h"
#include "gps_data.h"

#include "uartRingBufDMA.h"



// for GPS:
typedef struct __attribute__((packed)) {
	bool  status;              // 0 = invalid, 1 = valid

    int32_t  latitude_1e6;        // DMM * 10^6
    int32_t  longitude_1e6;       // DMM * 10^6
//    uint8_t  direction_flags;      // bit0: N/S, bit1: E/W

    uint16_t speed_x10;      // *10
    uint16_t course_x10;           // 0-359

    int16_t  altitude_p500;            // meters (signed with offset of 500m)

    uint8_t  fix;                  // 0=no fix, 1=1D, 2=2D, 3=3D

    RTC_DateTypeDef gDate;
    RTC_TimeTypeDef gTime;

} PayloadGPS_t;


// for satellites:
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


// for the main function:

int main(void)
{
    // initialization...

    /*==== INIT THE DMA *BEFORE* THE UART !! ====*/

    /* Initialize all configured peripherals */
    MX_DMA_Init();
    MX_USART1_UART_Init();

    // initialize the system

    // mutex FOR GPS COMMUNICATION
    gps_data_init();
    uart_mutex_init();
    // initialize the ring buffer for UART gps port
    Ringbuf_Init();

    // continue initialization...

    // Start scheduler (?)

    while (1)
    {
        // main loop (?)
    }

}


// request getting data..
bool Sensor_GPS_RMC(SensContext_t *SensCTX, ActiveCommand_t *cmd)
{

	// 1. read sensor's parameters
	GPS_StartRMC();

	Ringbuf_Init ();

    return true;
}


bool Sensor_GPS_RMC_get_data(void)
{
	static PayloadGPS_t gps;
	memset(&gps, 0, sizeof(PayloadGPS_t));

//	gps = {
    gps.status          = gGpsData_X.status;

    gps.latitude_1e6    = gGpsData_X.latitude_1e6;
    gps.longitude_1e6   = gGpsData_X.longitude_1e6;

//        .direction_flags =   gGpsData_X.direction_flags, // N + E

    gps.speed_x10       = gGpsData_X.speed_x10; // 12.5 knots
    gps.course_x10      =  gGpsData_X.course_x10;

    gps.altitude_p500   = gGpsData_X.altitude_p500;

    gps.fix             =   gGpsData_X.fix;

    gps.gDate.Year      =  gGpsData_X.gDate.Year;
    gps.gDate.Month     =   gGpsData_X.gDate.Month;
    gps.gDate.Date      =   gGpsData_X.gDate.Date;

    gps.gTime.Hours     =  gGpsData_X.gTime.Hours;
    gps.gTime.Minutes   =  gGpsData_X.gTime.Minutes;
    gps.gTime.Seconds   =  gGpsData_X.gTime.Seconds;
//	};



	// 2. write it to payload string

	memcpy(&SensCTX.payload, &gps, sizeof(PayloadGPS_t));

    // process it using RTOS:
	// 4. send to back to qAppRX
    xQueueSendToBack(qAppRX, &SensCTX.pktRX, 0);
    xTaskNotify(ApplicationTask_Handler, EVT_RX_PACKET, eSetBits);


    return true;
}


bool Sensor_GPS_GSV(SensContext_t *SensCTX, ActiveCommand_t *cmd)
{
	// 1. read sensor's parameters
	GPS_StartGSV();

	Ringbuf_Init ();

    return true;
}


bool Sensor_GPS_GSV_get_data(void)
{

	// 2. write it to payload string SensCTX
        memset(&SensCTX.payload, 0  , sizeof(SENSOR_PAYLOAD_SIZE));
        memcpy(&SensCTX.payload, &gGpsSats_X, sizeof(GPSsats_t));	// it reads GSV only once!!!

    // process it using RTOS:
	// 4. send to back to qAppRX
    xQueueSendToBack(qAppRX, &SensCTX.pktRX, 0);
    xTaskNotify(ApplicationTask_Handler, EVT_RX_PACKET, eSetBits);

    return true;

}




void Sensor_GPS_RMC_stop(SensContext_t *SensCTX, ActiveCommand_t *cmd)
{
	// 1. read sensor's parameters
	GPS_StopRMC();

}


void Sensor_GPS_GSV_stop(SensContext_t *SensCTX, ActiveCommand_t *cmd)
{
	// 1. read sensor's parameters
	GPS_StopGSV();
}




void Sensor_GPS_Standby(SensContext_t *SensCTX, ActiveCommand_t *cmd)
{
		// 1. read sensor's parameters
		GPS_Standby();
}


void Sensor_GPS_Version(SensContext_t *SensCTX, ActiveCommand_t *cmd)
{
		// 1. read sensor's parameters
		GPS_Get_Version();
}
