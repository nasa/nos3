/************************************************************************
** File:
**   $Id: cf_utils.h 1.16.1.1 2015/03/06 15:30:35EST sstrege Exp  $
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
**
** Notes:
**
** $Log: cf_utils.h  $
** Revision 1.16.1.1 2015/03/06 15:30:35EST sstrege 
** Added copyright information
** Revision 1.16 2011/05/09 11:52:11EDT rmcgraw 
** DCR13317:1 Allow Destintaion path to be blank
** Revision 1.15 2010/11/04 11:37:47EDT rmcgraw 
** Dcr13051:1 Wrap OS_printfs in platform cfg CF_DEBUG
** Revision 1.14 2010/10/25 11:21:48EDT rmcgraw 
** DCR12573:1 Changes to allow more than one incoming PDU MsgId
** Revision 1.13 2010/10/21 13:48:11EDT rmcgraw 
** DCR13060:2 Changed FindUpNodebyTransnum to FindUpNodeByTransID
** Revision 1.12 2010/10/20 14:55:54EDT rmcgraw 
** DCR12825:1 Change quick stat cmd to show active and suspended status
** Revision 1.11 2010/07/08 13:50:31EDT rmcgraw 
** DCR11510:1 Added proto for SendEventNoTerm
** Revision 1.10 2010/07/08 13:47:31EDT rmcgraw 
** DCR11510:1 Added termination checking on all cmds that take a string
** Revision 1.9 2010/07/07 17:41:52EDT rmcgraw 
** DCR11510:1 New prototypes for checking termination, validate paths etc
** Revision 1.8 2010/06/11 16:13:16EDT rmcgraw 
** DCR11510:1 ZeroCopy, Un-hardcoded cmd/tlm input/output pdus
** Revision 1.7 2010/06/01 11:02:41EDT rmcgraw 
** DCR11510:1 Removed CheckForGapsInTrans
** Revision 1.6 2010/04/23 15:46:45EDT rmcgraw 
** DCR11510:1 Added CF_CheckIfFileIsActive
** Revision 1.5 2010/04/23 08:39:14EDT rmcgraw 
** Dcr11510:1 Code Review Prep
** Revision 1.4 2010/03/26 15:30:22EDT rmcgraw 
** DCR11510 Various developmental changes
** Revision 1.3 2010/03/12 12:14:37EST rmcgraw 
** DCR11510:1 Initial check-in towards CF Version 1000
** Revision 1.2 2009/12/08 09:18:23EST rmcgraw 
** DCR10350:3 Added CF_ShowCfg function
** Revision 1.1 2009/11/24 12:48:57EST rmcgraw 
** Initial revision
** Member added to CFS CF project
**
*************************************************************************/
#ifndef _cf_utils_h_
#define _cf_utils_h_


/************************************************************************
** Includes
*************************************************************************/


int32 CF_BuildPutRequest(CF_QueueEntry_t *QueueEntryPtr);
void CF_SendEventPutStrTooLong(char *SrcFile);
int32 CF_BuildCmdedRequest(char *Req,char *Trans);
int32 CF_FindActiveTransIdByName(char *TransIdBuf,char *Filename);
CF_QueueEntry_t *CF_FindNodeAtFrontOfQueue(TRANS_STATUS TransInfo);
CF_QueueEntry_t *CF_FindUpNodeByTransID(uint32 Queue, char *SrcEntityId, uint32 Trans);
CF_QueueEntry_t *CF_FindPbNodeByTransNum(uint32 Chan, uint32 Queue, uint32 Trans);
CF_QueueEntry_t *CF_FindNodeByTransId(char *TransIdStr);
CF_QueueEntry_t *CF_FindUpHistoryNodeByName(char *SrcFile);
CF_QueueEntry_t *CF_FindUpActiveNodeByName(char *SrcFile);
CF_QueueEntry_t *CF_FindUpNodeByName(char *SrcFile);
CF_QueueEntry_t *CF_FindPbHistoryNodeByName(char *SrcFile);
CF_QueueEntry_t *CF_FindPbActiveNodeByName(char *SrcFile);
CF_QueueEntry_t *CF_FindPbPendingNodeByName(char *SrcFile);
CF_QueueEntry_t *CF_FindPbNodeByName(char *SrcFile);
CF_QueueEntry_t *CF_FindNodeByName(char *SrcFile);
void CF_IncrFaultCtr(TRANS_STATUS *TransInfoPtr);
int32 CF_ValidateEntityId(char *EntityIdStrPtr);
uint32 CF_FileOpenCheck(char *Filename);
int32 CF_CheckIfFileIsActive(char *Filename);
void CF_MoveUpNodeActiveToHistory(char *SrcEntity, uint32 TransNum);
void CF_MoveDwnNodeActiveToHistory(uint32 TransNum);
void CF_GetStatString(char *CallersBuf,uint32 Stat, uint32 BufSize);
void CF_GetFinalStatString(char *CallersBuf,uint32 FinalStat, uint32 BufSize);
void CF_GetCondCodeString(char *CallersBuf,uint32 CondCode,uint32 BufSize);
uint32  CF_GetPktType(CFE_SB_MsgId_t MsgId);
int32 CF_ValidatePathFile(char *PathFilename);
int32 CF_ValidateSrcPath(char *Pathname);
int32 CF_ValidateDstPath(char *Pathname);
int32 CF_ChkTermination(char *String, uint32 MaxLength);
int32 CF_ValidateFilenameReportErr(char *Filename, char *Source);
void CF_SendEventNoTerm(char *Source);
uint8 CF_GetResponseChanFromMsgId(CFE_SB_MsgPtr_t MessagePtr);
uint8 CF_GetResponseChanFromTransId(uint32 Queue, char *SrcEntityId, uint32 Trans);

#ifdef CF_DEBUG
    void CF_ShowTbl(void);
    void CF_ShowCfg(void);
    void CF_PrintPDUType (uint8 FileDirCode, uint8 DirCode2);
    void CF_PrintAckType (uint8 DirCode2);
    void CF_ShowQs(void);
#endif


#endif /* _cf_utils_h_ */

/************************/
/*  End of File Comment */
/************************/
