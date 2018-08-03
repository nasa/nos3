/************************************************************************
** File:
**   $Id: cf_cmds.c 1.27.1.1 2015/03/06 15:30:46EST sstrege Exp  $
**
**   Copyright © 2007-2014 United States Government as represented by the 
**   Administrator of the National Aeronautics and Space Administration. 
**   All Other Rights Reserved.  
**
**   This software was created at NASA's Goddard Space Flight Center.
**   This software is governed by the NASA Open Source Agreement and may be 
**   used, distributed and modified only pursuant to the terms of that 
**   agreement.
**
** Purpose:
**  The CFS CF Application file containing the command processing.
**
** Notes:
**
** $Log: cf_cmds.c  $
** Revision 1.27.1.1 2015/03/06 15:30:46EST sstrege 
** Added copyright information
** Revision 1.27 2011/05/19 15:32:06EDT rmcgraw 
** DCR15033:1 Add auto suspend processing
** Revision 1.26 2011/05/17 16:25:05EDT rmcgraw 
** Removed double comma in event id 57
** Revision 1.25 2011/05/17 16:19:50EDT rmcgraw 
** Removed newline (\n) in events
** Revision 1.24 2011/05/17 15:52:47EDT rmcgraw 
** DCR14967:5 Message ptr made consistent across all cmds.
** Revision 1.23 2011/05/17 09:23:47EDT rmcgraw 
** DCR14529:1 Added processing for GiveTake Cmd
** Revision 1.22 2011/05/10 16:34:38EDT rmcgraw 
** DCR13321:1 Added Dequeue Enable to telemetry for each channel
** Revision 1.21 2011/05/10 15:50:38EDT rmcgraw 
** DCR14527:1 Added semaphore value to each channels telemetry
** Revision 1.20 2011/05/10 10:44:05EDT rmcgraw 
** DCR14967:2 Added channel param check in write queue cmd
** Revision 1.19 2011/05/09 11:52:15EDT rmcgraw 
** DCR13317:1 Allow Destintaion path to be blank
** Revision 1.18 2011/05/09 10:40:01EDT rmcgraw 
** DCR14678:1 Fixed termination string chk on value param in set mib cmd
** Revision 1.17 2011/05/03 16:47:22EDT rmcgraw 
** Cleaned up events, removed \n from some events, removed event id ganging
** Revision 1.16 2011/04/29 14:29:19EDT rmcgraw 
** Reset Transactions Abandon counter in Reset Ctrs Cmd
** Revision 1.15 2011/04/28 15:01:17EDT rmcgraw 
** DCR13258:1 Initailized QIndex to eliminate warning
** Revision 1.14 2011/04/28 14:56:57EDT rmcgraw 
** Added ifdef wrapper and extern CF_AppData_t CF_AppData
** Revision 1.13 2011/03/14 15:30:15EDT rmcgraw 
** DCR14582:1 Removed QuickStatus buffer, WhichCmdBuf
** Revision 1.12 2010/11/04 13:05:24EDT rmcgraw 
** DCR13051:1 Changed #elif to #else
** Revision 1.11 2010/11/04 12:57:34EDT rmcgraw 
** DCR13051:1 Added DebugCompiledIn to cfg packet
** Revision 1.10 2010/11/04 11:37:48EDT rmcgraw 
** Dcr13051:1 Wrap OS_printfs in platform cfg CF_DEBUG
** Revision 1.9 2010/11/01 16:09:34EDT rmcgraw 
** DCR12802:1 Changes for decoupling peer entity id from channel
** Revision 1.8 2010/10/25 11:21:52EDT rmcgraw 
** DCR12573:1 Changes to allow more than one incoming PDU MsgId
** Revision 1.7 2010/10/20 14:55:55EDT rmcgraw 
** DCR12825:1 Change quick stat cmd to show active and suspended status
** Revision 1.6 2010/10/20 13:42:07EDT rmcgraw 
** Dcr12803:1 Added telemetry point to show low memory mark
** Revision 1.5 2010/10/20 11:12:23EDT rmcgraw 
** DCR12576:1 Increment cmd counter for kickstart cmd
** Revision 1.4 2010/08/06 18:45:59EDT rmcgraw 
** Dcr11510:1 Fixed cfg params with buffer sizes
** Revision 1.3 2010/08/04 15:17:39EDT rmcgraw 
** DCR11510:1 Changes prior to release
** Revision 1.2 2010/07/20 14:37:43EDT rmcgraw 
** Dcr11510:1 Remove Downlink buffer references
** Revision 1.1 2010/07/08 13:06:53EDT rmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cf/fsw/src/project.pj
**
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/
#include "cf_cmds.h"
#include "cf_utils.h"
#include "cf_events.h"
#include "cf_msgids.h"
#include "cf_verify.h"
#include "cf_perfids.h"
#include "cf_version.h"
#include "cf_platform_cfg.h"
#include "cf_callbacks.h"
#include "cfdp_syntax.h"
#include "cfdp_provides.h"
#include "cf_defs.h"
#include "cf_playback.h"
#include "cfdp_requires.h"
#include <string.h>
#include <ctype.h> /* toupper */

#ifdef CF_DEBUG
extern uint32       cfdbg;
#endif

extern CF_AppData_t CF_AppData;



/******************************************************************************
**  Function:  CF_HousekeepingCmd()
**
**  Purpose:
**      This function 
**
**  Arguments:
**
**
**  Return:
**
*/
void CF_HousekeepingCmd (CFE_SB_MsgPtr_t MessagePtr)
{
    int32               SemGetInfoRtn;
    uint32              i;    
    SUMMARY_STATUS      EngStat;
    OS_count_sem_prop_t SemInfo;
    

    if(CF_VerifyCmdLength(MessagePtr,CFE_SB_CMD_HDR_SIZE)==CF_BAD_MSG_LENGTH_RC)
    {
        CF_AppData.Hk.ErrCounter++;

    }else{
    
        /* get engine status */
        EngStat = cfdp_summary_status();
        
        /* MachinesAllocated - not yet populated */
        /* MachinesDeallocated - not yet populated */
        
        CF_AppData.Hk.Eng.are_any_partners_frozen = 
                EngStat.are_any_partners_frozen;/* Can be true even if there are
					                             * no transactions in-progress.*/
        CF_AppData.Hk.Eng.how_many_senders = EngStat.how_many_senders;
        CF_AppData.Hk.Eng.how_many_receivers = EngStat.how_many_receivers;
        CF_AppData.Hk.Eng.how_many_frozen = EngStat.how_many_frozen;
        CF_AppData.Hk.Eng.how_many_suspended = EngStat.how_many_suspended;
        CF_AppData.Hk.Eng.total_files_sent = EngStat.total_files_sent;
        CF_AppData.Hk.Eng.total_files_received = EngStat.total_files_received;
        CF_AppData.Hk.Eng.total_unsuccessful_senders = EngStat.total_unsuccessful_senders;
        CF_AppData.Hk.Eng.total_unsuccessful_receivers = EngStat.total_unsuccessful_receivers;

        CF_AppData.Hk.Up.UplinkActiveQFileCnt = CF_AppData.UpQ[CF_UP_ACTIVEQ].EntryCnt;
        
        CF_AppData.Hk.App.TotalInProgTrans   = CF_AppData.UpQ[CF_UP_ACTIVEQ].EntryCnt;
        CF_AppData.Hk.App.TotalFailedTrans   = CF_AppData.Hk.Up.FailedCounter;        
        CF_AppData.Hk.App.TotalSuccessTrans  = CF_AppData.Hk.Up.SuccessCounter;
        
        for(i=0;i<CF_MAX_PLAYBACK_CHANNELS;i++)
        {           

            if(CF_AppData.Tbl->OuCh[i].DequeueEnable == CF_ENABLED)
                CFE_SET(CF_AppData.Hk.Chan[i].Flags,0);
            else
                CFE_CLR(CF_AppData.Hk.Chan[i].Flags,0);

            if(CF_AppData.Chan[i].DataBlast == CF_IN_PROGRESS)
                CFE_SET(CF_AppData.Hk.Chan[i].Flags,1);
            else
                CFE_CLR(CF_AppData.Hk.Chan[i].Flags,1);                             
            
            if(CF_AppData.Chan[i].HandshakeSemId != CF_INVALID)
            {            
                SemGetInfoRtn = OS_CountSemGetInfo(CF_AppData.Chan[i].HandshakeSemId, &SemInfo); 
                if(SemGetInfoRtn == OS_SUCCESS){
                    CF_AppData.Hk.Chan[i].SemValue = SemInfo.value;
                }else{
                    CF_AppData.Hk.Chan[i].SemValue = 0;
                }/* end if */
            }/* end if */


            CF_AppData.Hk.Chan[i].PendingQFileCnt = CF_AppData.Chan[i].PbQ[CF_PB_PENDINGQ].EntryCnt;
            CF_AppData.Hk.Chan[i].ActiveQFileCnt  = CF_AppData.Chan[i].PbQ[CF_PB_ACTIVEQ].EntryCnt;
            CF_AppData.Hk.Chan[i].HistoryQFileCnt = CF_AppData.Chan[i].PbQ[CF_PB_HISTORYQ].EntryCnt;      
        
            CF_AppData.Hk.App.TotalInProgTrans   += CF_AppData.Hk.Chan[i].ActiveQFileCnt;            
            CF_AppData.Hk.App.TotalFailedTrans   += CF_AppData.Hk.Chan[i].FailedCounter;        
            CF_AppData.Hk.App.TotalSuccessTrans  += CF_AppData.Hk.Chan[i].SuccessCounter;
        }            

        CF_AppData.Hk.App.TotalCompletedTrans = CF_AppData.Hk.App.TotalSuccessTrans + 
                                                CF_AppData.Hk.App.TotalFailedTrans;


        CF_AppData.Hk.App.LowMemoryMark = CF_MEMORY_POOL_BYTES - CF_AppData.Hk.App.PeakMemInUse;

        /* Send housekeeping telemetry packet...        */
        CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &CF_AppData.Hk);
        CFE_SB_SendMsg((CFE_SB_Msg_t *) &CF_AppData.Hk);

    }

} /* End of CF_HousekeepingCmd() */


/******************************************************************************
**  Function:  CF_NoopCmd()
**
**  Purpose:
**      This function 
**
**  Arguments:
**
**
**  Return:
**
*/
void CF_NoopCmd (CFE_SB_MsgPtr_t MessagePtr)
{

    if(CF_VerifyCmdLength(MessagePtr,CFE_SB_CMD_HDR_SIZE)==CF_BAD_MSG_LENGTH_RC)
    {

        CF_AppData.Hk.ErrCounter++;

    }else{

        CFE_EVS_SendEvent (CF_NOOP_CMD_EID, CFE_EVS_INFORMATION,
            "CF No-op command, Version %d.%d.%d.%d",
             CF_MAJOR_VERSION,
             CF_MINOR_VERSION, 
             CF_REVISION, 
             CF_MISSION_REV);

        CF_AppData.Hk.CmdCounter++;
    }

} /* end of CF_NoopCmd() */


/******************************************************************************
**  Function:  CF_ResetCtrsCmd()
**
**  Purpose:
**      This function 
**
**  Arguments:
**
**
**  Return:
**
*/
void CF_ResetCtrsCmd (CFE_SB_MsgPtr_t MessagePtr)
{
    uint32 i;
    CF_ResetCtrsCmd_t   *CmdPtr;
    
    if(CF_VerifyCmdLength(MessagePtr,sizeof(CF_ResetCtrsCmd_t))==CF_BAD_MSG_LENGTH_RC)
    {

        CF_AppData.Hk.ErrCounter++;

    }else{

            CmdPtr = ((CF_ResetCtrsCmd_t *)MessagePtr);
            
            /* Command Counters */
            if((CmdPtr->Value == 0) || (CmdPtr->Value == 1))
            {
                CF_AppData.Hk.CmdCounter = 0;
                CF_AppData.Hk.ErrCounter = 0;
            
            }/* end if */
            
            /* Fault Counters */
            if((CmdPtr->Value == 0) || (CmdPtr->Value == 2))
            {

                CF_AppData.Hk.Cond.PosAckNum = 0; /* Positive ACK Limit Counter */          
                CF_AppData.Hk.Cond.FileStoreRejNum = 0; /* FileStore Rejection Counter */        
                CF_AppData.Hk.Cond.FileChecksumNum = 0; /* File Checksum Failure Counter */      
                CF_AppData.Hk.Cond.FileSizeNum = 0; /* Filesize Error Counter */         
                CF_AppData.Hk.Cond.NakLimitNum = 0; /* NAK Limit Counter */   
                CF_AppData.Hk.Cond.InactiveNum = 0; /* Inactivity Counter */         
                CF_AppData.Hk.Cond.CancelNum = 0; /* Cancel Request Counter */
            
            }/* end if */
            
            /* Uplink Counters */
            if((CmdPtr->Value == 0) || (CmdPtr->Value == 3))
            {
                CF_AppData.Hk.App.PDUsReceived  = 0;
                CF_AppData.Hk.App.PDUsRejected  = 0;
                CF_AppData.Hk.App.TotalAbandonTrans = 0;
                CF_AppData.Hk.Up.MetaCount = 0;    
                CF_AppData.Hk.Up.SuccessCounter = 0;
                CF_AppData.Hk.Up.FailedCounter = 0;
                
            
            }/* end if */
            
            /* Downlink Counters */
            if((CmdPtr->Value == 0) || (CmdPtr->Value == 4))
            {
                for(i=0;i<CF_MAX_PLAYBACK_CHANNELS;i++)
                {
                    CF_AppData.Hk.Chan[i].PDUsSent = 0;    
                    CF_AppData.Hk.Chan[i].FilesSent = 0;
                    CF_AppData.Hk.Chan[i].SuccessCounter = 0;
                    CF_AppData.Hk.Chan[i].FailedCounter = 0;
                    CF_AppData.Hk.Chan[i].RedLightCntr = 0;
                    CF_AppData.Hk.Chan[i].GreenLightCntr = 0;
                    CF_AppData.Hk.Chan[i].PollDirsChecked  = 0;
                    CF_AppData.Hk.Chan[i].PendingQChecked  = 0;
                }
                
                CF_AppData.Hk.App.TotalAbandonTrans = 0;
                
            }/* end if */
                
        CFE_EVS_SendEvent (CF_RESET_CMD_EID, CFE_EVS_DEBUG,
                           "Reset Counters command received - Value %u",CmdPtr->Value);
    }

} /* end of CF_ResetCtrsCmd() */




/******************************************************************************
**  Function:  CF_FreezeCmd()
**
**  Purpose:
**      This function 
**
**  Arguments:
**
**
**  Return:
**
*/
void CF_FreezeCmd(CFE_SB_MsgPtr_t MessagePtr)
{
                         
    if(CF_VerifyCmdLength(MessagePtr,sizeof(CF_NoArgsCmd_t))==CF_BAD_MSG_LENGTH_RC)
    {

        CF_AppData.Hk.ErrCounter++;

    }else{

        /* Alert ground that command has been received */
        CFE_EVS_SendEvent(CF_FREEZE_CMD_EID, 
                          CFE_EVS_INFORMATION,
                          "Freeze command received.");        
                                                       
        if (!cfdp_give_request ("freeze"))
        {
            CF_AppData.Hk.ErrCounter++;       
        }  
        else 
        {      
            /* Set freeze flag (bit 0) */
            CFE_SET(CF_AppData.Hk.Eng.Flags,0);
            CF_AppData.Hk.CmdCounter++;
        }
        
    }/* end if */

}/* end of CF_FreezeCmd function */


/******************************************************************************
**  Function:  CF_ThawCmd()
**
**  Purpose:
**      This function 
**
**  Arguments:
**
**
**  Return:
**
*/
void CF_ThawCmd(CFE_SB_MsgPtr_t MessagePtr)
{

    if(CF_VerifyCmdLength(MessagePtr,sizeof(CF_NoArgsCmd_t))==CF_BAD_MSG_LENGTH_RC)
    {

        CF_AppData.Hk.ErrCounter++;

    }else{

        /* Alert ground that command has been received */
        CFE_EVS_SendEvent(CF_THAW_CMD_EID, 
                          CFE_EVS_INFORMATION,
                          "Thaw command received.");        

        if (!cfdp_give_request ("thaw"))
        {
            CF_AppData.Hk.ErrCounter++;         
        }  
        else
        {
            /* Clear freeze flag (bit 0) */
            CFE_CLR(CF_AppData.Hk.Eng.Flags,0);
            CF_AppData.Hk.CmdCounter++;
        }

    }/* end if */

}/* end of CF_ThawCmd function */


/******************************************************************************
**  Function:  CF_CARSCmd()
**
**  Purpose:
**      Common function used for Cancel,Abandon,Resume,Suspend(CARS) Commands.
**
**  Arguments:
**
**
**  Return:
**
*/
void CF_CARSCmd(CFE_SB_MsgPtr_t MessagePtr, char *WhichCmd)
{
    CF_CARSCmd_t        *CmdPtr;/* CARS - Cancel,Abandon,Resume,Suspend */
    int32               Status;
    char                TransIdBuf[CF_MAX_TRANSID_CHARS];
    char                WhichCmdBuf[16];
    
    
    if(CF_VerifyCmdLength(MessagePtr,sizeof(CF_CARSCmd_t))==CF_BAD_MSG_LENGTH_RC)
    {

        CF_AppData.Hk.ErrCounter++;

    }else{

        CmdPtr = ((CF_CARSCmd_t *)MessagePtr);
                          
        if(CF_ChkTermination(CmdPtr->Trans,OS_MAX_PATH_LEN)==CF_ERROR)
        {
            /* Construct a string like "Suspend Cmd" */
            sprintf(WhichCmdBuf,"%s %s",WhichCmd,"Cmd");            
            CF_SendEventNoTerm(WhichCmdBuf);
            CF_AppData.Hk.ErrCounter++;
            return;
        }
        
        
        /* if parameter has a path/filename (as opposed to a TransId String) */
        if(CmdPtr->Trans[0] == '/')
        {
            
            /* Add ' Cmd' to string passed in */
            sprintf(WhichCmdBuf,"%s %s",WhichCmd,"Cmd");
            
            if(CF_ValidateFilenameReportErr(CmdPtr->Trans,WhichCmdBuf)==CF_ERROR)
            {                                
                CF_AppData.Hk.ErrCounter++;
                return;
            }  
                        
            /* convert filename to trans id string (i.e. 0.24_5) */
            Status = CF_FindActiveTransIdByName(&TransIdBuf[0],CmdPtr->Trans);
            if(Status == CF_ERROR)
            {
                CFE_EVS_SendEvent(CF_CARS_ERR1_EID,CFE_EVS_ERROR,
                                    "%s Cmd Error,File %s Not Active",
                                    WhichCmd,CmdPtr->Trans);
                CF_AppData.Hk.ErrCounter++;
                return;
            }

            /* error reported in CF_BuildCmdedRequest */
            Status = CF_BuildCmdedRequest(WhichCmd,&TransIdBuf[0]);
            
        
        /* param has TransId string, engine takes TransId string */
        }else{
            
            /* error reported in CF_BuildCmdedRequest */
            Status = CF_BuildCmdedRequest(WhichCmd,CmdPtr->Trans);
        
        }/* end if */    
        
        /* send event only on success */
        if(Status == CF_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_CARS_CMD_EID,CFE_EVS_INFORMATION,                           
                          "%s command received.%s",WhichCmd, CmdPtr->Trans);
            CF_AppData.Hk.CmdCounter++;
        
        }else{
        
            CF_AppData.Hk.ErrCounter++;
            
        }/* end if */
    
    }/* end if */

}/* end of CF_CARSCmd function */




/******************************************************************************
**  Function:  CF_SetMibCmd()
**
**  Purpose:
**      This function 
**
**  Arguments:
**
**
**  Return:
**
*/
void CF_SetMibCmd(CFE_SB_MsgPtr_t MessagePtr)
{  
    CF_SetMibParam_t        *CmdPtr;
    uint32                  i;
    int32                   ValueAsInt;
                          

    if(CF_VerifyCmdLength(MessagePtr,sizeof(CF_SetMibParam_t))==CF_BAD_MSG_LENGTH_RC)
    {

        CF_AppData.Hk.ErrCounter++;

    }else{

        CmdPtr = ((CF_SetMibParam_t *)MessagePtr);

        if(CF_ChkTermination(CmdPtr->Param,CF_MAX_CFG_PARAM_CHARS)==CF_ERROR)
        {           
            CF_SendEventNoTerm("SetMib Cmd, Param parameter");
            CF_AppData.Hk.ErrCounter++;
            return;
        }
        
        if(CF_ChkTermination(CmdPtr->Value,CF_MAX_CFG_VALUE_CHARS)==CF_ERROR)
        {           
            CF_SendEventNoTerm("SetMib Cmd, Value parameter");
            CF_AppData.Hk.ErrCounter++;
            return;
        }                 

        /* change param string to upper case to simplify string comparisons */
        i=0;
        while(CmdPtr->Param[i] != '\0')
        {
            /*CmdPtr->Param[i] = (char)toupper(CmdPtr->Param[i]);*/
            CmdPtr->Param[i] = (char)toupper((int)CmdPtr->Param[i]);
            i++;        
        }
                

        /* if changinging the 'outgoing file chunk size' be sure new setting does not overflow buffer */
        if(strcmp(CmdPtr->Param,"OUTGOING_FILE_CHUNK_SIZE")==0)
        {
            /* convert the new value to integer, then compare */
            ValueAsInt = atoi(&CmdPtr->Value[0]);
            if(ValueAsInt > CF_MAX_OUTGOING_CHUNK_SIZE)
            {                
                CFE_EVS_SendEvent(CF_SET_MIB_CMD_ERR1_EID, CFE_EVS_ERROR,
                    "Cannot set OUTGOING_FILE_CHUNK_SIZE(%d) > CF_MAX_OUTGOING_CHUNK_SIZE(%d)",
                    ValueAsInt,CF_MAX_OUTGOING_CHUNK_SIZE);  
                CF_AppData.Hk.ErrCounter++;
                
                return;
            }
        }


        /* if changing the 'flight entity id' be sure it validates */ 
        if(strcmp(CmdPtr->Param,"MY_ID")==0)
        {
            if(CF_ValidateEntityId(&CmdPtr->Value[0]) == CF_ERROR)
            {
                CFE_EVS_SendEvent(CF_SET_MIB_CMD_ERR2_EID, CFE_EVS_ERROR,
                    "Cannot set Flight Entity Id to %s, must be 2 byte, dotted decimal fmt",
                    &CmdPtr->Value[0]);  
                CF_AppData.Hk.ErrCounter++;
                
                return;
            }                
        }

        /* if the engine took the new parameter and returned success... */
        if(cfdp_set_mib_parameter(&CmdPtr->Param[0],&CmdPtr->Value[0]) == TRUE)  
        {                                   
            /* update table with new value */
            if(strcmp(CmdPtr->Param,"ACK_LIMIT")==0)
            {
                strncpy(&CF_AppData.Tbl->AckLimit[0],&CmdPtr->Value[0],CF_MAX_CFG_VALUE_CHARS);
            
            }else if(strcmp(CmdPtr->Param,"ACK_TIMEOUT")==0){
            
                strncpy(&CF_AppData.Tbl->AckTimeout[0],&CmdPtr->Value[0],CF_MAX_CFG_VALUE_CHARS);
            
            }else if(strcmp(CmdPtr->Param,"INACTIVITY_TIMEOUT")==0){
            
                strncpy(&CF_AppData.Tbl->InactivityTimeout[0],&CmdPtr->Value[0],CF_MAX_CFG_VALUE_CHARS);
            
            }else if(strcmp(CmdPtr->Param,"NAK_LIMIT")==0){
            
                strncpy(&CF_AppData.Tbl->NakLimit[0],&CmdPtr->Value[0],CF_MAX_CFG_VALUE_CHARS);
                                    
            }else if(strcmp(CmdPtr->Param,"NAK_TIMEOUT")==0){
            
                strncpy(&CF_AppData.Tbl->NakTimeout[0],&CmdPtr->Value[0],CF_MAX_CFG_VALUE_CHARS);
                
            }else if(strcmp(CmdPtr->Param,"SAVE_INCOMPLETE_FILES")==0){
            
                strncpy(&CF_AppData.Tbl->SaveIncompleteFiles[0],&CmdPtr->Value[0],CF_MAX_CFG_VALUE_CHARS);
                
            }else if(strcmp(CmdPtr->Param,"OUTGOING_FILE_CHUNK_SIZE")==0){
            
                strncpy(&CF_AppData.Tbl->OutgoingFileChunkSize[0],&CmdPtr->Value[0],CF_MAX_CFG_VALUE_CHARS);
                
            }else if(strcmp(CmdPtr->Param,"MY_ID")==0){
            
                strncpy(&CF_AppData.Tbl->FlightEntityId[0],&CmdPtr->Value[0],CF_MAX_CFG_VALUE_CHARS);
                
            }

            CFE_TBL_Modified(CF_AppData.ConfigTableHandle);
            CF_AppData.Hk.CmdCounter++;
        
            /* Alert ground that command has been received */
            CFE_EVS_SendEvent(CF_SET_MIB_CMD_EID, 
                          CFE_EVS_INFORMATION,
                          "Set MIB command received.Param %s Value %s",
                          &CmdPtr->Param[0], &CmdPtr->Value[0]);  
    
        }
        else 
        {
            /* engine returned error, event sent by engine */
            CF_AppData.Hk.ErrCounter++; 
        }   
        
    }/* end if */

}/* end of CF_SetMibCmd function */


/******************************************************************************
**  Function:  CF_GetMibCmd()
**
**  Purpose:
**      This function 
**
**  Arguments:
**
**
**  Return:
**
*/
void CF_GetMibCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    char                Value [CF_MAX_CFG_VALUE_CHARS];    
    CF_GetMibParam_t    *CmdPtr;
                          

    if(CF_VerifyCmdLength(MessagePtr,sizeof(CF_GetMibParam_t))==CF_BAD_MSG_LENGTH_RC)
    {
        CF_AppData.Hk.ErrCounter++;

    }else{

        CmdPtr = ((CF_GetMibParam_t *)MessagePtr);        
        
        if(CF_ChkTermination(CmdPtr->Param,CF_MAX_CFG_PARAM_CHARS)==CF_ERROR)
        {           
            CF_SendEventNoTerm("GetMib Cmd, Param parameter");
            CF_AppData.Hk.ErrCounter++;
            return;
        }

        if (cfdp_get_mib_parameter(&CmdPtr->Param[0], &Value[0]) == TRUE)
        {  
            CFE_EVS_SendEvent(CF_GET_MIB_CMD_EID, 
                  CFE_EVS_INFORMATION,
                  "Get MIB command received.Param %s Value %s",&CmdPtr->Param[0], Value); 
           
            CF_AppData.Hk.CmdCounter++;

        }
        else
        {
            CF_AppData.Hk.ErrCounter++; 
        }   

    }/* end if */

}/* end of CF_GetMibCmd function */


/******************************************************************************
**  Function:  CF_WriteQueueCmd()
**
**  Purpose:
**      This function 
**
**  Arguments:
**
**
**  Return:
**
*/
void CF_WriteQueueCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    CF_WriteQueueCmd_t  *CmdPtr;
    int32               Stat;
    uint8               QIndex;

    
    if(CF_VerifyCmdLength(MessagePtr,sizeof(CF_WriteQueueCmd_t))==CF_BAD_MSG_LENGTH_RC)
    {
        CF_AppData.Hk.ErrCounter++;
    
    }else{    
    
        CmdPtr = (CF_WriteQueueCmd_t *)MessagePtr;
    
        /* if uplink queue (incoming-file transaction) */
        if((CmdPtr->Type) == CF_UPLINK)
        {
            /* test Queue value */
            if((CmdPtr->Queue != CF_ACTIVEQ)&&(CmdPtr->Queue != CF_HISTORYQ))
            {
                CFE_EVS_SendEvent(CF_WR_CMD_ERR1_EID, CFE_EVS_ERROR,
                    "Invalid Queue Param %u in WriteQueueInfoCmd 1=active,2=history",
                    CmdPtr->Queue);
                CF_AppData.Hk.ErrCounter++;
                return;        
            }
            
            /* Cmd Param for uplink Q number:1=active,2=history, 0 invalid for uplink Q)*/
            /* Q array index is 0 for active, 1 for history */
            QIndex = CmdPtr->Queue - 1;
                
            if((CmdPtr->Filename[0])=='\0')
            {               
                Stat = CF_WriteQueueInfo(CF_DEFAULT_QUEUE_INFO_FILENAME,
                            CF_AppData.UpQ[QIndex].TailPtr);
            }else{
          
                Stat = CF_WriteQueueInfo(CmdPtr->Filename,
                            CF_AppData.UpQ[QIndex].TailPtr);
            }/* end if */
            
            CF_IncrCmdCtr(Stat);
                
        }
        else if(CmdPtr->Type == CF_PLAYBACK)
        {
            /* test Queue value */
            if(CmdPtr->Queue > 2)
            {
                CFE_EVS_SendEvent(CF_WR_CMD_ERR2_EID, 
                    CFE_EVS_ERROR,
                    "CF:Write Queue Info Error, Queue Value %u > max %u",
                    CmdPtr->Queue,2);
                CF_AppData.Hk.ErrCounter++;
                return;        
            }
            
            /* test Channel value */
            if(CmdPtr->Chan >= CF_MAX_PLAYBACK_CHANNELS)
            {
                CFE_EVS_SendEvent(CF_WR_CMD_ERR4_EID, 
                    CFE_EVS_ERROR,
                    "CF:Write Queue Info Error, Channel Value %u > max %u",
                    CmdPtr->Chan,(CF_MAX_PLAYBACK_CHANNELS-1));
                CF_AppData.Hk.ErrCounter++;
                return;        
            }            

            if((CmdPtr->Filename[0])=='\0')
            {
                Stat = CF_WriteQueueInfo(CF_DEFAULT_QUEUE_INFO_FILENAME,
                          CF_AppData.Chan[CmdPtr->Chan].PbQ[CmdPtr->Queue].TailPtr);
            }else{
          
                Stat = CF_WriteQueueInfo(CmdPtr->Filename,
                          CF_AppData.Chan[CmdPtr->Chan].PbQ[CmdPtr->Queue].TailPtr);
            }/* end if */    
            
            CF_IncrCmdCtr(Stat);
        
        }
        else
        {
            CFE_EVS_SendEvent(CF_WR_CMD_ERR3_EID, CFE_EVS_ERROR,
                "CF:Write Queue Info Error, Type Num %u not valid.",
                CmdPtr->Type);
            CF_AppData.Hk.ErrCounter++;
            return;    
        }
       
    }/* end verify cmd length */
    
}/* end CF_WriteQueueCmd */



/******************************************************************************
**  Function:  CF_WriteQueueInfo()
**
**  Purpose:
**    CF function to write the queue information to a file
**
**  Arguments:
**    Pointer to a filename
**
**  Return:
**    CF_ERROR for file I/O errors or CF_SUCCESS
*/
int32 CF_WriteQueueInfo(char *Filename,CF_QueueEntry_t *QueueEntryPtr){

    int32                       fd = 0;
    int32                       WriteStat;
    uint32                      FileSize = 0;
    uint32                      EntryCount = 0;
    CF_QueueInfoFileEntry_t     Entry;
    CFE_FS_Header_t             FileHdr;

    /* check for string termination and no spaces in filename */
    if(CF_ValidateFilenameReportErr(Filename,"WriteQueueCmd")==CF_ERROR)
    {                                
        return CF_ERROR;
    }/* end if */    
        
    fd = OS_creat(Filename, OS_WRITE_ONLY);
    if(fd < OS_FS_SUCCESS){
        OS_close(fd);
        CFE_EVS_SendEvent(CF_SND_QUE_ERR1_EID,CFE_EVS_ERROR,
                      "WriteQueueCmd:Error creating file %s, stat=0x%x",
                      Filename,fd);
        return CF_ERROR;
    }/* end if */

    /* clear out the cfe file header fields, then populate description and subtype */
    CFE_PSP_MemSet(&FileHdr, 0, sizeof(CFE_FS_Header_t));
    strcpy(&FileHdr.Description[0], "CF Queue Information");
    /*FileHdr.SubType = 0;*/

    WriteStat = CFE_FS_WriteHeader(fd, &FileHdr);
    if(WriteStat != sizeof(CFE_FS_Header_t)){
        CF_FileWriteByteCntErr(Filename,sizeof(CFE_FS_Header_t),WriteStat);
        OS_close(fd);
        return CF_ERROR;
    }/* end if */

    FileSize = WriteStat;

    while(QueueEntryPtr != NULL)
    {                        
        strncpy(&Entry.SrcFile[0],QueueEntryPtr->SrcFile,OS_MAX_PATH_LEN);
        strncpy(&Entry.SrcEntityId[0],QueueEntryPtr->SrcEntityId,CF_MAX_CFG_VALUE_CHARS);
        Entry.SrcFile[OS_MAX_PATH_LEN-1] = '\0';                
        Entry.TransNum    = QueueEntryPtr->TransNum;
        Entry.TransStatus = QueueEntryPtr->Status;
        
        WriteStat = OS_write (fd, &Entry, sizeof(CF_QueueInfoFileEntry_t));
        if(WriteStat != sizeof(CF_QueueInfoFileEntry_t)){
            CF_FileWriteByteCntErr(Filename,sizeof(CF_QueueInfoFileEntry_t),WriteStat);
            OS_close(fd);
            return CF_ERROR;
        }/* end if */

        FileSize += WriteStat;
        EntryCount ++;        
        
        QueueEntryPtr = QueueEntryPtr->Prev;
    }

    OS_close(fd);

    CFE_EVS_SendEvent(CF_SND_Q_INFO_EID,CFE_EVS_DEBUG,
                      "%s written:Size=%d,Entries=%d",
                      Filename,FileSize,EntryCount);

    return CF_SUCCESS;
    
}/* end CF_WriteQueueInfo */



/******************************************************************************
**  Function:  CF_WriteActiveTransCmd()
**
**  Purpose:
**      This function 
**
**  Arguments:
**
**
**  Return:
**
*/
void CF_WriteActiveTransCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    CF_WriteActiveTransCmd_t    *CmdPtr;
    int32                       Stat = 0;


    if(CF_VerifyCmdLength(MessagePtr,sizeof(CF_WriteActiveTransCmd_t))==CF_BAD_MSG_LENGTH_RC)
    {
        CF_AppData.Hk.ErrCounter++;
        return;

    }else{


        CmdPtr = (CF_WriteActiveTransCmd_t *)MessagePtr;
    
        if(CmdPtr->Type > CF_PLAYBACK)
        {
            CFE_EVS_SendEvent(CF_WRACT_ERR1_EID, CFE_EVS_ERROR,
                "CF:Write Active Cmd Error,Type Value %d > max %d",
                CmdPtr->Type,CF_PLAYBACK);
            CF_AppData.Hk.ErrCounter++;
            return;        
        }
            
    
        if((CmdPtr->Filename[0])=='\0')
        {
            Stat = CF_WriteActiveTransInfo(CF_DEFAULT_QUEUE_INFO_FILENAME,CmdPtr->Type);
    
        }else{
      
            Stat = CF_WriteActiveTransInfo(CmdPtr->Filename,(uint32)CmdPtr->Type);
    
        }/* end if */
        
        CF_IncrCmdCtr(Stat);
    
    }
               
}/* end CF_WriteActiveTransCmd */


int32 CF_WriteActiveTransInfo(char *Filename, uint32 WhichQueues){

    int32                       fd = 0;
    int32                       WriteStat,Status;
    uint32                      FileSize = 0;
    uint32                      EntryCount = 0;
    CFE_FS_Header_t             FileHdr;
    uint32                      i;

    /* check for string termination and no spaces in filename */
    if(CF_ValidateFilenameReportErr(Filename,"WriteActiveTransCmd")==CF_ERROR)
    {                                
        return CF_ERROR;
    }/* end if */
    
    
    fd = OS_creat(Filename, OS_WRITE_ONLY);
    if(fd < OS_FS_SUCCESS){
        OS_close(fd);
        CFE_EVS_SendEvent(CF_WRACT_ERR2_EID,CFE_EVS_ERROR,
                      "SendActiveTransCmd:Error creating file %s, stat=0x%x",
                      Filename,fd);
        return CF_ERROR;
    }/* end if */

    /* clear out the cfe file header fields, then populate description and subtype */
    CFE_PSP_MemSet(&FileHdr, 0, sizeof(CFE_FS_Header_t));
    strcpy(&FileHdr.Description[0], "CF Active Trans Information");
    /*FileHdr.SubType = 0;*/

    /* write cfe header to file */
    WriteStat = CFE_FS_WriteHeader(fd, &FileHdr);
    if(WriteStat != sizeof(CFE_FS_Header_t)){
        CF_FileWriteByteCntErr(Filename,sizeof(CFE_FS_Header_t),WriteStat);
        OS_close(fd);
        return CF_ERROR;
    }/* end if */

    FileSize = WriteStat;

    /* WhichQueues 0=all,1=up,2=down */
    if((WhichQueues == CF_ALL)||(WhichQueues == CF_UPLINK))
    {
        EntryCount = CF_AppData.UpQ[CF_UP_ACTIVEQ].EntryCnt;
        Status = CF_WrQEntrySubset(Filename,fd,CF_AppData.UpQ[CF_UP_ACTIVEQ].HeadPtr);
        if(Status == CF_ERROR) return CF_ERROR;        
    }
    
    if((WhichQueues == CF_ALL)||(WhichQueues == CF_PLAYBACK))
    {
        for(i=0;i<CF_MAX_PLAYBACK_CHANNELS;i++)
        {
            if(CF_AppData.Tbl->OuCh[i].EntryInUse == CF_ENTRY_IN_USE) 
            {   
                EntryCount += CF_AppData.Chan[i].PbQ[CF_PB_ACTIVEQ].EntryCnt;
                Status = CF_WrQEntrySubset(Filename,fd,CF_AppData.Chan[i].PbQ[CF_PB_ACTIVEQ].HeadPtr);
                if(Status == CF_ERROR) return CF_ERROR;
            }
        }
    }
    
    FileSize += (EntryCount * sizeof(CF_QueueInfoFileEntry_t));

    OS_close(fd);

    CFE_EVS_SendEvent(CF_WRACT_TRANS_EID,CFE_EVS_DEBUG,
                      "%s written:Size=%d,Entries=%d",
                      Filename,FileSize,EntryCount);

    return CF_SUCCESS;
    
}/* end CF_WriteActiveTransInfo */


int32 CF_WrQEntrySubset(char *Filename, int32 Fd,CF_QueueEntry_t *QueueEntryPtr){

    int32                       WriteStat;
    CF_QueueInfoFileEntry_t     Entry;

    while(QueueEntryPtr != NULL)
    {                        
        strncpy(&Entry.SrcFile[0],QueueEntryPtr->SrcFile,OS_MAX_PATH_LEN);
        strncpy(&Entry.SrcEntityId[0],QueueEntryPtr->SrcEntityId,CF_MAX_CFG_VALUE_CHARS);
        Entry.SrcFile[OS_MAX_PATH_LEN-1] = '\0';                
        Entry.TransNum = QueueEntryPtr->TransNum;
        Entry.TransStatus = QueueEntryPtr->Status;
        
        WriteStat = OS_write (Fd, &Entry, sizeof(CF_QueueInfoFileEntry_t));
        if(WriteStat != sizeof(CF_QueueInfoFileEntry_t)){
            CF_FileWriteByteCntErr(Filename,sizeof(CF_QueueInfoFileEntry_t),WriteStat);
            return CF_ERROR;
        }/* end if */      
        
        QueueEntryPtr = QueueEntryPtr->Next;
    }

    return CF_SUCCESS;

}/* CF_WrQEntrySubset */

/******************************************************************************
**  Function:  CF_SendTransDataCmd()
**
**  Purpose:
**    CF function to send transaction info in a packet
**
**  Arguments:
**
**  Return:
**
*/
void CF_SendTransDataCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    boolean             RetStat;
    TRANSACTION         TransToEng;
    TRANS_STATUS        StatFromEng;
    CF_QueueEntry_t     *QueueEntryPtr;
    CF_SendTransCmd_t   *CmdPtr;
    ID                  EntityIdInHex;
       

    if(CF_VerifyCmdLength(MessagePtr,sizeof(CF_SendTransCmd_t))==CF_BAD_MSG_LENGTH_RC)
    {
        CF_AppData.Hk.ErrCounter++;
        return;
    
    }else{

        CmdPtr = (CF_SendTransCmd_t *)MessagePtr;
        
        if(CF_ChkTermination(CmdPtr->Trans,OS_MAX_PATH_LEN)==CF_ERROR)
        {           
            CF_SendEventNoTerm("SendTransData Cmd, Trans parameter");
            CF_AppData.Hk.ErrCounter++;
            return;
        }

        /* if parameter has a path/filename */
        if(CmdPtr->Trans[0] == '/')
        {
            /* check for spaces in filename */
            if(CF_ValidateFilenameReportErr(CmdPtr->Trans,"SendTransDataCmd")==CF_ERROR)
            {                                
                CF_AppData.Hk.ErrCounter++;
                return;
            }            
            
            QueueEntryPtr = CF_FindNodeByName(CmdPtr->Trans);
    
        /* if parameter has a TransId string like 0.24_13 */
        }else{
        
            QueueEntryPtr = CF_FindNodeByTransId(CmdPtr->Trans);
            
        }/* end if */
                
        if(QueueEntryPtr == NULL){
            CFE_EVS_SendEvent(CF_SND_TRANS_ERR_EID,CFE_EVS_ERROR,
                "Send Trans Cmd Error, %s not found.",CmdPtr->Trans);
            CF_AppData.Hk.ErrCounter++;
            return;
        }/* end if */
            
        CF_AppData.Trans.App.Status         =   QueueEntryPtr->Status;
        CF_AppData.Trans.App.CondCode       =   QueueEntryPtr->CondCode;
        CF_AppData.Trans.App.Priority       =   QueueEntryPtr->Priority;
        CF_AppData.Trans.App.Class          =   QueueEntryPtr->Class;
        CF_AppData.Trans.App.ChanNum        =   QueueEntryPtr->ChanNum;
        CF_AppData.Trans.App.Source         =   QueueEntryPtr->Source;
        CF_AppData.Trans.App.NodeType       =   QueueEntryPtr->NodeType;
        CF_AppData.Trans.App.TransNum       =   QueueEntryPtr->TransNum;
        strncpy(&CF_AppData.Trans.App.SrcEntityId[0],QueueEntryPtr->SrcEntityId,CF_MAX_CFG_VALUE_CHARS);
        strncpy(&CF_AppData.Trans.App.SrcFile[0],QueueEntryPtr->SrcFile,OS_MAX_PATH_LEN);
        strncpy(&CF_AppData.Trans.App.DstFile[0],QueueEntryPtr->DstFile,OS_MAX_PATH_LEN);
            
        /* get status from engine only if transaction is active */
        if (QueueEntryPtr->Status == CF_STAT_ACTIVE)
        {
            TransToEng.number = QueueEntryPtr->TransNum;
    
            cfdp_id_from_string (QueueEntryPtr->SrcEntityId, &EntityIdInHex);
     
            TransToEng.source_id.length = EntityIdInHex.length;
            TransToEng.source_id.value[0] = EntityIdInHex.value[0];
            TransToEng.source_id.value[1] = EntityIdInHex.value[1];
                
            RetStat = cfdp_transaction_status (TransToEng, &StatFromEng);
            
            CF_AppData.Trans.Eng.Flags = 0;
                
            if(StatFromEng.abandoned)                           CFE_SET(CF_AppData.Trans.Eng.Flags,0);
            if(StatFromEng.cancelled)                           CFE_SET(CF_AppData.Trans.Eng.Flags,1);
            if(StatFromEng.external_file_xfer)                  CFE_SET(CF_AppData.Trans.Eng.Flags,2);
            if(StatFromEng.finished)                            CFE_SET(CF_AppData.Trans.Eng.Flags,3);
            if(StatFromEng.frozen)                              CFE_SET(CF_AppData.Trans.Eng.Flags,4);
            if(StatFromEng.has_md_been_received)                CFE_SET(CF_AppData.Trans.Eng.Flags,5);
            if(StatFromEng.is_this_trans_solely_for_ack_fin)    CFE_SET(CF_AppData.Trans.Eng.Flags,6);
            if(StatFromEng.suspended)                           CFE_SET(CF_AppData.Trans.Eng.Flags,7);
            if(StatFromEng.md.file_transfer)                    CFE_SET(CF_AppData.Trans.Eng.Flags,8);
            if(StatFromEng.md.segmentation_control)             CFE_SET(CF_AppData.Trans.Eng.Flags,9);
                    
            CF_AppData.Trans.Eng.TransNum = StatFromEng.trans.number;
            CF_AppData.Trans.Eng.TransLen = StatFromEng.trans.source_id.length;
            CF_AppData.Trans.Eng.TransVal = StatFromEng.trans.source_id.value[0];
            CF_AppData.Trans.Eng.Attempts = StatFromEng.attempts;
            CF_AppData.Trans.Eng.CondCode = StatFromEng.condition_code;
            CF_AppData.Trans.Eng.DeliCode = StatFromEng.delivery_code;
            CF_AppData.Trans.Eng.FdOffset = StatFromEng.fd_offset;
            CF_AppData.Trans.Eng.FdLength = StatFromEng.fd_length;
            CF_AppData.Trans.Eng.Checksum = StatFromEng.file_checksum_as_calculated;
            CF_AppData.Trans.Eng.FinalStat = StatFromEng.final_status;
            CF_AppData.Trans.Eng.Naks      = StatFromEng.how_many_naks;
            CF_AppData.Trans.Eng.FileSize = StatFromEng.md.file_size;
            CF_AppData.Trans.Eng.PartLen = StatFromEng.partner_id.length;
            CF_AppData.Trans.Eng.PartVal = StatFromEng.partner_id.value[0];
            CF_AppData.Trans.Eng.Phase = StatFromEng.phase;
            CF_AppData.Trans.Eng.RcvdFileSize = StatFromEng.received_file_size;
            CF_AppData.Trans.Eng.Role = StatFromEng.role;
            CF_AppData.Trans.Eng.State = StatFromEng.state;
            CF_AppData.Trans.Eng.StartTime = StatFromEng.start_time;
            strncpy(&CF_AppData.Trans.Eng.TmpFile[0],&StatFromEng.temp_file_name[0],OS_MAX_PATH_LEN); 
            strncpy(&CF_AppData.Trans.Eng.SrcFile[0],&StatFromEng.md.source_file_name[0],OS_MAX_PATH_LEN);
            strncpy(&CF_AppData.Trans.Eng.DstFile[0],&StatFromEng.md.dest_file_name[0],OS_MAX_PATH_LEN);    
    
        }
    
        CFE_EVS_SendEvent(CF_SND_TRANS_CMD_EID,CFE_EVS_DEBUG,
             "CF:Sending Transaction Pkt %s",CmdPtr->Trans);
    
        CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &CF_AppData.Trans);
        CFE_SB_SendMsg((CFE_SB_Msg_t *) &CF_AppData.Trans);        
   
    }
    
    CF_AppData.Hk.CmdCounter++;

} /* end of CF_SendTransDataCmd() */


/******************************************************************************
**  Function:  CF_SendCfgParams()
**
**  Purpose:
**    CF function to send configuration parameters in a packet
**
**  Arguments:
**
**
**  Return:
**    None
*/
void CF_SendCfgParams(CFE_SB_MsgPtr_t MessagePtr)
{
    char    AckLimit[CF_MAX_CFG_VALUE_CHARS]; 
    char    AckTimeout[CF_MAX_CFG_VALUE_CHARS]; 
    char    NakLimit[CF_MAX_CFG_VALUE_CHARS]; 
    char    NakTimeout[CF_MAX_CFG_VALUE_CHARS]; 
    char    InactTimeout[CF_MAX_CFG_VALUE_CHARS]; 
    char    OutGoingChunk[CF_MAX_CFG_VALUE_CHARS]; 
    char    SaveIncomplete[CF_MAX_CFG_VALUE_CHARS];


    if(CF_VerifyCmdLength(MessagePtr,sizeof(CF_NoArgsCmd_t))==CF_BAD_MSG_LENGTH_RC)
    {

        CF_AppData.Hk.ErrCounter++;

    }else{
                             
        cfdp_get_mib_parameter("ACK_LIMIT",     &AckLimit[0]);
        cfdp_get_mib_parameter("ACK_TIMEOUT",   &AckTimeout[0]);
        cfdp_get_mib_parameter("NAK_LIMIT",     &NakLimit[0]);
        cfdp_get_mib_parameter("NAK_TIMEOUT",   &NakTimeout[0]);
        cfdp_get_mib_parameter("INACTIVITY_TIMEOUT",    &InactTimeout[0]);
        cfdp_get_mib_parameter("OUTGOING_FILE_CHUNK_SIZE", &OutGoingChunk[0]);
        cfdp_get_mib_parameter("SAVE_INCOMPLETE_FILES", &SaveIncomplete[0]);    
        
        CF_AppData.CfgPkt.EngCycPerWakeup = CF_AppData.Tbl->NumEngCyclesPerWakeup;
        CF_AppData.CfgPkt.AckLimit = atoi(&AckLimit[0]);
        CF_AppData.CfgPkt.AckTimeout = atoi(&AckTimeout[0]);
        CF_AppData.CfgPkt.NakLimit = atoi(&NakLimit[0]);
        CF_AppData.CfgPkt.NakTimeout = atoi(&NakTimeout[0]);
        CF_AppData.CfgPkt.InactTimeout = atoi(&InactTimeout[0]);
        CF_AppData.CfgPkt.DefOutgoingChunkSize = atoi(&OutGoingChunk[0]);
        strncpy(&CF_AppData.CfgPkt.SaveIncompleteFiles[0],&SaveIncomplete[0],8);
    
        CF_AppData.CfgPkt.PipeDepth = CF_PIPE_DEPTH;
        CF_AppData.CfgPkt.MaxSimultaneousTrans = CF_MAX_SIMULTANEOUS_TRANSACTIONS;
        CF_AppData.CfgPkt.IncomingPduBufSize = CF_INCOMING_PDU_BUF_SIZE;
        CF_AppData.CfgPkt.OutgoingPduBufSize = CF_OUTGOING_PDU_BUF_SIZE;
        CF_AppData.CfgPkt.NumInputChannels = CF_NUM_INPUT_CHANNELS;
        CF_AppData.CfgPkt.MaxPlaybackChans = CF_MAX_PLAYBACK_CHANNELS;
        CF_AppData.CfgPkt.MaxPollingDirsPerChan = CF_MAX_POLLING_DIRS_PER_CHAN;
        CF_AppData.CfgPkt.MemPoolBytes = CF_MEMORY_POOL_BYTES;
        
#ifdef CF_DEBUG
        CF_AppData.CfgPkt.DebugCompiledIn = CF_TRUE;
#else
        CF_AppData.CfgPkt.DebugCompiledIn = CF_FALSE;
#endif        

        strncpy(&CF_AppData.CfgPkt.PipeName[0],CF_PIPE_NAME, OS_MAX_API_NAME);
        strncpy(&CF_AppData.CfgPkt.TmpFilePrefix[0],CF_ENGINE_TEMP_FILE_PREFIX,OS_MAX_PATH_LEN);
        strncpy(&CF_AppData.CfgPkt.CfgTblName[0],CF_CONFIG_TABLE_NAME,OS_MAX_PATH_LEN);
        strncpy(&CF_AppData.CfgPkt.CfgTbleFilename[0],CF_CONFIG_TABLE_FILENAME,OS_MAX_PATH_LEN);
        strncpy(&CF_AppData.CfgPkt.DefQInfoFilename[0],CF_DEFAULT_QUEUE_INFO_FILENAME,OS_MAX_PATH_LEN);
        
        CFE_EVS_SendEvent(CF_SND_CFG_CMD_EID,CFE_EVS_DEBUG,
             "CF:Sending Configuration Pkt");

        CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &CF_AppData.CfgPkt);
        CFE_SB_SendMsg((CFE_SB_Msg_t *) &CF_AppData.CfgPkt);    
        
        CF_AppData.Hk.CmdCounter++;
        
    }/* end if */

}/* CF_SendCfgParams */



/******************************************************************************
**  Function:  CF_SetPollParam()
**
**  Purpose:
**    CF function to send diag data
**
**  Arguments:
**
**
**  Return:
**    None
*/
void CF_SetPollParam(CFE_SB_MsgPtr_t MessagePtr)
{

    CF_SetPollParamCmd_t    *CmdPtr;

    if(CF_VerifyCmdLength(MessagePtr,sizeof(CF_SetPollParamCmd_t))==CF_BAD_MSG_LENGTH_RC)
    {

        CF_AppData.Hk.ErrCounter++;

    }else{

        CmdPtr  = ((CF_SetPollParamCmd_t *)MessagePtr);
    
        /* range check channel param */
        if(CmdPtr->Chan >= CF_MAX_PLAYBACK_CHANNELS)
        {
            CFE_EVS_SendEvent(CF_SET_POLL_PARAM_ERR1_EID, CFE_EVS_ERROR,
                "Invalid Chan Param %u in SetPollParamCmd,Max %u",
                CmdPtr->Chan,CF_MAX_PLAYBACK_CHANNELS-1);
            CF_AppData.Hk.ErrCounter++;
            return;
        }

        /* range check poll dir param */
        if(CmdPtr->Dir >= CF_MAX_POLLING_DIRS_PER_CHAN)
        {
            CFE_EVS_SendEvent(CF_SET_POLL_PARAM_ERR2_EID, CFE_EVS_ERROR,
                "Invalid PollDir Param %u in SetPollParamCmd,Max %u",
                CmdPtr->Dir, CF_MAX_POLLING_DIRS_PER_CHAN-1);
            CF_AppData.Hk.ErrCounter++;
            return;
        }
        
        /* range check class param */
        if((CmdPtr->Class < CF_CLASS_1)||(CmdPtr->Class > CF_CLASS_2))
        {
            CFE_EVS_SendEvent(CF_SET_POLL_PARAM_ERR3_EID, CFE_EVS_ERROR,
                "Invalid Class Param %u in SetPollParamCmd,Min %u,Max %u",
                CmdPtr->Class,CF_CLASS_1,CF_CLASS_2);
            CF_AppData.Hk.ErrCounter++;
            return;
        }

        /* range check preserve param */
        if(CmdPtr->Preserve > CF_KEEP_FILE)
        {
            CFE_EVS_SendEvent(CF_SET_POLL_PARAM_ERR4_EID, CFE_EVS_ERROR,
                "Invalid Preserve Param %u in SetPollParamCmd,Max %u",
                CmdPtr->Class,CF_KEEP_FILE);
            CF_AppData.Hk.ErrCounter++;
            return;
        }

        /* check that the paths are terminated, have */
        /* forward slash at last character and no spaces */
        if(CF_ValidateSrcPath(CmdPtr->SrcPath)==CF_ERROR)
        {
            CFE_EVS_SendEvent(CF_SET_POLL_PARAM_ERR5_EID, CFE_EVS_ERROR,
                "SrcPath in SetPollParam Cmd must be terminated,have no spaces,slash at end");
            CF_AppData.Hk.ErrCounter++;
            return;
        }
        
        if(CF_ValidateDstPath(CmdPtr->DstPath)==CF_ERROR)
        {
            CFE_EVS_SendEvent(CF_SET_POLL_PARAM_ERR6_EID, CFE_EVS_ERROR,
                "DstPath in SetPollParam Cmd must be terminated and have no spaces");
            CF_AppData.Hk.ErrCounter++;
            return;
        }
        
        /* check peer entity ID format */
        if(CF_ValidateEntityId(CmdPtr->PeerEntityId) == CF_ERROR)
        {
            CFE_EVS_SendEvent(CF_SET_POLL_PARAM_ERR7_EID,CFE_EVS_ERROR,
                "PeerEntityId %s in SetPollParam Cmd must be 2 byte,dotted decimal fmt.ex 0.24",
                CmdPtr->PeerEntityId);
            CF_AppData.Hk.ErrCounter++;
            return;        
        }

        CF_AppData.Tbl->OuCh[CmdPtr->Chan].PollDir[CmdPtr->Dir].Class = CmdPtr->Class;
        CF_AppData.Tbl->OuCh[CmdPtr->Chan].PollDir[CmdPtr->Dir].Priority = CmdPtr->Priority;
        CF_AppData.Tbl->OuCh[CmdPtr->Chan].PollDir[CmdPtr->Dir].Preserve = CmdPtr->Preserve;
        
        strncpy(&CF_AppData.Tbl->OuCh[CmdPtr->Chan].PollDir[CmdPtr->Dir].PeerEntityId[0],
                                    CmdPtr->PeerEntityId,
                                    CF_MAX_CFG_VALUE_CHARS);               
                
        strncpy(&CF_AppData.Tbl->OuCh[CmdPtr->Chan].PollDir[CmdPtr->Dir].SrcPath[0],
                                    CmdPtr->SrcPath,
                                    OS_MAX_PATH_LEN);
        
        strncpy(&CF_AppData.Tbl->OuCh[CmdPtr->Chan].PollDir[CmdPtr->Dir].DstPath[0],
                                    CmdPtr->DstPath,
                                    OS_MAX_PATH_LEN);                                                    
        
        CFE_EVS_SendEvent(CF_SET_POLL_PARAM1_EID, CFE_EVS_DEBUG,
            "SetPollParam Cmd Rcvd,Ch=%u,Dir=%u,Cl=%u,Pri=%u,Pre=%u",
            CmdPtr->Chan,CmdPtr->Dir,CmdPtr->Class,CmdPtr->Priority,CmdPtr->Preserve);   

        CF_AppData.Hk.CmdCounter++;    
    }

}/* end CF_SetPollParam */



void CF_DequeueNodeCmd(CFE_SB_MsgPtr_t MessagePtr)
{

    CF_QueueEntry_t     *QueueEntryPtr;
    CF_DequeueNodeCmd_t *CmdPtr;
    uint8               QIndex = 0;
    uint32              RemoveStat;
    char                QString[32];
    
    if(CF_VerifyCmdLength(MessagePtr,sizeof(CF_DequeueNodeCmd_t))==CF_BAD_MSG_LENGTH_RC)
    {

        CF_AppData.Hk.ErrCounter++;

    }else{

        CmdPtr = ((CF_DequeueNodeCmd_t *)MessagePtr);

        if(CF_ChkTermination(CmdPtr->Trans,OS_MAX_PATH_LEN)==CF_ERROR)
        {           
            CF_SendEventNoTerm("DequeueNode Cmd, Trans parameter");
            CF_AppData.Hk.ErrCounter++;
            return;
        }

        /* Trans param may be filename or Transaction Id */
        /* if parameter has a path/filename */
        if(CmdPtr->Trans[0] == '/')
        {
            /* check for spaces in filename */
            if(CF_ValidateFilenameReportErr(CmdPtr->Trans,"DequeueNodeCmd")==CF_ERROR)
            {                                
                CF_AppData.Hk.ErrCounter++;
                return;
            }

            QueueEntryPtr = CF_FindNodeByName(CmdPtr->Trans);
        
        /* if parameter has a TransId string like 0.24_13 */
        }else{
        
            QueueEntryPtr = CF_FindNodeByTransId(CmdPtr->Trans);
            
        }/* end if */
                
        if(QueueEntryPtr == NULL){
            CFE_EVS_SendEvent(CF_DEQ_NODE_ERR1_EID,CFE_EVS_ERROR,
                "Dequeue Node Cmd Error, %s not found.",CmdPtr->Trans);
            CF_AppData.Hk.ErrCounter++;
            return;
        }/* end if */  

        
        switch(QueueEntryPtr -> NodeType)
        {        
            case  CF_UPLINK:
                if(QueueEntryPtr->Status == CF_STAT_ACTIVE){
                
                    if(QueueEntryPtr->Warning == CF_WAS_ISSUED){                                       

                        QIndex = CF_UP_ACTIVEQ;
                        strcpy(QString,"Incoming Active");
                    
                    }else if(QueueEntryPtr->Warning == CF_NOT_ISSUED){
                    
                        CFE_EVS_SendEvent(CF_DEQ_NODE_ERR2_EID, CFE_EVS_CRITICAL,
                        "DequeueNodeCmd:Trans %s is ACTIVE! Must send cmd again to remove",
                        CmdPtr->Trans); 
                        QueueEntryPtr->Warning = CF_WAS_ISSUED;
                        CF_AppData.Hk.CmdCounter++; 
                        return;
                    
                    }/* end if */
                
                }else{
               
                    QIndex = CF_UP_HISTORYQ;
                    strcpy(QString,"Incoming History");
                
                }/* end if */
                
                RemoveStat = CF_RemoveFileFromUpQueue(QIndex, QueueEntryPtr); 
                if(RemoveStat == CF_SUCCESS)
                    CF_DeallocQueueEntry(QueueEntryPtr);
    
                CFE_EVS_SendEvent(CF_DEQ_NODE1_EID, CFE_EVS_DEBUG,
                    "DequeueNodeCmd %s Removed from %s Queue,Stat %d",
                    CmdPtr->Trans,QString,RemoveStat);                
    
                CF_AppData.Hk.CmdCounter++;                
                                
                break;
                
            
            case CF_PLAYBACK:
                if((QueueEntryPtr->Status == CF_STAT_PENDING)||
                    (QueueEntryPtr->Status == CF_STAT_PUT_REQ_ISSUED)){
                
                    QIndex = CF_PB_PENDINGQ;
                    strcpy(QString,"Outgoing Pending");
                    
                }else if(QueueEntryPtr->Status == CF_STAT_ACTIVE){
                
                    if(QueueEntryPtr->Warning == CF_WAS_ISSUED){                                       

                        QIndex = CF_PB_ACTIVEQ;
                        strcpy(QString,"Outgoing Active");
                    
                    }else if(QueueEntryPtr->Warning == CF_NOT_ISSUED){
                    
                        CFE_EVS_SendEvent(CF_DEQ_NODE_ERR3_EID, CFE_EVS_CRITICAL,
                        "DequeueNodeCmd:Trans %s is ACTIVE! Must send cmd again to remove",
                        CmdPtr->Trans); 
                        QueueEntryPtr->Warning = CF_WAS_ISSUED;
                        CF_AppData.Hk.CmdCounter++; 
                        return;
                    
                    }/* end if */

                    
                }else{
                
                    QIndex = CF_PB_HISTORYQ;
                    strcpy(QString,"Outgoing History");
                    
                }/* end if */
                
                RemoveStat = CF_RemoveFileFromPbQueue(QueueEntryPtr->ChanNum, QIndex, QueueEntryPtr); 
                if(RemoveStat == CF_SUCCESS)
                    CF_DeallocQueueEntry(QueueEntryPtr);
    
                CFE_EVS_SendEvent(CF_DEQ_NODE2_EID, CFE_EVS_DEBUG,
                    "DequeueNodeCmd %s Removed from Chan %u,%s Queue,Stat %d",
                    CmdPtr->Trans,QueueEntryPtr->ChanNum,QString,RemoveStat);                
    
                CF_AppData.Hk.CmdCounter++;
                break;

            
            default:
            
                CFE_EVS_SendEvent(CF_DEQ_NODE_ERR4_EID, CFE_EVS_ERROR,
                    "Unexpected NodeType %d in Queue node",
                        QueueEntryPtr -> NodeType);            
                CF_AppData.Hk.ErrCounter++;        
        
        }/* end switch */
                
    }/* end if */

}/* CF_DequeueNodeCmd */


void CF_PurgeQueueCmd(CFE_SB_MsgPtr_t MessagePtr)
{

    CF_QueueEntry_t     *PtrToEntry;
    CF_PurgeQueueCmd_t  *CmdPtr;
    uint32              NodesRemoved = 0;
    char                QString[16];
    
    if(CF_VerifyCmdLength(MessagePtr,sizeof(CF_PurgeQueueCmd_t))==CF_BAD_MSG_LENGTH_RC)
    {

        CF_AppData.Hk.ErrCounter++;

    }else{

        CmdPtr = ((CF_PurgeQueueCmd_t *)MessagePtr);
        
        switch(CmdPtr->Type)
        {

            case CF_INCOMING:        
                /* chan param ignored... */
                /* range check the queue param, purging active queue not allowed */
                if(CmdPtr->Queue != CF_HISTORYQ)
                {
                    if(CmdPtr->Queue == CF_ACTIVEQ)
                    {
                        CFE_EVS_SendEvent(CF_PURGEQ_ERR1_EID, CFE_EVS_ERROR,
                            "PurgeQueueCmd Err:Cannot purge Incoming ACTIVE Queue");
                    }else{
                    
                        CFE_EVS_SendEvent(CF_PURGEQ_ERR2_EID, CFE_EVS_ERROR,
                            "Invalid Queue Param %u in PurgeQueueCmd",
                            CmdPtr->Queue);
                    }/* end if */
                    CF_AppData.Hk.ErrCounter++;
                    return;
                }                

                PtrToEntry = CF_AppData.UpQ[CF_UP_HISTORYQ].HeadPtr;            
                while(PtrToEntry != NULL)
                {                   
                    CF_RemoveFileFromUpQueue(CF_UP_HISTORYQ, PtrToEntry); 
                    CF_DeallocQueueEntry(PtrToEntry);
                    NodesRemoved++;
                    PtrToEntry = CF_AppData.UpQ[CF_UP_HISTORYQ].HeadPtr;
                }
            
                CFE_EVS_SendEvent(CF_PURGEQ1_EID, CFE_EVS_INFORMATION,
                    "PurgeQueueCmd Removed %u Nodes from Uplink History Queue",
                                    NodesRemoved);

                CF_AppData.Hk.CmdCounter++; 
                break;


            case CF_OUTGOING:                         
                if(CmdPtr->Queue == CF_PB_ACTIVEQ)
                {
                    CFE_EVS_SendEvent(CF_PURGEQ_ERR3_EID, CFE_EVS_ERROR,
                        "PurgeQueueCmd Err:Cannot purge Outgoing ACTIVE Queue");
                    CF_AppData.Hk.ErrCounter++;
                    return;
                }
                
                if(CmdPtr->Queue > CF_PB_HISTORYQ)
                {
                    CFE_EVS_SendEvent(CF_PURGEQ_ERR4_EID, CFE_EVS_ERROR,
                        "Invalid Queue Param %u in PurgeQueueCmd,Max %u",
                        CmdPtr->Queue,CF_PB_HISTORYQ);
                    CF_AppData.Hk.ErrCounter++;
                    return;
                }

                if(CmdPtr->Chan >= CF_MAX_PLAYBACK_CHANNELS)
                {
                    CFE_EVS_SendEvent(CF_PURGEQ_ERR5_EID, CFE_EVS_ERROR,
                        "Invalid Chan Param %u in PurgeQueueCmd,Max %u",
                        CmdPtr->Chan,CF_MAX_PLAYBACK_CHANNELS-1);
                    CF_AppData.Hk.ErrCounter++;
                    return;
                }                                                    

                PtrToEntry = CF_AppData.Chan[CmdPtr->Chan].PbQ[CmdPtr->Queue].HeadPtr;            
                while(PtrToEntry != NULL)
                {                
                    CF_RemoveFileFromPbQueue(CmdPtr->Chan,CmdPtr->Queue,PtrToEntry); 
                    CF_DeallocQueueEntry(PtrToEntry);
                    NodesRemoved++;
                    PtrToEntry = CF_AppData.Chan[CmdPtr->Chan].PbQ[CmdPtr->Queue].HeadPtr;
                }
                
                if(CmdPtr->Queue == CF_PB_PENDINGQ)
                    strcpy(QString,"Pending");
                else
                    strcpy(QString,"History");
                
                CFE_EVS_SendEvent(CF_PURGEQ2_EID, CFE_EVS_INFORMATION,
                    "PurgeQueueCmd Removed %u Nodes from Chan %u,%s Queue",
                                    NodesRemoved,CmdPtr->Chan,QString);

                CF_AppData.Hk.CmdCounter++; 
                break;
            
        
            default:
            
                CFE_EVS_SendEvent(CF_PURGEQ_ERR6_EID, CFE_EVS_ERROR,
                    "Invalid Type Param %u in PurgeQueueCmd,must be uplink %u or playback %u",
                        CmdPtr->Type,CF_UPLINK,CF_PLAYBACK);            
                CF_AppData.Hk.ErrCounter++;        
        
        }/* end switch */
                
    }/* end if */

}/* end CF_PurgeQueueCmd */


void CF_EnableDequeueCmd(CFE_SB_MsgPtr_t MessagePtr){

    CF_EnDisDequeueCmd_t    *EnDisDequeueCmdPtr;
    uint8                   Chan;

    if(CF_VerifyCmdLength(MessagePtr,sizeof(CF_EnDisPollCmd_t))==CF_BAD_MSG_LENGTH_RC)
    {

        CF_AppData.Hk.ErrCounter++;

    }else{

        EnDisDequeueCmdPtr = (CF_EnDisDequeueCmd_t *)MessagePtr;
        Chan =  EnDisDequeueCmdPtr->Chan;

        if(Chan < CF_MAX_PLAYBACK_CHANNELS)
        {        
            CFE_EVS_SendEvent(CF_ENA_DQ_CMD_EID,CFE_EVS_DEBUG,
                "Channel %d Dequeue Enabled",Chan);
            
            CF_AppData.Tbl->OuCh[Chan].DequeueEnable = CF_ENABLED;
            CFE_TBL_Modified(CF_AppData.ConfigTableHandle);
            CFE_SET(CF_AppData.Hk.Chan[Chan].Flags,0);
            CF_AppData.Hk.CmdCounter++;
            
        }else{
        
            CFE_EVS_SendEvent(CF_DQ_CMD_ERR1_EID,CFE_EVS_ERROR,
                "Enable Dequeue Cmd Param Err Chan %d,Max is %d",
                Chan,(CF_MAX_PLAYBACK_CHANNELS-1));        
            CF_AppData.Hk.ErrCounter++;        
    
        }/* end if */
        
    }/* end if */

}/* end CF_EnableDequeueCmd */



void CF_DisableDequeueCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    CF_EnDisDequeueCmd_t    *EnDisDequeueCmdPtr;
    uint8                   Chan;

    if(CF_VerifyCmdLength(MessagePtr,sizeof(CF_EnDisPollCmd_t))==CF_BAD_MSG_LENGTH_RC)
    {

        CF_AppData.Hk.ErrCounter++;

    }else{

        EnDisDequeueCmdPtr = (CF_EnDisDequeueCmd_t *)MessagePtr;
        Chan =  EnDisDequeueCmdPtr->Chan;

        if(Chan < CF_MAX_PLAYBACK_CHANNELS)
        {
#ifdef CF_DEBUG            
            if(cfdbg > 0)
                OS_printf("CF:Channel %d Disabled",Chan);
#endif    
            CFE_EVS_SendEvent(CF_DIS_DQ_CMD_EID,CFE_EVS_DEBUG,
                "Channel %d Dequeue Disabled",Chan);
    
            CF_AppData.Tbl->OuCh[Chan].DequeueEnable = CF_DISABLED;
            CFE_TBL_Modified(CF_AppData.ConfigTableHandle);
            CFE_CLR(CF_AppData.Hk.Chan[Chan].Flags,0);
            CF_AppData.Hk.CmdCounter++;
            
        }else{
        
            CFE_EVS_SendEvent(CF_DQ_CMD_ERR2_EID,CFE_EVS_ERROR,
                "Disable Dequeue Cmd Param Err Chan %d,Max is %d",
                Chan,(CF_MAX_PLAYBACK_CHANNELS-1));      
            CF_AppData.Hk.ErrCounter++;        
    
        }/* end if */
        
    }/* end if */

}/* end CF_DisableDequeueCmd */


void CF_EnablePollCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    CF_EnDisPollCmd_t   *EnDisPollCmdPtr;
    uint32              i;
    uint8               Chan,Dir;

    if(CF_VerifyCmdLength(MessagePtr,sizeof(CF_EnDisPollCmd_t))==CF_BAD_MSG_LENGTH_RC)
    {

        CF_AppData.Hk.ErrCounter++;

    }else{

        EnDisPollCmdPtr = (CF_EnDisPollCmd_t *)MessagePtr;
        Chan =  EnDisPollCmdPtr->Chan;
        Dir  =  EnDisPollCmdPtr->Dir;
                    
        if(Chan >= CF_MAX_PLAYBACK_CHANNELS)
        {    
            CFE_EVS_SendEvent(CF_ENA_POLL_ERR1_EID,CFE_EVS_ERROR,
            "Channel Param Err in EnPollCmd.Chan %d,Max %d",
            Chan,(CF_MAX_PLAYBACK_CHANNELS-1));        
            CF_AppData.Hk.ErrCounter++;
            return;
    
        }/* end if */
    
        
        if((Dir >= CF_MAX_POLLING_DIRS_PER_CHAN)&&(Dir != 0xFF))
        {
            CFE_EVS_SendEvent(CF_ENA_POLL_ERR2_EID,CFE_EVS_ERROR,
            "Directory Param Err in EnPollCmd.Dir %d,0-%d and 255 allowed",
            Dir,(CF_MAX_POLLING_DIRS_PER_CHAN-1));        
            CF_AppData.Hk.ErrCounter++;
            return;
            
        }/* end if */
    
        if(Dir == 0xFF)
        {    
            for(i=0;i<CF_MAX_POLLING_DIRS_PER_CHAN;i++)
            {
                if(CF_AppData.Tbl->OuCh[Chan].PollDir[i].EntryInUse == CF_ENTRY_IN_USE)
                    CF_AppData.Tbl->OuCh[Chan].PollDir[i].EnableState = CF_ENABLED;
            
            }/* end for */
    
            CFE_TBL_Modified(CF_AppData.ConfigTableHandle);                
    
            CFE_EVS_SendEvent(CF_ENA_POLL_CMD1_EID,CFE_EVS_DEBUG,
                "All In-use Polling Directories on Channel %d Enabled",Chan);
    
        }else{
        
            if(CF_AppData.Tbl->OuCh[Chan].PollDir[Dir].EntryInUse == CF_ENTRY_IN_USE)
                CF_AppData.Tbl->OuCh[Chan].PollDir[Dir].EnableState = CF_ENABLED; 
                
            CFE_EVS_SendEvent(CF_ENA_POLL_CMD2_EID,CFE_EVS_DEBUG,
                "Polling Directory %d on Channel %d Enabled",Dir,Chan);
    
        }/* end if */
    
    
        CF_AppData.Hk.CmdCounter++;
    
    }/* end if */

}/* end CF_EnablePollCmd */


void CF_DisablePollCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    CF_EnDisPollCmd_t   *EnDisPollCmdPtr;    
    uint32              i;
    uint8               Chan,Dir;


    if(CF_VerifyCmdLength(MessagePtr,sizeof(CF_EnDisPollCmd_t))==CF_BAD_MSG_LENGTH_RC)
    {

        CF_AppData.Hk.ErrCounter++;

    }else{

        EnDisPollCmdPtr = (CF_EnDisPollCmd_t *)MessagePtr;
        Chan =  EnDisPollCmdPtr->Chan;
        Dir  =  EnDisPollCmdPtr->Dir;
    
        if(Chan >= CF_MAX_PLAYBACK_CHANNELS)
        {    
            CFE_EVS_SendEvent(CF_DIS_POLL_ERR1_EID,CFE_EVS_ERROR,
            "Channel Param Err in DisPollCmd.Chan %d,Max %d",
            Chan,(CF_MAX_PLAYBACK_CHANNELS-1));        
            CF_AppData.Hk.ErrCounter++;
            return;
    
        }/* end if */
    
        
        if((Dir >= CF_MAX_POLLING_DIRS_PER_CHAN)&&(Dir != 0xFF))
        {
            CFE_EVS_SendEvent(CF_DIS_POLL_ERR2_EID,CFE_EVS_ERROR,
            "Directory Param Err in DisPollCmd.Dir %d,0-%d and 255 allowed",
            Dir,(CF_MAX_POLLING_DIRS_PER_CHAN-1));        
            CF_AppData.Hk.ErrCounter++;
            return;
            
        }/* end if */
    
        if(Dir == 0xFF)
        {    
            for(i=0;i<CF_MAX_POLLING_DIRS_PER_CHAN;i++)
            {
                if(CF_AppData.Tbl->OuCh[Chan].PollDir[i].EntryInUse == CF_ENTRY_IN_USE)
                    CF_AppData.Tbl->OuCh[Chan].PollDir[i].EnableState = CF_DISABLED;
            
            }/* end for */
    
            CFE_TBL_Modified(CF_AppData.ConfigTableHandle);                
    
            CFE_EVS_SendEvent(CF_DIS_POLL_CMD1_EID,CFE_EVS_DEBUG,
                "All In-use Polling Directories on Channel %d Disabled",Chan);
    
        }else{
        
            if(CF_AppData.Tbl->OuCh[Chan].PollDir[Dir].EntryInUse == CF_ENTRY_IN_USE)
                CF_AppData.Tbl->OuCh[Chan].PollDir[Dir].EnableState = CF_DISABLED; 
                
            CFE_EVS_SendEvent(CF_DIS_POLL_CMD2_EID,CFE_EVS_DEBUG,
                "Polling Directory %d on Channel %d Disabled",Dir,Chan);
    
        }/* end if */
        
        CF_AppData.Hk.CmdCounter++;
    
    }/* end if */

}/* end CF_DisablePollCmd */



/* Kickstart Cmd is a safety net, in case the EOF-Sent */
/* is not detected and chan sending gets stuck */
void CF_KickstartCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    CF_KickstartCmd_t   *CmdPtr;

    if(CF_VerifyCmdLength(MessagePtr,sizeof(CF_KickstartCmd_t))==CF_BAD_MSG_LENGTH_RC)
    {

        CF_AppData.Hk.ErrCounter++;

    }else{

        CmdPtr = ((CF_KickstartCmd_t *)MessagePtr);
        
        if(CmdPtr->Chan >= CF_MAX_PLAYBACK_CHANNELS)
        {
            CFE_EVS_SendEvent(CF_KICKSTART_ERR1_EID, CFE_EVS_ERROR,
                "Invalid Chan Param %u in KickstartCmd,Max %u",
                CmdPtr->Chan,CF_MAX_PLAYBACK_CHANNELS-1);
            CF_AppData.Hk.ErrCounter++;
            return;
        }
        
        /* Set DataBlast flag so that the pending queue is checked periodically */
        CF_AppData.Chan[CmdPtr->Chan].DataBlast = CF_NOT_IN_PROGRESS;
        
        CF_AppData.Hk.CmdCounter++;
        
        CFE_EVS_SendEvent(CF_KICKSTART_CMD_EID, CFE_EVS_DEBUG,
                "Kickstart cmd received, chan %u",CmdPtr->Chan);
        
    }/* end if */

}/* end CF_KickstartCmd */


void CF_QuickStatusCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    CF_QuickStatCmd_t   *CmdPtr;
    CF_QueueEntry_t     *QueueEntryPtr;
    char                LocalStatBuf[CF_MAX_ERR_STRING_CHARS];
    char                LocalCondCodeBuf[CF_MAX_ERR_STRING_CHARS];
    TRANSACTION         TransToEng;
    TRANS_STATUS        StatFromEng;
    ID                  EntityIdInHex;        


    if(CF_VerifyCmdLength(MessagePtr,sizeof(CF_QuickStatCmd_t))==CF_BAD_MSG_LENGTH_RC)
    {

        CF_AppData.Hk.ErrCounter++;

    }else{

        CmdPtr = ((CF_QuickStatCmd_t *)MessagePtr);
        
        if(CF_ChkTermination(CmdPtr->Trans,OS_MAX_PATH_LEN)==CF_ERROR)
        {           
            CF_SendEventNoTerm("QuickStatusCmd, Trans parameter");
            CF_AppData.Hk.ErrCounter++;
            return;
        }


        /* if parameter has a path/filename (as opposed to a TransId String) */
        if(CmdPtr->Trans[0] == '/')
        {             
            if(CF_ValidateFilenameReportErr(CmdPtr->Trans,"QuickStatusCmd")==CF_ERROR)
            {                                
                CF_AppData.Hk.ErrCounter++;
                return;
            }  
                                    
            QueueEntryPtr = CF_FindNodeByName(CmdPtr->Trans);
        
        /* param has TransId string */
        }else{
            
            QueueEntryPtr = CF_FindNodeByTransId(CmdPtr->Trans);
        
        }/* end if */    
        
        if(QueueEntryPtr == NULL)
        {
            CFE_EVS_SendEvent(CF_QUICK_ERR1_EID,CFE_EVS_ERROR,
                                "Quick Status Cmd Error,Trans %s Not Found",
                                CmdPtr->Trans);
            CF_AppData.Hk.ErrCounter++;
            return;
        }

        CF_GetStatString(LocalStatBuf,
                        QueueEntryPtr->Status,
                        CF_MAX_ERR_STRING_CHARS);
                                                                            

        if(QueueEntryPtr->Status == CF_STAT_ACTIVE)
        {
            /* check if trans is suspended */            
            TransToEng.number = QueueEntryPtr->TransNum;
    
            cfdp_id_from_string (QueueEntryPtr->SrcEntityId, &EntityIdInHex);
     
            TransToEng.source_id.length = EntityIdInHex.length;
            TransToEng.source_id.value[0] = EntityIdInHex.value[0];
            TransToEng.source_id.value[1] = EntityIdInHex.value[1];
                
            cfdp_transaction_status (TransToEng, &StatFromEng);
                
            /* if trans is suspended, modify string to inform user */
            if(StatFromEng.suspended)
            {
                /* string will become ACTIVE/SUSPENDED */
                strcat(LocalStatBuf,"/SUSPENDED");            
            }
            
        }/* end if */
        
        CF_GetCondCodeString(LocalCondCodeBuf,
                            QueueEntryPtr->CondCode,
                            CF_MAX_ERR_STRING_CHARS);


        CFE_EVS_SendEvent(CF_QUICK_CMD_EID,CFE_EVS_INFORMATION,                           
                        "Trans %s_%u %s Stat=%s,CondCode=%s",
                        QueueEntryPtr->SrcEntityId,
                        QueueEntryPtr->TransNum,
                        QueueEntryPtr->SrcFile,
                        LocalStatBuf,
                        LocalCondCodeBuf);                          

        CF_AppData.Hk.CmdCounter++;        
    
    }/* end if */

    return;

}/* end CF_QuickStatusCmd */




void CF_GiveTakeSemaphoreCmd(CFE_SB_MsgPtr_t MessagePtr)
{

    CF_GiveTakeCmd_t   *CmdPtr;
    char                SemDir[16];
    int32               SemRtn;    
        
    if(CF_VerifyCmdLength(MessagePtr,sizeof(CF_GiveTakeCmd_t))==CF_BAD_MSG_LENGTH_RC)
    {

        CF_AppData.Hk.ErrCounter++;

    }else{

        CmdPtr = ((CF_GiveTakeCmd_t *)MessagePtr);
        
        
        if(CF_AppData.Chan[CmdPtr->Chan].HandshakeSemId == CF_INVALID)
        {
            CFE_EVS_SendEvent(CF_GIVETAKE_ERR1_EID, CFE_EVS_ERROR,
                "Invalid Param.Chan %d is not using a semaphore (not being throttled)",
                CmdPtr->Chan);
            CF_AppData.Hk.ErrCounter++;
            return;
        }          
        
        
        if(CmdPtr->Chan >= CF_MAX_PLAYBACK_CHANNELS)
        {
            CFE_EVS_SendEvent(CF_GIVETAKE_ERR2_EID, CFE_EVS_ERROR,
                "Invalid Chan Param %u in GiveTakeCmd,Max %u",
                CmdPtr->Chan,CF_MAX_PLAYBACK_CHANNELS-1);
            CF_AppData.Hk.ErrCounter++;
            return;
        }
        

        if((CmdPtr->GiveOrTakeSemaphore != CF_GIVE_SEMAPHORE) &&
           (CmdPtr->GiveOrTakeSemaphore != CF_TAKE_SEMAPHORE))
        {
            CFE_EVS_SendEvent(CF_GIVETAKE_ERR3_EID, CFE_EVS_ERROR,
                "Invalid GiveOrTake Param %u in GiveTakeSemaphoreCmd,Must be %d or %d",
                CmdPtr->GiveOrTakeSemaphore,CF_GIVE_SEMAPHORE,CF_TAKE_SEMAPHORE);
            CF_AppData.Hk.ErrCounter++;
            return;
        }


        if(CmdPtr->GiveOrTakeSemaphore == CF_TAKE_SEMAPHORE)
        {
            SemRtn = OS_CountSemTimedWait(CF_AppData.Chan[CmdPtr->Chan].HandshakeSemId,0);
        
        }else{     
        
            SemRtn = OS_CountSemGive (CF_AppData.Chan[CmdPtr->Chan].HandshakeSemId);
        }

  
        if(CmdPtr->GiveOrTakeSemaphore == CF_GIVE_SEMAPHORE)
        {        
            strcpy(SemDir,"Give");

        }else{

            strcpy(SemDir,"Take");
        }
  
        if(SemRtn == OS_SUCCESS)
        {

            CFE_EVS_SendEvent(CF_GIVETAKE_CMD_EID,CFE_EVS_INFORMATION,                           
                        "CF Semaphore %s on chan %u was successful",
                        SemDir,CmdPtr->Chan);                          

            CF_AppData.Hk.CmdCounter++; 
        
        }else{

            CFE_EVS_SendEvent(CF_GIVETAKE_ERR4_EID,CFE_EVS_ERROR,                           
                        "CF Semaphore %s error on chan %u,Rtn Val %d",
                        SemDir, CmdPtr->Chan, SemRtn);                          

            CF_AppData.Hk.ErrCounter++;
        
        }/* end if */
            
    
    }/* end if */

    return;

}/* end CF_GiveTakeSemaphoreCmd */



void CF_AutoSuspendEnCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    CF_AutoSuspendEnCmd_t   *CmdPtr;
        
    if(CF_VerifyCmdLength(CF_AppData.MsgPtr,sizeof(CF_AutoSuspendEnCmd_t))==CF_BAD_MSG_LENGTH_RC)
    {

        CF_AppData.Hk.ErrCounter++;

    }else{

        CmdPtr = ((CF_AutoSuspendEnCmd_t *)MessagePtr);
        
        if(CmdPtr->EnableDisable == CF_ENABLED)
        {            
            CF_AppData.Hk.AutoSuspend.EnFlag = CF_ENABLED;            
        }else{        
            CF_AppData.Hk.AutoSuspend.EnFlag = CF_DISABLED;
        }/* end if */
    
        CF_AppData.Hk.CmdCounter++;
    
        CFE_EVS_SendEvent(CF_ENDIS_AUTO_SUS_CMD_EID,CFE_EVS_DEBUG,                           
                        "Auto Suspend enable flag set to %u",
                        CF_AppData.Hk.AutoSuspend.EnFlag);
    
    }/* end if */

}/* end CF_AutoSuspendEnCmd */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Verify Command Length                                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CF_VerifyCmdLength (CFE_SB_MsgPtr_t MessagePtr,uint32 ExpectedLength)
{
    int32               Status = CF_SUCCESS;
    CFE_SB_MsgId_t      MessageID;
    uint16              CommandCode;
    uint16              ActualLength;

    ActualLength = CFE_SB_GetTotalMsgLength (MessagePtr);

    if (ExpectedLength != ActualLength)
    {

        MessageID   = CFE_SB_GetMsgId   (MessagePtr);
        CommandCode = CFE_SB_GetCmdCode (MessagePtr);

        CFE_EVS_SendEvent(CF_CMD_LEN_ERR_EID, CFE_EVS_ERROR,
          "Cmd Msg with Bad length Rcvd: ID = 0x%X, CC = %d, Exp Len = %d, Len = %d",
          MessageID, CommandCode, ExpectedLength, ActualLength);

        Status = CF_BAD_MSG_LENGTH_RC;

    }

    return Status;

} /* end of CF_VerifyCmdLength () */


/******************************************************************************
**  Function:  CF_IncrCmdCtr()
**
**  Purpose:
**    CF function to increment the proper cmd counter based on the
**    status input. This small utility was written to eliminate duplicate code.
**
**  Arguments:
**    status - typically CF_SUCCESS or CF_ERROR
**
**  Return:
**    None
*/
void CF_IncrCmdCtr(int32 Status)
{

    if(Status==CF_SUCCESS){
      CF_AppData.Hk.CmdCounter++;
    }else{
      CF_AppData.Hk.ErrCounter++;
    }/* end if */
    
}/* end CF_IncrCmdCtr */


/******************************************************************************
**  Function:  CF_FileWriteByteCntErr()
**
**  Purpose:
**    CF function to report a file write error
**
**  Arguments:
**
**
**  Return:
**    None
*/
void CF_FileWriteByteCntErr(char *Filename,uint32 Requested,uint32 Actual){

    CFE_EVS_SendEvent(CF_FILEWRITE_ERR_EID,CFE_EVS_ERROR,
                      "File write,byte cnt err,file %s,request=%d,actual=%d",
                       Filename,Requested,Actual);

}/* end CF_FileWriteByteCntErr() */


/************************/
/*  End of File Comment */
/************************/
