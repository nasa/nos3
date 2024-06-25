/*******************************************************************************
** File: newsim_device.h
**
** Purpose:
**   This is the header file for the NEWSIM device.
**
*******************************************************************************/
#ifndef _NEWSIM_DEVICE_H_
#define _NEWSIM_DEVICE_H_

/*
** Required header files.
*/
#include "device_cfg.h"
#include "hwlib.h"
#include "newsim_platform_cfg.h"


/*
** Type definitions
** TODO: Make specific to your application
*/
#define NEWSIM_DEVICE_HDR              0xDEAD
#define NEWSIM_DEVICE_HDR_0            0xDE
#define NEWSIM_DEVICE_HDR_1            0xAD

#define NEWSIM_DEVICE_NOOP_CMD         0x00
#define NEWSIM_DEVICE_REQ_HK_CMD       0x01
#define NEWSIM_DEVICE_REQ_DATA_CMD     0x02
#define NEWSIM_DEVICE_CFG_CMD          0x03

#define NEWSIM_DEVICE_TRAILER          0xBEEF
#define NEWSIM_DEVICE_TRAILER_0        0xBE
#define NEWSIM_DEVICE_TRAILER_1        0xEF

#define NEWSIM_DEVICE_HDR_TRL_LEN      4
#define NEWSIM_DEVICE_CMD_SIZE         9

/*
** NEWSIM device housekeeping telemetry definition
*/
typedef struct
{
    uint32_t  DeviceCounter;
    uint32_t  DeviceConfig;
    uint32_t  DeviceStatus;

} __attribute__((packed)) NEWSIM_Device_HK_tlm_t;
#define NEWSIM_DEVICE_HK_LNGTH sizeof ( NEWSIM_Device_HK_tlm_t )
#define NEWSIM_DEVICE_HK_SIZE NEWSIM_DEVICE_HK_LNGTH + NEWSIM_DEVICE_HDR_TRL_LEN


/*
** NEWSIM device data telemetry definition
*/
typedef struct
{
    uint32_t  DeviceCounter;
    uint16_t  DeviceDataX;
    uint16_t  DeviceDataY;
    uint16_t  DeviceDataZ;

} __attribute__((packed)) NEWSIM_Device_Data_tlm_t;
#define NEWSIM_DEVICE_DATA_LNGTH sizeof ( NEWSIM_Device_Data_tlm_t )
#define NEWSIM_DEVICE_DATA_SIZE NEWSIM_DEVICE_DATA_LNGTH + NEWSIM_DEVICE_HDR_TRL_LEN


/*
** Prototypes
*/
int32_t NEWSIM_ReadData(uart_info_t* device, uint8_t* read_data, uint8_t data_length);
int32_t NEWSIM_CommandDevice(uart_info_t* device, uint8_t cmd, uint32_t payload);
int32_t NEWSIM_RequestHK(uart_info_t* device, NEWSIM_Device_HK_tlm_t* data);
int32_t NEWSIM_RequestData(uart_info_t* device, NEWSIM_Device_Data_tlm_t* data);


#endif /* _NEWSIM_DEVICE_H_ */
