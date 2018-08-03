/************************************************************************
** File:
**   $Id: hk_app.c 1.17 2015/03/04 14:58:32EST sstrege Exp  $
**
**  Copyright © 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** Purpose:
**  The CFS Housekeeping (HK) Application file containing the application
**  initialization routines, the main routine and the command interface.
**
** Notes:
**
** $Log: hk_app.c  $
** Revision 1.17 2015/03/04 14:58:32EST sstrege 
** Added copyright information
** Revision 1.16 2012/08/23 17:12:31EDT aschoeni 
** Made internal commands no longer increment error counters (and changed event message slightly).
** Revision 1.15 2011/11/30 16:03:05EST jmdagost 
** Added new-line character to syslog call.
** Revision 1.14 2011/06/23 11:57:54EDT jmdagost 
** Removed error counter increment in housekeeping request command error (per guidelines)
** Revision 1.13 2010/06/09 12:00:49EDT jmdagost 
** Update subscription error messages in HK Init.
** Revision 1.12 2009/12/03 18:11:47EST jmdagost 
** Modified event message arguments to use updated event definitions.
** Revision 1.11 2009/12/03 15:24:57EST jmdagost 
** Modified the error logic to include an else-block that processes the message.  Also added a call to syslog.
** Revision 1.10 2009/12/03 15:14:49EST jmdagost 
** Replaced event message call with a call to syslog as requested.
** Revision 1.9 2008/09/17 15:52:18EDT rjmcgraw 
** DCR4040:1 Added spaces to align text
** Revision 1.8 2008/09/11 11:42:43EDT rjmcgraw 
** DCR4041:1 Added hk_version.h and hk_platform_cfg.h to #include list
** Revision 1.7 2008/08/08 14:34:58EDT rjmcgraw 
** DCR4209:1 Added #include hk_verify.h
** Revision 1.6 2008/07/16 10:01:12EDT rjmcgraw 
** DCR4042:1 Displaying app version number in init event and no-op event
** Revision 1.5 2008/06/19 13:21:35EDT rjmcgraw
** DCR3052:1 Call to CheckStatusOfTables instead of HandleUpdateToCopyTable 
**   during HK Req
** Revision 1.4 2008/05/15 09:33:25EDT rjmcgraw
** DCR1647:1 Fixed problem with zeroed out telemetry
** Revision 1.3 2008/05/07 10:00:07EDT rjmcgraw
** DCR1647:4 Removed the _CMD from HK_SEND_HK_CMD_MID
** Revision 1.2 2008/05/02 12:50:57EDT rjmcgraw
** DCR1647:1 Added #include hk_perfids.h
** Revision 1.1 2008/04/09 16:41:22EDT rjmcgraw
** Initial revision
** Member added to project cfs/hk/fsw/src/project.pj
**
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/
#include "hk_app.h"
#include "hk_events.h"
#include "hk_msgids.h"
#include "hk_perfids.h"
#include "hk_verify.h"
#include "hk_version.h"
#include "hk_platform_cfg.h"
#include <string.h>


/************************************************************************
** HK global data
*************************************************************************/
HK_AppData_t        HK_AppData;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HK application entry point and main process loop                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HK_AppMain(void)
{
   int32 Status;

   /*
   ** Register the Application with Executive Services
   */
   CFE_ES_RegisterApp();

   /*
   ** Create the first Performance Log entry
   */
   CFE_ES_PerfLogEntry(HK_APPMAIN_PERF_ID);


   /* Perform Application Initialization */
   Status = HK_AppInit();
   if (Status != CFE_SUCCESS)
   {
      HK_AppData.RunStatus = CFE_ES_APP_ERROR;
   }

   /*
   ** Application Main Loop.
   */
   while(CFE_ES_RunLoop(&HK_AppData.RunStatus) == TRUE)
   {
      /*
      ** Performance Log Exit Stamp.
      */
      CFE_ES_PerfLogExit(HK_APPMAIN_PERF_ID);

      /*
      ** Pend on the arrival of the next Software Bus message.
      */
      Status = CFE_SB_RcvMsg(&HK_AppData.MsgPtr,HK_AppData.CmdPipe,CFE_SB_PEND_FOREVER);

      if(Status != CFE_SUCCESS)
      {
        CFE_EVS_SendEvent(HK_RCV_MSG_ERR_EID, CFE_EVS_ERROR,
               "HK_APP Exiting due to CFE_SB_RcvMsg error 0x%08X", Status);

        /* Write to syslog in case there is a problem with event services */
        CFE_ES_WriteToSysLog("HK_APP Exiting due to CFE_SB_RcvMsg error 0x%08X\n", Status);

        HK_AppData.RunStatus = CFE_ES_APP_ERROR;
      }
      else  /* Success */
      {
        /*
        ** Performance Log Entry Stamp.
        */
        CFE_ES_PerfLogEntry(HK_APPMAIN_PERF_ID);


        /* Perform Message Processing */
        HK_AppPipe(HK_AppData.MsgPtr);
      }
   } /* end while */

   /*
    ** Performance Log Exit Stamp.
    */
    CFE_ES_PerfLogExit(HK_APPMAIN_PERF_ID);

    /*
    ** Exit the Application.
    */
    CFE_ES_ExitApp(HK_AppData.RunStatus);

} /* End of HK_AppMain() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HK application initialization routine                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 HK_AppInit(void)
{
    int32       Status = CFE_SUCCESS;

    HK_AppData.RunStatus = CFE_ES_APP_RUN;

    /* Initialize housekeeping packet  */
    CFE_SB_InitMsg(&HK_AppData.HkPacket,HK_HK_TLM_MID,sizeof(HK_HkPacket_t),TRUE);

    /* Register for event services...        */
    Status = CFE_EVS_Register (NULL, 0, CFE_EVS_BINARY_FILTER);
    if (Status != CFE_SUCCESS)
    {
       CFE_ES_WriteToSysLog("HK: error registering for event services: 0x%08X\n", Status);
       return (Status);
    }


    /* Create HK Command Pipe */
    Status = CFE_SB_CreatePipe (&HK_AppData.CmdPipe,HK_PIPE_DEPTH,HK_PIPE_NAME);
    if (Status != CFE_SUCCESS)
    {
      CFE_EVS_SendEvent(HK_CR_PIPE_ERR_EID, CFE_EVS_ERROR,
            "Error Creating SB Pipe,RC=0x%08X",Status);
       return (Status);
    }

    /* Subscribe to 'Send Combined HK Pkt' Command */
    Status = CFE_SB_Subscribe(HK_SEND_COMBINED_PKT_MID,HK_AppData.CmdPipe);
    if (Status != CFE_SUCCESS)
    {
      CFE_EVS_SendEvent(HK_SUB_CMB_ERR_EID, CFE_EVS_ERROR,
            "Error Subscribing to HK Snd Cmb Pkt, MID=0x%04X, RC=0x%08X",
              HK_SEND_COMBINED_PKT_MID, Status);
        return (Status);
     }

    /* Subscribe to Housekeeping Request */
    Status = CFE_SB_Subscribe(HK_SEND_HK_MID,HK_AppData.CmdPipe);
    if (Status != CFE_SUCCESS)
    {
      CFE_EVS_SendEvent(HK_SUB_REQ_ERR_EID, CFE_EVS_ERROR,
            "Error Subscribing to HK Request, MID=0x%04X, RC=0x%08X",
            HK_SEND_HK_MID, Status);
        return (Status);
     }

    /* Subscribe to HK ground commands */
    Status = CFE_SB_Subscribe(HK_CMD_MID,HK_AppData.CmdPipe);
    if (Status != CFE_SUCCESS)
    {
      CFE_EVS_SendEvent(HK_SUB_CMD_ERR_EID, CFE_EVS_ERROR,
            "Error Subscribing to HK Gnd Cmds, MID=0x%04X, RC=0x%08X",
            HK_CMD_MID, Status);
        return (Status);
     }


    /* Create a memory pool for combined output messages */
    Status = CFE_ES_PoolCreate (&HK_AppData.MemPoolHandle,
                                 HK_AppData.MemPoolBuffer,
                                 sizeof (HK_AppData.MemPoolBuffer) );
    if (Status != CFE_SUCCESS)
    {
      CFE_EVS_SendEvent(HK_CR_POOL_ERR_EID, CFE_EVS_ERROR,
            "Error Creating Memory Pool,RC=0x%08X",Status);
        return (Status);
     }

    HK_ResetHkData ();


    /* Register The HK Tables */
    Status = HK_TableInit();
    if(Status != CFE_SUCCESS)
    {
        /* Specific failure is detailed in function HK_TableInit */
      return (Status);
    }


    /* Application initialization event */
    Status = CFE_EVS_SendEvent (HK_INIT_EID, CFE_EVS_INFORMATION,
               "HK Initialized.  Version %d.%d.%d.%d",
                HK_MAJOR_VERSION,
                HK_MINOR_VERSION, 
                HK_REVISION, 
                HK_MISSION_REV);               

    if (Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog(
         "HK App:Error Sending Initialization Event,RC=0x%08X\n", Status);
     }


    return (Status);

} /* End of HK_AppInit() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HK application table initialization routine                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 HK_TableInit (void)
{
    int32       Status = CFE_SUCCESS;

    /* Register The HK Copy Table */
    Status = CFE_TBL_Register (&HK_AppData.CopyTableHandle,
                                HK_COPY_TABLE_NAME,
                                (sizeof (hk_copy_table_entry_t) * HK_COPY_TABLE_ENTRIES),
                                CFE_TBL_OPT_DBL_BUFFER | CFE_TBL_OPT_LOAD_DUMP,
                                HK_ValidateHkCopyTable);

    if (Status != CFE_SUCCESS)
    {
      CFE_EVS_SendEvent(HK_CPTBL_REG_ERR_EID, CFE_EVS_ERROR,
            "Error Registering Copy Table,RC=0x%08X",Status);
        return (Status);
     }


    /* Register The HK Runtime Table */
    Status = CFE_TBL_Register(&HK_AppData.RuntimeTableHandle,
                              HK_RUNTIME_TABLE_NAME,
                              (sizeof (hk_runtime_tbl_entry_t) * HK_COPY_TABLE_ENTRIES),
                              CFE_TBL_OPT_SNGL_BUFFER | CFE_TBL_OPT_DUMP_ONLY,
                              NULL);
    if (Status != CFE_SUCCESS)
    {
      CFE_EVS_SendEvent(HK_RTTBL_REG_ERR_EID, CFE_EVS_ERROR,
            "Error Registering Runtime Table,RC=0x%08X",Status);
        return (Status);
    }


    Status = CFE_TBL_Load (HK_AppData.CopyTableHandle,
                           CFE_TBL_SRC_FILE,
                           HK_COPY_TABLE_FILENAME);
    if (Status != CFE_SUCCESS)
    {
      CFE_EVS_SendEvent(HK_CPTBL_LD_ERR_EID, CFE_EVS_ERROR,
            "Error Loading Copy Table,RC=0x%08X",Status);
        return (Status);
     }


    Status = CFE_TBL_Manage (HK_AppData.CopyTableHandle);
    if (Status != CFE_SUCCESS)
    {
      CFE_EVS_SendEvent(HK_CPTBL_MNG_ERR_EID, CFE_EVS_ERROR,
            "Error from TBL Manage call for Copy Table,RC=0x%08X",Status);
        return (Status);
     }


    Status = CFE_TBL_Manage (HK_AppData.RuntimeTableHandle);
    if (Status != CFE_SUCCESS)
    {
      CFE_EVS_SendEvent(HK_RTTBL_MNG_ERR_EID, CFE_EVS_ERROR,
            "Error from TBL Manage call for Runtime Table,RC=0x%08X",Status);
        return (Status);
     }


    Status = CFE_TBL_GetAddress ( (void *) (& HK_AppData.CopyTablePtr),
                                      HK_AppData.CopyTableHandle);
    /* Status should be CFE_TBL_INFO_UPDATED because we loaded it above */
    if (Status != CFE_TBL_INFO_UPDATED)
    {
      CFE_EVS_SendEvent(HK_CPTBL_GADR_ERR_EID, CFE_EVS_ERROR,
            "Error Getting Adr for Cpy Tbl,RC=0x%08X",Status);
        return (Status);
     }


    Status = CFE_TBL_GetAddress ( (void *) (& HK_AppData.RuntimeTablePtr),
                                  HK_AppData.RuntimeTableHandle);
    if (Status != CFE_SUCCESS)
    {
      CFE_EVS_SendEvent(HK_RTTBL_GADR_ERR_EID, CFE_EVS_ERROR,
         "Error Getting Adr for Runtime Table,RC=0x%08X",Status);
        return (Status);
     }

    HK_ProcessNewCopyTable(HK_AppData.CopyTablePtr,HK_AppData.RuntimeTablePtr);

    return CFE_SUCCESS;


}   /* HK_TableInit */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Process a command pipe message                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HK_AppPipe (CFE_SB_MsgPtr_t MessagePtr)
{
    CFE_SB_MsgId_t  MessageID;
    uint16          CommandCode;

    MessageID = CFE_SB_GetMsgId (MessagePtr);
    switch (MessageID)
    {

        case HK_SEND_COMBINED_PKT_MID:
            if (CFE_SB_GetTotalMsgLength(MessagePtr) != sizeof(HK_Send_Out_Msg_t))
            {
                CFE_EVS_SendEvent( HK_MSG_LEN_ERR_EID, CFE_EVS_ERROR,
                                   "Msg with Bad length Rcvd: ID = 0x%04X, Exp Len = %d, Len = %d",
                                   MessageID,  
                                   sizeof(HK_Send_Out_Msg_t), 
                                   CFE_SB_GetTotalMsgLength(MessagePtr));
            }
            else
            {
                HK_SendCombinedHKCmd(MessagePtr);
            }
            break;

        /* Request for HK's Housekeeping data...      */
        case HK_SEND_HK_MID:
            if (CFE_SB_GetTotalMsgLength(MessagePtr) != CFE_SB_CMD_HDR_SIZE)
            {
                CFE_EVS_SendEvent( HK_MSG_LEN_ERR_EID, CFE_EVS_ERROR,
                                   "Msg with Bad length Rcvd: ID = 0x%04X, Exp Len = %d, Len = %d",
                                   MessageID,  
                                   CFE_SB_CMD_HDR_SIZE, 
                                   CFE_SB_GetTotalMsgLength(MessagePtr));
            }
            else
            {
                /* Send out HK's housekeeping data */
                HK_HousekeepingCmd(MessagePtr);
            }
            /* Check for copy table load and runtime dump request */
            HK_CheckStatusOfTables();
            break;

        /* HK ground commands   */
        case HK_CMD_MID:

            CommandCode = CFE_SB_GetCmdCode(MessagePtr);
            switch (CommandCode)
            {
                case HK_NOOP_CC:
                    HK_NoopCmd (MessagePtr);
                    break;

                case HK_RESET_CC:
                    HK_ResetCtrsCmd (MessagePtr);
                    break;

                default:
                    CFE_EVS_SendEvent(HK_CC_ERR_EID, CFE_EVS_ERROR,
                    "Cmd Msg with Invalid command code Rcvd -- ID = 0x%04X, CC = %d",
                    MessageID, CommandCode);

                    HK_AppData.ErrCounter++;
                    break;
            }
            break;

        /* Incoming housekeeping data from other Subsystems...       */
        default:

            HK_ProcessIncomingHkData (MessagePtr);
            break;
    }

    return;

} /* End of HK_AppPipe() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Send Combined Housekeeping Packet                               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HK_SendCombinedHKCmd (CFE_SB_MsgPtr_t MessagePtr)
{
    CFE_SB_MsgId_t  WhichCombinedPacket;

    WhichCombinedPacket = *((uint16 *)CFE_SB_GetUserData(MessagePtr));
    HK_SendCombinedHkPacket (WhichCombinedPacket);

    return;

} /* end of HK_SendCombinedHKCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Housekeeping request                                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HK_HousekeepingCmd (CFE_SB_MsgPtr_t MessagePtr)
{

    /* copy data into housekeeping packet */
    HK_AppData.HkPacket.CmdCounter          = HK_AppData.CmdCounter;
    HK_AppData.HkPacket.ErrCounter          = HK_AppData.ErrCounter;
    HK_AppData.HkPacket.MissingDataCtr      = HK_AppData.MissingDataCtr;
    HK_AppData.HkPacket.CombinedPacketsSent = HK_AppData.CombinedPacketsSent;
    HK_AppData.HkPacket.MemPoolHandle       = HK_AppData.MemPoolHandle;

    /* Send housekeeping telemetry packet...        */
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &HK_AppData.HkPacket);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &HK_AppData.HkPacket);

    return;

} /* End of HK_HousekeepingCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Noop command                                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HK_NoopCmd (CFE_SB_MsgPtr_t MessagePtr)
{

    if(HK_VerifyCmdLength(MessagePtr,CFE_SB_CMD_HDR_SIZE)==HK_BAD_MSG_LENGTH_RC)
    {

        HK_AppData.ErrCounter++;

    }else{

        CFE_EVS_SendEvent (HK_NOOP_CMD_EID, CFE_EVS_INFORMATION,
            "HK No-op command, Version %d.%d.%d.%d",
             HK_MAJOR_VERSION,
             HK_MINOR_VERSION, 
             HK_REVISION, 
             HK_MISSION_REV);

        HK_AppData.CmdCounter++;
    }

    return;

} /* End of HK_NoopCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset counters command                                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HK_ResetCtrsCmd (CFE_SB_MsgPtr_t MessagePtr)
{
    if(HK_VerifyCmdLength(MessagePtr,CFE_SB_CMD_HDR_SIZE)==HK_BAD_MSG_LENGTH_RC)
    {

        HK_AppData.ErrCounter++;

    }else{

        HK_ResetHkData ();
        CFE_EVS_SendEvent (HK_RESET_CNTRS_CMD_EID, CFE_EVS_DEBUG,
                           "HK Reset Counters command received");
    }

    return;

} /* End of HK_ResetCtrsCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset housekeeping data                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HK_ResetHkData (void)
{
    HK_AppData.CmdCounter = 0;
    HK_AppData.ErrCounter = 0;
    HK_AppData.CombinedPacketsSent = 0;
    HK_AppData.MissingDataCtr      = 0;
    return;

} /* End of HK_ResetHkData () */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Verify Command Length                                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 HK_VerifyCmdLength (CFE_SB_MsgPtr_t MessagePtr,uint32 ExpectedLength)
{
    int32               Status = HK_SUCCESS;
    CFE_SB_MsgId_t      MessageID;
    uint16              CommandCode;
    uint16              ActualLength;

    ActualLength  = CFE_SB_GetTotalMsgLength (MessagePtr);

    if (ExpectedLength != ActualLength)
    {

        MessageID   = CFE_SB_GetMsgId   (MessagePtr);
        CommandCode = CFE_SB_GetCmdCode (MessagePtr);

        CFE_EVS_SendEvent(HK_CMD_LEN_ERR_EID, CFE_EVS_ERROR,
          "Cmd Msg with Bad length Rcvd: ID = 0x%X, CC = %d, Exp Len = %d, Len = %d",
          MessageID, CommandCode, ExpectedLength, ActualLength);

        Status = HK_BAD_MSG_LENGTH_RC;

    }

    return Status;

} /* End of HK_VerifyCmdLength () */


/************************/
/*  End of File Comment */
/************************/
