/************************************************************************
** File:
**   $Id: cf_callbacks.c 1.29.1.1 2015/03/06 15:30:34EST sstrege Exp  $
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
**  The CFS CF Application file containing the callback routines
**  specified by the user.
**
** Notes:
**
** $Log: cf_callbacks.c  $
** Revision 1.29.1.1 2015/03/06 15:30:34EST sstrege 
** Added copyright information
** Revision 1.29 2011/05/19 15:32:02EDT rmcgraw 
** DCR15033:1 Add auto suspend processing
** Revision 1.28 2011/05/19 13:15:13EDT rmcgraw 
** DCR14532:1 Let user select fix or variable size outgoing PDU pkts
** Revision 1.27 2011/05/17 17:22:25EDT rmcgraw 
** DCR14967:11 In Mach Alloc, send event if mem allocation fails
** Revision 1.26 2011/05/17 10:21:45EDT rmcgraw 
** DCR14967:4 Function Register_callbacks cannot return err, check for err removed
** Revision 1.25 2011/05/10 15:05:52EDT rmcgraw 
** DCR14525:1 Give semaphore for errors in CF_PduOutputSend
** Revision 1.24 2011/05/03 16:47:20EDT rmcgraw 
** Cleaned up events, removed \n from some events, removed event id ganging
** Revision 1.23 2011/04/29 14:35:33EDT rmcgraw 
** Fixed Filesize performance marker near the end of the Filesize callback.
** Revision 1.22 2011/04/29 11:29:02EDT rmcgraw 
** Removed \n and mem set from engine events
** Revision 1.21 2010/11/04 11:37:46EDT rmcgraw 
** Dcr13051:1 Wrap OS_printfs in platform cfg CF_DEBUG
** Revision 1.20 2010/11/03 13:47:00EDT rmcgraw 
** DCR13219:1 Suppress Outgoing-trans-started event if when node not found
** Revision 1.19 2010/10/25 11:21:47EDT rmcgraw 
** DCR12573:1 Changes to allow more than one incoming PDU MsgId
** Revision 1.18 2010/10/21 13:48:10EDT rmcgraw 
** DCR13060:2 Changed FindUpNodebyTransnum to FindUpNodeByTransID
** Revision 1.17 2010/10/20 14:15:21EDT rmcgraw 
** DCR128222:1 Added tlm counter for total abandon transactions
** Revision 1.16 2010/10/20 14:03:03EDT rmcgraw 
** DCR12819:1 Change wording in timer expire events to include 'flight'.
** Revision 1.15 2010/08/09 17:48:33EDT rmcgraw 
** DCR11510:1 Fixed strcmp in pdurdy callback directing pdus to uplink response chan
** Revision 1.14 2010/08/04 15:17:36EDT rmcgraw 
** DCR11510:1 Changes prior to release
** Revision 1.13 2010/07/20 14:37:38EDT rmcgraw 
** Dcr11510:1 Remove Downlink buffer references
** Revision 1.12 2010/07/07 17:36:32EDT rmcgraw 
** DCR11510:1 Set QueueEntry, status param to new defines
** Revision 1.11 2010/06/17 10:08:48EDT rmcgraw 
** DCR11510:1 DataBlast Logic Added to prevent playback chain getting stuck during flt abandon or gnd cancel
** Revision 1.10 2010/06/11 16:13:15EDT rmcgraw 
** DCR11510:1 ZeroCopy, Un-hardcoded cmd/tlm input/output pdus
** Revision 1.9 2010/06/01 10:55:13EDT rmcgraw 
** DCR111510:1 moved trans finished processing to mach deallocated
** Revision 1.8 2010/04/27 09:09:21EDT rmcgraw 
** DCR11510:1 Added #include cf_verify.h
** Revision 1.7 2010/04/23 15:41:52EDT rmcgraw 
** DCR11510:1 Re-ordered cases to be chronological
** Revision 1.6 2010/04/23 08:39:14EDT rmcgraw 
** Dcr11510:1 Code Review Prep
** Revision 1.5 2010/03/26 15:30:21EDT rmcgraw 
** DCR11510 Various developmental changes
** Revision 1.4 2010/03/12 12:14:36EST rmcgraw 
** DCR11510:1 Initial check-in towards CF Version 1000
** Revision 1.3 2009/12/09 09:29:13EST rmcgraw 
** DCR10350:3 Only files from polling directories are deleted when complete
** Revision 1.2 2009/12/08 09:10:11EST rmcgraw 
** DCR10350:3 Minor Cleanup
** Revision 1.1 2009/11/24 12:48:51EST rmcgraw 
** Initial revision
** Member added to CFS CF project
**
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/
#include "cf_callbacks.h"
#include "cfdp_requires.h"
#include "cf_events.h"
#include "cf_verify.h"
#include "cf_app.h"
#include "cf_msgids.h"
#include "cf_defs.h"
#include "cf_playback.h"
#include "cf_utils.h"
#include "cf_perfids.h"
#include <string.h>

#ifdef CF_DEBUG
extern uint32           cfdbg;
#endif

extern CF_AppData_t     CF_AppData;

/* Some AutoSuspend variables are defined in HK pkt */
extern uint32           CF_AutoSuspendCnt;
extern uint32           CF_AutoSuspendArray[CF_AUTOSUSPEND_MAX_TRANS];


void CF_RegisterCallbacks(void){

    register_indication (CF_Indication);
    register_pdu_output_open (CF_PduOutputOpen);
    register_pdu_output_ready (CF_PduOutputReady);
    register_pdu_output_send (CF_PduOutputSend);
    register_printf_debug(CF_DebugEvent);
    register_printf_info(CF_InfoEvent);
    register_printf_warning(CF_WarningEvent);
    register_printf_error(CF_ErrorEvent);
    register_file_size(CF_FileSize);
    register_rename(CF_RenameFile);
    register_remove(CF_RemoveFile);
    register_fseek(CF_Fseek);   
    register_fopen(CF_Fopen);
    register_fread(CF_Fread);
    register_fwrite(CF_Fwrite);
    register_fclose(CF_Fclose);
    
    return;

}/* end CF_RegisterCallbacks */


/*
**             Function Prologue
**
** Function Name: CF_Indication
**
** Purpose: Indication of a transaction. Callback function
**          
**
** Input arguments:
**    Indication IndType(enum), and the transaction status(TRANS_STATUS data structure)
**
** Return values:
**    (none)
*/
void CF_Indication (INDICATION_TYPE IndType, TRANS_STATUS TransInfo)
{
   
    CF_QueueEntry_t *QueueEntryPtr = NULL;
    uint32          Chan;
    char            LocalFinalStatBuf[CF_MAX_ERR_STRING_CHARS];
    char            LocalCondCodeBuf[CF_MAX_ERR_STRING_CHARS];
    char            EntityIdBuf[CF_MAX_CFG_VALUE_CHARS];

    
    /*initialization*/
    TransInfo.md.source_file_name[MAX_FILE_NAME_LENGTH - 1] = '\0';
    TransInfo.md.dest_file_name[MAX_FILE_NAME_LENGTH - 1] = '\0';

    switch(IndType)
    {       

        case IND_TRANSACTION:
            break;
        
        case IND_MACHINE_ALLOCATED:
                                                                    
            /* if uplink trans, build new node */
            if((TransInfo.role ==  CLASS_1_RECEIVER) || 
               (TransInfo.role ==  CLASS_2_RECEIVER))
            {
                /* Build up a new Node */
                QueueEntryPtr = CF_AllocQueueEntry();
                
                if(QueueEntryPtr != NULL)
                {                    
                    /* fill-in queue entry */
                    if(TransInfo.role ==  CLASS_1_RECEIVER)
                        QueueEntryPtr->Class = 1;
                    else
                        QueueEntryPtr->Class = 2;

                    QueueEntryPtr->Status = CF_STAT_ACTIVE;
                    QueueEntryPtr->CondCode = 0;
                    QueueEntryPtr->Priority = 0xFF;
                    QueueEntryPtr->ChanNum  = CF_GetResponseChanFromMsgId(CF_AppData.MsgPtr);
                    QueueEntryPtr->Source   = 0xFF;
                    QueueEntryPtr->Warning  = CF_NOT_ISSUED;
                    QueueEntryPtr->NodeType = CF_UPLINK;
                    QueueEntryPtr->TransNum = TransInfo.trans.number;                    
                    sprintf(&QueueEntryPtr->SrcEntityId[0],"%d.%d",
                            TransInfo.trans.source_id.value[0],
                            TransInfo.trans.source_id.value[1]);                                       
                    
                    /* filenames not known until the metadata rcvd indication */
                    strcpy(&QueueEntryPtr->SrcFile[0],"UNKNOWN");
                    strcpy(&QueueEntryPtr->DstFile[0],"UNKNOWN");                    
                    
                    /* Place Node on Uplink Active Queue */
                    CF_AddFileToUpQueue(CF_UP_ACTIVEQ, QueueEntryPtr);
                }
                
                else{
                
                      CFE_EVS_SendEvent(CF_MACH_ALLOC_ERR_EID,CFE_EVS_ERROR,
                          "CF:MachAlloc:AllocateQueueEntry returned NULL!\n");

                }/* end if */
          
            }else{
               
                /* file-send transaction */                              
                QueueEntryPtr = CF_FindNodeAtFrontOfQueue(TransInfo);

                if(QueueEntryPtr != NULL)
                {                                    
                    CFE_EVS_SendEvent(CF_OUT_TRANS_START_EID,CFE_EVS_INFORMATION,
                                    "Outgoing trans started %d.%d_%d,src %s",                                                            
                                    TransInfo.trans.source_id.value[0],
                                    TransInfo.trans.source_id.value[1],
                                    TransInfo.trans.number,
                                    &TransInfo.md.source_file_name[0]); 

                    QueueEntryPtr->TransNum = TransInfo.trans.number;                     
                                                        
                    CF_AppData.Chan[QueueEntryPtr->ChanNum].DataBlast = 
                                                CF_IN_PROGRESS;
                    CF_AppData.Chan[QueueEntryPtr->ChanNum].TransNumBlasting = 
                                                TransInfo.trans.number;                    
                    
                    /* move node from pending queue to active queue */
                    CF_RemoveFileFromPbQueue(QueueEntryPtr->ChanNum, 
                                                CF_PB_PENDINGQ, 
                                                QueueEntryPtr);                                                 
                    CF_AddFileToPbQueue(QueueEntryPtr->ChanNum, CF_PB_ACTIVEQ, 
                                                QueueEntryPtr);
                    
                    QueueEntryPtr->Status = CF_STAT_ACTIVE;                                               
                
                }
#ifdef CF_DEBUG                
                else{

                    OS_printf("CF:MachAlloc:Node Not Found!\n");

                }/* end if */
#endif

            }/* end if */                    
            
            break;


        case IND_METADATA_SENT:
            break;       
       
         
        case IND_METADATA_RECV:
            
            CF_AppData.Hk.Up.MetaCount++;                                                                                   

            sprintf(EntityIdBuf,"%d.%d",TransInfo.trans.source_id.value[0],
                                        TransInfo.trans.source_id.value[1]);
            
            /* file-receive transaction */
            CFE_EVS_SendEvent(CF_IN_TRANS_START_EID,CFE_EVS_INFORMATION,
                                "Incoming trans started %d.%d_%d,dest %s",                                                            
                                TransInfo.trans.source_id.value[0],
                                TransInfo.trans.source_id.value[1],
                                TransInfo.trans.number,
                                TransInfo.md.dest_file_name);

            /*  find corresponding queue entry (created in mach allocated  */
            /*  indication) then fill in src and dest filenames */            
    
            QueueEntryPtr = CF_FindUpNodeByTransID(CF_UP_ACTIVEQ, EntityIdBuf, TransInfo.trans.number);
                                
            if(QueueEntryPtr != NULL)
            {            
                strncpy(&QueueEntryPtr->SrcFile[0],TransInfo.md.source_file_name,OS_MAX_PATH_LEN);
                strncpy(&QueueEntryPtr->DstFile[0],TransInfo.md.dest_file_name,OS_MAX_PATH_LEN);
            }
            
            break;


        case IND_EOF_SENT:                     
                            
            /* Find Channel Number, search the given queue for all channels */
            Chan = CF_GetChanNumFromTransId(CF_PB_ACTIVEQ, TransInfo.trans.number);
            if(Chan != CF_ERROR)
            {        

                if(CF_AppData.Hk.AutoSuspend.EnFlag == CF_ENABLED)
                {
                    /* GPM AutoSuspend patch - log trans num so it can be suspended later. */
                    /* Suspending now would be a recursive engine call, eng is not re-entrant */
                    
                    if(CF_AutoSuspendCnt < CF_AUTOSUSPEND_MAX_TRANS)
                    {
                        CF_AutoSuspendArray[CF_AutoSuspendCnt] = TransInfo.trans.number;
                        CF_AutoSuspendCnt++;
                        
                        if((CF_AUTOSUSPEND_MAX_TRANS - CF_AutoSuspendCnt) < CF_AppData.Hk.AutoSuspend.LowFreeMark)
                            CF_AppData.Hk.AutoSuspend.LowFreeMark = (CF_AUTOSUSPEND_MAX_TRANS - CF_AutoSuspendCnt);                                               
                    
                    }else{
                    
                        CFE_EVS_SendEvent(CF_TRANS_SUSPEND_OVRFLW_EID,CFE_EVS_ERROR,
                                    "Out Trans %u not suspended.Buffer overflow, max %u",
                                    TransInfo.trans.number,CF_AUTOSUSPEND_MAX_TRANS);
    
                    }/* end if CF_AutoSuspendCnt is max'd*/
                
                }/* end if suspending is enabled */


                CF_AppData.Chan[Chan].DataBlast = CF_NOT_IN_PROGRESS;
                CF_AppData.Chan[Chan].TransNumBlasting = 0;

                if(CF_AppData.Tbl->OuCh[Chan].DequeueEnable == CF_ENABLED)
                {
                    /* Start transfer of next file on queue (if queue has another file) */                
                    CF_StartNextFile(Chan);
        
                }/* end if */                       
        
            }
#ifdef CF_DEBUG            
            else{

                if(cfdbg > 0)
                OS_printf("CF:EOF Sent:Trans %d not found in Active Queue\n",TransInfo.trans.number);   

            }/* end if */
#endif            
            break;
            

        case IND_EOF_RECV:                                   
            break;
            
        case IND_TRANSACTION_FINISHED:            
            break;


        case IND_MACHINE_DEALLOCATED:    
                        
            /* do transaction-success processing */ 
            if(TransInfo.final_status == FINAL_STATUS_SUCCESSFUL)
            {
                /* successful file-receive transaction processing */
                if( (TransInfo.role ==  CLASS_1_RECEIVER) || 
                    (TransInfo.role ==  CLASS_2_RECEIVER) )
                {

                    sprintf(EntityIdBuf,"%d.%d",TransInfo.trans.source_id.value[0],
                                        TransInfo.trans.source_id.value[1]);


                    QueueEntryPtr = CF_FindUpNodeByTransID(CF_UP_ACTIVEQ, EntityIdBuf, TransInfo.trans.number);
                    if(QueueEntryPtr != NULL)
                    {
                        QueueEntryPtr->Status = CF_STAT_SUCCESS;
                    }

                    CF_AppData.Hk.Up.SuccessCounter++;
                    strncpy(&CF_AppData.Hk.Up.LastFileUplinked[0],&TransInfo.md.dest_file_name[0],OS_MAX_PATH_LEN);
                    
                    CF_MoveUpNodeActiveToHistory(EntityIdBuf, TransInfo.trans.number);
                
                    CFE_EVS_SendEvent(CF_IN_TRANS_OK_EID,CFE_EVS_INFORMATION,
                                "Incoming trans success %d.%d_%d,dest %s",                                                            
                                TransInfo.trans.source_id.value[0],
                                TransInfo.trans.source_id.value[1],
                                TransInfo.trans.number,
                                &TransInfo.md.dest_file_name[0]);

                }else{
                
                    /* successful file-send transaction processing */
                    Chan = CF_GetChanNumFromTransId(CF_PB_ACTIVEQ, TransInfo.trans.number);
                    
                    if(Chan != CF_ERROR)
                    {                    
                        CF_AppData.Hk.Chan[Chan].SuccessCounter++;
                        QueueEntryPtr = CF_FindPbNodeByTransNum(Chan, CF_PB_ACTIVEQ, TransInfo.trans.number);
                        if(QueueEntryPtr != NULL)
                        {
                            QueueEntryPtr->Status = CF_STAT_SUCCESS;
                        }
                    }                                          
                
                    if(QueueEntryPtr->Preserve == CF_DELETE_FILE)
                    {                                               
                        OS_remove(&TransInfo.md.source_file_name[0]);
                    }                
                
                    CF_MoveDwnNodeActiveToHistory(TransInfo.trans.number);
                
                    CFE_EVS_SendEvent(CF_OUT_TRANS_OK_EID,CFE_EVS_INFORMATION,
                                    "Outgoing trans success %d.%d_%d,src %s",                                                            
                                    TransInfo.trans.source_id.value[0],
                                    TransInfo.trans.source_id.value[1],
                                    TransInfo.trans.number,
                                    &TransInfo.md.source_file_name[0]);                                
                }
                                                
            
            }else{
            
                /* do transaction-failed processing */
                sprintf(&CF_AppData.Hk.App.LastFailedTrans[0],"%d.%d_%lu",
                        TransInfo.trans.source_id.value[0],
                        TransInfo.trans.source_id.value[1],
                        TransInfo.trans.number);
                                                                             
                /* increment the corresponding telemetry counter */
                CF_IncrFaultCtr(&TransInfo);
                
                /* for error event below */
                CF_GetFinalStatString(LocalFinalStatBuf,
                                      TransInfo.final_status,
                                      CF_MAX_ERR_STRING_CHARS);
                
                /* for error event below */
                CF_GetCondCodeString(LocalCondCodeBuf,
                                     TransInfo.condition_code,
                                     CF_MAX_ERR_STRING_CHARS);


                /* failed file-receive transaction processing */
                if( (TransInfo.role ==  CLASS_1_RECEIVER) || 
                    (TransInfo.role ==  CLASS_2_RECEIVER) )
                {
                    CF_AppData.Hk.Up.FailedCounter++;                                    
                    
                    sprintf(EntityIdBuf,"%d.%d",TransInfo.trans.source_id.value[0],
                                        TransInfo.trans.source_id.value[1]);                    
                    
                    QueueEntryPtr = CF_FindUpNodeByTransID(CF_UP_ACTIVEQ, EntityIdBuf, TransInfo.trans.number);
                    
                    if(QueueEntryPtr != NULL)
                    {
                        QueueEntryPtr->Status = TransInfo.final_status;
                        QueueEntryPtr->CondCode = TransInfo.condition_code;
                    }

                    CF_MoveUpNodeActiveToHistory(EntityIdBuf, TransInfo.trans.number);                
                    
                    CFE_EVS_SendEvent(CF_IN_TRANS_FAILED_EID,CFE_EVS_ERROR,
                                "Incoming trans %d.%d_%d %s,CondCode %s,dest %s",                                                            
                                TransInfo.trans.source_id.value[0],
                                TransInfo.trans.source_id.value[1],
                                TransInfo.trans.number,
                                LocalFinalStatBuf,
                                LocalCondCodeBuf,
                                TransInfo.md.dest_file_name);

                }else{
                
                    /* failed file-send transaction processing */
                    Chan = CF_GetChanNumFromTransId(CF_PB_ACTIVEQ, TransInfo.trans.number);
                    
                    if(Chan >= CF_MAX_PLAYBACK_CHANNELS)
                    {
#ifdef CF_DEBUG                         
                        OS_printf("CF:Mach Dealloc:FileSend Err: Chan %d invalid\n",Chan);
#endif
                        return;
                    }                                                           
                    
                    CF_AppData.Hk.Chan[Chan].FailedCounter++;
                    QueueEntryPtr = CF_FindPbNodeByTransNum(Chan, CF_PB_ACTIVEQ, TransInfo.trans.number);
                    if(QueueEntryPtr != NULL)
                    {
                        QueueEntryPtr->Status = TransInfo.final_status;
                        QueueEntryPtr->CondCode = TransInfo.condition_code;
                    }
                                
                    CF_MoveDwnNodeActiveToHistory(TransInfo.trans.number);
                    
                    CFE_EVS_SendEvent(CF_OUT_TRANS_FAILED_EID,CFE_EVS_ERROR,
                                "Outgoing trans %d.%d_%d %s,CondCode %s,Src %s,Ch %d ",                                                            
                                TransInfo.trans.source_id.value[0],
                                TransInfo.trans.source_id.value[1],
                                TransInfo.trans.number,
                                LocalFinalStatBuf,
                                LocalCondCodeBuf,
                                TransInfo.md.source_file_name,
                                Chan);


                    /* if trans was aborted before EOF was sent, need to  */
                    /* start the next file. This could happen via flight-side */
                    /* abandon or gnd-side cancel */                   
                    if(TransInfo.trans.number == CF_AppData.Chan[Chan].TransNumBlasting)
                    {                    
                        CF_AppData.Chan[Chan].DataBlast = CF_NOT_IN_PROGRESS;
                        CF_AppData.Chan[Chan].TransNumBlasting = 0;
    
                        if(CF_AppData.Tbl->OuCh[Chan].DequeueEnable == CF_ENABLED){             
                            CF_StartNextFile(Chan);
                        }/* end if */
                    
                    }/* end if */

                } /* end if */                                          

            }/* end if */

            break;            


        case IND_ACK_TIMER_EXPIRED:
            CFE_EVS_SendEvent(CF_IND_ACK_TIM_EXP_EID,CFE_EVS_INFORMATION,
                              "Flight Ack Timer Expired %d.%d_%d,%s",
                              TransInfo.trans.source_id.value[0],
                              TransInfo.trans.source_id.value[1],
                              TransInfo.trans.number,
                              &TransInfo.md.source_file_name[0]);                      
            break;


        case IND_INACTIVITY_TIMER_EXPIRED:
            CFE_EVS_SendEvent(CF_IND_INA_TIM_EXP_EID,CFE_EVS_INFORMATION,
                              "Flight Inactivity Timer Expired %d.%d_%d,%s",
                              TransInfo.trans.source_id.value[0],
                              TransInfo.trans.source_id.value[1],
                              TransInfo.trans.number,
                              &TransInfo.md.source_file_name[0]);                                            
            break;


        case IND_NAK_TIMER_EXPIRED:
            CFE_EVS_SendEvent(CF_IND_NACK_TIM_EXP_EID,CFE_EVS_INFORMATION,
                              "Flight Nack Timer Expired %d.%d_%d,%s",
                              TransInfo.trans.source_id.value[0],
                              TransInfo.trans.source_id.value[1],
                              TransInfo.trans.number,
                              &TransInfo.md.source_file_name[0]);            
            break;     


        case IND_SUSPENDED:
            CFE_EVS_SendEvent(CF_IND_XACT_SUS_EID,CFE_EVS_INFORMATION,
                              "Transaction Susupended %d.%d_%d,%s",
                              TransInfo.trans.source_id.value[0],
                              TransInfo.trans.source_id.value[1],
                              TransInfo.trans.number,
                              &TransInfo.md.source_file_name[0]);
            break;

        case IND_RESUMED:      
            CFE_EVS_SendEvent(CF_IND_XACT_RES_EID,CFE_EVS_INFORMATION,
                              "Transaction Resumed %d.%d_%d,%s",
                              TransInfo.trans.source_id.value[0],
                              TransInfo.trans.source_id.value[1],
                              TransInfo.trans.number,
                              &TransInfo.md.source_file_name[0]);                                                                                                                            
            break;
            
        case IND_REPORT:                                                    
            break;
            
        case IND_FAULT:
            /*Fault was generated by the engine*/
            CFE_EVS_SendEvent(CF_IND_XACT_FAU_EID,CFE_EVS_DEBUG,
                              "Fault %d,%d.%d_%d,%s",
                              TransInfo.condition_code,
                              TransInfo.trans.source_id.value[0],
                              TransInfo.trans.source_id.value[1],
                              TransInfo.trans.number, 
                              &TransInfo.md.source_file_name[0]);                        
            break;


        case IND_ABANDONED:             

            CFE_EVS_SendEvent(CF_IND_XACT_ABA_EID,CFE_EVS_INFORMATION,
                              "Indication:Transaction Abandon %d.%d_%d,%s",
                              TransInfo.trans.source_id.value[0],
                              TransInfo.trans.source_id.value[1],
                              TransInfo.trans.number,
                              &TransInfo.md.source_file_name[0]);                                                                                                                                                                                                                         
            
            CF_AppData.Hk.App.TotalAbandonTrans++;
            
            break;


        default:
            CFE_EVS_SendEvent(CF_IND_UNEXP_TYPE_EID,CFE_EVS_INFORMATION,
                              "Unexpected Indication Type %d",IndType);
            break;                                                                                                                                                                                                                     
    
    }/* end switch */
                                      
    return;
}/* end of CF_Indication function*/



/*
**             Function Prologue
**
** Function Name: CF_PduOutputOpen
**
** Purpose: Return TRUE
**
** Input arguments:
**    Spacecraft Entity ID, and Destination Entity ID
**
** Return values:
**    boolean type which states comm is initialized or not
*/
boolean CF_PduOutputOpen (ID SourceId, ID DestinationId)
{
   return (YES);
}/* end of CF_PduOutputOpen function */



/*
**             Function Prologue
**
** Function Name: CF_PduOutputReady
**
** Purpose: Meters when cycle function is executed.
**
** Input arguments:
**     PDU_TYPE, TRANSACTION, ID (partner entity ID)
**
** Return values:
**    boolean type which states outer layer is ready to send pdu or not
*/
boolean CF_PduOutputReady (PDU_TYPE PduType, TRANSACTION TransInfo,ID DestinationId)
{
        
    int32   SemTakeRtn,Chan;
    char    SrcEntityIdBuf[CF_MAX_CFG_VALUE_CHARS];
        
    sprintf(SrcEntityIdBuf,"%d.%d",TransInfo.source_id.value[0],TransInfo.source_id.value[1]);

    /* if playback transaction... */
    /* For playback transactions source id in pdu is same as Flight Entity Id in table */ 
    if(strncmp(SrcEntityIdBuf,CF_AppData.Hk.Eng.FlightEngineEntityId,CF_MAX_CFG_VALUE_CHARS)==0)
    {                    
        Chan = CF_GetChanNumFromTransId(CF_PB_ACTIVEQ, TransInfo.number);
        /* if trans id not found in active queue, then this trans may be soley  */
        /* for ack fin. Try to get channel number from history queue */ 
        if(Chan == CF_ERROR)
        {
            Chan = CF_GetChanNumFromTransId(CF_PB_HISTORYQ, TransInfo.number);
            if(Chan == CF_ERROR)
            {
                /* Cannot find channel number for transaction. 
                ** Must let the engine release the undirected pdu. 
                ** Report error in CF_PduOutputSend         
                */                
                return TRUE;
            }
        }        
                    
    /* if not a playback transaction, must be a class 2 uplink response... */
    }else{
        
        Chan = CF_GetResponseChanFromTransId(CF_UP_ACTIVEQ, SrcEntityIdBuf, TransInfo.number);
        
        /* if trans id not found in active queue, try to get channel number from history queue */
        if(Chan == CF_ERROR)
        {
            Chan = CF_GetResponseChanFromTransId(CF_UP_HISTORYQ, SrcEntityIdBuf, TransInfo.number);
            if(Chan == CF_ERROR)
            {
                /* Cannot find channel number for transaction. 
                ** Must let the engine release the undirected pdu. 
                ** Report error in CF_PduOutputSend         
                */
                return TRUE;
            }
        } 

    }/* end if */

    if((Chan < 0) || (Chan >= CF_MAX_PLAYBACK_CHANNELS))
    {
        
        /* Cannot find channel number for transaction. 
        ** Must let the engine release the undirected pdu. 
        ** Report error in CF_PduOutputSend         
        */
        return TRUE;
    
    }
                
    /* if app receiving the PDU (TO) has failed to create semaphore,  */
    /* send pdus whenever engine is ready */
    if(CF_AppData.Chan[Chan].HandshakeSemId == CF_INVALID)
    {        
        CF_AppData.Hk.Chan[Chan].GreenLightCntr++;
        return TRUE;
                
    }/* end if */
        

    /* check handshake semaphore to see if downlink app (TO) is ready for PDU */
    SemTakeRtn = OS_CountSemTimedWait(CF_AppData.Chan[Chan].HandshakeSemId,0);
    if(SemTakeRtn == OS_SUCCESS){
            
        CF_AppData.Hk.Chan[Chan].GreenLightCntr++;
        return TRUE;
            
    }else{
        
        /* failed to get semaphore */
        CF_AppData.Hk.Chan[Chan].RedLightCntr++;
        CFE_ES_PerfLogEntry(CF_REDLIGHT_PERF_ID);
        CFE_ES_PerfLogExit(CF_REDLIGHT_PERF_ID);
        return FALSE;
    
    }/* end if */           
        
}/* end of CF_PduOutputReady function */


/*
**             Function Prologue
**
** Function Name: CF_PduOutputSend
**
** Purpose: PDU is delivered to software bus
**
** Input arguments:
**     Partner Entity ID, PDU data, and TRANSACTION status structure
**
** Return values:
**    (none)
*/
void CF_PduOutputSend (TRANSACTION TransInfo,ID DestinationId, CFDP_DATA *PduPtr)
{    

    int32       Chan = 0;
    int32       ZeroCpyRtn;
    uint8       *PduPtrInSBBuf;
    uint32      CmdOrTlmPkt;
    uint32      TotalMsgLength;/* purposely not uint16 (length check)*/
    char        SrcEntityIdBuf[CF_MAX_CFG_VALUE_CHARS];
        
            
    /* Find which channel to send packet on */

    /* determine if pdu is from file-send trans or file-receive trans */
    if(CFE_TST(PduPtr->content[0],CF_PDUHDR_DIRECTION_BIT))
    {    
        /* direction is 'toward the file sender' (class 2 file-receive response) */
        sprintf(SrcEntityIdBuf,"%d.%d",TransInfo.source_id.value[0],TransInfo.source_id.value[1]);

        Chan = CF_GetResponseChanFromTransId(CF_UP_ACTIVEQ, SrcEntityIdBuf, TransInfo.number);
        
        /* if trans id not found in active queue, try to get channel number from history queue */
        if(Chan == CF_ERROR)
        {
            Chan = CF_GetResponseChanFromTransId(CF_UP_HISTORYQ, SrcEntityIdBuf, TransInfo.number);
        }

    }else{
    
        /* direction is 'toward the file receiver' (file-send transaction) */                 
        Chan = CF_GetChanNumFromTransId(CF_PB_ACTIVEQ, TransInfo.number);
        
        if(Chan == CF_ERROR)
        {            
            /* If trans id not found in active queue, then this trans may be  */
            /* soley for ack fin. Try to get channel number from history queue*/ 
            Chan = CF_GetChanNumFromTransId(CF_PB_HISTORYQ, TransInfo.number);
        }
                
    }/* end if */

    if((Chan < 0) || (Chan >= CF_MAX_PLAYBACK_CHANNELS))
    {
        /* cannot find channel number for transaction. Drop pdu and report error */ 
        CFE_EVS_SendEvent(CF_OUT_SND_ERR1_EID,CFE_EVS_ERROR,
            "Dropping PDU,cannot find channel number for TransId %d.%d_%d",            
            TransInfo.source_id.value[0],
            TransInfo.source_id.value[1],
            TransInfo.number);
        /* cannot give semaphore here because channel is unknown, making sem id unknown */
        return;
    }


    /* Get the CCSDS message length, for the SB buffer request ... */
    /* total message length is dependent on whether it's a cmd or tlm packet */
    CmdOrTlmPkt = CF_GetPktType(CF_AppData.Tbl->OuCh[Chan].OutgoingPduMsgId);

    if(CF_SEND_FIXED_SIZE_PKTS==0)
    {
        if(CmdOrTlmPkt==CF_TLM)
        {        
            TotalMsgLength = (PduPtr->length) + CFE_SB_TLM_HDR_SIZE;
        
        }else{
    
            TotalMsgLength = (PduPtr->length) + CFE_SB_CMD_HDR_SIZE;
        
        }/* end if */
    

        /* if total msg length exceeds cfe max... */
        if(TotalMsgLength > CFE_SB_MAX_SB_MSG_SIZE)
        {
            CFE_EVS_SendEvent(CF_OUT_SND_ERR2_EID,CFE_EVS_ERROR,
                "Dropping PDU,Ch %d,Msg Size %d > Max %d,TransId %d.%d_%d",            
                Chan,
                TotalMsgLength,
                CFE_SB_MAX_SB_MSG_SIZE,
                TransInfo.source_id.value[0],
                TransInfo.source_id.value[1],
                TransInfo.number);
            OS_CountSemGive(CF_AppData.Chan[Chan].HandshakeSemId);        
            return;
        }

    }else{
    
        /* Check of TotalMsgLength > CFE_SB_MAX_SB_MSG_SIZE not needed here */
        /* because it is checked at compile time in the cf_verify.h file */
        TotalMsgLength = CF_SEND_FIXED_SIZE_PKTS;
    
    }/* end if */

    /* request buffer from software bus */ 
    CF_AppData.Chan[Chan].ZeroCpyMsgPtr = CFE_SB_ZeroCopyGetPtr(
                                        TotalMsgLength,
                                        &CF_AppData.Chan[Chan].ZeroCpyHandle);
                                        
    /* if we fail to get a buffer from software bus, drop pdu and report error */
    if(CF_AppData.Chan[Chan].ZeroCpyMsgPtr == NULL)
    {
        CFE_EVS_SendEvent(CF_OUT_SND_ERR3_EID,CFE_EVS_ERROR,
            "Dropping PDU,Ch %d,Failed to get SB buffer %d,TransId %d.%d_%d",            
            Chan,
            TotalMsgLength,
            TransInfo.source_id.value[0],
            TransInfo.source_id.value[1],
            TransInfo.number);
        OS_CountSemGive(CF_AppData.Chan[Chan].HandshakeSemId);
        return;
    } 


    /* Initialize the CCSDS pkt with the (table param) MsgId and the */
    /* TotalMsgLength calculated above */
    CFE_SB_InitMsg(CF_AppData.Chan[Chan].ZeroCpyMsgPtr,
                    (CFE_SB_MsgId_t)CF_AppData.Tbl->OuCh[Chan].OutgoingPduMsgId,
                    0, TRUE);
    
    CFE_SB_SetTotalMsgLength(CF_AppData.Chan[Chan].ZeroCpyMsgPtr,
                             (uint16)TotalMsgLength);

    /* Setup the destination pointer (in SB buffer) for the pdu mem copy... */
    /* ...first set the pointer to the start of the SB buffer....*/
    PduPtrInSBBuf = (uint8 *)CF_AppData.Chan[Chan].ZeroCpyMsgPtr;
    
    /* ...then advance the pointer past the CCSDS hdr, */
    /* to the first byte of the pdu hdr */
    if(CmdOrTlmPkt==CF_TLM)
    {        
        PduPtrInSBBuf += CFE_SB_TLM_HDR_SIZE;
    
    }else{

        PduPtrInSBBuf += CFE_SB_CMD_HDR_SIZE;
    
    }/* end if */
    
    /* Copy pdu from engine buffer to Software Bus buffer */  
    CFE_PSP_MemCpy(PduPtrInSBBuf,&PduPtr->content[0],PduPtr->length);
    
#ifdef CF_DEBUG
    if(cfdbg > 0){
        if(CFE_TST(PduPtr->content[0],CF_PDUHDR_PDUTYPE_BIT)){
            OS_printf("CF:Sending File Data PDU, len=%d\n",PduPtr->length);
        }else{
            OS_printf("CF:Sending ");
            CF_PrintPDUType(PduPtr->content[12],
                            PduPtr->content[13]);
            OS_printf(" PDU,len=%d\n",PduPtr->length);
        }/* end if */

    }/* end if */ 
#endif

    CFE_SB_TimeStampMsg(CF_AppData.Chan[Chan].ZeroCpyMsgPtr);   
    ZeroCpyRtn = CFE_SB_ZeroCopySend(CF_AppData.Chan[Chan].ZeroCpyMsgPtr,
                          CF_AppData.Chan[Chan].ZeroCpyHandle);

    if(ZeroCpyRtn != CFE_SUCCESS){
      OS_CountSemGive(CF_AppData.Chan[Chan].HandshakeSemId);
      return;
    }       

    CF_AppData.Hk.Chan[Chan].PDUsSent++; 

}/* end of CF_PduOutputSend */




/*
**             Function Prologue
**
** Function Name: CF_RenameFile
**
** Purpose: Copies data from the old filename to the new filename and deletes the old filename.
**          NOTE: POSIX rename() does not work across devices or partitions therefore the function had to be
*           implemented this way. This rename callback is used for file-receive direction only.
**          
**
** Input arguments:
**    const char*,const char*
**
** Return values:
**    int
*/

/*This is how much file data will be read and written at the most per call.*/
#define CF_RENAME_BUF    1024
int CF_RenameFile(const char *TempFileName, const char *NewName)
{
    int32    OldFd,NewFd,Status;
    uint32   NumReadFromOld,NumWrittenToNew;
    int32    FileStorage[CF_RENAME_BUF];
    
#ifdef CF_DEBUG
    if(cfdbg > 5)
    OS_printf("CF_RenameFile\n");
#endif

    /*Use POSIX return status for the CFDP library*/
    OldFd = 0; /*initialize*/
    NewFd = 0; /*initialize*/
    Status = 0; /*SUCCESS!*/
    CFE_PSP_MemSet(&FileStorage[0],'\0',CF_RENAME_BUF);    
    
    if((OldFd = OS_open(TempFileName,OS_READ_WRITE,0)) < OS_FS_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_FILE_IO_ERR1_EID,
                          CFE_EVS_ERROR,
                         "Unable to open file = %s!",TempFileName);   
        Status = 1;                                   
    }
    
    if((NewFd = OS_creat(NewName,OS_READ_WRITE)) < OS_FS_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_FILE_IO_ERR2_EID,
                          CFE_EVS_ERROR,
                         "Unable to create file = %s!",NewName);   
        Status = 1;                                   
    }
    
    
    if(Status != 1)
    {
        while((NumReadFromOld = OS_read(OldFd,(void*)&FileStorage[0],CF_RENAME_BUF)) != OS_FS_SUCCESS)
        {
            if((NumWrittenToNew = OS_write(NewFd,(void*)&FileStorage[0],NumReadFromOld)) != NumReadFromOld)
            {
                CFE_EVS_SendEvent(CF_FILE_IO_ERR3_EID,
                                  CFE_EVS_ERROR,
                                 "File write error! Should have written  = %d bytes but only wrote %d bytes to the file!",
                                 NumReadFromOld,NumWrittenToNew);
                Status = 1;                                 
            }
            /*don't want left over data from the last buffer written and I want to terminate filedata with a NULL*/
            CFE_PSP_MemSet(&FileStorage[0],'\0',CF_RENAME_BUF);
        }
        OS_close(OldFd);
        OS_close(NewFd);  
        if((OS_remove(TempFileName)) != OS_FS_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_REMOVE_ERR1_EID, 
                              CFE_EVS_ERROR,
                              "Could not remove file %s in rename callback.",TempFileName);
            Status = 1;                              
        }
    }
    
    return(Status);
}/* end of CF_Rename function */


/*
**             Function Prologue
**
** Function Name: CF_RemoveFile
**
** Purpose: Deletes the file off the filesystem. The *Name is an absolute path with the filename!
**          
**
** Input arguments:
**    const char*
**
** Return values:
**    int(same as int32)
*/
int CF_RemoveFile(const char *Name)
{
    int32 Status;

#ifdef CF_DEBUG
    if(cfdbg > 5)
        OS_printf("CF_RemoveFile %s\n",Name);
#endif

    /*Use POSIX return status for the CFDP library*/
    Status = 0; /*SUCCESS!*/
    
    if((OS_remove(Name)) != OS_FS_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_REMOVE_ERR2_EID, 
                          CFE_EVS_ERROR,
                          "Could not remove file %s in remove callback",Name);
        Status = 1;                              
    }


    return(Status);
}/* end of CF_RemoveFile function*/


/*
**             Function Prologue
**
** Function Name: CF_Fopen
**
** Purpose: Fopen callback needed because of logical pathnames to physical pathname translation. OS_API does not have any 
*           file I/O calls yet as of version cFE-4.01.
**          
**
** Input arguments:
**    const char*,const char*
**
** Return values:
**    CFDP_FILE *(same as int32 in ANSI-C. This should match what is in cfdp_config.h)
*/
CFDP_FILE * CF_Fopen(const char *Name, const char *Mode)
{
    int32   FileHandle;
                
    CFE_ES_PerfLogEntry(CF_FOPEN_PERF_ID);            

    if(((strcmp(Mode,"r")) == 0) || ((strcmp(Mode,"rb")) == 0))    
    {
        if((FileHandle = CF_Tmpopen(Name,OS_READ_ONLY,0)) < OS_SUCCESS)
        {
#ifdef CF_DEBUG
            if(cfdbg > 1)
                OS_printf("OS_open returned %x for File %s\n",FileHandle,Name);
#endif
            FileHandle = 0;/*error creating file*/        
        }
        /*OS_printf("File Handle %d\n",FileHandle);*/
    }
    else if(((strcmp(Mode,"rw")) == 0) || ((strcmp(Mode,"rwb")) == 0))
    {
        if((FileHandle = CF_Tmpopen(Name,OS_READ_WRITE,0)) < OS_SUCCESS)
        {
            FileHandle = 0;/*error creating file*/        
        }                
    }
    else if(((strcmp(Mode,"w")) == 0) || ((strcmp(Mode,"wb")) == 0))
    {    
        /* fopen has the ability to create a file if it does not exist
         * open does not so when the mode signature matches a write, create the
         * file. 
         * This function is always called. the temporary file is opened during a
         * file being received or uploaded.  
         */    
        if((FileHandle = CF_Tmpcreat(Name,OS_READ_WRITE)) < OS_FS_SUCCESS)
        {
            FileHandle = 0;/*error creating file*/                                                                    
        }        
    }
    else
    {
        /*There are too many combinations therefore to be on the safe side file open the file for read/write*/
        if((FileHandle = CF_Tmpopen(Name,OS_READ_WRITE,0)) < OS_SUCCESS)
        {
            FileHandle = 0;/*error creating file*/        
        }                        
    }      
        
    CFE_ES_PerfLogExit(CF_FOPEN_PERF_ID);

    return((CFDP_FILE *)FileHandle);

}/* end of CF_Fopen function */



/*
**             Function Prologue
**
** Function Name: CF_FileSize
**
** Purpose: Filesize callback needed because of logical pathnames to physical pathname translation. OS_API does not have any 
*           file I/O calls yet as of version cFE-4.01. The stat call needed the physical device name, not the logical.
**          
**
** Input arguments:
**    const char*
**
** Return values:
**    u_int_4
*/
u_int_4 CF_FileSize(const char *Name)
{
    os_fstat_t          OsStatBuf;        
    int32               StatVal;    
    u_int_4             FileSize;
    
    CFE_ES_PerfLogEntry(CF_FILESIZE_PERF_ID);  

    FileSize = 0;   

#ifdef CF_DEBUG
    if(cfdbg > 5)
        OS_printf("CF_FileSize %s\n",Name);
#endif

    StatVal = OS_stat(Name,&OsStatBuf);
    if(StatVal >= OS_FS_SUCCESS)
    {
        FileSize = OsStatBuf.st_size;        
    }
    else
    {
        CFE_EVS_SendEvent (CF_LOGIC_NAME_ERR_EID,
                           CFE_EVS_ERROR,
                           "The file %s size could not be retrieved because it does not exist.",Name);                
    }    

    CFE_ES_PerfLogExit(CF_FILESIZE_PERF_ID);

    return(FileSize);
}/* end of CF_FileSize function */

/*
**             Function Prologue
**
** Function Name: CF_Fseek
**
** Purpose: fseek callback needed for non-buffered I/O.**          
**
** Input arguments:
**    CFDP_FILE *, long int, int
**
** Return values:
**    int
*/
int CF_Fseek(CFDP_FILE *File, long int Offset, int Whence)
{
    int     ReturnVal;
    uint16  WhenceVal;
    int32   SeekVal;
    
    WhenceVal = 0; /*initialization*/
    ReturnVal = OS_FS_SUCCESS;/*success = 0, nonzero = error*/
    
#ifdef CF_DEBUG
    if(cfdbg > 5)
        OS_printf("CF_Fseek\n");
#endif

    if(Whence == SEEK_SET)
    {
        WhenceVal = OS_SEEK_SET;   
    }else if(Whence == SEEK_CUR)
    {
        WhenceVal = OS_SEEK_CUR;           
    }else if(Whence == SEEK_END)
    {
        WhenceVal = OS_SEEK_END;           
    }
    
    SeekVal = CF_Tmplseek((int32)File,(int32)Offset,WhenceVal);
    
    if(SeekVal == OS_FS_ERROR)
    {
        ReturnVal = 1;
    }
    else
    {
        ReturnVal = OS_FS_SUCCESS;        
    }
    
    return(ReturnVal);
}/* end of CF_Fseek function */


/*
**             Function Prologue
**
** Function Name: CF_Fread
**
** Purpose: fread callback needed for non-buffered I/O.
**          
**
** Input arguments:
**    void *, size_t,size_t, CFDP_FILE *
**
** Return values:
**    size_t
int CF_Fread(void *Buffer, size_t Size,size_t Count, CFDP_FILE *File);*/

size_t CF_Fread(void *Buffer, size_t Size,size_t Count, CFDP_FILE *File)
{
    int32 BytesRead;
    int32 ReturnCount;

    CFE_ES_PerfLogEntry(CF_FREAD_PERF_ID);

    if((Size == 0) || (Count == 0))
    {
       ReturnCount = 0;
    }
    else
    {
    
       BytesRead = CF_Tmpread((uint32)File,Buffer,(uint32)(Size*Count));
       if(BytesRead <= 0)  
       {  /* The OS_read call will return a negative value of failure 
          **  which in an invalid return for the fread call expected by the engine.
          **  So, a zero is returned.
          */
          ReturnCount = 0;
       }
       else
       {
          ReturnCount = BytesRead/Size;
       }
    }
    
   CFE_ES_PerfLogExit(CF_FREAD_PERF_ID);
   
   return(ReturnCount);
}/* end of CF_Fread function */


/*
**             Function Prologue
**
** Function Name: CF_Fwrite
**
** Purpose: fwrite callback needed for non-buffered I/O.
**          
**
** Input arguments:
**    const void *, size_t,size_t, CFDP_FILE *
**
** Return values:
**    size_t
*/
size_t CF_Fwrite(const void *Buffer, size_t Size,size_t Count, CFDP_FILE *File)
{
    int32 BytesWritten;
    int32 ReturnCount;
    
    CFE_ES_PerfLogEntry(CF_FWRITE_PERF_ID);

    if((Size == 0) || (Count == 0))
    {
       ReturnCount = 0;
    }
    else
    {
       BytesWritten = CF_Tmpwrite((uint32)File,(void*)Buffer,(uint32)(Size*Count));
       if(BytesWritten <= 0)  
       {  /* The OS_read call will return a negative value of failure 
          **  which in an invalid return for the fread call expected by the engine.
          **  So, a zero is returned.
          */
          ReturnCount = 0;
       }
       else
       {
          ReturnCount = BytesWritten/Size;
       }
    }
        
   CFE_ES_PerfLogExit(CF_FWRITE_PERF_ID);
   
   return(ReturnCount);
    
}/* end of CF_Fread function */

/*
**             Function Prologue
**
** Function Name: CF_Fclose
**
** Purpose: fclose callback needed for non-buffered I/O.
**          
**
** Input arguments:
**    CFDP_FILE *
**
** Return values:
**    int
*/
int CF_Fclose(CFDP_FILE *File)
{
    int32  CloseVal;
    
    CFE_ES_PerfLogEntry(CF_FCLOSE_PERF_ID);

    CloseVal = CF_Tmpclose((uint32)File);
    
    if(CloseVal != OS_FS_SUCCESS)
    {
        CFE_EVS_SendEvent (CF_FILE_CLOSE_ERR_EID,
                           CFE_EVS_ERROR,
                           "Could not close file from callback function! OS_close Val = %d",CloseVal);
    }
    
    CFE_ES_PerfLogExit(CF_FCLOSE_PERF_ID);
    
    return((int)CloseVal);
}/* end of CF_Fclose function */



#define CF_FD_ZERO_REPLACEMENT 0x7FFFFFFF
int32   CF_Tmpcreat  (const char *path, int32  access)
{

    int32 fd;

    /* returns 0 and positive for success, neg for error */
    fd = OS_creat(path,access);
    if(fd == 0) fd = CF_FD_ZERO_REPLACEMENT;
    
    return fd;

}

int32   CF_Tmpopen   (const char *path,  int32 access,  uint32 mode)
{

    int32 fd;

    /* returns 0 and positive for success, neg for error */
    fd = OS_open(path,access,mode);
    if(fd == 0) fd = CF_FD_ZERO_REPLACEMENT;
    
    return fd;

}

int32   CF_Tmpclose(int32  filedes)
{

    if(filedes == CF_FD_ZERO_REPLACEMENT) filedes = 0;
    return (OS_close(filedes));
}

int32   CF_Tmpread(int32  filedes, void *buffer, uint32 nbytes)
{

    if(filedes == CF_FD_ZERO_REPLACEMENT) filedes = 0;
     return (OS_read(filedes,buffer,nbytes));

}

int32   CF_Tmpwrite(int32  filedes, void *buffer, uint32 nbytes)
{
    if(filedes == CF_FD_ZERO_REPLACEMENT) filedes = 0;
     return(OS_write(filedes,buffer,nbytes));
}

int32   CF_Tmplseek  (int32  filedes, int32 offset, uint32 whence)
{

    if(filedes == CF_FD_ZERO_REPLACEMENT) filedes = 0;
        return (OS_lseek((uint32)filedes,offset,whence));
}



/*
**             Function Prologue
**
** Function Name: CF_DebugEvent (Code 582 Library Supplement Function)
**
** Purpose: Callback function for Debug i.e. printf(). Also generates Event messages
**          for cFE (This is the callback for all event messages for the CFDP engine.
**          
**
** Input arguments:
**    The user input string(const char *)
**
** Return values:
**    (none)
*/
int CF_DebugEvent(const char *Format, ...)
{
    va_list         ArgPtr;
    char            BigBuf[CFE_EVS_MAX_MESSAGE_LENGTH];
    uint32          Status,i;    

    va_start (ArgPtr, Format); 
    vsnprintf(BigBuf,CFE_EVS_MAX_MESSAGE_LENGTH,Format,ArgPtr); 
    va_end (ArgPtr);   

    for (i=0;i<CFE_EVS_MAX_MESSAGE_LENGTH;i++){
      if(BigBuf[i] == '\n'){
          BigBuf[i] = '\0';
          break;
      }
    }

    Status = CFE_EVS_SendEvent(CF_CFDP_ENGINE_DEB_EID,
                      CFE_EVS_DEBUG,
                      BigBuf);
                        

    return(Status);
    
}/* end of CF_DebugEvent function */



/*
**             Function Prologue
**
** Function Name: CF_InfoEvent (Code 582 Library Supplement Function)
**
** Purpose: Callback function for Information i.e. printf(). Also generates Event messages
**          for cFE (This is the callback for all event messages for the CFDP engine.
**          Note all of the informational messages are labeled as DEBUG for EVS services. 
**          
**
** Input arguments:
**    The user input string(const char *)
**
** Return values:
**    (none)
*/
int CF_InfoEvent(const char *Format, ...)
{
    va_list         ArgPtr;
    char            BigBuf[CFE_EVS_MAX_MESSAGE_LENGTH];
    uint32          Status,i;
        
    va_start (ArgPtr, Format);
    vsnprintf(BigBuf,CFE_EVS_MAX_MESSAGE_LENGTH,Format,ArgPtr);  
    va_end (ArgPtr);
      
    for (i=0;i<CFE_EVS_MAX_MESSAGE_LENGTH;i++){
      if(BigBuf[i] == '\n'){
          BigBuf[i] = '\0';
          break;
      }
    }

    Status = CFE_EVS_SendEvent(CF_CFDP_ENGINE_INFO_EID,
                               CFE_EVS_DEBUG,
                               BigBuf);

    return(Status);
    
}/* end of CF_InfoEvent function */


/*
**             Function Prologue
**
** Function Name: CF_WarningEvent (Code 582 Library Supplement Function)
**
** Purpose: Callback function for Warning i.e. printf(). Also generates Event messages
**          for cFE (This is the callback for all event messages for the CFDP engine.
**          
**
** Input arguments:
**    The user input string(const char *)
**
** Return values:
**    (none)
*/
int CF_WarningEvent(const char *Format, ...)
{
    va_list         ArgPtr;
    char            BigBuf[CFE_EVS_MAX_MESSAGE_LENGTH];
    uint32          Status,i;
        
    va_start (ArgPtr, Format);
    vsnprintf(BigBuf,CFE_EVS_MAX_MESSAGE_LENGTH, Format,ArgPtr);  
    va_end (ArgPtr);     
    
    for (i=0;i<CFE_EVS_MAX_MESSAGE_LENGTH;i++){
      if(BigBuf[i] == '\n'){
          BigBuf[i] = '\0';
          break;
      }
    }

    Status = CFE_EVS_SendEvent(CF_CFDP_ENGINE_WARN_EID,
                      CFE_EVS_INFORMATION,
                      BigBuf);            
    
    return(Status);
    
}/* end of CF_WarningEvent function */


/*
**             Function Prologue
**
** Function Name: CF_ErrorEvent (Code 582 Library Supplement Function)
**
** Purpose: Callback function for Error i.e. printf(). Also generates Event messages
**          for cFE (This is the callback for all event messages for the CFDP engine.
**          
**
** Input arguments:
**    The user input string(const char *)
**
** Return values:
**    (none)
*/
int CF_ErrorEvent(const char *Format, ...)
{
    va_list         ArgPtr;
    char            BigBuf[CFE_EVS_MAX_MESSAGE_LENGTH];
    uint32          Status,i;

    va_start (ArgPtr, Format);
    vsnprintf(BigBuf,CFE_EVS_MAX_MESSAGE_LENGTH,Format,ArgPtr);    
    va_end (ArgPtr);
    
    for (i=0;i<CFE_EVS_MAX_MESSAGE_LENGTH;i++){
      if(BigBuf[i] == '\n'){
          BigBuf[i] = '\0';
          break;
      }
    }

    Status = CFE_EVS_SendEvent(CF_CFDP_ENGINE_ERR_EID,
                      CFE_EVS_ERROR,
                      BigBuf);            
    
    return(Status);
    
}/* end of CF_ErrorEvent function*/



/*  NOTE:
**  This fuction assumes that the head of the list (back of the queue)
**  contains a newly added node that needs to be inserted at the proper 
**  priority level.
*/
int32 CF_PendingQueueSort(uint8 Channel)
{

    CF_QueueEntry_t *NewNodePtr;
    CF_QueueEntry_t *StepNodePtr;
    

    if(CF_AppData.Chan[Channel].PbQ[CF_PB_PENDINGQ].HeadPtr == NULL)
    {
        /* List should never be empty when calling this function */
        return  CF_ERROR;
    }

    NewNodePtr = CF_AppData.Chan[Channel].PbQ[CF_PB_PENDINGQ].HeadPtr;

    StepNodePtr = NewNodePtr->Next;

    if(StepNodePtr == NULL)
    {
        /* New node is the only node */
        return CF_SUCCESS;
    }

    /* if NewNode priority is lower(higher value) or equal to the node at the 
       back of the queue...  */    
    if(NewNodePtr->Priority >= StepNodePtr->Priority)
    {
        /* New node is currently in the correct position */
        return CF_SUCCESS;
    }
    
    StepNodePtr = StepNodePtr->Next;
        
    /* remove new node from end of queue (in will be inserted in the */
    /* correct position below */
    CF_RemoveFileFromPbQueue(Channel, CF_PB_PENDINGQ, NewNodePtr);

    /* traverse the list and insert the new node behind the next highest priority file */
    /* if priority matches an existing node, insert new node behind the existing node */
    while(StepNodePtr != NULL)
    {
        if(NewNodePtr->Priority >= StepNodePtr->Priority)
        {
            /* insert new node at proper location (before step node) */
            CF_InsertPbNode(Channel,CF_PB_PENDINGQ, NewNodePtr, StepNodePtr);

            return CF_SUCCESS;
        }
        
        StepNodePtr = StepNodePtr->Next;
    }
   
    /* Stepped past the front of the queue, insert new node at front */
    CF_InsertPbNodeAtFront(Channel,CF_PB_PENDINGQ, NewNodePtr);
    
    return CF_SUCCESS;

}/* end CF_PendingQueueSort */


/************************/
/*  End of File Comment */
/************************/
