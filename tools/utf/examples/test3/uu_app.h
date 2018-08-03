/*
** File:     uu_app.h
**
** Purpose:  cFE Application sample (UU) header file.
**
** Author:
**
** Notes:
**
** $Log: uu_app.h  $
** Revision 1.1 2008/04/17 07:09:14GMT-05:00 ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/examples/test3/project.pj
** Revision 1.2 2006/08/01 08:46:51EDT wfmoleski 
** Changes to upgrade this example to cFE 3.3.0
** Revision 1.1 2006/03/21 16:44:16EST sslegel 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/examples/test3/project.pj
** Modifications
** 2005/09/19 nschweiss Initial version based on qq_app.h template.
** 2005/11/01 nschweiss Changed to uu_app.h to support setting return
**           code test.
*/

/*************************************************************************/

/*
** Ensure that header is included only once...
*/
#ifndef _uu_app_
#define _uu_app_


/*
** Required header files...
*/
#include "cfe.h"

/* #include "app_msgids.h" */
#include "cfe_msgids.h"
#include "cfe_tbl.h"
/*************************************************************************/

/*
** Event message ID's...
*/
#define UU_INIT_INF_EID    1    /* start up message "informational" */

#define UU_NOOP_INF_EID    2    /* processed command "informational" */
#define UU_RESET_INF_EID   3

#define UU_MID_ERR_EID     4    /* invalid command packet "error" */
#define UU_CC1_ERR_EID     5
#define UU_LEN_ERR_EID     6

#define UU_EVT_COUNT       6    /* count of event message ID's */

/*
** UU command packet command codes...
*/
#define UU_NOOP_CC         0    /* no-op command */
#define UU_RESET_CC        1    /* reset counters */

/*************************************************************************/

/*
** Type definition (generic "no arguments" command)...
*/
typedef struct
{
  uint8                 CmdHeader[CFE_SB_CMD_HDR_SIZE];

} UU_NoArgsCmd_t;

/*************************************************************************/

/*
** Type definition (UU housekeeping)...
*/
typedef struct
{
  uint8                 TlmHeader[CFE_SB_TLM_HDR_SIZE];

  /*
  ** Command interface counters...
  */
  uint8                 CmdCount;
  uint8                 ErrCount;
  uint16				   UpdateCount;

} UU_HkPacket_t;

/*************************************************************************/

/*
** Type definition (UU app global data)...
*/
typedef struct
{
  /*
  ** Command interface counters...
  */
  uint8                 CmdCount;
  uint8                 ErrCount;

  /*
  ** Housekeeping telemetry packet...
  */
  UU_HkPacket_t         HkPacket;

  /*
  ** Operational data (not reported in housekeeping)...
  */
  CFE_SB_MsgPtr_t       MsgPtr;
  CFE_SB_PipeId_t       CmdPipe;

  /*
  ** Initialization data (not reported in housekeeping)...
  */
  char                  PipeName[16];
  uint16                PipeDepth;

  uint8                 LimitHK;
  uint8                 LimitCmd;

  CFE_EVS_BinFilter_t   EventFilters[UU_EVT_COUNT];
  
  CFE_TBL_Handle_t      TblHandle;

} UU_AppData_t;

/*************************************************************************/

/*
** Local function prototypes...
**
** Note: Except for the entry point (UU_AppMain), these
**       functions are not called from any other source module.
*/
void UU_AppMain(void);
void UU_AppInit(void);
void UU_AppPipe(CFE_SB_MsgPtr_t msg);

void UU_HousekeepingCmd(CFE_SB_MsgPtr_t msg);

void UU_NoopCmd(CFE_SB_MsgPtr_t msg);
void UU_ResetCmd(CFE_SB_MsgPtr_t msg);

boolean UU_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);

/*************************************************************************/

#endif /* _uu_app_ */

/************************/
/*  End of File Comment */
/************************/
