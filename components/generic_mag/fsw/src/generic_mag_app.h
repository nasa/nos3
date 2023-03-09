/*******************************************************************************
** File: 
**  generic_mag_app.h
**
** Purpose:
**   This file is main header file for the Generic_mag application.
**
*******************************************************************************/
#ifndef _GENERIC_MAG_APP_H_
#define _GENERIC_MAG_APP_H_

/*
** Required header files.
*/
#include "generic_mag_app_msg.h"
#include "generic_mag_app_events.h"
#include "cfe_sb.h"
#include "cfe_evs.h"

/***********************************************************************/
#define GENERIC_MAG_PIPE_DEPTH 32 /* Depth of the Command Pipe for Application */

/************************************************************************
** Type Definitions
*************************************************************************/

/*
 * Buffer to hold telemetry data prior to sending
 * Defined as a union to ensure proper alignment for a CFE_SB_Msg_t type
 */
typedef union
{
    CFE_SB_Msg_t   MsgHdr;
    GENERIC_MAG_HkTlm_t HkTlm;
} GENERIC_MAG_HkBuffer_t;

/*
** Global Data
*/
typedef struct
{
    /*
    ** Housekeeping telemetry packet...
    */
    GENERIC_MAG_HkBuffer_t HkBuf;

    /*
    ** Operational data (not reported in housekeeping)...
    */
    CFE_SB_PipeId_t CommandPipe;
    CFE_SB_MsgPtr_t MsgPtr;

    /*
    ** Initialization data (not reported in housekeeping)...
    */
    char   PipeName[16];
    uint16 PipeDepth;

    CFE_EVS_BinFilter_t EventFilters[GENERIC_MAG_EVENT_COUNTS];

} GENERIC_MAG_AppData_t;

/****************************************************************************/
/*
** Function prototypes.
**
** Note: Except for the entry point (GENERIC_MAG_AppMain), these
**       functions are not called from any other source module.
*/
void  GENERIC_MAG_AppMain(void);

#endif /* _generic_mag_app_h_ */

/************************/
/*  End of File Comment */
/************************/
