/*******************************************************************************
** File:
**  generic_mag_device.h
**
** Purpose:
**   This file is the header file for the Generic_mag device
**
**
*******************************************************************************/

#ifndef _GENERIC_MAG_DEVICE_H_
#define _GENERIC_MAG_DEVICE_H_

#include "generic_mag_device_msg.h"

/*
 * Buffers to hold telemetry data prior to sending
 * Defined as a union to ensure proper alignment for a CFE_SB_Msg_t type
 */

typedef union
{
    CFE_SB_Msg_t         MsgHdr;
    GENERIC_MAG_DeviceHkTlm_t HkTlm;
} GENERIC_MAG_DeviceHkBuffer_t;

typedef union
{
    CFE_SB_Msg_t             MsgHdr;
    GENERIC_MAG_DeviceGeneric_magTlm_t Generic_magTlm;
} GENERIC_MAG_DeviceGeneric_magBuffer_t;

/*
** Run Status variable used in the main processing loop.  If the device is asynchronous, this Status
** variable is also used in the device child processing loop.
*/
extern uint32 RunStatus;

/****************************************************************************/
/*
** Function prototypes.
**
*/
int32 GENERIC_MAG_DeviceInit(void);
int32 GENERIC_MAG_DeviceShutdown(void);
void  GENERIC_MAG_DeviceResetCounters(void);

void  GENERIC_MAG_DeviceGetGeneric_magDataCommand(void);
void  GENERIC_MAG_DeviceConfigurationCommand(uint32_t millisecond_stream_delay);
void  GENERIC_MAG_DeviceOtherCommand(void);
void  GENERIC_MAG_DeviceRawCommand(const uint8 cmd[], const uint32_t cmd_length);

void GENERIC_MAG_ReportDeviceHousekeeping(void);
void GENERIC_MAG_ReportDeviceGeneric_magData(void);

#endif

/************************/
/*  End of File Comment */
/************************/
