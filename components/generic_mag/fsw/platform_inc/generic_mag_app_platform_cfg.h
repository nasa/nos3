/************************************************************************
** File:
**  generic_mag_app_platform_cfg.h
**
** Purpose: 
**  Define Generic_mag App platform configuration parameters
**
** Notes:
**
**
*************************************************************************/
#ifndef _GENERIC_MAG_APP_PLATFORM_CFG_H_
#define _GENERIC_MAG_APP_PLATFORM_CFG_H_

#define GENERIC_MAG_UART_BUFFER_SIZE 1024

#define GENERIC_MAG_DEVICE_NAME       "GENERIC_MAG_DEVICE"
/* Note: Generic_mag child task stack and priority get defined here. Main app is specified in cfe_es_startup.scr */
#define GENERIC_MAG_DEVICE_CHILD_STACK_SIZE  2048
#define GENERIC_MAG_DEVICE_CHILD_PRIORITY    100
#define GENERIC_MAG_DEVICE_MUTEX_NAME "GENERIC_MAG_DEVICE_MUTEX"

/*
** Default Generic_mag Configuration
** Note: In practice this configuration should be specified in the mission_defs/cpuX_device_cfg.h
**       These parameters are for the I/O to the device and can change per platform (NOS3 vs. flight vs. flatsat)
**       The definitions here will only be used if GENERIC_MAG_CFG is not in the cpuX_device_cfg.h 
*/
#ifndef GENERIC_MAG_CFG
    /* Note: NOS3 uart requires matching handle and bus number */
    #define GENERIC_MAG_CFG_STRING           "usart_29"
    #define GENERIC_MAG_CFG_HANDLE           29 
    #define GENERIC_MAG_CFG_BAUDRATE_HZ      115200
    #define GENERIC_MAG_CFG_MS_TIMEOUT       250
#endif

#endif /* _GENERIC_MAG_PLATFORM_CFG_H_ */

/************************/
/*  End of File Comment */
/************************/
