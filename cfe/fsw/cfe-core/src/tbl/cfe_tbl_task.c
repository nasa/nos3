/*
** $Id: cfe_tbl_task.c 1.5 2012/01/18 16:32:58GMT-05:00 jmdagost Exp  $
**
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement.
**  
**
** Subsystem: cFE TBL Task
**
** Author: David Kobe (the Hammers Company, Inc.)
**
** Notes:
**
** $Log: cfe_tbl_task.c  $
** Revision 1.5 2012/01/18 16:32:58GMT-05:00 jmdagost 
** Updated init event msg to include cFE version numbers.
** Revision 1.4 2012/01/13 12:17:41EST acudmore 
** Changed license text to reflect open source
** Revision 1.3 2010/10/04 17:05:21EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.2 2008/07/31 15:41:30EDT apcudmore 
** Added execution counter API:
**   -- Added execution counter to ES internal task data
**   -- Added code to increment counter in RunLoop
**   -- Added code to report counter in GetInfo APIs
**   -- Added API to increment counter for child tasks
**   -- Added code to cFE Core apps to increment counters.
** Revision 1.1 2008/04/17 08:05:35EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/tbl/project.pj
** Revision 1.21 2007/05/15 10:58:04EDT rjmcgraw 
** Exit main loop for CFE_SB_RcvMsg errors
** Revision 1.20 2007/05/09 12:49:49EDT dlkobe 
** Added SysLog message to indicate SB error on Command Pipe
** Revision 1.19 2007/05/04 15:56:22EDT dlkobe 
** Added Delete CDS command
** Revision 1.18 2007/04/30 14:13:44EDT rjmcgraw 
** Standardized task initialization
** Revision 1.17 2007/04/09 14:45:59EDT rjmcgraw 
** Changed names of performance ids to include CFE_
** Revision 1.16 2006/10/31 12:21:43EST dlkobe 
** Removed all references to the TBL_MOD_DUMP_ONLY option command
** Revision 1.15 2006/06/21 12:23:22GMT-05:00 kkaudra 
** Removed cfe_es_perf.h
**
*/


/*
** Required header files
*/
#include "private/cfe_private.h"
#include "cfe_version.h"
#include "cfe_tbl_internal.h"
#include "cfe_tbl_events.h"
#include "cfe_tbl_msg.h"
#include "cfe_tbl_task_cmds.h"
#include "cfe_tbl_verify.h"
#include <string.h>


/*
** Table task global data
*/
CFE_TBL_TaskData_t    CFE_TBL_TaskData;


/* Constant Data */

const CFE_TBL_CmdHandlerTblRec_t CFE_TBL_CmdHandlerTbl[] = {
/*        Message ID,    Command Code,                     Msg Size,            Function Ptr,  Msg/Cmd/Terminator */
{CFE_TBL_SEND_HK_MID,                      0,  sizeof(CFE_TBL_NoArgsCmd_t),CFE_TBL_HousekeepingCmd,CFE_TBL_MSG_MSGTYPE},
{    CFE_TBL_CMD_MID,        CFE_TBL_NOOP_CC,  sizeof(CFE_TBL_NoArgsCmd_t),        CFE_TBL_NoopCmd,CFE_TBL_CMD_MSGTYPE},
{    CFE_TBL_CMD_MID,       CFE_TBL_RESET_CC,  sizeof(CFE_TBL_NoArgsCmd_t),       CFE_TBL_ResetCmd,CFE_TBL_CMD_MSGTYPE},
{    CFE_TBL_CMD_MID,        CFE_TBL_LOAD_CC,    sizeof(CFE_TBL_LoadCmd_t),        CFE_TBL_LoadCmd,CFE_TBL_CMD_MSGTYPE},
{    CFE_TBL_CMD_MID,        CFE_TBL_DUMP_CC,    sizeof(CFE_TBL_DumpCmd_t),        CFE_TBL_DumpCmd,CFE_TBL_CMD_MSGTYPE},
{    CFE_TBL_CMD_MID,    CFE_TBL_VALIDATE_CC,sizeof(CFE_TBL_ValidateCmd_t),    CFE_TBL_ValidateCmd,CFE_TBL_CMD_MSGTYPE},
{    CFE_TBL_CMD_MID,    CFE_TBL_ACTIVATE_CC,sizeof(CFE_TBL_ActivateCmd_t),    CFE_TBL_ActivateCmd,CFE_TBL_CMD_MSGTYPE},
{    CFE_TBL_CMD_MID,    CFE_TBL_DUMP_REG_CC, sizeof(CFE_TBL_DumpRegCmd_t),     CFE_TBL_DumpRegCmd,CFE_TBL_CMD_MSGTYPE},
{    CFE_TBL_CMD_MID,     CFE_TBL_TLM_REG_CC,  sizeof(CFE_TBL_TlmRegCmd_t),      CFE_TBL_TlmRegCmd,CFE_TBL_CMD_MSGTYPE},
{    CFE_TBL_CMD_MID,  CFE_TBL_DELETE_CDS_CC,  sizeof(CFE_TBL_DelCDSCmd_t),   CFE_TBL_DeleteCDSCmd,CFE_TBL_CMD_MSGTYPE},
{    CFE_TBL_CMD_MID,  CFE_TBL_ABORT_LOAD_CC, sizeof(CFE_TBL_AbortLdCmd_t),   CFE_TBL_AbortLoadCmd,CFE_TBL_CMD_MSGTYPE},
{                  0,                      0,                            0,                   NULL,CFE_TBL_TERM_MSGTYPE}
};


/******************************************************************************/

void CFE_TBL_TaskMain(void)
{
    int32  Status;

    CFE_ES_PerfLogEntry(CFE_TBL_MAIN_PERF_ID);

    Status = CFE_TBL_TaskInit();
    
    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TBL:Application Init Failed,RC=0x%08X\n", (unsigned int)Status);
      CFE_ES_PerfLogExit(CFE_TBL_MAIN_PERF_ID);      
      /* Note: CFE_ES_ExitApp will not return */
      CFE_ES_ExitApp(CFE_ES_RUNSTATUS_CORE_APP_INIT_ERROR);
    }/* end if */

    /*
     * Wait for other apps to start.
     * It is important that the core apps are present before this starts receiving
     * messages from the command pipe, as some of those handlers might depend on
     * the other core apps.
     */
    CFE_ES_WaitForStartupSync(CFE_CORE_MAX_STARTUP_MSEC);

    /* Main loop */
    while (Status == CFE_SUCCESS)
    {
        /* Increment the Main task Execution Counter */
        CFE_ES_IncrementTaskCounter();

        CFE_ES_PerfLogExit(CFE_TBL_MAIN_PERF_ID);

        /* Pend on receipt of packet */
        Status = CFE_SB_RcvMsg( &CFE_TBL_TaskData.MsgPtr,
                                CFE_TBL_TaskData.CmdPipe,
                                CFE_SB_PEND_FOREVER);

        CFE_ES_PerfLogEntry(CFE_TBL_MAIN_PERF_ID);

        if (Status == CFE_SUCCESS)
        {
            /* Process cmd pipe msg */
            CFE_TBL_TaskPipe(CFE_TBL_TaskData.MsgPtr);
        }else{
            CFE_ES_WriteToSysLog("TBL:Error reading cmd pipe,RC=0x%08X\n",(unsigned int)Status);
        }/* end if */

    }/* end while */

    /* while loop exits only if CFE_SB_RcvMsg returns error */
    CFE_ES_ExitApp(CFE_ES_RUNSTATUS_CORE_APP_RUNTIME_ERROR);

} /* end CFE_TBL_TaskMain() */


/******************************************************************************/

int32 CFE_TBL_TaskInit(void)
{
    int32 Status = CFE_SUCCESS;

    /*
    ** Register Table Services with ES
    */
    Status = CFE_ES_RegisterApp();

    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TBL:Call to CFE_ES_RegisterApp Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */
    
    /*
    ** Initialize global Table Services data
    */
    CFE_TBL_InitData();

    /*
    ** Register event filter table
    */
    Status = CFE_EVS_Register(NULL, 0, 0);

    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TBL:Call to CFE_EVS_Register Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */
    
    /*
    ** Create Software Bus message pipe
    */
    Status = CFE_SB_CreatePipe(&CFE_TBL_TaskData.CmdPipe,
                                CFE_TBL_TaskData.PipeDepth,
                                CFE_TBL_TaskData.PipeName);
    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TBL:Error creating cmd pipe:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */                                                                

    /*
    ** Subscribe to Housekeeping request commands
    */
    Status = CFE_SB_Subscribe(CFE_TBL_SEND_HK_MID, CFE_TBL_TaskData.CmdPipe);

    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TBL:Error subscribing to HK Request:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */

    /*
    ** Subscribe to Table task ground command packets
    */
    Status = CFE_SB_Subscribe(CFE_TBL_CMD_MID, CFE_TBL_TaskData.CmdPipe);

    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TBL:Error subscribing to gnd cmds:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */
    
    /*
    ** Task startup event message
    */
    Status = CFE_EVS_SendEvent(CFE_TBL_INIT_INF_EID, CFE_EVS_INFORMATION, "cFE TBL Initialized.  cFE Version %d.%d.%d.%d",
                               CFE_MAJOR_VERSION,CFE_MINOR_VERSION,CFE_REVISION,CFE_MISSION_REV);

    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TBL:Error sending init event:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */

    return CFE_SUCCESS;

} /* End of CFE_TBL_TaskInit() */


/******************************************************************************/

void CFE_TBL_InitData(void)
{
    /* Initialize Counters */
    CFE_TBL_TaskData.CmdCounter = 0;
    CFE_TBL_TaskData.ErrCounter = 0;
    CFE_TBL_TaskData.SuccessValCtr = 0;
    CFE_TBL_TaskData.FailedValCtr = 0;

    /* Get the assigned Application ID for the Table Services Task */
    CFE_ES_GetAppID(&CFE_TBL_TaskData.TableTaskAppId);

    /* Initialize Command Pipe Parameters */
    CFE_TBL_TaskData.PipeDepth = CFE_TBL_TASK_PIPE_DEPTH;
    strncpy(CFE_TBL_TaskData.PipeName, CFE_TBL_TASK_PIPE_NAME, 16);

    /* Initialize Packet Headers */
    CFE_SB_InitMsg(&CFE_TBL_TaskData.HkPacket,
                   CFE_TBL_HK_TLM_MID,
                   sizeof(CFE_TBL_HkPacket_t),
                   TRUE);

    CFE_SB_InitMsg(&CFE_TBL_TaskData.TblRegPacket,
                   CFE_TBL_REG_TLM_MID,
                   sizeof(CFE_TBL_TblRegPacket_t),
                   TRUE);

} /* End of CFE_TBL_InitData() */


/******************************************************************************/

void CFE_TBL_TaskPipe(CFE_SB_Msg_t *MessagePtr)
{
    CFE_SB_MsgId_t       MessageID = CFE_SB_GetMsgId(MessagePtr);
    uint16               CommandCode = CFE_SB_GetCmdCode(MessagePtr);
    int16                CmdIndx;
    uint32               ActualLength;
    CFE_TBL_CmdProcRet_t CmdStatus = CFE_TBL_INC_ERR_CTR; /* Assume a failed command */

    /* Search the Command Handler Table for a matching message */
    CmdIndx = CFE_TBL_SearchCmdHndlrTbl(MessageID, CommandCode);

    /* Check to see if a matching command was found */
    if (CmdIndx >= 0)
    {
        /* Verify Message Length before processing */
        ActualLength = CFE_SB_GetTotalMsgLength(MessagePtr);
        if (ActualLength == CFE_TBL_CmdHandlerTbl[CmdIndx].ExpectedLength)
        {
            /* All checks have passed, call the appropriate message handler */
            CmdStatus = (CFE_TBL_CmdHandlerTbl[CmdIndx].MsgProcFuncPtr)(&MessagePtr->Byte[CFE_SB_CMD_HDR_SIZE]);
        }
        else /* Bad Message Length */
        {
            CFE_EVS_SendEvent( CFE_TBL_LEN_ERR_EID, CFE_EVS_ERROR,
                               "Invalid msg length -- ID = 0x%04X, CC = %d, Len = %d (!= %d)",
                               (unsigned int)MessageID, (int)CommandCode, (int)ActualLength,
                               (int)CFE_TBL_CmdHandlerTbl[CmdIndx].ExpectedLength );
        }

        /* Only update command counters when message has a command code */
        if (CFE_TBL_CmdHandlerTbl[CmdIndx].MsgTypes == CFE_TBL_CMD_MSGTYPE)
        {
            if (CmdStatus == CFE_TBL_INC_CMD_CTR)
            {
                CFE_TBL_TaskData.CmdCounter++;
            }
            else if (CmdStatus == CFE_TBL_INC_ERR_CTR)
            {
                CFE_TBL_TaskData.ErrCounter++;
            }
        }
    }
    else
    {
        /* Determine whether event message should be */
        /* "Bad Command Code" or "Bad Message ID"    */
        if (CmdIndx == CFE_TBL_BAD_CMD_CODE)
        {
            CFE_EVS_SendEvent(CFE_TBL_CC1_ERR_EID, CFE_EVS_ERROR,
                              "Invalid command code -- ID = 0x%04X, CC = %d",
                              (unsigned int)MessageID, (int)CommandCode);

            /* Update the command error counter */
            CFE_TBL_TaskData.ErrCounter++;
        }
        else  /* CmdIndx == CFE_TBL_BAD_MSG_ID */
        {
            CFE_EVS_SendEvent(CFE_TBL_MID_ERR_EID, CFE_EVS_ERROR,
                             "Invalid message ID -- ID = 0x%04X",
                              (unsigned int)MessageID);
            /*
            ** Note: we only increment the command error counter when
            **    processing messages with command codes
            */
        }
    }

    return;

} /* End of CFE_TBL_TaskPipe() */


/******************************************************************************/

int16 CFE_TBL_SearchCmdHndlrTbl( CFE_SB_MsgId_t MessageID, uint16 CommandCode )
{
    int16     TblIndx = CFE_TBL_BAD_CMD_CODE;
    boolean   FoundMsg = FALSE;
    boolean   FoundMatch = FALSE;

    do
    {
        /* Point to next entry in Command Handler Table */
        TblIndx++;

        /* Check to see if we found a matching Message ID */
        if ((CFE_TBL_CmdHandlerTbl[TblIndx].MsgId == MessageID) &&
            (CFE_TBL_CmdHandlerTbl[TblIndx].MsgTypes != CFE_TBL_TERM_MSGTYPE))
        {
            /* Flag any found message IDs so that if there is an error,        */
            /* we can determine if it was a bad message ID or bad command code */
            FoundMsg = TRUE;

            /* If entry in the Command Handler Table is a command entry, */
            /* then check for a matching command code                    */
            if (CFE_TBL_CmdHandlerTbl[TblIndx].MsgTypes == CFE_TBL_CMD_MSGTYPE)
            {
                if (CFE_TBL_CmdHandlerTbl[TblIndx].CmdCode == CommandCode)
                {
                    /* Found matching message ID and Command Code */
                    FoundMatch = TRUE;
                }
            }
            else /* Message is not a command message with specific command code */
            {
                /* Automatically assume a match when legit */
                /* Message ID is all that is required      */
                FoundMatch = TRUE;
            }
        }
    } while ((!FoundMatch) && (CFE_TBL_CmdHandlerTbl[TblIndx].MsgTypes != CFE_TBL_TERM_MSGTYPE));

    /* If we failed to find a match, return a negative index */
    if (!FoundMatch)
    {
        /* Determine if the message ID was bad or the command code */
        if (FoundMsg)
        {
            /* A matching message ID was found, so the command code must be bad */
            TblIndx = CFE_TBL_BAD_CMD_CODE;
        }
        else /* No matching message ID was found */
        {
            TblIndx = CFE_TBL_BAD_MSG_ID;
        }
    }

    return TblIndx;
}   /* End of CFE_TBL_SearchCmdHndlrTbl() */


/************************/
/*  End of File Comment */
/************************/

