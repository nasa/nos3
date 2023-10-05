/*******************************************************************************
** File: generic_star_tracker_device.h
**
** Purpose:
**   This is the header file for the GENERIC_STAR_TRACKER device.
**
*******************************************************************************/
#ifndef _GENERIC_STAR_TRACKER_DEVICE_H_
#define _GENERIC_STAR_TRACKER_DEVICE_H_

/*
** Required header files.
*/
#include "device_cfg.h"
#include "hwlib.h"
#include "generic_star_tracker_platform_cfg.h"


/*
** Type definitions
** TODO: Make specific to your application
*/
#define GENERIC_STAR_TRACKER_DEVICE_HDR              0xDEAD
#define GENERIC_STAR_TRACKER_DEVICE_HDR_0            0xDE
#define GENERIC_STAR_TRACKER_DEVICE_HDR_1            0xAD

#define GENERIC_STAR_TRACKER_DEVICE_NOOP_CMD         0x00
#define GENERIC_STAR_TRACKER_DEVICE_REQ_HK_CMD       0x01
#define GENERIC_STAR_TRACKER_DEVICE_REQ_DATA_CMD     0x02
#define GENERIC_STAR_TRACKER_DEVICE_CFG_CMD          0x03

#define GENERIC_STAR_TRACKER_DEVICE_TRAILER          0xBEEF
#define GENERIC_STAR_TRACKER_DEVICE_TRAILER_0        0xBE
#define GENERIC_STAR_TRACKER_DEVICE_TRAILER_1        0xEF

#define GENERIC_STAR_TRACKER_DEVICE_HDR_TRL_LEN      4
#define GENERIC_STAR_TRACKER_DEVICE_CMD_SIZE         9

/*
** GENERIC_STAR_TRACKER device housekeeping telemetry definition
*/
typedef struct
{
    uint32_t  DeviceCounter;
    uint32_t  DeviceConfig;
    uint32_t  DeviceStatus;

} __attribute__((packed)) GENERIC_STAR_TRACKER_Device_HK_tlm_t;
#define GENERIC_STAR_TRACKER_DEVICE_HK_LNGTH sizeof ( GENERIC_STAR_TRACKER_Device_HK_tlm_t )
#define GENERIC_STAR_TRACKER_DEVICE_HK_SIZE GENERIC_STAR_TRACKER_DEVICE_HK_LNGTH + GENERIC_STAR_TRACKER_DEVICE_HDR_TRL_LEN


/*
** GENERIC_STAR_TRACKER device data telemetry definition
*/
typedef struct
{
    uint32_t  DeviceCounter;
    uint16_t  DeviceDataX;
    uint16_t  DeviceDataY;
    uint16_t  DeviceDataZ;

} __attribute__((packed)) GENERIC_STAR_TRACKER_Device_Data_tlm_t;
#define GENERIC_STAR_TRACKER_DEVICE_DATA_LNGTH sizeof ( GENERIC_STAR_TRACKER_Device_Data_tlm_t )
#define GENERIC_STAR_TRACKER_DEVICE_DATA_SIZE GENERIC_STAR_TRACKER_DEVICE_DATA_LNGTH + GENERIC_STAR_TRACKER_DEVICE_HDR_TRL_LEN


/*
** Prototypes
*/
int32_t GENERIC_STAR_TRACKER_ReadData(uart_info_t* device, uint8_t* read_data, uint8_t data_length);
int32_t GENERIC_STAR_TRACKER_CommandDevice(uart_info_t* device, uint8_t cmd, uint32_t payload);
int32_t GENERIC_STAR_TRACKER_RequestHK(uart_info_t* device, GENERIC_STAR_TRACKER_Device_HK_tlm_t* data);
int32_t GENERIC_STAR_TRACKER_RequestData(uart_info_t* device, GENERIC_STAR_TRACKER_Device_Data_tlm_t* data);


#endif /* _GENERIC_STAR_TRACKER_DEVICE_H_ */
