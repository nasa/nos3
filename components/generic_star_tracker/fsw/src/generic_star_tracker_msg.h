/*******************************************************************************
** File:
**   generic_star_tracker_msg.h
**
** Purpose:
**  Define GENERIC_STAR_TRACKER application commands and telemetry messages
**
*******************************************************************************/
#ifndef _GENERIC_STAR_TRACKER_MSG_H_
#define _GENERIC_STAR_TRACKER_MSG_H_

#include "cfe.h"
#include "generic_star_tracker_device.h"


/*
** Ground Command Codes
** TODO: Add additional commands required by the specific component
*/
#define GENERIC_STAR_TRACKER_NOOP_CC                 0
#define GENERIC_STAR_TRACKER_RESET_COUNTERS_CC       1
#define GENERIC_STAR_TRACKER_ENABLE_CC               2
#define GENERIC_STAR_TRACKER_DISABLE_CC              3
#define GENERIC_STAR_TRACKER_CONFIG_CC               4


/* 
** Telemetry Request Command Codes
** TODO: Add additional commands required by the specific component
*/
#define GENERIC_STAR_TRACKER_REQ_HK_TLM              0
#define GENERIC_STAR_TRACKER_REQ_DATA_TLM            1


/*
** Generic "no arguments" command type definition
*/
typedef struct
{
    /* Every command requires a header used to identify it */
    CFE_MSG_CommandHeader_t CmdHeader;

} GENERIC_STAR_TRACKER_NoArgs_cmd_t;


/*
** GENERIC_STAR_TRACKER write configuration command
*/
typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader;
    uint32   DeviceCfg;

} GENERIC_STAR_TRACKER_Config_cmd_t;


/*
** GENERIC_STAR_TRACKER device telemetry definition
*/
typedef struct 
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    GENERIC_STAR_TRACKER_Device_Data_tlm_t Generic_star_tracker;

} __attribute__((packed)) GENERIC_STAR_TRACKER_Device_tlm_t;
#define GENERIC_STAR_TRACKER_DEVICE_TLM_LNGTH sizeof ( GENERIC_STAR_TRACKER_Device_tlm_t )


/*
** GENERIC_STAR_TRACKER housekeeping type definition
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
    GENERIC_STAR_TRACKER_Device_HK_tlm_t DeviceHK;

} __attribute__((packed)) GENERIC_STAR_TRACKER_Hk_tlm_t;
#define GENERIC_STAR_TRACKER_HK_TLM_LNGTH sizeof ( GENERIC_STAR_TRACKER_Hk_tlm_t )

#endif /* _GENERIC_STAR_TRACKER_MSG_H_ */
