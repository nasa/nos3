/*
** File:     tt_app.h
**
** Purpose:  cFE Application "template" (TT) header file.
**           This is a model of an application which
**           uses cFE Table Services.
**
** Author:
**
** Notes:
**
** $Log: tt_app.h  $
** Revision 1.1 2008/04/17 07:09:11GMT-05:00 ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/examples/test2/project.pj
** Revision 1.2 2006/08/01 08:45:25EDT wfmoleski 
** Changes to upgrade this example to cFE 3.3.0
** Revision 1.1 2006/03/21 16:44:13EST sslegel 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/examples/test2/project.pj
** Modifications
** 2005/09/19 nschweiss Initial version based on qq_app.h template.
*/

/*************************************************************************/

/*
** Ensure that header is included only once...
*/
#ifndef _tt_app_
#define _tt_app_


/*
** Required header files...
*/

#include "cfe.h"


/*
** Event message ID's...
*/
#define TT_INIT_INF_EID    1    /* start up message "informational" */

#define TT_NOOP_INF_EID    2    /* processed command "informational" */
#define TT_RESET_INF_EID   3

#define TT_MID_ERR_EID     4    /* invalid command packet "error" */
#define TT_CC1_ERR_EID     5
#define TT_LEN_ERR_EID     6

#define TT_EVT_COUNT       6    /* count of event message ID's */


/*
** TT command packet command codes...
*/
#define TT_NOOP_CC         0    /* no-op command */
#define TT_RESET_CC        1    /* reset counters */

/*************************************************************************/

/*
** Type definition (generic "no arguments" command)...
*/
typedef struct
{
  uint8                 CmdHeader[CFE_SB_CMD_HDR_SIZE];

} TT_NoArgsCmd_t;

/*************************************************************************/

/*
** Type definition (TT housekeeping)...
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

} TT_HkPacket_t;

/*************************************************************************/

/*
** Type definition (TT app global data)...
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
  TT_HkPacket_t         HkPacket;

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

  CFE_EVS_BinFilter_t   EventFilters[TT_EVT_COUNT];
  
  CFE_TBL_Handle_t      TblHandle;

} TT_AppData_t;

/*************************************************************************/

/*
** Local function prototypes...
**
** Note: Except for the entry point (TT_AppMain), these
**       functions are not called from any other source module.
*/
void TT_AppMain(void);
void TT_AppInit(void);
void TT_AppPipe(CFE_SB_MsgPtr_t msg);

void TT_HousekeepingCmd(CFE_SB_MsgPtr_t msg);

void TT_NoopCmd(CFE_SB_MsgPtr_t msg);
void TT_ResetCmd(CFE_SB_MsgPtr_t msg);

boolean TT_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);

/*************************************************************************/

#endif /* _tt_app_ */

/************************/
/*  End of File Comment */
/************************/
