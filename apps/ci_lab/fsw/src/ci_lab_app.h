/*******************************************************************************
** File: ci_lab_app.h
**
** Purpose:
**   This file is main hdr file for the Command Ingest lab application.
**
** $Log: ci_lab_app.h  $
** Revision 1.5 2010/11/17 08:13:27GMT-05:00 wmoleski 
** The TO_Lab_APP, CI_Lab_APP and SCH_Lab_APP did not compile with cFE6.1.0. An include statement of cfe_error.h needed to be added to each app.
** Revision 1.4 2010/09/02 09:53:55EDT wmoleski 
** Modifications to the CI_Lab code to capture, drop and/or manipulate CFDP PDUs that are uplinked
** from the ground. These changes were needed to test the CF fault detection requirements.
** Revision 1.3 2008/09/22 13:58:24EDT apcudmore 
** Added RunLoop call to CI_LAB app. Also added the task delete handler to close the CI socket.
** Revision 1.2 2008/04/30 15:24:32EDT rjmcgraw 
** DCR1718:1 Added version number in initialization event
** Revision 1.1 2008/04/30 13:56:18EDT rjmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/ci_lab/fsw/src/project.pj
**
*******************************************************************************/

#ifndef _ci_lab_app_h_
#define _ci_lab_app_h_

/*
** Required header files...
*/
#include "network_includes.h"
#include "common_types.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include "osapi.h"
#include "ccsds.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>

/****************************************************************************/

#define cfgCI_PORT                      1234
#define CI_MAX_INGEST                    768
#define CI_PIPE_DEPTH                     32

/************************************************************************
** Type Definitions
*************************************************************************/
typedef struct
{
    uint8   Octet1;
    uint16  PDataLen;
    uint8   Octet4;
    uint16  SrcEntityId;
    uint32  TransSeqNum;
    uint16  DstEntityId;

}OS_PACK CF_PDU_Hdr_t;

/****************************************************************************/
/*
** Local function prototypes...
**
** Note: Except for the entry point (CI_task_main), these
**       functions are not called from any other source module.
*/
void CI_task_main(void);
void CI_TaskInit(void);
void CI_ProcessCommandPacket(void);
void CI_ProcessGroundCommand(void);
void CI_ReportHousekeeping(void);
void CI_ResetCounters(void);
void CI_ModifyFileSizeCmd(CFE_SB_MsgPtr_t msg);
void CI_CorruptChecksumCmd(CFE_SB_MsgPtr_t msg);
void CI_DropPDUCmd(CFE_SB_MsgPtr_t msg);
void CI_CapturePDUsCmd(CFE_SB_MsgPtr_t msg);
void CI_StopPDUCaptureCmd(CFE_SB_MsgPtr_t msg);
void CI_ProcessPDU(void);
void CI_ReadUpLink(void);

boolean CI_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);

#endif /* _ci_lab_app_h_ */
