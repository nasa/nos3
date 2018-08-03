/*******************************************************************************
** File: ci_lab_app.c
**
** Purpose:
**   This file contains the source code for the Command Ingest task.
**
** $Log: ci_lab_app.c  $
** Revision 1.7 2010/09/20 12:27:19GMT-05:00 wmoleski 
** Modified the CI_LAB, SCH_LAB and TO_LAB applications to use unique message IDs and Pipe Names. The "_LAB"
** was added to all definitions so that a mission can use these "Lab" apps as well as their own mission apps together.
** Revision 1.6 2010/09/02 09:53:55EDT wmoleski 
** Modifications to the CI_Lab code to capture, drop and/or manipulate CFDP PDUs that are uplinked
** from the ground. These changes were needed to test the CF fault detection requirements.
** Revision 1.5 2008/09/22 13:58:20EDT apcudmore 
** Added RunLoop call to CI_LAB app. Also added the task delete handler to close the CI socket.
** Revision 1.4 2008/09/19 15:30:42EDT rjmcgraw 
** DCR4337:1 Added #include version.h and display version after initialization is complete
** Revision 1.3 2008/05/01 11:49:21EDT rjmcgraw 
** DCR1718:1 Changed wording in intiialization event
** Revision 1.2 2008/04/30 15:24:31EDT rjmcgraw 
** DCR1718:1 Added version number in initialization event
** Revision 1.1 2008/04/30 13:56:37EDT rjmcgraw 
** Initial revision
** Member added to CFS project
**
*******************************************************************************/

/*
**   Include Files:
*/

#include "ci_lab_app.h"
#include "ci_lab_perfids.h"
#include "ci_lab_msgids.h"
#include "ci_lab_msg.h"
#include "ci_lab_defs.h"
#include "ci_lab_events.h"
#include "ci_lab_version.h"

/*
** CI global data...
*/

boolean            CI_SocketConnected = FALSE;
ci_hk_tlm_t        CI_HkTelemetryPkt;
CFE_SB_PipeId_t    CI_CommandPipe;
CFE_SB_MsgPtr_t    CIMsgPtr;
int                CI_SocketID;
struct sockaddr_in CI_SocketAddress;
uint8              CI_IngestBuffer[CI_MAX_INGEST];
CFE_SB_Msg_t       *CI_IngestPointer = (CFE_SB_Msg_t *) &CI_IngestBuffer[0];
CFE_SB_MsgId_t     PDUMessageID = 0;
boolean            adjustFileSize = FALSE;
int		   PDUFileSizeAdjustment;
boolean            dropFileData = FALSE;
int		   dropFileDataCnt;
boolean            dropEOF = FALSE;
int		   dropEOFCnt;
boolean            dropFIN = FALSE;
int		   dropFINCnt;
boolean            dropACK = FALSE;
int		   dropACKCnt;
boolean            dropMetaData = FALSE;
int		   dropMetaDataCnt;
boolean            dropNAK = FALSE;
int		   dropNAKCnt;
boolean            corruptChecksum = FALSE;

static CFE_EVS_BinFilter_t  CI_EventFilters[] =
          {  /* Event ID    mask */
            {CI_SOCKETCREATE_ERR_EID,  0x0000},
            {CI_SOCKETBIND_ERR_EID,    0x0000},
            {CI_STARTUP_INF_EID,       0x0000},
            {CI_COMMAND_ERR_EID,       0x0000},
            {CI_COMMANDNOP_INF_EID,    0x0000},
            {CI_COMMANDRST_INF_EID,    0x0000},
            {CI_INGEST_INF_EID,        0x0000},
            {CI_INGEST_ERR_EID,        0x0000}
          };

/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* CI_Lab_AppMain() -- Application entry point and main process loop          */
/* Purpose: This is the Main task event loop for the Command Ingest Task      */
/*            The task handles all interfaces to the data system through      */
/*            the software bus. There is one pipeline into this task          */
/*            The task is sceduled by input into this pipeline.               */
/*            It can receive Commands over this pipeline                      */
/*            and acts accordingly to process them.                           */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
CFS_MODULE_DECLARE_APP(ci_lab, 50, 8192);

void ci_lab_Main( void )
{
    int32  status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    CFE_ES_PerfLogEntry(CI_MAIN_TASK_PERF_ID);

    CI_TaskInit();

    /*
    ** CI Runloop
    */
    while (CFE_ES_RunLoop(&RunStatus) == TRUE)
    {
        CFE_ES_PerfLogExit(CI_MAIN_TASK_PERF_ID);

        /* Pend on receipt of command packet -- timeout set to 500 millisecs */
        status = CFE_SB_RcvMsg(&CIMsgPtr, CI_CommandPipe, 500);
        
        CFE_ES_PerfLogEntry(CI_MAIN_TASK_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            CI_ProcessCommandPacket();
        }

        /* Regardless of packet vs timeout, always process uplink queue      */
        if (CI_SocketConnected)
        {
            CI_ReadUpLink();
        }
    }

    CFE_ES_ExitApp(RunStatus);

} /* End of CI_Lab_AppMain() */

/*
** CI delete callback function.
** This function will be called in the event that the CI app is killed. 
** It will close the network socket for CI
*/
void CI_delete_callback(void)
{
    OS_printf("CI delete callback -- Closing CI Network socket.\n");
    close(CI_SocketID);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* CI_TaskInit() -- CI initialization                                         */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void CI_TaskInit(void)
{
    CFE_ES_RegisterApp() ;

    CFE_EVS_Register(CI_EventFilters,
                     sizeof(CI_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);

    CFE_SB_CreatePipe(&CI_CommandPipe, CI_PIPE_DEPTH,"CI_LAB_CMD_PIPE");
    CFE_SB_Subscribe(CI_LAB_CMD_MID, CI_CommandPipe);
    CFE_SB_Subscribe(CI_LAB_SEND_HK_MID, CI_CommandPipe);

    if ( (CI_SocketID = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        CFE_EVS_SendEvent(CI_SOCKETCREATE_ERR_EID,CFE_EVS_ERROR,"CI: create socket failed = %d", errno);
    }
    else
    {
       memset(&CI_SocketAddress, 0, sizeof(CI_SocketAddress));
       CI_SocketAddress.sin_family      = AF_INET;
       CI_SocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
       CI_SocketAddress.sin_port        = htons(cfgCI_PORT);

       if ( (bind(CI_SocketID, (struct sockaddr *) &CI_SocketAddress, sizeof(CI_SocketAddress)) < 0) )
       {
           CFE_EVS_SendEvent(CI_SOCKETBIND_ERR_EID,CFE_EVS_ERROR,"CI: bind socket failed = %d", errno);
       }
       else
       {
           CI_SocketConnected = TRUE;
           #ifdef _HAVE_FCNTL_
              /*
              ** Set the socket to non-blocking 
              ** This is not available to vxWorks, so it has to be
              ** Conditionally compiled in
              */
              fcntl(CI_SocketID, F_SETFL, O_NONBLOCK);
           #endif
       }
    }

    CI_ResetCounters();

    /*
    ** Install the delete handler
    */
    OS_TaskInstallDeleteHandler(&CI_delete_callback);

    CFE_SB_InitMsg(&CI_HkTelemetryPkt,
                   CI_LAB_HK_TLM_MID,
                   CI_LAB_HK_TLM_LNGTH, TRUE);

				
    CFE_EVS_SendEvent (CI_STARTUP_INF_EID, CFE_EVS_INFORMATION,
               "CI Lab Initialized.  Version %d.%d.%d.%d",
                CI_LAB_MAJOR_VERSION,
                CI_LAB_MINOR_VERSION, 
                CI_LAB_REVISION, 
                CI_LAB_MISSION_REV);


				
} /* End of CI_TaskInit() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  CI_ProcessCommandPacket                                            */
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the CI command*/
/*     pipe. The packets received on the CI command pipe are listed here:     */
/*                                                                            */
/*        1. NOOP command (from ground)                                       */
/*        2. Request to reset telemetry counters (from ground)                */
/*        3. Request for housekeeping telemetry packet (from HS task)         */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void CI_ProcessCommandPacket(void)
{
CFE_SB_MsgId_t  MsgId;
MsgId = CFE_SB_GetMsgId(CIMsgPtr);

    switch (MsgId)
    {
        case CI_LAB_CMD_MID:
            CI_ProcessGroundCommand();
            break;

        case CI_LAB_SEND_HK_MID:
            CI_ReportHousekeeping();
            break;

        default:
            CI_HkTelemetryPkt.ci_command_error_count++;
            CFE_EVS_SendEvent(CI_COMMAND_ERR_EID,CFE_EVS_ERROR,
			"CI: invalid command packet,MID = 0x%x", MsgId);
            break;
    }

    return;

} /* End CI_ProcessCommandPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* CI_ProcessGroundCommand() -- CI ground commands                            */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/

void CI_ProcessGroundCommand(void)
{
uint16 CommandCode;

    CommandCode = CFE_SB_GetCmdCode(CIMsgPtr);

    /* Process "known" CI task ground commands */
    switch (CommandCode)
    {
        case CI_NOOP_CC:
            CI_HkTelemetryPkt.ci_command_count++;
            CFE_EVS_SendEvent(CI_COMMANDNOP_INF_EID,CFE_EVS_INFORMATION,
			"CI: NOOP command");
            break;

        case CI_RESET_COUNTERS_CC:
            CI_ResetCounters();
            break;

        case CI_MODIFY_PDU_FILESIZE_CC:
            CI_ModifyFileSizeCmd(CIMsgPtr);
            break;

        case CI_CORRUPT_PDU_CHECKSUM_CC:
            CI_CorruptChecksumCmd(CIMsgPtr);
            break;

        case CI_DROP_PDUS_CC:
            CI_DropPDUCmd(CIMsgPtr);
            break;

        case CI_CAPTURE_PDUS_CC:
            CI_CapturePDUsCmd(CIMsgPtr);
            break;

        case CI_STOP_PDU_CAPTURE_CC:
            CI_StopPDUCaptureCmd(CIMsgPtr);
            break;

        /* default case already found during FC vs length test */
        default:
            break;
    }


    return;

} /* End of CI_ProcessGroundCommand() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  CI_ReportHousekeeping                                              */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the CI task  */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void CI_ReportHousekeeping(void)
{
    CI_HkTelemetryPkt.SocketConnected = CI_SocketConnected;
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &CI_HkTelemetryPkt);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &CI_HkTelemetryPkt);
    return;

} /* End of CI_ReportHousekeeping() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  CI_ResetCounters                                                   */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void CI_ResetCounters(void)
{
    /* Status of commands processed by CI task */
    CI_HkTelemetryPkt.ci_command_count       = 0;
    CI_HkTelemetryPkt.ci_command_error_count = 0;

    /* Status of packets ingested by CI task */
    CI_HkTelemetryPkt.IngestPackets = 0;
    CI_HkTelemetryPkt.IngestErrors  = 0;

    /* Status of packets ingested by CI task */
    CI_HkTelemetryPkt.FDPdusDropped  = 0;
    CI_HkTelemetryPkt.EOFPdusDropped = 0;
    CI_HkTelemetryPkt.FINPdusDropped = 0;
    CI_HkTelemetryPkt.ACKPdusDropped = 0;
    CI_HkTelemetryPkt.MDPdusDropped  = 0;
    CI_HkTelemetryPkt.NAKPdusDropped = 0;
    CI_HkTelemetryPkt.PDUsCaptured   = 0;

    CFE_EVS_SendEvent(CI_COMMANDRST_INF_EID, CFE_EVS_INFORMATION,
		"CI: RESET command");
    return;

} /* End of CI_ResetCounters() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* CI_ModifyFileSizeCmd() -- task ground command ()                           */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void CI_ModifyFileSizeCmd(CFE_SB_MsgPtr_t msg)
{
uint16 ExpectedLength = sizeof(CI_ModifyFileSizeCmd_t);
CI_ModifyFileSizeCmd_t *CmdPtr;

  /*
   ** Verify command packet length...
  */
  if (CI_VerifyCmdLength(msg, ExpectedLength))
  {
    CmdPtr = ((CI_ModifyFileSizeCmd_t *)msg);

    /* Get the direction to modify */
    if (CmdPtr->Direction == PDU_SIZE_ADD) 
      PDUFileSizeAdjustment = CmdPtr->Amount;
    else
      PDUFileSizeAdjustment = 0 - CmdPtr->Amount;

    /* Set the flag to modify File Size */
    adjustFileSize = TRUE;

    CI_HkTelemetryPkt.ci_command_count++;
    CFE_EVS_SendEvent(CI_MOD_PDU_FILESIZE_CMD_EID, CFE_EVS_DEBUG,
              "CI: Modify PDU File Size\n");
  }

  return;

} /* End of CI_ModifyFileSizeCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* CI_CorruptChecksumCmd() -- task ground command ()                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void CI_CorruptChecksumCmd(CFE_SB_MsgPtr_t msg)
{
uint16 ExpectedLength = sizeof(CI_NoArgsCmd_t);

  /*
   ** Verify command packet length...
  */
  if (CI_VerifyCmdLength(msg, ExpectedLength))
  {
    /* Set the flag to modify File Size */
    corruptChecksum = TRUE;

    CI_HkTelemetryPkt.ci_command_count++;
    CFE_EVS_SendEvent(CI_CORRUPT_CHECKSUM_CMD_EID, CFE_EVS_DEBUG,
              "CI: Corrupt PDU Checksum\n");
  }

  return;

} /* End of CI_CorruptChecksumCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* CI_DropPDUCmd() -- task ground command ()                                  */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void CI_DropPDUCmd(CFE_SB_MsgPtr_t msg)
{
uint16 ExpectedLength = sizeof(CI_DropPDUCmd_t);
CI_DropPDUCmd_t *CmdPtr;

  /*
   ** Verify command packet length...
  */
  if (CI_VerifyCmdLength(msg, ExpectedLength))
  {
    CmdPtr = ((CI_DropPDUCmd_t*)msg);

    /* Get the PDU Type */
    if (CmdPtr->PDUType == FILE_DATA_PDU) 
    {
      dropFileData = TRUE;
      dropFileDataCnt = CmdPtr->PDUsToDrop;
    }
    else if (CmdPtr->PDUType == EOF_PDU) 
    {
      dropEOF = TRUE;
      dropEOFCnt = CmdPtr->PDUsToDrop;
    }
    else if (CmdPtr->PDUType == FIN_PDU) 
    {
      dropFIN = TRUE;
      dropFINCnt = CmdPtr->PDUsToDrop;
    }
    else if (CmdPtr->PDUType == ACK_PDU) 
    {
      dropACK = TRUE;
      dropACKCnt = CmdPtr->PDUsToDrop;
    }
    else if (CmdPtr->PDUType == META_DATA_PDU) 
    {
      dropMetaData = TRUE;
      dropMetaDataCnt = CmdPtr->PDUsToDrop;
    }
    else if (CmdPtr->PDUType == NAK_PDU) 
    {
      dropNAK = TRUE;
      dropNAKCnt = CmdPtr->PDUsToDrop;
    }

    CI_HkTelemetryPkt.ci_command_count++;
    CFE_EVS_SendEvent(CI_DROP_PDU_CMD_EID, CFE_EVS_DEBUG, "CI: Drop PDU\n");
  }

  return;

} /* End of CI_DropPDUCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* CI_CapturePDUsCmd() -- task ground command ()                              */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void CI_CapturePDUsCmd(CFE_SB_MsgPtr_t msg)
{
uint16 ExpectedLength = sizeof(CI_CapturePDUCmd_t);
CI_CapturePDUCmd_t *CmdPtr;

  /*
   ** Verify command packet length...
  */
  if (CI_VerifyCmdLength(msg, ExpectedLength))
  {
    CmdPtr = ((CI_CapturePDUCmd_t *)msg);

    if (CmdPtr->PDUMsgID <= CFE_SB_HIGHEST_VALID_MSGID)
    {
      /* Save the messageID in a global variable */
      PDUMessageID = CmdPtr->PDUMsgID;
      
      CI_HkTelemetryPkt.ci_command_count++;
      CFE_EVS_SendEvent(CI_CAPTUREPDU_CMD_EID, CFE_EVS_DEBUG,
                "CI: PDU Capture initialized for 0x%04X\n",CmdPtr->PDUMsgID);
    }
    else
    {
      CI_HkTelemetryPkt.ci_command_error_count++;
      CFE_EVS_SendEvent(CI_INVALID_MSGID_ERR_EID, CFE_EVS_ERROR,
              "CI: Invalid PDU MsgID: 0x%04x\n",CmdPtr->PDUMsgID);
    }
  }

  return;

} /* End of CI_CapturePDUsCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* CI_StopPDUCaptureCmd() -- task ground command ()                           */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void CI_StopPDUCaptureCmd(CFE_SB_MsgPtr_t msg)
{ 
uint16 ExpectedLength = sizeof(CI_NoArgsCmd_t);

  /*
   ** Verify command packet length...
  */
  if (CI_VerifyCmdLength(msg, ExpectedLength))
  {
    if (PDUMessageID != 0)
    {
      CI_HkTelemetryPkt.ci_command_count++;
      CFE_EVS_SendEvent(CI_STOP_PDUCAPTURE_CMD_EID, CFE_EVS_DEBUG,
                "CI: PDU Capture stopped for 0x%04X\n",PDUMessageID);

      /* Set the global data back to there initial values */
      PDUMessageID = 0;
      adjustFileSize = FALSE;
      PDUFileSizeAdjustment = 0;
      dropFileData = FALSE;
      dropFileDataCnt = 0;
      dropEOF = FALSE;
      dropEOFCnt = 0;
      dropFIN = FALSE;
      dropFINCnt = 0;
      dropACK = FALSE;
      dropACKCnt = 0;
      dropMetaData = FALSE;
      dropMetaDataCnt = 0;
      dropNAK = FALSE;
      dropNAKCnt = 0;
      corruptChecksum = FALSE;
    }
    else
    {
      CI_HkTelemetryPkt.ci_command_error_count++;
      CFE_EVS_SendEvent(CI_NOCAPTURE_ERR_EID, CFE_EVS_ERROR,
              "CI: PDU Capture is not enabled\n");
    }
  }

  return;

} /* End of CI_StopPDUCaptureCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* CI_ProcessPDU() --                                                         */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void CI_ProcessPDU(void)
{
CF_PDU_Hdr_t *PduHdrPtr;
uint8 *PduDataPtr;
uint8 *IncomingPduPtr;
uint8 PduData0;
uint8 EntityIdBytes, TransSeqBytes, PduHdrBytes;
CFE_SB_MsgId_t MessageID = CFE_SB_GetMsgId(CI_IngestPointer);
boolean sendToSB = FALSE;
uint32 *checkSumPtr;
uint32 *fileSizePtr;

  if (MessageID == PDUMessageID)
  {
    IncomingPduPtr = ((uint8 *)CI_IngestPointer);
    if (CFE_TST(MessageID,12) != 0)
    {
      IncomingPduPtr += CFE_SB_CMD_HDR_SIZE;
    }
    else
    {
      IncomingPduPtr += CFE_SB_TLM_HDR_SIZE;
    }

    PduHdrPtr = (CF_PDU_Hdr_t *)IncomingPduPtr;

    /* calculate size of incoming pdu to ensure we don't overflow the buf */
    EntityIdBytes = ((PduHdrPtr->Octet4 >> 4) & 0x07) + 1;
    TransSeqBytes = (PduHdrPtr->Octet4 & 0x07) + 1;
    PduHdrBytes = 4 + (EntityIdBytes * 2) + TransSeqBytes;

/*    OS_printf("CI: Captured PDU with length = %d\n",PduHdrPtr->PDataLen); */

    CI_HkTelemetryPkt.PDUsCaptured++;

    /* Check if a File Data PDU  was rcv'd */
    if (CFE_TST(PduHdrPtr->Octet1,4))
    {
      OS_printf("CI: File Data PDU rcv'd\n");
      if ((dropFileData == TRUE) && (dropFileDataCnt > 0))
      {
	dropFileDataCnt--;
        OS_printf("CI: File Data PDU dropped\n");
        CI_HkTelemetryPkt.FDPdusDropped++;
      }
      else
      {
	sendToSB = TRUE;
	dropFileData = FALSE;
      }
    }
    else
    {
      /* Not a File Data PDU */
      PduDataPtr = (uint8 *)PduHdrPtr + PduHdrBytes;
      PduData0 = *PduDataPtr;

      OS_printf("CI: PDU Data Type = %d\n",PduData0);
      switch (PduData0)
      {
	case 4:
          OS_printf("CI: EOF PDU rcv'd\n");
          if ((dropEOF == TRUE) && (dropEOFCnt > 0))
	  {
	    dropEOFCnt--;
            OS_printf("CI: EOF PDU dropped\n");
	    CI_HkTelemetryPkt.EOFPdusDropped++;
          }
          else
	  {
	    sendToSB = TRUE;
	    dropEOF = FALSE;
          }

	  PduDataPtr += 2;
	  checkSumPtr = (uint32 *)PduDataPtr;
	  fileSizePtr = checkSumPtr + 1;

          if (corruptChecksum == TRUE)
	  {
            OS_printf("CI: good checksum = %x\n",(unsigned int)*checkSumPtr);
	    /* Corrupt the checksum */
	    *checkSumPtr = 0x12345678;
            OS_printf("CI: corrupted checksum = %x\n",(unsigned int)*checkSumPtr);
	    corruptChecksum = FALSE;
	  }

          if (adjustFileSize == TRUE)
	  {
            OS_printf("CI: good file size = %d\n",(int)*fileSizePtr);
	    /* Adjust the file size */
	    *fileSizePtr += PDUFileSizeAdjustment;
            OS_printf("CI: adjusted file size = %d\n",(int)*fileSizePtr);
	    adjustFileSize = FALSE;
	  }

	  break;

	case 5:
          OS_printf("CI: FIN PDU rcv'd\n");
          if ((dropFIN == TRUE) && (dropFINCnt > 0))
	  {
	    dropFINCnt--;
            OS_printf("CI: FIN PDU dropped\n");
	    CI_HkTelemetryPkt.FINPdusDropped++;
          }
          else
	  {
	    sendToSB = TRUE;
	    dropFIN = FALSE;
          }

	  break;

	case 6:
          OS_printf("CI: ACK PDU rcv'd\n");
          if ((dropACK == TRUE) && (dropACKCnt > 0))
	  {
	    dropACKCnt--;
            OS_printf("CI: ACK PDU dropped\n");
	    CI_HkTelemetryPkt.ACKPdusDropped++;
          }
          else
	  {
	    sendToSB = TRUE;
	    dropACK = FALSE;
          }

	  break;

	case 7:
          OS_printf("CI: Meta Data PDU rcv'd\n");
          if ((dropMetaData == TRUE) && (dropMetaDataCnt > 0))
	  {
	    dropMetaDataCnt--;
            OS_printf("CI: Meta Data PDU dropped\n");
	    CI_HkTelemetryPkt.MDPdusDropped++;
          }
          else
	  {
	    sendToSB = TRUE;
	    dropMetaData = FALSE;
          }

	  break;

	case 8:
          OS_printf("CI: NAK PDU rcv'd\n");
          if ((dropNAK == TRUE) && (dropNAKCnt > 0))
	  {
	    dropNAKCnt--;
            OS_printf("CI: NAK PDU dropped\n");
	    CI_HkTelemetryPkt.NAKPdusDropped++;
          }
          else
	  {
	    sendToSB = TRUE;
	    dropNAK = FALSE;
          }

	  break;

	default:
	  break;
      }
    }
  }
  else
  {
    sendToSB = TRUE;
  }

  if (sendToSB == TRUE) 
  {
    CFE_ES_PerfLogEntry(CI_SOCKET_RCV_PERF_ID);
    CI_HkTelemetryPkt.IngestPackets++;
    CFE_SB_SendMsg(CI_IngestPointer);
    CFE_ES_PerfLogExit(CI_SOCKET_RCV_PERF_ID);
  }

  return;

} /* End of CI_ProcessPDU() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* CI_ReadUpLink() --                                                         */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void CI_ReadUpLink(void)
{
    socklen_t addr_len;
    int i;
    int status;

    addr_len = sizeof(CI_SocketAddress);

    memset(&CI_SocketAddress, 0, sizeof(CI_SocketAddress));

    for (i = 0; i <= 10; i++)
    {
        status = recvfrom(CI_SocketID, (char *)&CI_IngestBuffer[0], sizeof(CI_IngestBuffer), MSG_DONTWAIT,
                         (struct sockaddr *) &CI_SocketAddress, &addr_len);

        if ( (status < 0) && (errno == EWOULDBLOCK) )
            break; /* no (more) messages */
        else
        {
            if (status <= CI_MAX_INGEST)
            {
		if (PDUMessageID != 0)
		{
		  CI_ProcessPDU();
		}
                else
		{
		  CFE_ES_PerfLogEntry(CI_SOCKET_RCV_PERF_ID);
                  CI_HkTelemetryPkt.IngestPackets++;
                  CFE_SB_SendMsg(CI_IngestPointer);
                  CFE_ES_PerfLogExit(CI_SOCKET_RCV_PERF_ID);
		}
            }
            else
            {
                CI_HkTelemetryPkt.IngestErrors++;
            }
        }
    }

    return;

} /* End of CI_ReadUpLink() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* CI_VerifyCmdLength() -- Verify command packet length                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
boolean CI_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength)
{     
boolean result = TRUE;
uint16 ActualLength = CFE_SB_GetTotalMsgLength(msg);

    /*
    ** Verify the command packet length...
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_SB_MsgId_t MessageID = CFE_SB_GetMsgId(msg);
        uint16 CommandCode = CFE_SB_GetCmdCode(msg);

        CFE_EVS_SendEvent(CI_LEN_ERR_EID, CFE_EVS_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              MessageID, CommandCode, ActualLength, ExpectedLength);
        result = FALSE;
        CI_HkTelemetryPkt.ci_command_error_count++;
    }

    return(result);

} /* End of CI_VerifyCmdLength() */

