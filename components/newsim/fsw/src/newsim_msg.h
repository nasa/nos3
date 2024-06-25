/*******************************************************************************
** File:
**   newsim_msg.h
**
** Purpose:
**  Define NEWSIM application commands and telemetry messages
**
*******************************************************************************/
#ifndef _NEWSIM_MSG_H_
#define _NEWSIM_MSG_H_

#include "cfe.h"
#include "newsim_device.h"


/*
** Ground Command Codes
** TODO: Add additional commands required by the specific component
*/
#define NEWSIM_NOOP_CC                 0
#define NEWSIM_RESET_COUNTERS_CC       1
#define NEWSIM_ENABLE_CC               2
#define NEWSIM_DISABLE_CC              3
#define NEWSIM_CONFIG_CC               4


/* 
** Telemetry Request Command Codes
** TODO: Add additional commands required by the specific component
*/
#define NEWSIM_REQ_HK_TLM              0
#define NEWSIM_REQ_DATA_TLM            1


/*
** Generic "no arguments" command type definition
*/
typedef struct
{
    /* Every command requires a header used to identify it */
    CFE_MSG_CommandHeader_t CmdHeader;

} NEWSIM_NoArgs_cmd_t;


/*
** NEWSIM write configuration command
*/
typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader;
    uint32   DeviceCfg;

} NEWSIM_Config_cmd_t;


/*
** NEWSIM device telemetry definition
*/
typedef struct 
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    NEWSIM_Device_Data_tlm_t Newsim;

} __attribute__((packed)) NEWSIM_Device_tlm_t;
#define NEWSIM_DEVICE_TLM_LNGTH sizeof ( NEWSIM_Device_tlm_t )


/*
** NEWSIM housekeeping type definition
*/
typedef struct 
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    uint8   CommandErrorCount;
    uint8   CommandCount;
    uint8   DeviceErrorCount;
    uint8   DeviceCount;
  
    /*
    ** TODO: Edit and add specific telemetry values to this struct
    */
    uint8   DeviceEnabled;
    NEWSIM_Device_HK_tlm_t DeviceHK;

} __attribute__((packed)) NEWSIM_Hk_tlm_t;
#define NEWSIM_HK_TLM_LNGTH sizeof ( NEWSIM_Hk_tlm_t )

#endif /* _NEWSIM_MSG_H_ */
