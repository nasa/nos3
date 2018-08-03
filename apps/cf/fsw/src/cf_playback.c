/************************************************************************
** File:
**   $Id: cf_playback.c 1.29.1.1 2015/03/06 15:30:24EST sstrege Exp  $
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
** $Log: cf_playback.c  $
** Revision 1.29.1.1 2015/03/06 15:30:24EST sstrege 
** Added copyright information
** Revision 1.29 2011/05/17 17:09:17EDT rmcgraw 
** DCR14967:10 Change GetPoolBuf rtn check to include 0 as an error.
** Revision 1.28 2011/05/17 17:04:46EDT rmcgraw 
** DCR14967:9 removed parameter checks in QueueDirectoryFiles
** Revision 1.27 2011/05/09 11:52:10EDT rmcgraw 
** DCR13317:1 Allow Destintaion path to be blank
** Revision 1.26 2011/05/03 16:47:21EDT rmcgraw 
** Cleaned up events, removed \n from some events, removed event id ganging
** Revision 1.25 2010/11/04 11:37:47EDT rmcgraw 
** Dcr13051:1 Wrap OS_printfs in platform cfg CF_DEBUG
** Revision 1.24 2010/11/01 16:09:33EDT rmcgraw 
** DCR12802:1 Changes for decoupling peer entity id from channel
** Revision 1.23 2010/10/25 11:21:49EDT rmcgraw 
** DCR12573:1 Changes to allow more than one incoming PDU MsgId
** Revision 1.22 2010/08/04 15:17:37EDT rmcgraw 
** DCR11510:1 Changes prior to release
** Revision 1.21 2010/07/20 14:37:39EDT rmcgraw 
** Dcr11510:1 Remove Downlink buffer references
** Revision 1.20 2010/07/08 13:47:32EDT rmcgraw 
** DCR11510:1 Added termination checking on all cmds that take a string
** Revision 1.19 2010/07/07 17:39:29EDT rmcgraw 
** DCR11510:1 Added validate path calls and removed AddSlash calls
** Revision 1.18 2010/06/17 10:11:21EDT rmcgraw 
** DCR11510:1 Cleaned while loop logic in StartNextFile
** Revision 1.17 2010/06/11 16:13:16EDT rmcgraw 
** DCR11510:1 ZeroCopy, Un-hardcoded cmd/tlm input/output pdus
** Revision 1.16 2010/06/02 21:40:12EDT rmcgraw 
** DCR11510:1 Fixed cmd error counter problems with playback dir cmd
** Revision 1.15 2010/06/01 11:00:25EDT rmcgraw 
** DCR11510:1 Replaced polling tmp directory with pending queue check
** Revision 1.14 2010/04/27 09:13:31EDT rmcgraw 
** DCR11510:1 Fixed path params with or w/o ending slash in playback dir
** Revision 1.13 2010/04/26 14:37:42EDT rmcgraw 
** DCR11510:1 Fixed problem queueing files in a directory
** Revision 1.12 2010/04/26 14:01:23EDT rmcgraw 
** DCR11510:1 Changed strncmp to strncpy in CF_PlaybackDirectoryCmd
** Revision 1.11 2010/04/26 12:13:13EDT rmcgraw 
** DCR11510:1 Fixed Class check in QueueDirFiles and event in playbackfile cmd
** Revision 1.10 2010/04/26 10:11:25EDT rmcgraw 
** DCR11510:1 Fixed PB Directory cmd param error event
** Revision 1.9 2010/04/23 15:55:08EDT rmcgraw 
** DCR11510:1 Fixed pointer problem
** Revision 1.8 2010/04/23 15:44:39EDT rmcgraw 
** DCR11510:1 Protection against starting a transfer if file is already active
** Revision 1.7 2010/04/23 08:39:15EDT rmcgraw 
** Dcr11510:1 Code Review Prep
** Revision 1.6 2010/03/26 15:30:22EDT rmcgraw 
** DCR11510 Various developmental changes
** Revision 1.5 2010/03/12 12:14:33EST rmcgraw 
** DCR11510:1 Initial check-in towards CF Version 1000
** Revision 1.4 2009/12/09 11:06:27EST rmcgraw 
** DCR10350:3 Removed Dest path check in CF_FindNodeAtFrontOfQueue
** Revision 1.3 2009/12/08 09:17:13EST rmcgraw 
** DCR10350:3 Fixed bug by checking active queue for file before enqueue
** Revision 1.2 2009/11/24 13:58:37EST rmcgraw 
** DCR10349:1 Moved polling check printf
** Revision 1.1 2009/11/24 12:48:54EST rmcgraw 
** Initial revision
** Member added to CFS CF project
**
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/
#include "cf_app.h"
#include "cf_msg.h"
#include "cf_defs.h"
#include "cf_verify.h"
#include "cf_events.h"
#include "cf_utils.h"
#include "cf_playback.h"
#include "cf_callbacks.h"
#include "cf_perfids.h"
#include <string.h>


/************************************************************************
** CF global data
*************************************************************************/
#ifdef CF_DEBUG
extern uint32              cfdbg;
#endif

extern CF_AppData_t        CF_AppData;


void CF_PlaybackFileCmd(CFE_SB_MsgPtr_t MessagePtr)
{

    CF_PlaybackFileCmd_t    *PlaybackFileCmdPtr;
    CF_QueueEntry_t         *NewQueueEntry;
    
    if(CF_VerifyCmdLength(MessagePtr,sizeof(CF_PlaybackFileCmd_t))==CF_BAD_MSG_LENGTH_RC)
    {

        CF_AppData.Hk.ErrCounter++;

    }else{

        PlaybackFileCmdPtr = (CF_PlaybackFileCmd_t  *)MessagePtr;  
                
        /* check params */
        if( (PlaybackFileCmdPtr->Class == 0) ||
            (PlaybackFileCmdPtr->Class > 2) ||
            (PlaybackFileCmdPtr->Channel >= CF_MAX_PLAYBACK_CHANNELS))
        {
        
            CFE_EVS_SendEvent(CF_PB_FILE_ERR1_EID,CFE_EVS_ERROR,
                "Playback File Cmd Parameter error, class %d, chan %d",
                PlaybackFileCmdPtr->Class,PlaybackFileCmdPtr->Channel);
            CF_AppData.Hk.ErrCounter++;
            return;
        }

        /* Check that the channel is "in use" (defined in table) */
        /* Cannot gang this test with above checks because chan param may be out of bounds */
        if(CF_AppData.Tbl->OuCh[PlaybackFileCmdPtr->Channel].EntryInUse == CF_ENTRY_UNUSED)
        {    
            CFE_EVS_SendEvent(CF_PB_FILE_ERR2_EID,CFE_EVS_ERROR,
                "CF:Playback File Cmd Parameter Error, Chan %u is not in use.",
                PlaybackFileCmdPtr->Channel);
            CF_AppData.Hk.ErrCounter++;
            return;
        }

        /* check that the filename has forward slash at start, no spaces and */
        /* is properly terminated */
        if(CF_ValidateFilenameReportErr(PlaybackFileCmdPtr->SrcFilename, 
                                        "PlaybackFileCmd")==CF_ERROR)
        {
            CF_AppData.Hk.ErrCounter++;
            return;
        } 
        
        /* check that the filename has forward slash at start, no spaces and */
        /* is properly terminated */
        if(CF_ValidateFilenameReportErr(PlaybackFileCmdPtr->DstFilename, 
                                        "PlaybackFileCmd")==CF_ERROR)
        {
            CF_AppData.Hk.ErrCounter++;
            return;
        }         
        
        /* Check that the there is enough room on the pending queue */
        if(CF_AppData.Chan[PlaybackFileCmdPtr->Channel].PbQ[CF_PB_PENDINGQ].EntryCnt >=
            CF_AppData.Tbl->OuCh[PlaybackFileCmdPtr->Channel].PendingQDepth)
        {    
            CFE_EVS_SendEvent(CF_PB_FILE_ERR3_EID,CFE_EVS_ERROR,
                "CF:Playback File Cmd Error, Chan %u Pending Queue is full %u.",
                PlaybackFileCmdPtr->Channel,
                CF_AppData.Chan[PlaybackFileCmdPtr->Channel].PbQ[CF_PB_PENDINGQ].EntryCnt );
            CF_AppData.Hk.ErrCounter++;
            return;
        }    
    
        /* check peer entity ID format */
        if(CF_ValidateEntityId(PlaybackFileCmdPtr->PeerEntityId) == CF_ERROR)
        {
            CFE_EVS_SendEvent(CF_PB_FILE_ERR6_EID,CFE_EVS_ERROR,
                "CF:PB File Cmd Err, PeerEntityId %s must be 2 byte,dotted decimal fmt.ex 0.24",
                PlaybackFileCmdPtr->PeerEntityId);
            CF_AppData.Hk.ErrCounter++;
            return;        
        }

        /* Be sure the file is not open */
        if(CF_FileOpenCheck(PlaybackFileCmdPtr->SrcFilename) == CF_OPEN)
        {
            CFE_EVS_SendEvent(CF_PB_FILE_ERR4_EID,CFE_EVS_ERROR,
                "CF:Playback File Cmd Error, File is Open:%s",
                PlaybackFileCmdPtr->SrcFilename);
            CF_AppData.Hk.ErrCounter++;
            return;
        }        
    

        /* check that the file is not already pending or active */            
        if((CF_FileIsOnQueue(PlaybackFileCmdPtr->Channel, CF_PB_PENDINGQ, PlaybackFileCmdPtr->SrcFilename)==CF_TRUE)||
           (CF_FileIsOnQueue(PlaybackFileCmdPtr->Channel, CF_PB_ACTIVEQ, PlaybackFileCmdPtr->SrcFilename)==CF_TRUE))
        {
            CFE_EVS_SendEvent(CF_PB_FILE_ERR5_EID,CFE_EVS_ERROR,
                "CF:Playback File Cmd Error, File is Already Pending or Active:%s",
                PlaybackFileCmdPtr->SrcFilename);
            CF_AppData.Hk.ErrCounter++;
            return;
        }


        /* allocate queue entry */
        NewQueueEntry = CF_AllocQueueEntry();
        if(NewQueueEntry == NULL)
        {
            /* Ran out of memory 
            ** Options: Free memory by removing history nodes (Dequeue Cmd) or
            **          Reconfigure Mem pool size in cf_platform_cfg.h(requires recompile) or
            **          Lower History-Queue-Depth settings in table (requires table load/reboot app)
            */

            CFE_EVS_SendEvent(CF_QDIR_NOMEM1_EID,CFE_EVS_ERROR,
                "PB File %s Cmd Ignored,Error Allocating Queue Node.", 
                PlaybackFileCmdPtr->SrcFilename);
            CF_AppData.Hk.ErrCounter++;    
            return;
        }
        
        /* fill-in queue entry */
        NewQueueEntry->Class    = PlaybackFileCmdPtr->Class;
        NewQueueEntry->Priority = PlaybackFileCmdPtr->Priority;
        NewQueueEntry->ChanNum  = PlaybackFileCmdPtr->Channel;
        NewQueueEntry->Source   = CF_PLAYBACKFILECMD;
        NewQueueEntry->NodeType = CF_OUTGOING;
        NewQueueEntry->CondCode = 0;
        NewQueueEntry->Status   = CF_STAT_PENDING;
        NewQueueEntry->Warning  = CF_NOT_ISSUED;
        NewQueueEntry->Preserve = PlaybackFileCmdPtr->Preserve;
        strncpy(&NewQueueEntry->SrcEntityId[0],CF_AppData.Hk.Eng.FlightEngineEntityId,CF_MAX_CFG_VALUE_CHARS);
        strncpy(&NewQueueEntry->PeerEntityId[0],PlaybackFileCmdPtr->PeerEntityId,CF_MAX_CFG_VALUE_CHARS);
        strncpy(&NewQueueEntry->SrcFile[0],PlaybackFileCmdPtr->SrcFilename,OS_MAX_PATH_LEN);
        strncpy(&NewQueueEntry->DstFile[0],PlaybackFileCmdPtr->DstFilename,OS_MAX_PATH_LEN);
        
        /* ensure strings are null terminated */
        NewQueueEntry->SrcFile[OS_MAX_PATH_LEN - 1] = '\0';
        NewQueueEntry->DstFile[OS_MAX_PATH_LEN - 1] = '\0';
            
        /* place queue entry on queue */
        CF_AddFileToPbQueue(PlaybackFileCmdPtr->Channel, CF_PB_PENDINGQ, NewQueueEntry);
        
        CF_PendingQueueSort(PlaybackFileCmdPtr->Channel);
        
        CF_AppData.Hk.CmdCounter++;
    
        CFE_EVS_SendEvent(CF_PLAYBACK_FILE_EID,CFE_EVS_DEBUG,
          "Playback File Cmd Rcvd,Cl %d,Ch %d,Pri %d,Pre %d,Peer %s,File %s",
                            PlaybackFileCmdPtr->Class,
                            PlaybackFileCmdPtr->Channel,
                            PlaybackFileCmdPtr->Priority,
                            PlaybackFileCmdPtr->Preserve,
                            PlaybackFileCmdPtr->PeerEntityId,
                            PlaybackFileCmdPtr->SrcFilename);


    }

}/* end CF_PlaybackFileCmd */



void CF_PlaybackDirectoryCmd(CFE_SB_MsgPtr_t MessagePtr)
{

    CF_PlaybackDirCmd_t     *PlaybackDirCmdPtr;
    CF_QueueDirFiles_t      LocalStruct;
    CF_QueueDirFiles_t      *LocalPtr = &LocalStruct;
    int32       Status;
    
    if(CF_VerifyCmdLength(MessagePtr,sizeof(CF_PlaybackDirCmd_t))==CF_BAD_MSG_LENGTH_RC)
    {

        CF_AppData.Hk.ErrCounter++;

    }else{

        PlaybackDirCmdPtr = (CF_PlaybackDirCmd_t  *)MessagePtr;  
            
        /* check params */
        if( (PlaybackDirCmdPtr->Class == 0) ||
            (PlaybackDirCmdPtr->Class > 2) ||
            (PlaybackDirCmdPtr->Chan >= CF_MAX_PLAYBACK_CHANNELS)||
            (PlaybackDirCmdPtr->Preserve > 2))
        {
        
            CFE_EVS_SendEvent(CF_PB_DIR_ERR1_EID,CFE_EVS_ERROR,
                "Playback Dir Cmd Parameter error,class %d,chan %d,preserve %d",
                PlaybackDirCmdPtr->Class,PlaybackDirCmdPtr->Chan,
                PlaybackDirCmdPtr->Preserve);
            CF_AppData.Hk.ErrCounter++;
            return;
        }
        
        
        /* Check that the channel is "in use" (defined in table) */
        /* Cannot gang this test with above checks because chan param may be out of bounds */
        if(CF_AppData.Tbl->OuCh[PlaybackDirCmdPtr->Chan].EntryInUse == CF_ENTRY_UNUSED)
        {    
            CFE_EVS_SendEvent(CF_PB_DIR_ERR2_EID,CFE_EVS_ERROR,
                "CF:Playback Dir Cmd Parameter Error, Chan %u is not in use.",
                PlaybackDirCmdPtr->Chan);
            CF_AppData.Hk.ErrCounter++;
            return;
        }
                            
        /* check that the paths are terminated, have */
        /* forward slash at last character and no spaces*/
        if(CF_ValidateSrcPath(PlaybackDirCmdPtr->SrcPath)==CF_ERROR)
        {
            CFE_EVS_SendEvent(CF_PB_DIR_ERR3_EID, CFE_EVS_ERROR,
                "SrcPath in PB Dir Cmd must be terminated,have no spaces,slash at end");
            CF_AppData.Hk.ErrCounter++;
            return;
        }
        
        if(CF_ValidateDstPath(PlaybackDirCmdPtr->DstPath)==CF_ERROR)
        {
            CFE_EVS_SendEvent(CF_PB_DIR_ERR4_EID, CFE_EVS_ERROR,
                "DstPath in PB Dir Cmd must be terminated and have no spaces");
            CF_AppData.Hk.ErrCounter++;
            return;
        }        

        /* check peer entity ID format */
        if(CF_ValidateEntityId(PlaybackDirCmdPtr->PeerEntityId) == CF_ERROR)
        {
            CFE_EVS_SendEvent(CF_PB_DIR_ERR5_EID,CFE_EVS_ERROR,
                "CF:PB Dir Cmd Err,PeerEntityId %s must be 2 byte,dotted decimal fmt.ex 0.24",
                PlaybackDirCmdPtr->PeerEntityId);
            CF_AppData.Hk.ErrCounter++;
            return;        
        }

        /* Copy cmd params to the structure needed for CF_QueueDirectoryFiles */
        LocalStruct.Chan = PlaybackDirCmdPtr->Chan;
        LocalStruct.Class = PlaybackDirCmdPtr->Class;
        LocalStruct.Priority = PlaybackDirCmdPtr->Priority;
        LocalStruct.Preserve = PlaybackDirCmdPtr->Preserve;
        LocalStruct.CmdOrPoll = CF_PLAYBACKDIRCMD; 

        strncpy(&LocalStruct.PeerEntityId[0],
                PlaybackDirCmdPtr->PeerEntityId,
                CF_MAX_CFG_VALUE_CHARS);
        strncpy(&LocalStruct.SrcPath[0],
                &PlaybackDirCmdPtr->SrcPath[0],
                OS_MAX_PATH_LEN);
        strncpy(&LocalStruct.DstPath[0],
                &PlaybackDirCmdPtr->DstPath[0],
                OS_MAX_PATH_LEN);

        Status = CF_QueueDirectoryFiles(LocalPtr);
    
        if(Status == CF_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_PLAYBACK_DIR_EID,CFE_EVS_DEBUG,
              "Playback Dir Cmd Rcvd,Ch %d,Cl %d,Pri %d,Pre %d,Peer %s, Src %s,Dst %s",
                                PlaybackDirCmdPtr->Chan,
                                PlaybackDirCmdPtr->Class,
                                PlaybackDirCmdPtr->Priority,
                                PlaybackDirCmdPtr->Preserve,
                                PlaybackDirCmdPtr->PeerEntityId,
                                PlaybackDirCmdPtr->SrcPath,
                                PlaybackDirCmdPtr->DstPath);
            
        
            CF_AppData.Hk.CmdCounter++;
        
        }else{
        
            CF_AppData.Hk.ErrCounter++;
        }

    }

}/* end CF_PlaybackDirectoryCmd */


/******************************************************************************
**  Function:   CF_QueueDirectoryFiles()
**
**  Purpose:
**    This function loads the pending queue with the closed files from the given 
**    directory. 
**
**  Arguments:
**    
**
**  Return:
**    
*/

int32 CF_QueueDirectoryFiles(CF_QueueDirFiles_t  *Ptr)
{
    os_dirp_t       dirp;
    os_dirent_t     *direntp;
    char            FullSrcName[OS_MAX_PATH_LEN];
    char            FullDstName[OS_MAX_PATH_LEN];
    CF_QueueEntry_t *NewQueueEntry;
    uint32          SrcPathLength,DstPathLength,FilenameLength;

    CFE_ES_PerfLogEntry(CF_QDIRFILES_PERF_ID);
                                        
    /* parameters have been checked earlier */
            
    /* open the directory to access the files */
    if((dirp = OS_opendir(Ptr->SrcPath)) == NULL) /* directory is invalid */
    {
        CFE_EVS_SendEvent(CF_OPEN_DIR_ERR_EID,CFE_EVS_ERROR,
            "Playback Dir Error %d,cannot open directory %s",dirp,Ptr->SrcPath);
        CFE_ES_PerfLogExit(CF_QDIRFILES_PERF_ID);
        return CF_ERROR;
    }
    
    SrcPathLength = strlen(Ptr->SrcPath);
    DstPathLength = strlen(Ptr->DstPath);

    CFE_ES_PerfLogExit(CF_QDIRFILES_PERF_ID);
    
    while ((direntp = OS_readdir (dirp)) != NULL)
    {                                                                                                     
        CFE_ES_PerfLogEntry(CF_QDIRFILES_PERF_ID);
    
        /* if file is a 'dot' directory... continue to next file */
        if((strcmp(direntp->d_name,".") == 0) || 
           (strcmp(direntp->d_name,"..") == 0))
        {
            CFE_ES_PerfLogExit(CF_QDIRFILES_PERF_ID);
            continue;
        }
        
        /* if it's a sub directory... continue to next file */
        #ifdef DT_DIR
        if(direntp->d_type == DT_DIR)
        {
            CFE_ES_PerfLogExit(CF_QDIRFILES_PERF_ID);
            continue;
        }
        #endif

        /* Check that there is enough room on the pending queue */
        if(CF_AppData.Chan[Ptr->Chan].PbQ[CF_PB_PENDINGQ].EntryCnt >=
            CF_AppData.Tbl->OuCh[Ptr->Chan].PendingQDepth)
        {    
            CFE_EVS_SendEvent(CF_QDIR_PQFUL_EID,CFE_EVS_ERROR,
                "Queue Dir %s Aborted,Ch %d Pending Queue is Full,%u Entries",
                Ptr->SrcPath,Ptr->Chan, 
                CF_AppData.Chan[Ptr->Chan].PbQ[CF_PB_PENDINGQ].EntryCnt);
            
            OS_closedir(dirp);
            CFE_ES_PerfLogExit(CF_QDIRFILES_PERF_ID);
            return CF_ERROR;
        }

        if(CF_ChkTermination(direntp->d_name,OS_MAX_PATH_LEN)==CF_ERROR)
        {
            CFE_EVS_SendEvent(CF_QDIR_INV_NAME1_EID,CFE_EVS_ERROR,
                "File not queued from %s,Filename not terminated or too long",Ptr->SrcPath);

            CFE_ES_PerfLogExit(CF_QDIRFILES_PERF_ID);
            continue;
        }        
        
        FilenameLength = strlen(direntp->d_name);
        
        if(((FilenameLength + SrcPathLength) >= OS_MAX_PATH_LEN) ||
           ((FilenameLength + DstPathLength) >= OS_MAX_PATH_LEN))
        {
            CFE_EVS_SendEvent(CF_QDIR_INV_NAME2_EID,CFE_EVS_ERROR,
                "File not queued from %s,sum of Pathname,Filename too long",Ptr->SrcPath);
            CFE_ES_PerfLogExit(CF_QDIRFILES_PERF_ID);
            continue;
        }

        FullSrcName[0] = '\0';
        FullDstName[0] = '\0';
                
        /* Append filename to src path */ 
        strcat(FullSrcName,Ptr->SrcPath);
        strcat(FullSrcName,direntp->d_name);

        /* Append filename to dst path */
        strcat(FullDstName,Ptr->DstPath);
        strcat(FullDstName,direntp->d_name);
                
        /* check that the file is not already pending or active */            
        if((CF_FileIsOnQueue(Ptr->Chan, CF_PB_PENDINGQ, FullSrcName)==CF_TRUE)||
           (CF_FileIsOnQueue(Ptr->Chan, CF_PB_ACTIVEQ, FullSrcName)==CF_TRUE))
        {
            CFE_EVS_SendEvent(CF_QDIR_ACTIVEFILE_EID,CFE_EVS_DEBUG,
                    "File %s not queued because it's active or pending",FullSrcName);

            CFE_ES_PerfLogExit(CF_QDIRFILES_PERF_ID);
            continue;
        }

        /* check that the file is not open */
        if(CF_FileOpenCheck(FullSrcName) == CF_OPEN)
        {               
            CFE_EVS_SendEvent(CF_QDIR_OPENFILE_EID,CFE_EVS_INFORMATION,
                "File %s not queued because it's open",FullSrcName);
            CFE_ES_PerfLogExit(CF_QDIRFILES_PERF_ID);
            continue;
        }
        
        /* allocate queue entry */
        NewQueueEntry = CF_AllocQueueEntry();
        if(NewQueueEntry == NULL)
        {
            /* Ran out of memory 
            ** Options: Free memory by removing history nodes (Dequeue Cmd) or
            **          Reconfigure Mem pool size in cf_platform_cfg.h(requires recompile) or
            **          Lower History-Queue-Depth settings in table (requires table load/reboot app)
            */

            CFE_EVS_SendEvent(CF_QDIR_NOMEM2_EID,CFE_EVS_ERROR,
                "PB Dir %s Aborted,Error Allocating Queue Node.", 
                Ptr->SrcPath);                                   
            OS_closedir(dirp);
            CFE_ES_PerfLogExit(CF_QDIRFILES_PERF_ID);
            return CF_ERROR;
        }
        
        if(Ptr->CmdOrPoll == CF_POLLDIRECTORY)
        {
            NewQueueEntry->Source = CF_POLLDIRECTORY;        
        }else{                    
            NewQueueEntry->Source = CF_PLAYBACKDIRCMD;        
        }/* end if */

        /* fill-in queue entry */
        NewQueueEntry->Class    = Ptr->Class;
        NewQueueEntry->Priority = Ptr->Priority;
        NewQueueEntry->ChanNum  = Ptr->Chan;
        NewQueueEntry->Preserve = Ptr->Preserve;            
        NewQueueEntry->NodeType = CF_OUTGOING;
        NewQueueEntry->Status   = CF_STAT_PENDING;
        NewQueueEntry->Warning  = CF_NOT_ISSUED;
        NewQueueEntry->CondCode = 0;


        strncpy(NewQueueEntry->SrcEntityId,
                CF_AppData.Hk.Eng.FlightEngineEntityId,
                CF_MAX_CFG_VALUE_CHARS); 
                
        strncpy(NewQueueEntry->PeerEntityId,
                Ptr->PeerEntityId,
                CF_MAX_CFG_VALUE_CHARS);
                
        strncpy(NewQueueEntry->SrcFile,FullSrcName,OS_MAX_PATH_LEN);
        strncpy(NewQueueEntry->DstFile,FullDstName,OS_MAX_PATH_LEN);        
            
        /* place queue entry on queue */
        CF_AddFileToPbQueue(Ptr->Chan, CF_PB_PENDINGQ, NewQueueEntry);
        
        CF_PendingQueueSort(Ptr->Chan);                                
        
#ifdef CF_DEBUG
        if(cfdbg > 1)
        {                      
          OS_printf("CF:Queueing File %s\n",NewQueueEntry->SrcFile);
          OS_printf("CF:Ch %d,Cl %d,Pri %d,Pre %d,Src %d\n",
                    NewQueueEntry->ChanNum,
                    NewQueueEntry->Class,
                    NewQueueEntry->Priority,
                    NewQueueEntry->Preserve,
                    NewQueueEntry->Source);
          OS_printf("CF:Dest Filename %s\n\n",NewQueueEntry->DstFile);
        }
#endif                                                   
        CFE_ES_PerfLogExit(CF_QDIRFILES_PERF_ID);
    
    }/* end while */
    
       
#ifdef CF_DEBUG
    if(cfdbg > 10)
        OS_printf("Closing Dir %s, dirp = %x\n",Ptr->SrcPath,dirp);
#endif    

    OS_closedir(dirp);
    
    return CF_SUCCESS;

}/* end CF_QueueDirectoryFiles */    


/******************************************************************************
**  Function:   CF_AllocQueueEntry()
**
**  Purpose:
**    This function gets a queue entry from the CF memory pool.
**
**  Arguments:
**    None
**
**  Return:
**    Pointer to the queue entry
*/
CF_QueueEntry_t *CF_AllocQueueEntry(void)
{
    int32 Stat;
    CF_QueueEntry_t *QueueEntry = NULL;
        
    /* Allocate a new queue entry from the CF memory pool.*/
    Stat = CFE_ES_GetPoolBuf((uint32 **)&QueueEntry, CF_AppData.Mem.PoolHdl,  sizeof(CF_QueueEntry_t));
    if(Stat <= 0){
        CFE_EVS_SendEvent(CF_MEM_ALLOC_ERR_EID,CFE_EVS_ERROR,
         "Memory Allocation Error, GetPoolBuf Returned 0x%x, dec %d",Stat,Stat);                       
        return NULL;
    }
        
    /* Add the size returned to the memory-in-use ctr and */
    /* adjust the high water mark if needed */
    CF_AppData.Hk.App.MemInUse += Stat;
    if(CF_AppData.Hk.App.MemInUse > CF_AppData.Hk.App.PeakMemInUse){
       CF_AppData.Hk.App.PeakMemInUse = CF_AppData.Hk.App.MemInUse;
    }/* end if */

#ifdef CF_DEBUG
    if(cfdbg > 5)
        OS_printf("CF:Queue Entry Allocated.size=%d,allocated %d,meminuse=%d\n",
                    sizeof(CF_QueueEntry_t),Stat,CF_AppData.Hk.App.MemInUse);
#endif    

    CF_AppData.Hk.App.QNodesAllocated++;
    
    return QueueEntry;

}/* end CF_AllocQueueEntry */



/******************************************************************************
**  Function:   CF_DeallocQueueEntry()
**
**  Purpose:
**    This function returns a queue entry to the CF memory pool.
**
**  Arguments:
**    None Pointer to the
**
**  Return:
**    Error for bad argument, otherwise success
*/
int32 CF_DeallocQueueEntry(CF_QueueEntry_t *Entry)
{
    int32 Stat;

    if(Entry==NULL){
        return CF_ERROR;
    }/* end if */
    
    /* give the destination block back to the SB memory pool */
    Stat = CFE_ES_PutPoolBuf(CF_AppData.Mem.PoolHdl, (uint32 *)Entry);
    if(Stat > 0){
            
        /* Substract the size of the queue entry from the Memory in use ctr */
        CF_AppData.Hk.App.MemInUse-=Stat;
    
    }else{
            CFE_EVS_SendEvent(CF_MEM_DEALLOC_ERR_EID,CFE_EVS_ERROR,
                "Deallocation failed for queue entry. Stat = %d",Stat);
            return CF_ERROR;
    }/* end if */

#ifdef CF_DEBUG
    if(cfdbg > 5)
        OS_printf("CF:Queue Entry Deallocated. size = %d\n",Stat);    
#endif
    CF_AppData.Hk.App.QNodesDeallocated++;
    
    return CF_SUCCESS;

}/* end CF_DeallocQueueEntry */


/******************************************************************************
**  Function:  CF_AddFileToPbQueue()
**
**  Purpose:
**      This function will add the given node to the head of the given playback 
**      queue.
**  Arguments:
**      Chan - Playback Output Channel
**      Queue - Pending,Active or History Queue (0,1,2 respectively)
**      NewNode - Pointer to the entry to add to the list
**
**  Return:
**
*/
int32 CF_AddFileToPbQueue(uint32 Chan, uint32 Queue, CF_QueueEntry_t *NewNode){

    CF_QueueEntry_t *WBS;/* Will Be Second (WBS) node */  
    
    if((Chan >= CF_MAX_PLAYBACK_CHANNELS) || (NewNode == NULL) || (Queue > 2))
    {
#ifdef CF_DEBUG
        OS_printf ("CF:Bad Param,CF_AddFileToPbQueue,Chan %d,Queue %u,Ptr=%p\n",
                    Chan,Queue,NewNode);
#endif                    
        return CF_ERROR;
        
    }/* end if */


    /* if first node in list */
    if(CF_AppData.Chan[Chan].PbQ[Queue].HeadPtr == NULL){
    
        /* initialize the new node */
        NewNode->Next = NULL;   
        NewNode->Prev = NULL; 
        
        /* insert the new node */
        CF_AppData.Chan[Chan].PbQ[Queue].HeadPtr = NewNode;
        CF_AppData.Chan[Chan].PbQ[Queue].TailPtr = NewNode;
        
    }else{
    
        WBS = CF_AppData.Chan[Chan].PbQ[Queue].HeadPtr;
    
        /* initialize the new node */
        NewNode->Next = WBS;   
        NewNode->Prev = NULL;
        
        /* insert the new node */
        WBS -> Prev = NewNode;    
        CF_AppData.Chan[Chan].PbQ[Queue].HeadPtr = NewNode;
        
    }/* end if */
        

    CF_AppData.Chan[Chan].PbQ[Queue].EntryCnt++;    

#ifdef CF_DEBUG
    if(cfdbg > 3)
        OS_printf("CF:File %s added to Chan %d, Queue %d, file cnt %d\n",
                    NewNode->SrcFile,Chan,Queue,CF_AppData.Chan[Chan].PbQ[Queue].EntryCnt);
#endif

    return CF_SUCCESS;

}/* CF_AddFileToPbQueue */




/******************************************************************************
**  Function:  CF_RemoveFileFromPbQueue()
**
**  Purpose:
**      This function will remove the given node from the list.
**      This function assumes there is at least one node in the list.
**
**  Arguments:
**      Chan - Playback Output Channel
**      Queue - Pending,Active or History Queue (0,1,2 respectively)
**      NodeToRemove - Pointer to the entry to remove from the list
**
**  Return:
**
*/
int32 CF_RemoveFileFromPbQueue(uint32 Chan, uint32 Queue, CF_QueueEntry_t *NodeToRemove){

    CF_QueueEntry_t *PrevNode;
    CF_QueueEntry_t *NextNode;
    
    if((Chan >= CF_MAX_PLAYBACK_CHANNELS) || (NodeToRemove == NULL) || (Queue > 2))
    {
#ifdef CF_DEBUG
        OS_printf ("Bad Param,CF_RemoveFileFromPbQueue,Chan %d, Queue %d, Ptr = %p\n",
                    Chan,Queue,NodeToRemove);
#endif                    
        return CF_ERROR;
        
    }/* end if */


    /* if this is the only node in the list */
    if((NodeToRemove->Prev == NULL) && (NodeToRemove->Next == NULL)){
    
        CF_AppData.Chan[Chan].PbQ[Queue].HeadPtr = NULL;
        CF_AppData.Chan[Chan].PbQ[Queue].TailPtr = NULL;
        
    /* if first node in the list and list has more than one */
    }else if(NodeToRemove->Prev == NULL){
        
        NextNode = NodeToRemove->Next;
        
        NextNode -> Prev = NULL;
        
        CF_AppData.Chan[Chan].PbQ[Queue].HeadPtr = NextNode;
        
    /* if last node in the list and list has more than one */
    }else if(NodeToRemove->Next == NULL){
    
        PrevNode = NodeToRemove->Prev;
        
        PrevNode -> Next = NULL;
        
        CF_AppData.Chan[Chan].PbQ[Queue].TailPtr = PrevNode;
        
    /* NodeToRemove has node(s) before and node(s) after */
    }else{
    
        PrevNode = NodeToRemove->Prev;
        NextNode = NodeToRemove->Next;
        
        PrevNode -> Next = NextNode;
        NextNode -> Prev = PrevNode;
        
    }/* end if */
        
    
    /* initialize the node before returning it to the heap */
    NodeToRemove -> Next = NULL;
    NodeToRemove -> Prev = NULL; 
    
    CF_AppData.Chan[Chan].PbQ[Queue].EntryCnt--;
    
    return CF_SUCCESS;

}/* CF_RemoveFileFromPbQueue */



/******************************************************************************
**  Function:  CF_InsertPbNode()
**
**  Purpose:
**      This function will insert a playback node in a list.
**  Arguments:
**      Chan - Playabck channel
**      Queue - Pending, Active or History Queue (0,1,2 respectively)
**      NewNode - Entry pointer of node to insert
**
**  Return:
**
**  Note: This function does not verify that NodeAfterInsertPt belongs to the 
**          channel or queue given as parameters
**
*/
int32 CF_InsertPbNode(uint8 Chan, uint32 Queue, CF_QueueEntry_t *NodeToInsert, 
                        CF_QueueEntry_t *NodeAfterInsertPt)
{

    CF_QueueEntry_t     *NodeBeforeInsertPt;
    
    if((Chan >= CF_MAX_PLAYBACK_CHANNELS)  || (Queue > 2) ||
            (NodeToInsert == NULL) || (NodeAfterInsertPt == NULL))
    {
#ifdef CF_DEBUG    
        OS_printf ("Bad Param,CF_InsertPbNode,Chan %d,Queue %d,InsertNode %p,NodeAfter %p\n",
                    Chan,Queue,NodeToInsert,NodeAfterInsertPt);
#endif                    
        return CF_ERROR;
        
    }/* end if */

    /* Find node before Insetion point */
    NodeBeforeInsertPt = NodeAfterInsertPt -> Prev;
    
    /* initialize new node */
    NodeToInsert -> Next = NodeAfterInsertPt;
    NodeToInsert -> Prev = NodeBeforeInsertPt;
        
    /* insert node */
    NodeBeforeInsertPt -> Next = NodeToInsert;
    NodeAfterInsertPt  -> Prev = NodeToInsert;
    
    CF_AppData.Chan[Chan].PbQ[Queue].EntryCnt++;

    return CF_SUCCESS;

}/* end CF_InsertPbNode */


/******************************************************************************
**  Function:  CF_InsertPbNodeAtFront()
**
**  Purpose:
**      This function will insert a playback node at the front of the queue.
**      The front of the queue is the tail of the list.
**  Arguments:
**      Chan - Playabck channel
**      Queue - Pending, Active or History Queue (0,1,2 respectively)
**      NewNode - Entry pointer of node to insert
**
**  Return:
**
*/
int32 CF_InsertPbNodeAtFront(uint8 Chan,uint32 Queue, CF_QueueEntry_t *NodeToInsert)
{

    CF_QueueEntry_t     *OldNodeAtFront;
    
    if((Chan >= CF_MAX_PLAYBACK_CHANNELS)  || (Queue > 2) ||
            (NodeToInsert == NULL))
    {
#ifdef CF_DEBUG    
        OS_printf ("Bad Param,CF_InsertPbNodeAtFront,Chan %d,Queue %d,InsertNode %p\n",
                    Chan,Queue,NodeToInsert);
#endif                    
        return CF_ERROR;
        
    }/* end if */

    OldNodeAtFront = CF_AppData.Chan[Chan].PbQ[Queue].TailPtr;
    
    /* Initialize the Node To Insert */
    NodeToInsert -> Next = NULL;
    NodeToInsert -> Prev = OldNodeAtFront;
    
    /* insert node */
    OldNodeAtFront -> Next = NodeToInsert;
    CF_AppData.Chan[Chan].PbQ[Queue].TailPtr = NodeToInsert;
    
    CF_AppData.Chan[Chan].PbQ[Queue].EntryCnt++;

    return CF_SUCCESS;

}/* end CF_InsertPbNodeAtFront */


/******************************************************************************
**  Function:  CF_AddFileToUpQueue()
**
**  Purpose:
**      This function will add the given node to the head of the given uplink 
**      queue.
**  Arguments:
**      Queue - Active or History Queue (0 or 1 respectively)
**      NewNode - Entry pointer of node to add
**
**  Return:
**
*/
int32 CF_AddFileToUpQueue(uint32 Queue, CF_QueueEntry_t *NewNode){

    CF_QueueEntry_t *WBS;/* Will Be Second (WBS) node */  
    
    if((NewNode == NULL) || (Queue > 1))
    {
    
#ifdef CF_DEBUG
        if(cfdbg > 0)
        OS_printf ("CF:Bad Param,CF_AddFileToUpQueue,Queue %s, Ptr=%p\n",
                    Queue,NewNode);
#endif                    
        return CF_ERROR;
        
    }/* end if */


    /* if first node in list */
    if(CF_AppData.UpQ[Queue].HeadPtr == NULL){
    
        /* initialize the new node */
        NewNode->Next = NULL;   
        NewNode->Prev = NULL; 
        
        /* insert the new node */
        CF_AppData.UpQ[Queue].HeadPtr = NewNode;
        CF_AppData.UpQ[Queue].TailPtr = NewNode;
        
    }else{
    
        WBS = CF_AppData.UpQ[Queue].HeadPtr;
    
        /* initialize the new node */
        NewNode->Next = WBS;   
        NewNode->Prev = NULL;
        
        /* insert the new node */
        WBS -> Prev = NewNode;    
        CF_AppData.UpQ[Queue].HeadPtr = NewNode;
        
    }/* end if */
    
    
    CF_AppData.UpQ[Queue].EntryCnt++;
    
#ifdef CF_DEBUG
    if(cfdbg > 5)
        OS_printf("CF:File %s added to Uplink Queue %d, file cnt %d\n",
                    NewNode->SrcFile,Queue,CF_AppData.UpQ[Queue].EntryCnt);
#endif

    return CF_SUCCESS;

}/* CF_AddFileToUpQueue */




/******************************************************************************
**  Function:  CF_RemoveFileFromUpQueue()
**
**  Purpose:
**      This function will remove the given node from the list.
**      This function assumes there is at least one node in the list.
**
**  Arguments:
**      Queue - Active or History Uplink Queue (0,1 respectively)
**      NodeToRemove - Pointer to the entry to remove from the list
**
**  Return:
**
*/
int32 CF_RemoveFileFromUpQueue(uint32 Queue, CF_QueueEntry_t *NodeToRemove){

    CF_QueueEntry_t *PrevNode;
    CF_QueueEntry_t *NextNode;
    
    if((NodeToRemove == NULL) || (Queue > 1))
    {    
#ifdef CF_DEBUG
        if(cfdbg > 0)
        OS_printf ("Bad Param,CF_RemoveFileFromUpQueue, Queue %d, Ptr = %p\n",
                    Queue,NodeToRemove);
#endif                    
        return CF_ERROR;
        
    }/* end if */


    /* if this is the only node in the list */
    if((NodeToRemove->Prev == NULL) && (NodeToRemove->Next == NULL)){
    
        CF_AppData.UpQ[Queue].HeadPtr = NULL;
        CF_AppData.UpQ[Queue].TailPtr = NULL;
        
    /* if first node in the list and list has more than one */
    }else if(NodeToRemove->Prev == NULL){
        
        NextNode = NodeToRemove->Next;
        
        NextNode -> Prev = NULL;
        
        CF_AppData.UpQ[Queue].HeadPtr = NextNode;
        
    /* if last node in the list and list has more than one */
    }else if(NodeToRemove->Next == NULL){
    
        PrevNode = NodeToRemove->Prev;
        
        PrevNode -> Next = NULL;
        
        CF_AppData.UpQ[Queue].TailPtr = PrevNode;
        
    /* NodeToRemove has node(s) before and node(s) after */
    }else{
    
        PrevNode = NodeToRemove->Prev;
        NextNode = NodeToRemove->Next;
        
        PrevNode -> Next = NextNode;
        NextNode -> Prev = PrevNode;
        
    }/* end if */
        
    
    /* initialize the node before returning it to the heap */
    NodeToRemove -> Next = NULL;
    NodeToRemove -> Prev = NULL; 
    
    CF_AppData.UpQ[Queue].EntryCnt--;
    
#ifdef CF_DEBUG
    if(cfdbg > 5)    
        OS_printf("CF:File %s Removed from Uplink Queue %d, %d more\n",
        NodeToRemove->SrcFile,Queue,CF_AppData.UpQ[Queue].EntryCnt);
#endif    
    return CF_SUCCESS;

}/* CF_RemoveFileFromUpQueue */


/******************************************************************************
**  Function:  CF_DequeueUpNode()
**
**  Purpose:
**      This function removes the node on the front of the given uplink queue.
**
**  Arguments:
**
**  Return:
**
*/
CF_QueueEntry_t *CF_DequeueUpNode(uint32 Queue)
{
    CF_QueueEntry_t *NodeToRemove;
    
    NodeToRemove = CF_AppData.UpQ[Queue].TailPtr;
    
    if(NodeToRemove == NULL)
        return NULL;
    
    CF_RemoveFileFromUpQueue(Queue, NodeToRemove);
    
    return NodeToRemove;

}/* end CF_DequeueUpNode */


/******************************************************************************
**  Function:  CF_DequeuePbNode()
**
**  Purpose:
**      This function removes the node on the front of the given playback queue.
**
**  Arguments:
**
**  Return:
**
*/
CF_QueueEntry_t *CF_DequeuePbNode(uint32 Chan, uint32 Queue)
{
    CF_QueueEntry_t *NodeToRemove;
    
    NodeToRemove = CF_AppData.Chan[Chan].PbQ[Queue].TailPtr;
    
    if(NodeToRemove == NULL)
        return NULL;
    
    CF_RemoveFileFromPbQueue(Chan, Queue, NodeToRemove);
    
    return NodeToRemove;

}/* end CF_DequeuePbNode */


/******************************************************************************
**  Function:  CF_FindNodeAtFrontOfQueue()
**
**  Purpose:
**      This function checks the front of the playback pending queues
**
**  Note:      
**      The 'front' of the queue is equivalent to the 'tail' of the list.
**
**  Arguments:
**
**  Return:
**
*/
CF_QueueEntry_t *CF_FindNodeAtFrontOfQueue(TRANS_STATUS TransInfo)
{

    uint32      i;
    CF_QueueEntry_t *QueueEntryPtr;

    for(i=0;i<CF_MAX_PLAYBACK_CHANNELS;i++)
    {

        if(CF_AppData.Tbl->OuCh[i].EntryInUse == CF_ENTRY_IN_USE) 
        {

            if(CF_AppData.Chan[i].PbQ[CF_PB_PENDINGQ].TailPtr != NULL)
            {                                              
                QueueEntryPtr = CF_AppData.Chan[i].PbQ[CF_PB_PENDINGQ].TailPtr;                

                if((strncmp(&TransInfo.md.source_file_name[0],QueueEntryPtr->SrcFile,OS_MAX_PATH_LEN)==0)&&
                  (QueueEntryPtr->Status==CF_STAT_PUT_REQ_ISSUED))
                {
#ifdef CF_DEBUG                   
                   if(cfdbg > 5)
                    OS_printf("File %s found at front of queue %d\n",
                        QueueEntryPtr->SrcFile,i); 
#endif    
                    return QueueEntryPtr;
            
                }/* end if */
                
            }/* end if */
                                                                           
        }/* end if */
        
    }/* end for */

    return NULL;
    
}/* end CF_FindNodeAtFrontOfQueue */

/******************************************************************************
**  Function:  CF_GetChanNumFromTransId()
**
**  Purpose:
**      This function takes a transaction number and returns the channel number
**
**  Arguments:
**      Queue - Which of the three qs to check (0-Pending,1-Active,2-History)
**      Tansaction number - the 'number' portion of the engine transaction id.
**      The source id portion of the engine transaction number is always
**      the same for playback files and therefore not needed.
**
**  Return:
**      Chan - Playback Output Channel
**
*/
int32 CF_GetChanNumFromTransId(uint32 Queue, uint32 Trans)
{
    int32 i;
    CF_QueueEntry_t *QNodePtr;
    
    /* traverse the given queue looking for trans num match */
    for(i=0;i<CF_MAX_PLAYBACK_CHANNELS;i++)
    {
        
        if(CF_AppData.Tbl->OuCh[i].EntryInUse == CF_ENTRY_IN_USE)
        {                        
            QNodePtr = CF_AppData.Chan[i].PbQ[Queue].HeadPtr;            
            while(QNodePtr != NULL)
            {                
                if(QNodePtr->TransNum == Trans)
                    return i;
                
                QNodePtr = QNodePtr->Next;
            }
        }/* end if */
        
    }/* end for */
    
    return CF_ERROR; 
    /*return 0; used to get class 2 uplink working in beta version */
    
}/* end CF_GetChanNumFromTransId */




void CF_CheckPollDirs(uint32 Chan)
{

    uint32              i;
    int32               Status;
    CF_QueueDirFiles_t  LocalStruct;
    CF_QueueDirFiles_t  *LocalPtr = &LocalStruct;

    for(i=0;i<CF_MAX_POLLING_DIRS_PER_CHAN;i++)
    {
        
        if((CF_AppData.Tbl->OuCh[Chan].PollDir[i].EntryInUse == CF_ENTRY_IN_USE) &&
            (CF_AppData.Tbl->OuCh[Chan].PollDir[i].EnableState == CF_ENABLED))
        {        
#ifdef CF_DEBUG            
            if(cfdbg > 3)
                OS_printf("CF:Checking Poll Directory %d for Channel %d\n",i,Chan);
#endif                                             

            LocalStruct.Chan = Chan;
            LocalStruct.Class = CF_AppData.Tbl->OuCh[Chan].PollDir[i].Class;
            LocalStruct.Priority = CF_AppData.Tbl->OuCh[Chan].PollDir[i].Priority;
            LocalStruct.Preserve = CF_AppData.Tbl->OuCh[Chan].PollDir[i].Preserve;
            LocalStruct.CmdOrPoll = CF_POLLDIRECTORY; 
            strncpy(&LocalStruct.PeerEntityId[0],
                CF_AppData.Tbl->OuCh[Chan].PollDir[i].PeerEntityId,
                CF_MAX_CFG_VALUE_CHARS);            
            strncpy(&LocalStruct.SrcPath[0],
                    &CF_AppData.Tbl->OuCh[Chan].PollDir[i].SrcPath[0],
                    OS_MAX_PATH_LEN);
            strncpy(&LocalStruct.DstPath[0],
                    &CF_AppData.Tbl->OuCh[Chan].PollDir[i].DstPath[0],
                    OS_MAX_PATH_LEN);                        
            
            Status = CF_QueueDirectoryFiles(LocalPtr);

        }/* end if Inuse and Enabled */
        
    }/* end for */

}/* end CF_CheckPollDirs */


void CF_StartNextFile(uint32 Chan)
{

    uint32          NoMoreFiles = CF_FALSE;
    uint32          FileStarted = CF_FALSE;
    CF_QueueEntry_t *NextFileOnQ;
    
    NextFileOnQ = CF_AppData.Chan[Chan].PbQ[CF_PB_PENDINGQ].TailPtr;
    if(NextFileOnQ == NULL) NoMoreFiles = CF_TRUE;
    
    
    /* loop until no more files on pending queue or file started successfully */
    while((NoMoreFiles == CF_FALSE) && (FileStarted == CF_FALSE))
    {
    
        /* if file is currently active on uplink or an output channel */
        if(CF_CheckIfFileIsActive(NextFileOnQ->SrcFile)==CF_FILE_IS_ACTIVE)
        {
            /* move node from pend queue to history queue and set error status */
            CF_ProcessFileStartError(NextFileOnQ,CF_STAT_ALRDY_ACTIVE);
                        
            /* that file failed,  get next file on pending queue */
            NextFileOnQ = CF_AppData.Chan[Chan].PbQ[CF_PB_PENDINGQ].TailPtr;
            if(NextFileOnQ == NULL) NoMoreFiles = CF_TRUE;

        }
        else if(CF_BuildPutRequest(NextFileOnQ)==CF_ERROR)
        {                
            /* move node from pend queue to history queue and set error status */
            CF_ProcessFileStartError(NextFileOnQ,CF_STAT_PUT_REQ_FAIL);

            /* that file failed,  get next file on pending queue */
            NextFileOnQ = CF_AppData.Chan[Chan].PbQ[CF_PB_PENDINGQ].TailPtr;
            if(NextFileOnQ == NULL) NoMoreFiles = CF_TRUE;
            
        }else{
        
            FileStarted = CF_TRUE;            
        
        }/* end if */
               
    }/* end while */    
    
}/* end CF_StartNextFile */


void CF_ProcessFileStartError(CF_QueueEntry_t *QueueEntryPtr,uint32 ErrType)
{

#ifdef CF_DEBUG
    if(cfdbg > 3)
    OS_printf("CF:Moving File from Chan %d Pending Queue to Hist Q\n",QueueEntryPtr->ChanNum);
#endif    
    CF_RemoveFileFromPbQueue(QueueEntryPtr->ChanNum, CF_PB_PENDINGQ, QueueEntryPtr);
    QueueEntryPtr->Status = ErrType;
    CF_AddFileToPbQueue(QueueEntryPtr->ChanNum, CF_PB_HISTORYQ, QueueEntryPtr);
    
    CF_AppData.Hk.Chan[QueueEntryPtr->ChanNum].FailedCounter++;
    
    /* add to last failed transaction string in HK */
    
}



uint32 CF_FileIsOnQueue(uint32 Chan, uint32 Queue, char *Filename)
{

    CF_QueueEntry_t     *PtrToEntry;
                 
    PtrToEntry = CF_AppData.Chan[Chan].PbQ[Queue].HeadPtr;            
    while(PtrToEntry != NULL)
    {                
        if(strncmp(PtrToEntry->SrcFile,Filename,OS_MAX_PATH_LEN) == 0)
        {    
            return  CF_TRUE;
        }/* end if */
        
        PtrToEntry = PtrToEntry->Next;
    }
    
    return CF_FALSE;

}




/************************/
/*  End of File Comment */
/************************/
