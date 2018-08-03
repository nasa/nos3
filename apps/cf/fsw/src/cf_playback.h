/************************************************************************
** File:
**   $Id: cf_playback.h 1.7.1.1 2015/03/06 15:30:21EST sstrege Exp  $
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
**  The CFS CF Application header file
**
** Notes:
**
** $Log: cf_playback.h  $
** Revision 1.7.1.1 2015/03/06 15:30:21EST sstrege 
** Added copyright information
** Revision 1.7 2010/08/04 15:17:40EDT rmcgraw 
** DCR11510:1 Changes prior to release
** Revision 1.6 2010/07/07 17:39:55EDT rmcgraw 
** DCR11510:1 Included cmds.h
** Revision 1.4 2010/04/23 13:42:36EDT rmcgraw 
** DCR11510:1 Changed playbackdir to playbackdirectory
** Revision 1.3 2010/04/23 08:39:18EDT rmcgraw 
** Dcr11510:1 Code Review Prep
** Revision 1.2 2010/03/26 15:30:25EDT rmcgraw 
** DCR11510 Various developmental changes
** Revision 1.1 2009/11/24 12:48:55EST rmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cf/fsw/src/project.pj
**
*************************************************************************/
#ifndef _cf_playback_h_
#define _cf_playback_h_


/************************************************************************
** Includes
*************************************************************************/
#include "cf_app.h"
#include "cf_cmds.h"


/************************************************************************
** Defines
*************************************************************************/


/************************************************************************
** Function Prototypes
*************************************************************************/


void CF_PlaybackFileCmd(CFE_SB_MsgPtr_t MessagePtr);
void CF_PlaybackDirectoryCmd(CFE_SB_MsgPtr_t MessagePtr);
CF_QueueEntry_t *CF_AllocQueueEntry(void);
int32 CF_DeallocQueueEntry(CF_QueueEntry_t *NodePtr);
int32 CF_AddFileToUpQueue(uint32 Queue, CF_QueueEntry_t *NewNode);
int32 CF_RemoveFileFromUpQueue(uint32 Queue, CF_QueueEntry_t *NodeToRemove);
int32 CF_AddFileToPbQueue(uint32 Chan, uint32 Queue, CF_QueueEntry_t *NewNode);
int32 CF_RemoveFileFromPbQueue(uint32 Chan, uint32 Queue, CF_QueueEntry_t *NodeToRemove);
int32 CF_GetChanNumFromTransId(uint32 Queue, uint32 Trans);
void CF_CheckPollDirs(uint32 Chan);
void CF_StartNextFile(uint32 Chan);
uint32 CF_FileIsOnQueue(uint32 Chan, uint32 Queue, char *Filename);
CF_QueueEntry_t *CF_DequeueUpNode(uint32 Queue);
CF_QueueEntry_t *CF_DequeuePbNode(uint32 Chan, uint32 Queue);
int32 CF_InsertPbNode(uint8 Chan, uint32 Queue, CF_QueueEntry_t *NodeToInsert,  
                    CF_QueueEntry_t *NodeAfterInsertPt);
int32 CF_InsertPbNodeAtFront(uint8 Chan, uint32 Queue, 
                    CF_QueueEntry_t *NodeToInsert);
int32 CF_QueueDirectoryFiles(CF_QueueDirFiles_t  *Ptr);
void CF_ProcessFileStartError(CF_QueueEntry_t *QueueEntryPtr,uint32 ErrType);

#endif /* _cf_playback_ */

/************************/
/*  End of File Comment */
/************************/
