/*******************************************************************************
** File:
**  generic_mag_device_msg.h
**
** Purpose:
**  Define Generic_mag Device Messages and info
**
** Notes:
**
**
*******************************************************************************/
#ifndef _GENERIC_MAG_DEVICE_MSG_H_
#define _GENERIC_MAG_DEVICE_MSG_H_

#include "osapi.h" // for types used below
#include "cfe_sb.h" // for CFE_SB_TLM_HDR_SIZE

/*************************************************************************/
/*
** Type definition (GENERIC_MAG Device housekeeping)
*/

typedef struct
{
    uint8 CommandErrorCounter;
    uint8 GetDataCmdCounter;
    uint8 CfgCmdCounter;
    uint8 OtherCmdCounter;
    uint8 RawCmdCounter;
} OS_PACK GENERIC_MAG_DeviceCmdData_t;

typedef struct
{
    uint8 CfgRespCounter;
    uint8 OtherRespCounter;
    uint8 RawRespCounter;
    uint32 UnknownResponseCounter;
    uint32 DeviceGeneric_magDataCounter;
    uint32 MillisecondStreamDelay;
} OS_PACK GENERIC_MAG_DeviceRespHkData_t;

typedef struct
{
    GENERIC_MAG_DeviceCmdData_t    GENERIC_MAG_DeviceCmdData;
    GENERIC_MAG_DeviceRespHkData_t GENERIC_MAG_DeviceRespHkData;
} OS_PACK GENERIC_MAG_DeviceHkTlm_Payload_t;

typedef struct
{
    uint8                        TlmHeader[CFE_SB_TLM_HDR_SIZE];
    GENERIC_MAG_DeviceHkTlm_Payload_t Payload;

} OS_PACK GENERIC_MAG_DeviceHkTlm_t;

/*************************************************************************/
/*
** Type definition (GENERIC_MAG Device Data)
*/

typedef struct
{
    uint32 Generic_magProcessedTimeSeconds;
    uint32 Generic_magProcessedTimeSubseconds;
    uint32 Generic_magsSent;
    uint16 Generic_magDataX;
    uint16 Generic_magDataY;
    uint16 Generic_magDataZ;
} OS_PACK GENERIC_MAG_DeviceRespGeneric_magData_t;

typedef struct
{
    GENERIC_MAG_DeviceRespGeneric_magData_t GENERIC_MAG_DeviceRespGeneric_magData;
} OS_PACK GENERIC_MAG_Generic_magTlm_Payload_t;

typedef struct
{
    uint8                      TlmHeader[CFE_SB_TLM_HDR_SIZE];
    GENERIC_MAG_Generic_magTlm_Payload_t Payload;

} OS_PACK GENERIC_MAG_DeviceGeneric_magTlm_t;

#endif /* _GENERIC_MAG_DEVICE_MSG_H_ */

/************************/
/*  End of File Comment */
/************************/
