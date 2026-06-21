# GPS Driver for STM32 + FreeRTOS

Portable GPS driver supporting multiple GNSS protocols through a unified API.

The driver was developed and tested on STM32 microcontrollers running FreeRTOS, but most modules are hardware-independent and can be adapted to other MCUs or bare-metal projects.

## Features

- Multi-protocol architecture
  - PMTK support
  - PCAS support
  - Easy extension for additional protocols

- Unified API
  - Protocol-independent application interface
  - Function-pointer based driver binding

- NMEA sentence processing
  - RMC parser
  - GSV parser
  - GSA parser
  - TXT parser
  - Sentence routing mechanism

- GPS control functions
  - Start/Stop RMC output
  - Start/Stop GSV output
  - Baud-rate configuration
  - Hot start
  - Cold start
  - Restart
  - Standby mode
  - Firmware/version request

- UART communication
  - DMA-based reception
  - Ring-buffer support
  - Idle-line detection

- FreeRTOS integration
  - Task notifications
  - Queue-based communication
  - Mutex-protected UART access

## Architecture

```text
Application
     │
     ▼
gps.c
     │
     ├──── PMTK Driver
     │
     ├──── PCAS Driver
     │
     └──── NMEA Parser
                 │
                 ├──── RMC
                 ├──── GSV
                 ├──── GSA
                 └──── TXT
```

The application communicates only with the generic GPS API.

Protocol-specific details are hidden behind a function table, allowing the active GPS protocol to be selected during initialization.

## Directory Structure

```text
gps.c
gps.h

gps_types.h
gps_internal.h

gps_data.c
gps_data.h

gps_nmea.c
gps_nmea.h

gps_pcasN.c
gps_pcasN.h

gps_pmtkN.c
gps_pmtkN.h

uart_transport.c
uart_transport.h

uartRingBufDMA.c
uartRingBufDMA.h

example.c
```

## Supported NMEA Sentences

| Sentence | Description |
|-----------|-------------|
| RMC | Position, speed, date and time |
| GSV | Satellites in view |
| GSA | Fix type and DOP |
| TXT | Firmware and module information |

## Example

```c
GPS_Init();

GPS_StartRMC();
GPS_StartGSV();

GPS_GetVersion();
```

## Notes

- UART ring-buffer implementation is based on the Controllerstech ring-buffer project and was modified to fit this architecture.
- Tested with:
  - Dalang DL18Z6A
  - Zhongke Micro AT6558R
  - STM32F4 series
  - FreeRTOS

## Future Work

- Additional GNSS protocol support
- Event-driven sentence dispatching
- GPX recording integration
- Distance and navigation utilities
