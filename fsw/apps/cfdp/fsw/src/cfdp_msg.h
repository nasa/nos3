/*******************************************************************************
** File:
**   cfdp_msg.h
**
** Purpose:
**  Define CFDP application commands and telemetry messages
**
*******************************************************************************/
#ifndef _CFDP_MSG_H_
#define _CFDP_MSG_H_

#include "cfe.h"
#include "cfdp_cfdp.h"

/* 
** Telemetry Request Command Codes
** TODO: Add additional commands required by the specific component
*/
#define CFDP_REQ_HK_TLM              0
#define CFDP_REQ_DATA_TLM            1
#define CFDP_FILENAME_MAX_PATH      64
#define CFDP_CMD_MAX_LENGTH		   128

typedef struct 
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CFDP_SendHkCmd_t;

typedef struct
{
    /* Every command requires a header used to identify it */
    uint8    CmdHeader[sizeof(CFE_MSG_CommandHeader_t)];

} CFDP_NoArgs_cmd_t;



/*
** CFDP housekeeping type definition
*/
typedef struct 
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    uint8   cmdErrorCount;
    uint8   cmdCount;

} __attribute__((packed)) CFDP_Hk_tlm_t;
#define CFDP_HK_TLM_LNGTH sizeof ( CFDP_Hk_tlm_t )

typedef struct 
{
	CFE_MSG_TelemetryHeader_t TlmHeader;
	uint8_t pdu_type;
	uint8_t direction;
	char buffer[CFDP_CHUNK_SIZE*16]; //4096
	char destination[CFDP_FILENAME_MAX_PATH];
	char source[CFDP_FILENAME_MAX_PATH];
} CFDP_CFDP_PDU_t;
#define CFDP_CFDP_PDU_TLM_LNGTH sizeof ( CFDP_CFDP_PDU_t )

typedef struct 
{
	CFE_MSG_CommandHeader_t CmdHeader;
	char Destination[CFDP_FILENAME_MAX_PATH];
	char Source[CFDP_FILENAME_MAX_PATH];
} CFDP_FileTxSatellite_t;

typedef struct 
{
	CFE_MSG_CommandHeader_t CmdHeader;
	uint16_t length;
	uint8_t pdu_type;
	uint8_t padding;
	char destination[CFDP_FILENAME_MAX_PATH];
	char buffer[CFDP_CHUNK_SIZE*2];
} CFDP_FileSatellite_t;

#endif /* _CFDP_MSG_H_ */
