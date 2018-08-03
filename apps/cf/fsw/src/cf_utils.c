/************************************************************************
** File:
**   $Id: cf_utils.c 1.22.1.1 2015/03/06 15:30:38EST sstrege Exp  $
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
**  The CFS CF Application file containing 
**
** Notes:
**
** $Log: cf_utils.c  $
** Revision 1.22.1.1 2015/03/06 15:30:38EST sstrege 
** Added copyright information
** Revision 1.22 2011/05/10 14:38:39EDT rmcgraw 
** Removed event id 25 in CF_MoveDwnNodeActiveToHistory
** Revision 1.21 2011/05/09 11:52:11EDT rmcgraw 
** DCR13317:1 Allow Destintaion path to be blank
** Revision 1.20 2011/05/03 16:47:24EDT rmcgraw 
** Cleaned up events, removed \n from some events, removed event id ganging
** Revision 1.19 2010/11/04 11:37:49EDT rmcgraw 
** Dcr13051:1 Wrap OS_printfs in platform cfg CF_DEBUG
** Revision 1.18 2010/11/01 16:09:37EDT rmcgraw 
** DCR12802:1 Changes for decoupling peer entity id from channel
** Revision 1.17 2010/10/25 11:21:50EDT rmcgraw 
** DCR12573:1 Changes to allow more than one incoming PDU MsgId
** Revision 1.16 2010/10/21 13:48:11EDT rmcgraw 
** DCR13060:2 Changed FindUpNodebyTransnum to FindUpNodeByTransID
** Revision 1.15 2010/10/20 14:55:56EDT rmcgraw 
** DCR12825:1 Change quick stat cmd to show active and suspended status
** Revision 1.14 2010/08/06 18:46:00EDT rmcgraw 
** Dcr11510:1 Fixed cfg params with buffer sizes
** Revision 1.13 2010/07/20 14:37:40EDT rmcgraw 
** Dcr11510:1 Remove Downlink buffer references
** Revision 1.12 2010/07/08 13:47:32EDT rmcgraw 
** DCR11510:1 Added termination checking on all cmds that take a string
** Revision 1.11 2010/07/07 17:41:07EDT rmcgraw 
** DCR11510:1 Added string length checks in BuildPutRequest
** Revision 1.10 2010/06/11 16:13:18EDT rmcgraw 
** DCR11510:1 ZeroCopy, Un-hardcoded cmd/tlm input/output pdus
** Revision 1.9 2010/06/01 11:02:32EDT rmcgraw 
** DCR11510:1 Removed CheckForGapsInTrans
** Revision 1.8 2010/04/27 09:10:26EDT rmcgraw 
** DCR11510:1 Fix for filename as param in Cancel/Suspend/Resume/Abandon
** Revision 1.7 2010/04/26 16:23:23EDT rmcgraw 
** DCR11510:1 Fixed missing trans num in CF_FindActiveTransIdByName
** Revision 1.6 2010/04/23 15:45:54EDT rmcgraw 
** DCR11510:1 Protection against starting a transfer if file is already active
** Revision 1.5 2010/04/23 08:39:15EDT rmcgraw 
** Dcr11510:1 Code Review Prep
** Revision 1.4 2010/03/26 15:30:23EDT rmcgraw 
** DCR11510 Various developmental changes
** Revision 1.3 2010/03/12 12:14:38EST rmcgraw 
** DCR11510:1 Initial check-in towards CF Version 1000
** Revision 1.2 2009/12/08 09:18:17EST rmcgraw 
** DCR10350:3 Added CF_ShowCfg function
** Revision 1.1 2009/11/24 12:48:56EST rmcgraw 
** Initial revision
** Member added to CFS CF project
**
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "cf_app.h"
#include "cf_defs.h"
#include "cf_utils.h"
#include "cf_verify.h"
#include "cf_events.h"
#include "cf_playback.h"
#include "cfdp_provides.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef CF_DEBUG
extern uint32 cfdbg;
#endif


CF_QueueEntry_t *CF_FindUpHistoryNodeByName(char *SrcFile)
{
    CF_QueueEntry_t *QNodePtr;
    
    QNodePtr = CF_AppData.UpQ[CF_UP_HISTORYQ].HeadPtr;            
    while(QNodePtr != NULL)
    {                
        if(strncmp(QNodePtr->SrcFile,SrcFile,OS_MAX_PATH_LEN) == 0) 
                return QNodePtr;

        
        QNodePtr = QNodePtr->Next;
    }
    
    return NULL;

}



CF_QueueEntry_t *CF_FindUpActiveNodeByName(char *SrcFile)
{

    CF_QueueEntry_t *QNodePtr;
    
    QNodePtr = CF_AppData.UpQ[CF_UP_ACTIVEQ].HeadPtr;            
    while(QNodePtr != NULL)
    {                
        if(strncmp(QNodePtr->SrcFile,SrcFile,OS_MAX_PATH_LEN) == 0)
                return QNodePtr;

        
        QNodePtr = QNodePtr->Next;
    }
    
    return NULL;

}



CF_QueueEntry_t *CF_FindUpNodeByName(char *SrcFile)
{

    CF_QueueEntry_t *QNodePtr;
    
    QNodePtr = CF_FindUpActiveNodeByName(SrcFile);
    if(QNodePtr != NULL)
        return QNodePtr;
        
    QNodePtr = CF_FindUpHistoryNodeByName(SrcFile);
    if(QNodePtr != NULL)
        return QNodePtr; 
        
    return NULL;

}



CF_QueueEntry_t *CF_FindPbHistoryNodeByName(char *SrcFile)
{

    CF_QueueEntry_t *QNodePtr;
    uint32          i;
    
    for(i=0;i < CF_MAX_PLAYBACK_CHANNELS;i++)
    {
        if(CF_AppData.Tbl->OuCh[i].EntryInUse == CF_ENTRY_IN_USE)
        {
            QNodePtr = CF_AppData.Chan[i].PbQ[CF_PB_HISTORYQ].HeadPtr;            
            while(QNodePtr != NULL)
            {                
                if(strncmp(QNodePtr->SrcFile,SrcFile,OS_MAX_PATH_LEN) == 0) 
                        return QNodePtr;
        
                
                QNodePtr = QNodePtr->Next;
            }
                        
        }
    }

    return NULL;

}

CF_QueueEntry_t *CF_FindPbActiveNodeByName(char *SrcFile)
{

    CF_QueueEntry_t *QNodePtr;
    uint32          i;
    
    for(i=0;i < CF_MAX_PLAYBACK_CHANNELS;i++)
    {
        if(CF_AppData.Tbl->OuCh[i].EntryInUse == CF_ENTRY_IN_USE)
        {
            QNodePtr = CF_AppData.Chan[i].PbQ[CF_PB_ACTIVEQ].HeadPtr;            
            while(QNodePtr != NULL)
            {                
                if(strncmp(QNodePtr->SrcFile,SrcFile,OS_MAX_PATH_LEN) == 0) 
                        return QNodePtr;
        
                
                QNodePtr = QNodePtr->Next;
            }
            
        }
    }
    
    return NULL;
}



CF_QueueEntry_t *CF_FindPbPendingNodeByName(char *SrcFile)
{

    CF_QueueEntry_t *QNodePtr;
    uint32          i;
    
    for(i=0;i < CF_MAX_PLAYBACK_CHANNELS;i++)
    {
        if(CF_AppData.Tbl->OuCh[i].EntryInUse == CF_ENTRY_IN_USE)
        {
            QNodePtr = CF_AppData.Chan[i].PbQ[CF_PB_PENDINGQ].HeadPtr;            
            while(QNodePtr != NULL)
            {                
                if(strncmp(QNodePtr->SrcFile,SrcFile,OS_MAX_PATH_LEN) == 0)
                        return QNodePtr;
        
                
                QNodePtr = QNodePtr->Next;
            }
            
        }
    }

    return NULL;

}


CF_QueueEntry_t *CF_FindPbNodeByName(char *SrcFile)
{

    CF_QueueEntry_t *QNodePtr;
    
    QNodePtr = CF_FindPbActiveNodeByName(SrcFile);
    if(QNodePtr != NULL)
        return QNodePtr;
        
    QNodePtr = CF_FindPbPendingNodeByName(SrcFile);
    if(QNodePtr != NULL)
        return QNodePtr;

    QNodePtr = CF_FindPbHistoryNodeByName(SrcFile);
    if(QNodePtr != NULL)
        return QNodePtr; 
        
    return NULL;

}



CF_QueueEntry_t *CF_FindNodeByName(char *SrcFile)
{

    CF_QueueEntry_t *QNodePtr;

    /* check uplink queues */
    QNodePtr = CF_FindUpNodeByName(SrcFile);
    if(QNodePtr != NULL)
        return QNodePtr;
        
    /* check playback queues */
    QNodePtr = CF_FindPbNodeByName(SrcFile);
    if(QNodePtr != NULL)
        return QNodePtr;    
    
    return NULL;

}





CF_QueueEntry_t *CF_FindUpNodeByTransID(uint32 Queue, char *SrcEntityId, uint32 Trans)
{

    CF_QueueEntry_t *QNodePtr;
    
    QNodePtr = CF_AppData.UpQ[Queue].HeadPtr;            
    while(QNodePtr != NULL)
    {                
        if(QNodePtr->TransNum == Trans)
        {
            if(strncmp(SrcEntityId, QNodePtr->SrcEntityId,CF_MAX_CFG_VALUE_CHARS) == 0)        
                return QNodePtr;
        }
        QNodePtr = QNodePtr->Next;
    }
    
    return NULL;

}


CF_QueueEntry_t *CF_FindPbNodeByTransNum(uint32 Chan, uint32 Queue, uint32 Trans)
{

    CF_QueueEntry_t *QNodePtr;
    
    QNodePtr = CF_AppData.Chan[Chan].PbQ[Queue].HeadPtr;            
    while(QNodePtr != NULL)
    {                
        if(QNodePtr->TransNum == Trans)
            return QNodePtr;
        
        QNodePtr = QNodePtr->Next;
    }
    
    return NULL;

}


CF_QueueEntry_t *CF_FindNodeByTransId(char *TransIdStr)
{
    char            *TmpPtr;
    uint8           DownlinkTrans = 0;
    uint8           UplinkTrans = 0;
    char            GivenEntityId[CF_MAX_CFG_VALUE_CHARS];
    int32           GivenTransNum = 0;
    uint32          i,Chan,Q;
    CF_QueueEntry_t *NodePtr;
    
       
    /* Verify the given EntityId is valid, this will also tell the code what */
    /* queues to search (uplink or downlink) */
    /* To verify the given EntityId, we need to..*/
    /* separate the SrcEntityId (0.24) from the transaction id (0.24_3) so we */
    /* can compare the given SrcEntityId string with the FlightEntityId string*/
    /* in the table. If strings are equal, SrcEntityId is valid and downlink, */
    /* otherwise compare to PeerEntityId for each channel that is In-use */
    
    /* For uplink trans, peer entity id is the source */
    /* For downlink trans, flight entity id is the source */
    
    /* copy the string so that this function can manipulate the string as needed */
    strncpy(&GivenEntityId[0],TransIdStr,CF_MAX_CFG_VALUE_CHARS);
    
    
    /* Extract source EntityId (ex. 0.24) from TransId (ex. 0.24_13) by */
    /* replacing the underscore with a null terminator */
    TmpPtr = &GivenEntityId[0];
    
    for(i=0;i<CF_MAX_CFG_VALUE_CHARS;i++)
    {    
        if(*TmpPtr == '_')
        {
            *TmpPtr = '\0';
            
            /* Extract the TransNum (ex.13) from the TransId (ex. 0.24_13) */
            TmpPtr++;
            GivenTransNum = atoi(TmpPtr);            
            
            break;
        }
        
        TmpPtr++;
    }
    
    
    /* Validate the Entity Id */
    if(CF_ValidateEntityId(&GivenEntityId[0]) != CF_SUCCESS)
    {
        return NULL;
    } 
    
    /* determine if transaction is uplink or downlink */
    if(strncmp(&GivenEntityId[0], &CF_AppData.Tbl->FlightEntityId[0],
                CF_MAX_CFG_VALUE_CHARS) == 0)
    {
        /* match found, flight is source, must be downlink trans */
        DownlinkTrans = 1;
    
    }else{
        /* if not downlink, must be uplink */
        UplinkTrans = 1;

    }
    
    if(UplinkTrans)
    {
        /* check uplink, active queue */
        NodePtr = CF_FindUpNodeByTransID(CF_UP_ACTIVEQ,GivenEntityId,GivenTransNum);
        if(NodePtr != NULL) return NodePtr;
        
        /* check uplink, history queue */
        NodePtr = CF_FindUpNodeByTransID(CF_UP_HISTORYQ,GivenEntityId,GivenTransNum);
        if(NodePtr != NULL) return NodePtr;
    
    }else if(DownlinkTrans){

        for(Chan=0;Chan<CF_MAX_PLAYBACK_CHANNELS;Chan++)
        {
            if(CF_AppData.Tbl->OuCh[Chan].EntryInUse == CF_ENTRY_IN_USE)
            {
                for(Q=0;Q<CF_QUEUES_PER_CHAN;Q++)
                {
                    NodePtr = CF_FindPbNodeByTransNum(Chan, Q, GivenTransNum);
                    if(NodePtr != NULL) return NodePtr;
                }
            }
        }
    }
      
    return NULL;
    
}/* CF_FindNodeByTransId */





int32 CF_FindActiveTransIdByName(char *TransIdBuf,char *Filename)
{
    CF_QueueEntry_t     *QueueEntryPtr;
    char                TmpBuf[CF_MAX_TRANSID_CHARS];
    char                TmpTransNumBuf[CF_MAX_TRANSID_CHARS];

    /* Check uplink, active queue for given Filename */
    QueueEntryPtr = CF_FindUpActiveNodeByName(Filename);
    
    if(QueueEntryPtr != NULL)
    {
        /* convert the trans num to a string */
        sprintf(&TmpTransNumBuf[0],"%u",(unsigned int)QueueEntryPtr->TransNum);

        /* build the transaction-id formatted string */
        strncpy(&TmpBuf[0],&QueueEntryPtr->SrcEntityId[0],CF_MAX_TRANSID_CHARS);
        strcat(&TmpBuf[0],"_");
        strcat(&TmpBuf[0],&TmpTransNumBuf[0]);

        /* copy string from local buf to callers buf */
        strncpy(TransIdBuf,&TmpBuf[0],CF_MAX_TRANSID_CHARS);
        
        return CF_SUCCESS;    
    }
    
    QueueEntryPtr = CF_FindPbActiveNodeByName(Filename);
    if(QueueEntryPtr != NULL)
    {    

        /* convert the trans num to a string */
        sprintf(&TmpTransNumBuf[0],"%u",(unsigned int)QueueEntryPtr->TransNum);

        /* build the transaction-id formatted string */
        strncpy(&TmpBuf[0],&QueueEntryPtr->SrcEntityId[0],CF_MAX_TRANSID_CHARS);
        strcat(&TmpBuf[0],"_");
        strcat(&TmpBuf[0],&TmpTransNumBuf[0]);
        
        /* copy string from local buf to callers buf */
        strncpy(TransIdBuf,&TmpBuf[0],CF_MAX_TRANSID_CHARS);
        
        return CF_SUCCESS;    

    }else{
        
        return CF_ERROR;
        
    }        

}/* end CF_FindActiveTransIdByName */


/* Str length checks needed because engine sets MAX_REQUEST_STRING_LENGTH to 128 */
/* and fsw allows src and dest path/file sizes to be 64 (OS_MAX_PATH_LEN) each */
/* this doesn't leave room the the other necessary params. e.g. PUT,-class1,Entity ID etc*/
int32 CF_BuildPutRequest(CF_QueueEntry_t *QueueEntryPtr)
{
    static char EntireString[MAX_REQUEST_STRING_LENGTH];
    uint32      ReqLen = 0;
    uint32      Chan;
    
    Chan = QueueEntryPtr->ChanNum;
    
    strcpy(EntireString,"PUT ");
    ReqLen = 5;/* size of "PUT " plus one for null term */
    
    if(QueueEntryPtr->Class == 1)
    {
        strcat(EntireString,"-class1 ");
        ReqLen += 8;
    }
            
    ReqLen += strlen(QueueEntryPtr->SrcFile);
    if(ReqLen >= (MAX_REQUEST_STRING_LENGTH - 1))/* minus one for added space */
    {
        CF_SendEventPutStrTooLong(QueueEntryPtr->SrcFile);
        return CF_ERROR;
    }
    strcat(&EntireString[0],(const char *)&QueueEntryPtr->SrcFile);
    strcat(&EntireString[0]," ");


    ReqLen += (strlen(QueueEntryPtr->PeerEntityId));
    if(ReqLen >= (MAX_REQUEST_STRING_LENGTH - 1))
    {
        CF_SendEventPutStrTooLong(QueueEntryPtr->SrcFile);
        return CF_ERROR;
    }
    strcat(&EntireString[0],QueueEntryPtr->PeerEntityId);
    strcat(&EntireString[0]," ");


    ReqLen += (strlen(QueueEntryPtr->DstFile));
    if(ReqLen >= (MAX_REQUEST_STRING_LENGTH - 1))
    {
        CF_SendEventPutStrTooLong(QueueEntryPtr->SrcFile);
        return CF_ERROR;
    }
    strcat(&EntireString[0],(const char *)&QueueEntryPtr->DstFile);
        
    QueueEntryPtr->Status = CF_STAT_PUT_REQ_ISSUED;
    if(cfdp_give_request(EntireString) == FALSE)
    {                                        
        CFE_EVS_SendEvent(CF_PUT_REQ_ERR2_EID,CFE_EVS_ERROR,
                        "Engine put request returned error for %s",
                        QueueEntryPtr->SrcFile);
                
        return CF_ERROR;    
    }
    
    return CF_SUCCESS;
    
}/*end of CF_BuildPutRequest */


void CF_SendEventPutStrTooLong(char *SrcFile)
{
        CFE_EVS_SendEvent(CF_PUT_REQ_ERR1_EID,CFE_EVS_ERROR,
                        "Put request for %s > MAX_REQUEST_STRING_LENGTH %u",
                        SrcFile,MAX_REQUEST_STRING_LENGTH);

}/* end CF_SendEventPutStrTooLong */
    



/* Req is a request like cancel,abandon,suspend,resume etc. */
/* Trans is a transaction id of the form 0.24_5 */
int32 CF_BuildCmdedRequest(char *Req,char *Trans)
{
    static char  EntireString[MAX_REQUEST_STRING_LENGTH];
    
    strcpy(EntireString,Req);
    strcat(&EntireString[0]," ");
    strcat(&EntireString[0],Trans);
    strcat(&EntireString[0]," ");/* ensures null terminator */
    EntireString[MAX_REQUEST_STRING_LENGTH - 1] = '\0';
    
#ifdef CF_DEBUG
    if(cfdbg > 2)
        OS_printf("CF:Giving engine Cmded Request %s\n",EntireString);
#endif    

    if(cfdp_give_request(EntireString) == FALSE)
    {           
#ifdef CF_DEBUG        
        OS_printf("CF:Engine Cmded request returned error for %s",
                        EntireString);
#endif        
        return CF_ERROR;                        
    }
    
    return CF_SUCCESS;
    
}/*end of CF_BuildCmdedRequest */



void CF_IncrFaultCtr(TRANS_STATUS *TransInfoPtr)
{

    switch(TransInfoPtr->condition_code)
    {
    
        case NO_ERROR:
            break;
        
        case POSITIVE_ACK_LIMIT_REACHED:
            CF_AppData.Hk.Cond.PosAckNum++;
            break;
       
        case FILESTORE_REJECTION:
            CF_AppData.Hk.Cond.FileStoreRejNum++; 
            break;
        
        case FILE_CHECKSUM_FAILURE:
            CF_AppData.Hk.Cond.FileChecksumNum++;
            break;

        case FILE_SIZE_ERROR:
            CF_AppData.Hk.Cond.FileSizeNum++;
            break;
        
        case NAK_LIMIT_REACHED:
            CF_AppData.Hk.Cond.NakLimitNum++;
            break;
        
        case INACTIVITY_DETECTED:
            CF_AppData.Hk.Cond.InactiveNum++; 
            break;
        
        case SUSPEND_REQUEST_RECEIVED:
            CF_AppData.Hk.Cond.SuspendNum++;
            break;
        
        case CANCEL_REQUEST_RECEIVED:
            CF_AppData.Hk.Cond.CancelNum++;
            break;

        default:
            CFE_EVS_SendEvent(CF_IND_FAU_UNEX_EID,CFE_EVS_ERROR,
                "Unexpected Condition Code %u in Trans Finished Indication",
                TransInfoPtr->condition_code);
            break;
                                                           
    }/* end switch */

}/* end CF_IncrFaultCtr */


/* check that the entity id is in 2 byte dotted decimal format */
/* first value (before the dot) can be 1,2 or 3 digits. */
/* second value (after the dot) can be 1,2 or 3 digits. */
/* each value must be zero to 255 */
int32 CF_ValidateEntityId(char *EntityIdStrPtr)
{
    char    TmpBuf[CF_MAX_TRANSID_CHARS];
    char    *TmpPtr = &TmpBuf[0]; 
    uint32  DotLocation;/* zero based */
    char    *PtrToDot;
    uint32  i;
    uint32  FirstValue;
    uint32  SecondValue;
    uint32  LengthIsGood;

    /* copy input string into temporary buffer */
    strncpy(TmpBuf,EntityIdStrPtr,CF_MAX_TRANSID_CHARS);
    
    /* string length must be between 3 bytes (ex. 2.5) and 7 bytes (ex. 255.255) */
    
    /* check that string is not too short */
    for(i=0;i<3;i++)
    {
        if(*TmpPtr == '\0') 
            return CF_ERROR;
        TmpPtr++;
    }/* end for */
    /* TmpPtr at 4th char */

    /* Check that the null terminator is in the 4th - 8th position */
    LengthIsGood = CF_FALSE;    
    for(i=0;i<5;i++)
    {
        if(*TmpPtr == '\0')LengthIsGood = CF_TRUE;
        TmpPtr++;
    
    }/* end for */    

    if(LengthIsGood == CF_FALSE) 
        return CF_ERROR;
    
    /* length is good, reset pointer */
    TmpPtr = &TmpBuf[0];

    /* check for a dot in the string */
    PtrToDot = strstr(TmpPtr,".");
    if(PtrToDot==NULL) 
        return CF_ERROR;
    
    /* DotLocation is zero based, zero is first character */
    DotLocation = (uint32)(PtrToDot - TmpPtr);    
    
    if((DotLocation < 1) || (DotLocation > 3)) 
        return CF_ERROR;
    
    /* temporarily replace the dot with a null terminator */
    *PtrToDot = '\0';
    
    /* get first value */
    FirstValue = atoi(TmpPtr);
    
    /* put back to original */
    *PtrToDot = '.';
    
    /* Set TmpPtr to first char of second value */
    TmpPtr = PtrToDot;
    TmpPtr++;
    
    /* get second value */
    SecondValue = atoi(TmpPtr);
    
    /* Values must fit in a  byte */
    if((FirstValue > 255) || (SecondValue > 255))
        return CF_ERROR;
        
    return CF_SUCCESS;

}/* end CF_ValidateEntityId */


/* This check works only if the file was opened through the OSAL */
/* If file was opened with direct os calls, this check will not work */
uint32 CF_FileOpenCheck(char *Filename)
{
    int32   Status;
    uint32  i;
    OS_FDTableEntry Fd_Prop;

    for (i = 0; i < OS_MAX_NUM_OPEN_FILES; i++)
    {
        Status = OS_FDGetInfo(i, &Fd_Prop);
        
        if (Status != OS_FS_ERR_INVALID_FD)
        {
            if (strncmp(Filename,Fd_Prop.Path,OS_MAX_PATH_LEN) == 0)
                return CF_OPEN;        
        }
        
    }/* end for */ 
    
    return CF_CLOSED;

}/* end CF_FileOpenCheck */


int32 CF_CheckIfFileIsActive(char *Filename)
{

    CF_QueueEntry_t *QueueNode;

    QueueNode = CF_FindUpActiveNodeByName(Filename);
    if(QueueNode != NULL) 
        return CF_FILE_IS_ACTIVE;
    
    QueueNode = CF_FindPbActiveNodeByName(Filename);
    if(QueueNode != NULL) 
        return CF_FILE_IS_ACTIVE;
    
    return CF_FILE_NOT_ACTIVE;
    
}/* end CF_CheckIfFileIsActive */


void CF_MoveUpNodeActiveToHistory(char *SrcEntity, uint32 TransNum)
{
    CF_QueueEntry_t *QueueEntryPtr; 
    CF_QueueEntry_t *QueueNodeToFree;
        
    /* Find the transaction in the Active Q */
    QueueEntryPtr = CF_FindUpNodeByTransID(CF_UP_ACTIVEQ, SrcEntity, TransNum);
    if(QueueEntryPtr == NULL)
    {
        
        CFE_EVS_SendEvent(CF_MV_UP_NODE_EID,CFE_EVS_ERROR,
            "TransId %s_%d not found in CF_MoveUpNodeActiveToHistory",SrcEntity,TransNum);               

    }else{                
                    
        CF_RemoveFileFromUpQueue(CF_UP_ACTIVEQ, QueueEntryPtr);                             

        /* if history queue is full, remove oldest node and 
        ** free the memory */
        if(CF_AppData.UpQ[CF_UP_HISTORYQ].EntryCnt >=
            CF_AppData.Tbl->UplinkHistoryQDepth)
        {
            QueueNodeToFree = CF_DequeueUpNode(CF_UP_HISTORYQ);
            CF_DeallocQueueEntry(QueueNodeToFree);
        } 

        CF_AddFileToUpQueue(CF_UP_HISTORYQ, QueueEntryPtr);
    }

}/* end CF_MoveUpNodeActiveToHistory */



void CF_MoveDwnNodeActiveToHistory(uint32 TransNum)
{
    CF_QueueEntry_t *QueueEntryPtr = NULL;
    CF_QueueEntry_t *QueueNodeToFree;
    uint32          Chan;

    /* Find Channel Number, search the given queue for all channels */
    Chan = CF_GetChanNumFromTransId(CF_PB_ACTIVEQ, TransNum);
    if(Chan != CF_ERROR){
               
        /* Find the transaction in the Active Q */
        QueueEntryPtr = CF_FindPbNodeByTransNum(Chan, CF_PB_ACTIVEQ, TransNum);
        if(QueueEntryPtr != NULL)
        {
            /* move transaction info from active queue to history queue */
            Chan = QueueEntryPtr->ChanNum;
                    
            if(Chan < CF_MAX_PLAYBACK_CHANNELS)
            {                                                        
                CF_RemoveFileFromPbQueue(Chan, CF_PB_ACTIVEQ, QueueEntryPtr); 
                
                /* if history queue is full, remove oldest node and 
                ** free the memory */
                if(CF_AppData.Chan[Chan].PbQ[CF_PB_HISTORYQ].EntryCnt >=
                    CF_AppData.Tbl->OuCh[Chan].HistoryQDepth)
                {
                    QueueNodeToFree = CF_DequeuePbNode(Chan,CF_PB_HISTORYQ);
                    CF_DeallocQueueEntry(QueueNodeToFree);
                }                            
                
                CF_AddFileToPbQueue(Chan, CF_PB_HISTORYQ, QueueEntryPtr);                
            
                CF_AppData.Hk.Chan[Chan].FilesSent++;                                                        
                                      
            }/* end if */            
    
        }
#ifdef CF_DEBUG        
        else{
        
            OS_printf("Trans Num %d not found in CF_MoveDwnNodeActiveToHistory\n",
                        TransNum);

        }/* end if */    
#endif    

    }/* end if */
            

}/* CF_MoveDwnNodeActiveToHistory */



int32 CF_ValidateFilenameReportErr(char *Filename, char *Source)
{

    if(CF_ValidatePathFile(Filename)==CF_ERROR)
    {
        CFE_EVS_SendEvent(CF_INV_FILENAME_EID, CFE_EVS_ERROR,
            "Filename in %s must be terminated and have no spaces",
            Source);
        return CF_ERROR;
    }
        
    return CF_SUCCESS;

}/* end CF_ValidateFilenameReportErr */


/* this function checks for an unterminated string and */
/* invalid spaces in the path and filename */
int32 CF_ValidatePathFile(char *PathFilename)
{    
    char    *CharPtr;
    uint32  i;
    
    CharPtr = PathFilename;

    if(CharPtr == NULL)
        return CF_ERROR;
        
    if(CF_ChkTermination(PathFilename,OS_MAX_PATH_LEN)==CF_ERROR)
        return CF_ERROR;
    
    for(i=0;i<OS_MAX_PATH_LEN;i++)
    {
        if(*CharPtr == ' ')
        {
            /* invalid space */
            return CF_ERROR;
        
        }else if(*CharPtr == '\0'){
        
            return CF_SUCCESS;            
        }
        
        CharPtr++;
    }

    /* Unterminated string */
    return CF_ERROR;
    
}/* CF_ValidatePathFile */


/* checks that path is terminated, has no spaces and */
/* last character has forward slash. */
/* Used only for strings with max length = OS_MAX_PATH_LEN */
int32 CF_ValidateSrcPath(char *Pathname)
{
    char    *CharPtr;
    uint32  i;
    
    CharPtr = Pathname;

    if(CharPtr == NULL)
        return CF_ERROR;
        
    for(i=0;i<OS_MAX_PATH_LEN;i++)
    {                
        if(*CharPtr == ' ')
        {
            /* invalid space */
            return CF_ERROR;
        
        /* properly terminated */
        }else if(*CharPtr == '\0'){
        
            /* check that the last char has forward slash */
            CharPtr--;
            if(*CharPtr != '/'){
                /* last char must be forward slash */
                return CF_ERROR;
            }/* end if */
            
            return CF_SUCCESS;            
        }
        
        CharPtr++;
    }

    /* Unterminated string */
    return CF_ERROR;
    
}/* CF_ValidateSrcPath */


/* checks that path is terminated and has no spaces */
/* Used only for strings with max length = OS_MAX_PATH_LEN */
int32 CF_ValidateDstPath(char *Pathname)
{
    char    *CharPtr;
    uint32  i;
    
    CharPtr = Pathname;

    if(CharPtr == NULL)
        return CF_ERROR;
        
    for(i=0;i<OS_MAX_PATH_LEN;i++)
    {                
        if(*CharPtr == ' ')
        {
            /* invalid space */
            return CF_ERROR;
        
        /* properly terminated */
        }else if(*CharPtr == '\0'){
            
            return CF_SUCCESS;            
        }
        
        CharPtr++;
    }

    /* Unterminated string */
    return CF_ERROR;
    
}/* CF_ValidateDstPath */


int32 CF_ChkTermination(char *String, uint32 MaxLength)
{
    char    *CharPtr;
    uint32  i;
        
    CharPtr = String;

    if(CharPtr == NULL)
        return CF_ERROR;    
        
    for(i=0; i < MaxLength; i++)
    {
                
        if(*CharPtr == '\0')
        {
            return CF_SUCCESS;            
        }
        
        CharPtr++;
    }

    /* Unterminated string */
    return CF_ERROR;

}/* end CF_ChkTermination */


void CF_SendEventNoTerm(char *Source)
{

    CFE_EVS_SendEvent(CF_NO_TERM_ERR_EID,CFE_EVS_ERROR,
                        "Unterminated string found in %s",Source);

}/* end CF_SendEventNoTerm */


void CF_GetStatString(char *CallersBuf,uint32 Stat, uint32 BufSize)
{

    switch(Stat)
    {            
        case  CF_STAT_UNKNOWN:
            strncpy(CallersBuf,"UNKNOWN",BufSize);
            break;

        case  CF_STAT_SUCCESS:
            strncpy(CallersBuf,"SUCCESSFUL",BufSize);
            break;
   
        case  CF_STAT_CANCELLED:
            strncpy(CallersBuf,"CANCELLED",BufSize);
            break;

        case  CF_STAT_ABANDON:
            strncpy(CallersBuf,"ABANDONED",BufSize);
            break;

        case  CF_STAT_NO_META:
            strncpy(CallersBuf,"NO_METADATA",BufSize);
            break;

        case  CF_STAT_PENDING:
            strncpy(CallersBuf,"PENDING",BufSize);
            break;

        case  CF_STAT_ALRDY_ACTIVE:
            strncpy(CallersBuf,"ALRDY_ACTIVE",BufSize);
            break;
   
        case  CF_STAT_PUT_REQ_ISSUED:
            strncpy(CallersBuf,"PUT_REQ_ISSUED",BufSize);
            break;

        case  CF_STAT_PUT_REQ_FAIL:
            strncpy(CallersBuf,"PUT_REQ_FAILED",BufSize);
            break;

        case  CF_STAT_ACTIVE:
            strncpy(CallersBuf,"ACTIVE",BufSize);
            break;

        default:
            strncpy(CallersBuf,"INV_FINAL_STAT",BufSize);
            break;
            
    }

    return;

}/* end CF_GetStatString */






void CF_GetFinalStatString(char *CallersBuf,uint32 FinalStat, uint32 BufSize)
{

    switch(FinalStat)
    {            
        case  FINAL_STATUS_UNKNOWN:
            strncpy(CallersBuf,"UNKNOWN",BufSize);
            break;

        case  FINAL_STATUS_SUCCESSFUL:
            strncpy(CallersBuf,"SUCCESSFUL",BufSize);
            break;
   
        case  FINAL_STATUS_CANCELLED:
            strncpy(CallersBuf,"CANCELLED",BufSize);
            break;

        case  FINAL_STATUS_ABANDONED:
            strncpy(CallersBuf,"ABANDONED",BufSize);
            break;

        case  FINAL_STATUS_NO_METADATA:
            strncpy(CallersBuf,"NO_METADATA",BufSize);
            break;

        default:
            strncpy(CallersBuf,"INV_FINAL_STAT",BufSize);
            break;
            
    }

    return;

}/* end CF_GetFinalStatString */
   



void CF_GetCondCodeString(char *CallersBuf,uint32 CondCode,uint32 BufSize)
{
    switch(CondCode)
    {            
        case  NO_ERROR:
            strncpy(CallersBuf,"NO_ERR",BufSize);
            break;
   
        case  POSITIVE_ACK_LIMIT_REACHED:
            strncpy(CallersBuf,"ACK_LIMIT",BufSize);
            break;

        case  FILESTORE_REJECTION:
            strncpy(CallersBuf,"FILESTORE_ERR",BufSize);
            break;

        case  FILE_CHECKSUM_FAILURE:
            strncpy(CallersBuf,"CHKSUM_FAIL",BufSize);
            break;

        case  FILE_SIZE_ERROR:
            strncpy(CallersBuf,"FILESIZE_ERR",BufSize);
            break;
            
        case  NAK_LIMIT_REACHED:
            strncpy(CallersBuf,"NAK_LIMIT",BufSize);
            break;
            
        case  INACTIVITY_DETECTED:
            strncpy(CallersBuf,"INACTIVITY_DETECTED",BufSize);
            break;
            
        case  SUSPEND_REQUEST_RECEIVED:
            strncpy(CallersBuf,"SUSPEND_REQ_RCVD",BufSize);
            break;
            
        case  CANCEL_REQUEST_RECEIVED:
            strncpy(CallersBuf,"CANCEL_REQ_RCVD",BufSize);
            break;            

        default:
            sprintf(CallersBuf,"UNEXPECTED %lu",CondCode);
            break;
            
    }/* end switch */
    
    return;

}/* CF_GetCondCodeString */



/* returns a 0 for Tlm, and a 1 for CMD */
uint32  CF_GetPktType(CFE_SB_MsgId_t MsgId)
{
    return CFE_TST(MsgId,12);

}/* end CF_GetPktType */


/* this function assumes the global SB msg ptr is still pointing to the incoming PDU pkt */ 
uint8 CF_GetResponseChanFromMsgId(CFE_SB_MsgPtr_t MessagePtr)
{
    uint32          i;
    CFE_SB_MsgId_t  MessageID;
    
    /* get MsgId from input packet and lookup the class2 response channel in the table.*/
    MessageID = CFE_SB_GetMsgId (MessagePtr);
    
    for(i=0;i<CF_NUM_INPUT_CHANNELS;i++)
    {
      if(MessageID == (CFE_SB_MsgId_t)CF_AppData.Tbl->InCh[i].IncomingPDUMsgId)
          return CF_AppData.Tbl->InCh[i].OutChanForClass2Response;          
    }
    
    /* if no match found, return an invalid channel number */
    return 0xFF;

}/* end CF_GetResponseChanFromMsgId */


uint8 CF_GetResponseChanFromTransId(uint32 Queue, char *SrcEntityId, uint32 Trans)
{

    CF_QueueEntry_t *QNodePtr;
    
    QNodePtr = CF_AppData.UpQ[Queue].HeadPtr;            
    while(QNodePtr != NULL)
    {                
        if(QNodePtr->TransNum == Trans)
        {
            if(strncmp(SrcEntityId, QNodePtr->SrcEntityId,CF_MAX_CFG_VALUE_CHARS) == 0)        
                return QNodePtr->ChanNum;
        }
        QNodePtr = QNodePtr->Next;
    }
    
    return 0xFF;

}/* end CF_GetResponseChanFromTransId */



#ifdef CF_DEBUG

    void CF_ShowTbl(void){
    
        uint32  h,i,j;
    
        /* General Config Parameters */
        OS_printf("\nTableIdString %s\n",CF_AppData.Tbl->TableIdString);
        OS_printf("TableVersion %d\n",CF_AppData.Tbl->TableVersion);
        OS_printf("NumEngCyclesPerWakeup %d\n",CF_AppData.Tbl->NumEngCyclesPerWakeup);
        OS_printf("NumWakeupsPerQueueChk %d\n",CF_AppData.Tbl->NumWakeupsPerQueueChk);
        OS_printf("NumWakeupsPerPollDirChk %d\n",CF_AppData.Tbl->NumWakeupsPerPollDirChk);
        OS_printf("UplinkHistoryQDepth %d\n",CF_AppData.Tbl->UplinkHistoryQDepth);
           
        OS_printf("AckTimeout %s\n",CF_AppData.Tbl->AckTimeout);
        OS_printf("AckLimit %s\n",CF_AppData.Tbl->AckLimit);
        OS_printf("NakTimeout %s\n",CF_AppData.Tbl->NakTimeout);
        OS_printf("NakLimit %s\n",CF_AppData.Tbl->NakLimit);
        OS_printf("InactivityTimeout %s\n",CF_AppData.Tbl->InactivityTimeout);
        OS_printf("OutgoingFileChunkSize %s\n",CF_AppData.Tbl->OutgoingFileChunkSize);
        OS_printf("SaveIncompleteFiles %s\n",CF_AppData.Tbl->SaveIncompleteFiles);
        OS_printf("FlightEntityId %s\n",CF_AppData.Tbl->FlightEntityId);
    
        OS_printf("\n");
        for(h=0;h<CF_NUM_INPUT_CHANNELS;h++)
        {    
          OS_printf("Incoming PDU MsgId = 0x%04X, class 2 uplink response chan %d\n",
                              CF_AppData.Tbl->InCh[h].IncomingPDUMsgId,
                              CF_AppData.Tbl->InCh[h].OutChanForClass2Response);
        }   
        OS_printf("\n");
        
        for(j=0;j<CF_MAX_PLAYBACK_CHANNELS;j++){
           
            
            if(CF_AppData.Tbl->OuCh[j].EntryInUse == CF_ENTRY_IN_USE)
            {
            
                /* Playback Output Channel 0 parameters */
                OS_printf("Chan %d:Inuse = %d\n",j,CF_AppData.Tbl->OuCh[j].EntryInUse);    
                OS_printf("Chan %d:DequeueEnable = %d\n",j,CF_AppData.Tbl->OuCh[j].DequeueEnable);
                OS_printf("Chan %d:Outgoing PDU MsgId = 0x%04X\n",j,CF_AppData.Tbl->OuCh[j].OutgoingPduMsgId);
                OS_printf("Chan %d:PendingQDepth = %d\n",j,CF_AppData.Tbl->OuCh[j].PendingQDepth);
                OS_printf("Chan %d:HistoryQDepth = %d\n",j,CF_AppData.Tbl->OuCh[j].HistoryQDepth);   
                OS_printf("Chan %d:ChanName = %s\n",j,CF_AppData.Tbl->OuCh[j].ChanName);
                OS_printf("Chan %d:SemName = %s\n\n",j,CF_AppData.Tbl->OuCh[j].SemName);
            
                for(i=0;i<CF_MAX_POLLING_DIRS_PER_CHAN;i++)
                {
                        
                    if(CF_AppData.Tbl->OuCh[j].PollDir[i].EntryInUse == CF_ENTRY_IN_USE)
                    {
                        /*Poll Directory 0 parameters for Channel 0 */
                        OS_printf("Chan %d:PollDir %d:Inuse = %d\n",j,i,CF_AppData.Tbl->OuCh[j].PollDir[i].EntryInUse);
                        OS_printf("Chan %d:PollDir %d:Enable = %d\n",j,i,CF_AppData.Tbl->OuCh[j].PollDir[i].EnableState);
                        OS_printf("Chan %d:PollDir %d:Preserve = %d\n",j,i,CF_AppData.Tbl->OuCh[j].PollDir[i].Preserve);
                        OS_printf("Chan %d:PollDir %d:Class = %d\n",j,i,CF_AppData.Tbl->OuCh[j].PollDir[i].Class);
                        OS_printf("Chan %d:PollDir %d:Priority = %d\n",j,i,CF_AppData.Tbl->OuCh[j].PollDir[i].Priority);
                        OS_printf("Chan %d:PollDir %d:PeerEntityId = %s\n",j,i,CF_AppData.Tbl->OuCh[j].PollDir[i].PeerEntityId); 
                        OS_printf("Chan %d:PollDir %d:SrcPath = %s\n",j,i,CF_AppData.Tbl->OuCh[j].PollDir[i].SrcPath);
                        OS_printf("Chan %d:PollDir %d:DstPath = %s\n",j,i,CF_AppData.Tbl->OuCh[j].PollDir[i].DstPath);
                    
                    }/* end if */
                
                }/* end for */
                
            }/* end if */
            
        }/* end for */
    
    }/* end CF_ShowTbl */
    
    
    
    void CF_ShowCfg(void)
    {
        char    AckLimit[CF_MAX_CFG_VALUE_CHARS]; 
        char    AckTimeout[CF_MAX_CFG_VALUE_CHARS]; 
        char    NakLimit[CF_MAX_CFG_VALUE_CHARS]; 
        char    NakTimeout[CF_MAX_CFG_VALUE_CHARS]; 
        char    InactTimeout[CF_MAX_CFG_VALUE_CHARS]; 
        char    OutGoingChunk[CF_MAX_CFG_VALUE_CHARS]; 
        char    SaveIncomplete[CF_MAX_CFG_VALUE_CHARS];
    
    
        cfdp_get_mib_parameter("ACK_LIMIT",     &AckLimit[0]);
        cfdp_get_mib_parameter("ACK_TIMEOUT",   &AckTimeout[0]);
        cfdp_get_mib_parameter("NAK_LIMIT",     &NakLimit[0]);
        cfdp_get_mib_parameter("NAK_TIMEOUT",   &NakTimeout[0]);
        cfdp_get_mib_parameter("INACTIVITY_TIMEOUT",    &InactTimeout[0]);
        cfdp_get_mib_parameter("OUTGOING_FILE_CHUNK_SIZE", &OutGoingChunk[0]);
        cfdp_get_mib_parameter("SAVE_INCOMPLETE_FILES", &SaveIncomplete[0]);    
        
        OS_printf("AckTimeout %u\n",atoi(&AckTimeout[0]));
        OS_printf("AckLimit %u\n",atoi(&AckLimit[0]));
        OS_printf("NakTimeout %u\n",atoi(&NakTimeout[0]));
        OS_printf("NakLimit %u\n",atoi(&NakLimit[0]));
        OS_printf("Inactivity Timeout %u\n",atoi(&InactTimeout[0]));
        OS_printf("OutGoingChunk %u\n",atoi(&OutGoingChunk[0]));
        OS_printf("SaveIncompleteFiles %s\n", &SaveIncomplete[0]);/* prints YES or NO */
    
        OS_printf("PipeDepth %u\n",CF_PIPE_DEPTH);
        OS_printf("MaxSimultaneousTrans %u\n",CF_MAX_SIMULTANEOUS_TRANSACTIONS);
        OS_printf("UplinkPduDataBufSize %u\n",CF_INCOMING_PDU_BUF_SIZE);
        OS_printf("DownlinkPduDataBufSize %u\n",CF_OUTGOING_PDU_BUF_SIZE);
    
        OS_printf("MaxPlaybackChans %u\n",CF_MAX_PLAYBACK_CHANNELS);
        OS_printf("MaxPollingDirsPerChan %u\n",CF_MAX_POLLING_DIRS_PER_CHAN);
        OS_printf("MemPoolBytes %u\n",CF_MEMORY_POOL_BYTES);
    
        OS_printf("Pipe Name %s\n",CF_PIPE_NAME);
        OS_printf("Engine temp file prefix %s\n",CF_ENGINE_TEMP_FILE_PREFIX);
        OS_printf("Configuration tbl name %s\n",CF_CONFIG_TABLE_NAME);
        OS_printf("Configuration tbl filename %s\n",CF_CONFIG_TABLE_FILENAME);
        OS_printf("Default queue info filename %s\n\n",CF_DEFAULT_QUEUE_INFO_FILENAME);
    
    }/* end CF_ShowCfg */
    
    
    /*
    **             Function Prologue
    **
    ** Function Name: CF_PrintPDUType
    **
    ** Purpose: Debug option to print PDU type
    **
    ** Input arguments:
    **
    ** Return values:
    **    (none)
    */
    void CF_PrintPDUType (uint8 FileDirCode, uint8 DirCode2)
    {    
            switch (FileDirCode){
    
                case 4:
                    OS_printf("EOF ");                
                    break;
                                    
                case 5:
                    OS_printf("FIN");
                    break;
                    
                case 6:
                    OS_printf("ACK");
                    CF_PrintAckType(DirCode2);
                    break;                
    
                case 7:
                    OS_printf("MD");
                    break;
    
                case 8:
                    OS_printf("NAK");
                    break;
                    
                default:
                    OS_printf("Inv%d",FileDirCode);
                    
            }/* end switch */
        
    }/* end of CF_PrintPDUType */
    
    
    void CF_PrintAckType (uint8 DirCode2)
    {    
            switch (DirCode2){
    
                case 64:
                    OS_printf("-EOF ");                
                    break;
                                    
                case 80:
                    OS_printf("-FIN");
                    break;
                    
                default:
                    OS_printf("-Inv0x%x",DirCode2);
                    
            }/* end switch */
        
    }/* end of CF_PrintAckType */
    
    
    /******************************************************************************
    **  Function:  CF_ShowQs()
    **
    **  Purpose:
    **      This function prints the files on the all pending queues
    **
    **  Arguments:
    **      Channel
    **
    **  Return:
    **      None
    **
    */
    void CF_ShowQs(){
    
        uint32              i;
        CF_QueueEntry_t     *PtrToEntry;
    
        OS_printf("\nUplink Active Queue files:\n");            
        PtrToEntry = CF_AppData.UpQ[CF_UP_ACTIVEQ].HeadPtr;            
        while(PtrToEntry != NULL)
        {                
            OS_printf("%s_%d %s\n",PtrToEntry->SrcEntityId,
                        PtrToEntry->TransNum, PtrToEntry->SrcFile);
            PtrToEntry = PtrToEntry->Next;
        }
        OS_printf("Uplink Active Queue - File Count = %d\n\n",
                    CF_AppData.UpQ[CF_UP_ACTIVEQ].EntryCnt);
                    
        
        OS_printf("Uplink History Queue files:\n");            
        PtrToEntry = CF_AppData.UpQ[CF_UP_HISTORYQ].HeadPtr;            
        while(PtrToEntry != NULL)
        {                
            OS_printf("%s_%d %s\n",PtrToEntry->SrcEntityId,
                        PtrToEntry->TransNum, PtrToEntry->SrcFile);
            PtrToEntry = PtrToEntry->Next;
        }
        OS_printf("Uplink History Queue - File Count = %d\n\n",
                    CF_AppData.UpQ[CF_UP_HISTORYQ].EntryCnt);                
    
    
    
        for(i=0;i<CF_MAX_PLAYBACK_CHANNELS;i++)
        {
            
            if(CF_AppData.Tbl->OuCh[i].EntryInUse == CF_ENTRY_IN_USE)
            {                    
    
                OS_printf("Playback Pending Queue %d files:\n",i);            
                PtrToEntry = CF_AppData.Chan[i].PbQ[CF_PB_PENDINGQ].HeadPtr;            
                while(PtrToEntry != NULL)
                {                
                    OS_printf("%s_%d %s %d\n",PtrToEntry->SrcEntityId,
                        PtrToEntry->TransNum, PtrToEntry->SrcFile,
                        PtrToEntry->Priority);
                    PtrToEntry = PtrToEntry->Next;
                }
                OS_printf("Playback Pending Queue %d File Count = %d\n\n",i,
                            CF_AppData.Chan[i].PbQ[CF_PB_PENDINGQ].EntryCnt); 
                            
    
                OS_printf("Playback Active Queue %d files:\n",i);            
                PtrToEntry = CF_AppData.Chan[i].PbQ[CF_PB_ACTIVEQ].HeadPtr;            
                while(PtrToEntry != NULL)
                {                
                    OS_printf("%s_%d %s %d\n",PtrToEntry->SrcEntityId,
                        PtrToEntry->TransNum, PtrToEntry->SrcFile,
                        PtrToEntry->Priority);
                    PtrToEntry = PtrToEntry->Next;
                }
                OS_printf("Playback Active Queue %d File Count = %d\n\n",i,
                            CF_AppData.Chan[i].PbQ[CF_PB_ACTIVEQ].EntryCnt);
                            
                            
                OS_printf("Playback History Queue %d files:\n",i);            
                PtrToEntry = CF_AppData.Chan[i].PbQ[CF_PB_HISTORYQ].HeadPtr;            
                while(PtrToEntry != NULL)
                {                
                    OS_printf("%s_%d %s %d\n",PtrToEntry->SrcEntityId,
                        PtrToEntry->TransNum, PtrToEntry->SrcFile,
                        PtrToEntry->Priority);
                    PtrToEntry = PtrToEntry->Next;
                }
                OS_printf("Playback History Queue %d File Count = %d\n\n",i,
                            CF_AppData.Chan[i].PbQ[CF_PB_HISTORYQ].EntryCnt);                        
                
            }/* end if */
        
        }/* end for */
    
    }/* end CF_ShowQs */
    
#endif




/************************/
/*  End of File Comment */
/************************/
