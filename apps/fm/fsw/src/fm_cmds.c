/*
** $Id: fm_cmds.c 1.49 2015/02/28 17:50:44EST sstrege Exp  $
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
** Title: File Manager (FM) Application Ground Commands
**
** Purpose: Provides functions for the execution of the FM ground commands
**
** Author: Susanne L. Strege, Code 582 NASA GSFC
**
** Notes:
**
** $Log: fm_cmds.c  $
** Revision 1.49 2015/02/28 17:50:44EST sstrege 
** Added copyright information
** Revision 1.48 2014/12/12 15:53:08EST lwalling 
** Cazll CFE_TBL_Modified() after freespace table update
** Revision 1.47 2014/12/04 17:52:10EST lwalling 
** Removed unused CommandWarnCounter
** Revision 1.46 2011/08/29 15:08:21EDT lwalling 
** Removed unused arg to function FM_DeleteFileCmd(), set actual CC as cmd arg to child task
** Revision 1.45 2011/07/04 17:45:15EDT lwalling 
** Modified to use child task to process move, rename, delete, create dir and delete dir commands.
** Revision 1.44 2011/05/31 17:11:44EDT lwalling 
** Added ground cmd vs other app arg to delete file command handler prototype
** Revision 1.43 2011/05/19 14:52:06EDT lwalling 
** Change pointer use that violates strict-aliasing rules to call CFE_PSP_MemCpy
** Revision 1.42 2011/04/19 16:44:07EDT lwalling 
** Modified copy and move file cmd handlers to test target filename per overwrite cmd argument
** Revision 1.41 2010/03/08 15:49:54EST lwalling 
** Remove uint64 data type from free space packet
** Revision 1.40 2010/03/03 18:13:38EST lwalling 
** Added CMD to Set Table State event ID macro name
** Revision 1.39 2010/02/25 13:31:34EST lwalling 
** Enable calls to OS_fsBytesFree
** Revision 1.38 2009/11/20 15:32:18EST lwalling 
** Remove return code and error events from FM_AppendPathSep
** Revision 1.37 2009/11/17 13:40:49EST lwalling 
** Remove global open files list data structure
** Revision 1.36 2009/11/13 16:22:43EST lwalling 
** Modify macro names, add CRC arg to GetFileInfo cmd, add SetTableEntryState cmd
** Revision 1.35 2009/11/09 17:01:12EST lwalling 
** Fix return code for ResetCounters cmd, change dest to target, cleanup event text
** Revision 1.34 2009/10/30 15:59:40EDT lwalling 
** Add include fm_msgdefs.h, modify free space table state definitions
** Revision 1.33 2009/10/30 14:02:26EDT lwalling 
** Remove trailing white space from all lines
** Revision 1.32 2009/10/30 10:47:37EDT lwalling
** Remove detail from function prologs, use command specific packet structures
** Revision 1.31 2009/10/29 11:42:24EDT lwalling
** Make common structure for open files list and open file telemetry packet, change open file to open files
** Revision 1.30 2009/10/27 17:28:36EDT lwalling
** Add a command warning counter for child task command handlers
** Revision 1.29 2009/10/26 16:42:41EDT lwalling
** Move GetFileInfo cmd to child task, change struct/var names, add child vars to reset cmd
** Revision 1.28 2009/10/26 11:31:00EDT lwalling
** Remove Close File command from FM application
** Revision 1.27 2009/10/23 14:36:29EDT lwalling
** Create FM child task to process slow commands, event text updates
** Revision 1.26 2009/10/16 15:49:50EDT lwalling
** Update event text, event ID names, function names, add global directory list structures
** Revision 1.25 2009/10/09 17:23:53EDT lwalling
** Create command to generate file system free space packet, replace device table with free space table
** Revision 1.24 2009/10/06 11:06:11EDT lwalling
** Clean up after create common filename verify functions
** Revision 1.23 2009/09/29 13:41:24EDT lwalling
** Perform tests for file open against current system list of open files, allow open files for copy/move/rename commands
** Revision 1.22 2009/09/28 15:29:55EDT lwalling
** Review and modify event text
** Revision 1.21 2009/09/28 14:15:27EDT lwalling
** Create common filename verification functions
** Revision 1.20 2009/09/14 16:10:32EDT lwalling
** Removed insert string term and modified calls to FM_IsValidPathname() in all cmd handlers
** Revision 1.19 2009/09/10 13:03:05EDT lwalling
** Modified FM_CloseFileCmd() to change from OS_NameChange() to OS_TranslatePath()
** Revision 1.18 2009/06/12 14:16:27EDT rmcgraw
** DCR82191:1 Changed OS_Mem function calls to CFE_PSP_Mem
** Revision 1.17 2008/12/30 14:56:18EST sstrege
** Updated event message reporting
** Revision 1.16 2008/12/24 16:24:42EST sstrege
** Updated DeleteFile and DeleteAllFiles command functions
** Revision 1.15 2008/12/22 16:28:54EST sstrege
** Added code for supporting default directory listing file
** Revision 1.14 2008/12/22 15:45:47EST sstrege
** Updated IsValidDeleteFile utility function to accept Event Type as an input parameter
** Revision 1.13 2008/12/08 18:41:50EST sstrege
** Concat command updated to copy source file 1 to destination, before destination is opened
** Revision 1.12 2008/11/30 17:25:30EST sstrege
** Concat command updated to open src file 2 before destination file
** Revision 1.11 2008/11/30 16:51:09EST sstrege
** Moved OS_cp call in concat command
** Revision 1.10 2008/11/30 15:22:03EST sstrege
** Removed double existence check on concat command destination file
** Revision 1.9 2008/11/05 18:17:07EST sstrege
** Updated local variable OpenFileMsg to be static
** Prepended "FM_" to all static local variables and added extra comments
** Revision 1.8 2008/10/06 11:29:05EDT sstrege
** Added call to OS_lseek before writing directory listing statistics to the directory listing file
** Revision 1.7 2008/10/03 16:22:00EDT sstrege
** Removed OS_NameChange calls in DecompressFileCmd and replaced OS_BSBDecompress call with CFE_FS_Decompress
** Revision 1.6 2008/10/03 15:53:49EDT sstrege
** Added include to new fm_version.h header files
** Added version information to noop command event message
** Revision 1.5 2008/10/01 16:20:04EDT sstrege
** Updated FM_DirListMsgCmd to use new FM_SourceUint32DataCmd_t and removed local DirListOffset variable
** Revision 1.4 2008/09/30 17:35:12EDT sstrege
** Removed Delete All Files command invalidation for OS error and path too long conditions
** Revision 1.3 2008/09/30 16:40:05EDT sstrege
** Added include statement for fm_perfids.h
** Revision 1.2 2008/06/20 16:21:28EDT slstrege
** Member moved from fsw/src/fm_cmds.c in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/cfs_fm.pj to fm_cmds.c in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/fsw/src/project.pj.
** Revision 1.1 2008/06/20 15:21:28ACT slstrege
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/cfs_fm.pj
*/

#include "cfe.h"
#include "fm_msg.h"
#include "fm_msgdefs.h"
#include "fm_msgids.h"
#include "fm_events.h"
#include "fm_app.h"
#include "fm_cmds.h"
#include "fm_cmd_utils.h"
#include "fm_perfids.h"
#include "fm_platform_cfg.h"
#include "fm_version.h"
#include "fm_verify.h"

#include <string.h>


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM command handler -- NOOP                                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean FM_NoopCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    char *CmdText = "No-op";
    boolean CommandResult;

    /* Verify message length */
    CommandResult = FM_IsValidCmdPktLength(MessagePtr, sizeof(FM_NoopCmd_t),
                                           FM_NOOP_PKT_ERR_EID, CmdText);

    /* Send command completion event (info) */
    if (CommandResult == TRUE)
    {
        CFE_EVS_SendEvent(FM_NOOP_CMD_EID, CFE_EVS_INFORMATION,
                         "%s command: FM version %d.%d.%d.%d", CmdText,
                          FM_MAJOR_VERSION, FM_MINOR_VERSION, FM_REVISION, FM_MISSION_REV);
    }

    return(CommandResult);

} /* End of FM_NoopCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM command handler -- Reset Counters                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean FM_ResetCountersCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    char *CmdText = "Reset Counters";
    boolean CommandResult;

    /* Verify message length */
    CommandResult = FM_IsValidCmdPktLength(MessagePtr, sizeof(FM_ResetCmd_t),
                                           FM_RESET_PKT_ERR_EID, CmdText);

    /* Reset command counters */
    if (CommandResult == TRUE)
    {
        FM_GlobalData.CommandCounter = 0;
        FM_GlobalData.CommandErrCounter = 0;

        FM_GlobalData.ChildCmdCounter = 0;
        FM_GlobalData.ChildCmdErrCounter = 0;
        FM_GlobalData.ChildCmdWarnCounter = 0;

        /* Send command completion event (debug) */
        CFE_EVS_SendEvent(FM_RESET_CMD_EID, CFE_EVS_DEBUG, "%s command", CmdText);
    }

    return(CommandResult);

} /* End of FM_ResetCountersCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM command handler -- Copy File                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean FM_CopyFileCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    FM_CopyFileCmd_t *CmdPtr = (FM_CopyFileCmd_t *) MessagePtr;
    FM_ChildQueueEntry_t *CmdArgs;
    char *CmdText = "Copy File";
    boolean CommandResult;

    /* Verify command packet length */
    CommandResult = FM_IsValidCmdPktLength(MessagePtr, sizeof(FM_CopyFileCmd_t),
                                           FM_COPY_PKT_ERR_EID, CmdText);

    /* Verify that overwrite argument is valid */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyOverwrite(CmdPtr->Overwrite,
                                           FM_COPY_OVR_ERR_EID, CmdText);
    }

    /* Verify that source file exists and is not a directory */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyFileExists(CmdPtr->Source, sizeof(CmdPtr->Source),
                                            FM_COPY_SRC_ERR_EID, CmdText);
    }

    /* Verify target filename per the overwrite argument */
    if (CommandResult == TRUE)
    {
        if (CmdPtr->Overwrite == 0)
        {
            CommandResult = FM_VerifyFileNoExist(CmdPtr->Target, sizeof(CmdPtr->Target),
                                                 FM_COPY_TGT_ERR_EID, CmdText);
        }
        else
        {
            CommandResult = FM_VerifyFileNotOpen(CmdPtr->Target, sizeof(CmdPtr->Target),
                                                 FM_COPY_TGT_ERR_EID, CmdText);
        }
    }

    /* Check for lower priority child task availability */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyChildTask(FM_COPY_CHILD_ERR_EID, CmdText);
    }

    /* Prepare command for child task execution */
    if (CommandResult == TRUE)
    {
        CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildWriteIndex];

        /* Set handshake queue command args */
        CmdArgs->CommandCode = FM_COPY_CC;
        strcpy(CmdArgs->Source1, CmdPtr->Source);
        strcpy(CmdArgs->Target,  CmdPtr->Target);

        /* Invoke lower priority child task */
        FM_InvokeChildTask();
    }

    return(CommandResult);

} /* End of FM_CopyFileCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM command handler -- Move File                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean FM_MoveFileCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    FM_MoveFileCmd_t  *CmdPtr = (FM_MoveFileCmd_t *) MessagePtr;
    FM_ChildQueueEntry_t *CmdArgs;
    char *CmdText = "Move File";
    boolean CommandResult;

    /* Verify command packet length */
    CommandResult = FM_IsValidCmdPktLength(MessagePtr, sizeof(FM_MoveFileCmd_t),
                                           FM_MOVE_PKT_ERR_EID, CmdText);

    /* Verify that overwrite argument is valid */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyOverwrite(CmdPtr->Overwrite,
                                           FM_MOVE_OVR_ERR_EID, CmdText);
    }

    /* Verify that source file exists and not a directory */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyFileExists(CmdPtr->Source, sizeof(CmdPtr->Source),
                                            FM_MOVE_SRC_ERR_EID, CmdText);
    }

    /* Verify target filename per the overwrite argument */
    if (CommandResult == TRUE)
    {
        if (CmdPtr->Overwrite == 0)
        {
            CommandResult = FM_VerifyFileNoExist(CmdPtr->Target, sizeof(CmdPtr->Target),
                                                 FM_MOVE_TGT_ERR_EID, CmdText);
        }
        else
        {
            CommandResult = FM_VerifyFileNotOpen(CmdPtr->Target, sizeof(CmdPtr->Target),
                                                 FM_MOVE_TGT_ERR_EID, CmdText);
        }
    }

    /* Check for lower priority child task availability */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyChildTask(FM_MOVE_CHILD_ERR_EID, CmdText);
    }

    /* Prepare command for child task execution */
    if (CommandResult == TRUE)
    {
        CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildWriteIndex];

        /* Set handshake queue command args */
        CmdArgs->CommandCode = FM_MOVE_CC;
        strcpy(CmdArgs->Source1, CmdPtr->Source);
        strcpy(CmdArgs->Target,  CmdPtr->Target);

        /* Invoke lower priority child task */
        FM_InvokeChildTask();
    }

    return(CommandResult);

} /* End of FM_MoveFileCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM command handler -- Rename File                               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean FM_RenameFileCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    FM_RenameFileCmd_t  *CmdPtr = (FM_RenameFileCmd_t *) MessagePtr;
    FM_ChildQueueEntry_t *CmdArgs;
    char *CmdText = "Rename File";
    boolean CommandResult;

    /* Verify command packet length */
    CommandResult = FM_IsValidCmdPktLength(MessagePtr, sizeof(FM_RenameFileCmd_t),
                                           FM_RENAME_PKT_ERR_EID, CmdText);

    /* Verify that source file exists and is not a directory */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyFileExists(CmdPtr->Source, sizeof(CmdPtr->Source),
                                            FM_RENAME_SRC_ERR_EID, CmdText);
    }

    /* Verify that target file does not exist */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyFileNoExist(CmdPtr->Target, sizeof(CmdPtr->Target),
                                             FM_RENAME_TGT_ERR_EID, CmdText);
    }

    /* Check for lower priority child task availability */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyChildTask(FM_RENAME_CHILD_ERR_EID, CmdText);
    }

    /* Prepare command for child task execution */
    if (CommandResult == TRUE)
    {
        CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildWriteIndex];

        /* Set handshake queue command args */
        CmdArgs->CommandCode = FM_RENAME_CC;
        strcpy(CmdArgs->Source1, CmdPtr->Source);
        strcpy(CmdArgs->Target,  CmdPtr->Target);

        /* Invoke lower priority child task */
        FM_InvokeChildTask();
    }

    return(CommandResult);

} /* End of FM_RenameFileCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM command handler -- Delete File                               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean FM_DeleteFileCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    FM_DeleteFileCmd_t *CmdPtr = (FM_DeleteFileCmd_t *) MessagePtr;
    FM_ChildQueueEntry_t *CmdArgs;
    char *CmdText = "Delete File";
    boolean CommandResult;

    /* Verify command packet length */
    CommandResult = FM_IsValidCmdPktLength(MessagePtr, sizeof(FM_DeleteFileCmd_t),
                                           FM_DELETE_PKT_ERR_EID, CmdText);

    /* Verify that file exists, is not a directory and is not open */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyFileClosed(CmdPtr->Filename, sizeof(CmdPtr->Filename),
                                            FM_DELETE_SRC_ERR_EID, CmdText);
    }

    /* Check for lower priority child task availability */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyChildTask(FM_DELETE_CHILD_ERR_EID, CmdText);
    }

    /* Prepare command for child task execution */
    if (CommandResult == TRUE)
    {
        CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildWriteIndex];

        /* Set handshake queue command args - might be global or internal CC */
        CmdArgs->CommandCode = CFE_SB_GetCmdCode(MessagePtr);
        strcpy(CmdArgs->Source1, CmdPtr->Filename);

        /* Invoke lower priority child task */
        FM_InvokeChildTask();
    }

    return(CommandResult);

} /* End of FM_DeleteFileCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM command handler -- Delete All Files                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean FM_DeleteAllFilesCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    FM_DeleteAllCmd_t *CmdPtr = (FM_DeleteAllCmd_t *) MessagePtr;
    char *CmdText = "Delete All Files";
    char DirWithSep[OS_MAX_PATH_LEN];
    FM_ChildQueueEntry_t *CmdArgs;
    boolean CommandResult;

    /* Verify message length */
    CommandResult = FM_IsValidCmdPktLength(MessagePtr, sizeof(FM_DeleteAllCmd_t),
                                           FM_DELETE_ALL_PKT_ERR_EID, CmdText);

    /* Verify that the directory exists */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyDirExists(CmdPtr->Directory, sizeof(CmdPtr->Directory),
                                           FM_DELETE_ALL_SRC_ERR_EID, CmdText);
    }

    if (CommandResult == TRUE)
    {
        /* Append a path separator to the end of the directory name */
        strcpy(DirWithSep, CmdPtr->Directory);
        FM_AppendPathSep(DirWithSep, OS_MAX_PATH_LEN);

        /* Check for lower priority child task availability */
        CommandResult = FM_VerifyChildTask(FM_DELETE_ALL_CHILD_ERR_EID, CmdText);
    }

    /* Prepare command for child task execution */
    if (CommandResult == TRUE)
    {
        CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildWriteIndex];

        /* Set handshake queue command args */
        CmdArgs->CommandCode = FM_DELETE_ALL_CC;
        strcpy(CmdArgs->Source1, CmdPtr->Directory);
        strcpy(CmdArgs->Source2, DirWithSep);

        /* Invoke lower priority child task */
        FM_InvokeChildTask();
    }

    return(CommandResult);

} /* End of FM_DeleteAllFilesCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM command handler -- Decompress File                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean FM_DecompressFileCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    FM_DecompressCmd_t  *CmdPtr = (FM_DecompressCmd_t *) MessagePtr;
    char *CmdText = "Decompress File";
    FM_ChildQueueEntry_t *CmdArgs;
    boolean CommandResult;

    /* Verify command packet length */
    CommandResult = FM_IsValidCmdPktLength(MessagePtr, sizeof(FM_DecompressCmd_t),
                                           FM_DECOM_PKT_ERR_EID, CmdText);

    /* Verify that source file exists, is not a directory and is not open */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyFileClosed(CmdPtr->Source, sizeof(CmdPtr->Source),
                                            FM_DECOM_SRC_ERR_EID, CmdText);
    }

    /* Verify that target file does not exist */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyFileNoExist(CmdPtr->Target, sizeof(CmdPtr->Target),
                                             FM_DECOM_TGT_ERR_EID, CmdText);
    }

    /* Check for lower priority child task availability */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyChildTask(FM_DECOM_CHILD_ERR_EID, CmdText);
    }

    /* Prepare command for child task execution */
    if (CommandResult == TRUE)
    {
        CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildWriteIndex];

        /* Set handshake queue command args */
        CmdArgs->CommandCode = FM_DECOMPRESS_CC;
        strcpy(CmdArgs->Source1, CmdPtr->Source);
        strcpy(CmdArgs->Target, CmdPtr->Target);

        /* Invoke lower priority child task */
        FM_InvokeChildTask();
    }

    return(CommandResult);

} /* End of FM_DecompressFileCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM command handler -- Concatenate Files                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean FM_ConcatFilesCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    FM_ConcatCmd_t *CmdPtr = (FM_ConcatCmd_t *) MessagePtr;
    char *CmdText = "Concat Files";
    FM_ChildQueueEntry_t *CmdArgs;
    boolean CommandResult;

    /* Verify command packet length */
    CommandResult = FM_IsValidCmdPktLength(MessagePtr, sizeof(FM_ConcatCmd_t),
                                           FM_CONCAT_PKT_ERR_EID, CmdText);

    /* Verify that source file #1 exists, is not a directory and is not open */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyFileClosed(CmdPtr->Source1, sizeof(CmdPtr->Source1),
                                            FM_CONCAT_SRC1_ERR_EID, CmdText);
    }

    /* Verify that source file #2 exists, is not a directory and is not open */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyFileClosed(CmdPtr->Source2, sizeof(CmdPtr->Source2),
                                            FM_CONCAT_SRC2_ERR_EID, CmdText);
    }

    /* Verify that target file does not exist */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyFileNoExist(CmdPtr->Target, sizeof(CmdPtr->Target),
                                             FM_CONCAT_TGT_ERR_EID, CmdText);
    }

    /* Check for lower priority child task availability */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyChildTask(FM_CONCAT_CHILD_ERR_EID, CmdText);
    }

    /* Prepare command for child task execution */
    if (CommandResult == TRUE)
    {
        CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildWriteIndex];

        /* Set handshake queue command args */
        CmdArgs->CommandCode = FM_CONCAT_CC;
        strcpy(CmdArgs->Source1, CmdPtr->Source1);
        strcpy(CmdArgs->Source2, CmdPtr->Source2);
        strcpy(CmdArgs->Target, CmdPtr->Target);

        /* Invoke lower priority child task */
        FM_InvokeChildTask();
    }

    return(CommandResult);

} /* End of FM_ConcatFilesCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM command handler -- Get File Info                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean FM_GetFileInfoCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    FM_GetFileInfoCmd_t *CmdPtr = (FM_GetFileInfoCmd_t *) MessagePtr;
    char *CmdText = "Get File Info";
    FM_ChildQueueEntry_t *CmdArgs;
    boolean CommandResult;
    uint32 FilenameState;

    /* Verify command packet length */
    CommandResult = FM_IsValidCmdPktLength(MessagePtr, sizeof(FM_GetFileInfoCmd_t),
                                           FM_GET_FILE_INFO_PKT_ERR_EID, CmdText);

    /* Verify that the source name is valid for a file or directory */
    if (CommandResult == TRUE)
    {
        FilenameState = FM_VerifyNameValid(CmdPtr->Filename, sizeof(CmdPtr->Filename),
                                           FM_GET_FILE_INFO_SRC_ERR_EID, CmdText);

        if (FilenameState == FM_NAME_IS_INVALID)
        {
            CommandResult = FALSE;
        }
    }

    /* Check for lower priority child task availability */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyChildTask(FM_GET_FILE_INFO_CHILD_ERR_EID, CmdText);
    }

    /* Prepare command for child task execution */
    if (CommandResult == TRUE)
    {
        CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildWriteIndex];

        /* Set handshake queue command args */
        CmdArgs->CommandCode = FM_GET_FILE_INFO_CC;
        strcpy(CmdArgs->Source1, CmdPtr->Filename);
        CmdArgs->FileInfoState = FilenameState;
        CmdArgs->FileInfoCRC = CmdPtr->FileInfoCRC;

        /* Global data set during call to FM_VerifyNameValid */
        CmdArgs->FileInfoSize = FM_GlobalData.FileStatSize;
        CmdArgs->FileInfoTime = CFE_TIME_FS2CFESeconds(FM_GlobalData.FileStatTime);

        /* Invoke lower priority child task */
        FM_InvokeChildTask();
    }

    return(CommandResult);

} /* End of FM_GetFileInfoCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM command handler -- Get List of Open Files                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean FM_GetOpenFilesCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    char *CmdText = "Get Open Files";
    boolean CommandResult;
    uint32 NumOpenFiles;

    /* Verify command packet length */
    CommandResult = FM_IsValidCmdPktLength(MessagePtr, sizeof(FM_GetOpenFilesCmd_t),
                                           FM_GET_OPEN_FILES_PKT_ERR_EID, CmdText);
    if (CommandResult == TRUE)
    {
        /* Initialize open files telemetry packet */
        CFE_SB_InitMsg(&FM_GlobalData.OpenFilesPkt, FM_OPEN_FILES_TLM_MID,
                        sizeof(FM_OpenFilesPkt_t), TRUE);

        /* Get list of open files and count */
        NumOpenFiles = FM_GetOpenFilesData(FM_GlobalData.OpenFilesPkt.OpenFilesList);
        FM_GlobalData.OpenFilesPkt.NumOpenFiles = NumOpenFiles;

        /* Timestamp and send open files telemetry packet */
        CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &FM_GlobalData.OpenFilesPkt);
        CFE_SB_SendMsg((CFE_SB_Msg_t *) &FM_GlobalData.OpenFilesPkt);

        /* Send command completion event (debug) */
        CFE_EVS_SendEvent(FM_GET_OPEN_FILES_CMD_EID, CFE_EVS_DEBUG,
                         "%s command", CmdText);
    }

    return(CommandResult);

} /* End of FM_GetOpenFilesCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM command handler -- Create Directory                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean FM_CreateDirectoryCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    FM_CreateDirCmd_t *CmdPtr = (FM_CreateDirCmd_t *) MessagePtr;
    FM_ChildQueueEntry_t *CmdArgs;
    char *CmdText = "Create Directory";
    boolean CommandResult;

    /* Verify command packet length */
    CommandResult = FM_IsValidCmdPktLength(MessagePtr, sizeof(FM_CreateDirCmd_t),
                                           FM_CREATE_DIR_PKT_ERR_EID, CmdText);

    /* Verify that the directory name is not already in use */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyDirNoExist(CmdPtr->Directory, sizeof(CmdPtr->Directory),
                                            FM_CREATE_DIR_SRC_ERR_EID, CmdText);
    }

    /* Check for lower priority child task availability */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyChildTask(FM_CREATE_DIR_CHILD_ERR_EID, CmdText);
    }

    /* Prepare command for child task execution */
    if (CommandResult == TRUE)
    {
        CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildWriteIndex];

        /* Set handshake queue command args */
        CmdArgs->CommandCode = FM_CREATE_DIR_CC;
        strcpy(CmdArgs->Source1, CmdPtr->Directory);

        /* Invoke lower priority child task */
        FM_InvokeChildTask();
    }

    return(CommandResult);

} /* End of FM_CreateDirectoryCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM command handler -- Delete Directory                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean FM_DeleteDirectoryCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    FM_DeleteDirCmd_t *CmdPtr = (FM_DeleteDirCmd_t *) MessagePtr;
    FM_ChildQueueEntry_t *CmdArgs;
    char *CmdText = "Delete Directory";
    boolean CommandResult;

    /* Verify command packet length */
    CommandResult = FM_IsValidCmdPktLength(MessagePtr, sizeof(FM_DeleteDirCmd_t),
                                           FM_DELETE_DIR_PKT_ERR_EID, CmdText);

    /* Verify that the directory exists */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyDirExists(CmdPtr->Directory, sizeof(CmdPtr->Directory),
                                           FM_DELETE_DIR_SRC_ERR_EID, CmdText);
    }

    /* Check for lower priority child task availability */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyChildTask(FM_DELETE_DIR_CHILD_ERR_EID, CmdText);
    }

    /* Prepare command for child task execution */
    if (CommandResult == TRUE)
    {
        CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildWriteIndex];

        /* Set handshake queue command args */
        CmdArgs->CommandCode = FM_DELETE_DIR_CC;
        strcpy(CmdArgs->Source1, CmdPtr->Directory);

        /* Invoke lower priority child task */
        FM_InvokeChildTask();
    }

    return(CommandResult);

} /* End of FM_DeleteDirectoryCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM command handler -- Get List of Directory Entries (to file)   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean FM_GetDirListFileCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    FM_GetDirFileCmd_t *CmdPtr = (FM_GetDirFileCmd_t *) MessagePtr;
    char *CmdText = "Directory List to File";
    char DirWithSep[OS_MAX_PATH_LEN];
    char Filename[OS_MAX_PATH_LEN];
    FM_ChildQueueEntry_t *CmdArgs;
    boolean CommandResult;

    /* Verify command packet length */
    CommandResult = FM_IsValidCmdPktLength(MessagePtr, sizeof(FM_GetDirFileCmd_t),
                                           FM_GET_DIR_FILE_PKT_ERR_EID, CmdText);

    /* Verify that source directory exists */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyDirExists(CmdPtr->Directory, sizeof(CmdPtr->Directory),
                                           FM_GET_DIR_FILE_SRC_ERR_EID, CmdText);
    }

    /* Verify that target file is not already open */
    if (CommandResult == TRUE)
    {
        /* Use default filename if not specified in the command */
        if (CmdPtr->Filename[0] == '\0')
        {
            strcpy(Filename, FM_DIR_LIST_FILE_DEFNAME);
        }
        else
        {
            CFE_PSP_MemCpy(Filename, CmdPtr->Filename, OS_MAX_PATH_LEN);
        }

        /* Note: it is OK for this file to overwrite a previous version of the file */
        CommandResult = FM_VerifyFileNotOpen(Filename, sizeof(Filename),
                                             FM_GET_DIR_FILE_TGT_ERR_EID, CmdText);
    }

    /* Check for lower priority child task availability */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyChildTask(FM_GET_DIR_FILE_CHILD_ERR_EID, CmdText);
    }

    /* Prepare command for child task execution */
    if (CommandResult == TRUE)
    {
        CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildWriteIndex];

        /* Append a path separator to the end of the directory name */
        strcpy(DirWithSep, CmdPtr->Directory);
        FM_AppendPathSep(DirWithSep, OS_MAX_PATH_LEN);

        /* Set handshake queue command args */
        CmdArgs->CommandCode = FM_GET_DIR_FILE_CC;
        strcpy(CmdArgs->Source1, CmdPtr->Directory);
        strcpy(CmdArgs->Source2, DirWithSep);
        strcpy(CmdArgs->Target, Filename);

        /* Invoke lower priority child task */
        FM_InvokeChildTask();
    }

    return(CommandResult);

} /* End of FM_GetDirListFileCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM command handler -- Get List of Directory Entries (to pkt)    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean FM_GetDirListPktCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    FM_GetDirPktCmd_t *CmdPtr = (FM_GetDirPktCmd_t *) MessagePtr;
    char *CmdText = "Directory List to Packet";
    char DirWithSep[OS_MAX_PATH_LEN];
    FM_ChildQueueEntry_t *CmdArgs;
    boolean CommandResult;

    /* Verify command packet length */
    CommandResult = FM_IsValidCmdPktLength(MessagePtr, sizeof(FM_GetDirPktCmd_t),
                                           FM_GET_DIR_PKT_PKT_ERR_EID, CmdText);

    /* Verify that source directory exists */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyDirExists(CmdPtr->Directory, sizeof(CmdPtr->Directory),
                                           FM_GET_DIR_PKT_SRC_ERR_EID, CmdText);
    }

    /* Check for lower priority child task availability */
    if (CommandResult == TRUE)
    {
        CommandResult = FM_VerifyChildTask(FM_GET_DIR_PKT_CHILD_ERR_EID, CmdText);
    }

    /* Prepare command for child task execution */
    if (CommandResult == TRUE)
    {
        CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildWriteIndex];

        /* Append a path separator to the end of the directory name */
        strcpy(DirWithSep, CmdPtr->Directory);
        FM_AppendPathSep(DirWithSep, OS_MAX_PATH_LEN);

        /* Set handshake queue command args */
        CmdArgs->CommandCode = FM_GET_DIR_PKT_CC;
        strcpy(CmdArgs->Source1, CmdPtr->Directory);
        strcpy(CmdArgs->Source2, DirWithSep);
        CmdArgs->DirListOffset = CmdPtr->DirListOffset;

        /* Invoke lower priority child task */
        FM_InvokeChildTask();
    }

    return(CommandResult);

} /* End of FM_GetDirListPktCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM command handler -- Get File System Free Space                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean FM_GetFreeSpaceCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    char *CmdText = "Get Free Space";
    boolean CommandResult;
    uint32 i;
    uint64 FreeSpace64;


    /* Verify command packet length */
    CommandResult = FM_IsValidCmdPktLength(MessagePtr, sizeof(FM_GetFreeSpaceCmd_t),
                                           FM_GET_FREE_SPACE_PKT_ERR_EID, CmdText);
    if (CommandResult == TRUE)
    {
        /* Verify that we have a pointer to the file system table data */
        if (FM_GlobalData.FreeSpaceTablePtr == (FM_FreeSpaceTable_t *) NULL)
        {
            CommandResult = FALSE;

            CFE_EVS_SendEvent(FM_GET_FREE_SPACE_TBL_ERR_EID, CFE_EVS_ERROR,
                             "%s error: file system free space table is not loaded", CmdText);
        }
        else
        {
            /* Initialize the file system free space telemetry packet */
            CFE_SB_InitMsg(&FM_GlobalData.FreeSpacePkt, FM_FREE_SPACE_TLM_MID,
                           sizeof(FM_FreeSpacePkt_t), TRUE);

            /* Process enabled file system table entries */
            for (i = 0; i < FM_TABLE_ENTRY_COUNT; i++)
            {
                if (FM_GlobalData.FreeSpaceTablePtr->FileSys[i].State == FM_TABLE_ENTRY_ENABLED)
                {
                    /* Get file system name */
                    strcpy(FM_GlobalData.FreeSpacePkt.FileSys[i].Name,
                           FM_GlobalData.FreeSpaceTablePtr->FileSys[i].Name);

                    /* Get file system free space */
                    FreeSpace64 = 0;
                    OS_fsBytesFree(FM_GlobalData.FreeSpacePkt.FileSys[i].Name,  &FreeSpace64);

                    /* Store as bytes to avoid boundary, endian and strict-aliasing issues */
                    CFE_PSP_MemCpy(&FM_GlobalData.FreeSpacePkt.FileSys[i].FreeSpace_A,
                                   &FreeSpace64, sizeof(uint64));
                }
            }

            /* Timestamp and send file system free space telemetry packet */
            CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &FM_GlobalData.FreeSpacePkt);
            CFE_SB_SendMsg((CFE_SB_Msg_t *) &FM_GlobalData.FreeSpacePkt);

            /* Send command completion event (debug) */
            CFE_EVS_SendEvent(FM_GET_FREE_SPACE_CMD_EID, CFE_EVS_DEBUG,
                             "%s command", CmdText);
        }
    }

    return(CommandResult);

} /* End of FM_GetFreeSpaceCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM command handler -- Set Table Entry Enable/Disable State      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean FM_SetTableStateCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    FM_SetTableStateCmd_t *CmdPtr = (FM_SetTableStateCmd_t *) MessagePtr;
    char *CmdText = "Set Table State";
    boolean CommandResult;

    /* Verify command packet length */
    CommandResult = FM_IsValidCmdPktLength(MessagePtr, sizeof(FM_SetTableStateCmd_t),
                                           FM_SET_TABLE_STATE_PKT_ERR_EID, CmdText);
    if (CommandResult == TRUE)
    {
        if (FM_GlobalData.FreeSpaceTablePtr == (FM_FreeSpaceTable_t *) NULL)
        {
            /* File system table has not been loaded */
            CommandResult = FALSE;

            CFE_EVS_SendEvent(FM_SET_TABLE_STATE_TBL_ERR_EID, CFE_EVS_ERROR,
               "%s error: file system free space table is not loaded", CmdText);
        }
        else if (CmdPtr->TableEntryIndex >= FM_TABLE_ENTRY_COUNT)
        {
            /* Table index argument is out of range */
            CommandResult = FALSE;

            CFE_EVS_SendEvent(FM_SET_TABLE_STATE_ARG_ERR_EID, CFE_EVS_ERROR,
               "%s error: invalid command argument: index = %d", CmdText, CmdPtr->TableEntryIndex);
        }
        else if ((CmdPtr->TableEntryState != FM_TABLE_ENTRY_ENABLED) &&
            (CmdPtr->TableEntryState != FM_TABLE_ENTRY_DISABLED))
        {
            /* State argument must be either enabled or disabled */
            CommandResult = FALSE;

            CFE_EVS_SendEvent(FM_SET_TABLE_STATE_ARG_ERR_EID, CFE_EVS_ERROR,
               "%s error: invalid command argument: state = %d", CmdText, CmdPtr->TableEntryState);
        }
        else if (FM_GlobalData.FreeSpaceTablePtr->FileSys[CmdPtr->TableEntryIndex].State == FM_TABLE_ENTRY_UNUSED)
        {
            /* Current table entry state must not be unused */
            CommandResult = FALSE;

            CFE_EVS_SendEvent(FM_SET_TABLE_STATE_UNUSED_ERR_EID, CFE_EVS_ERROR,
               "%s error: cannot modify unused table entry: index = %d", CmdText, CmdPtr->TableEntryIndex);
        }
        else
        {
            /* Update the table entry state as commanded */
            FM_GlobalData.FreeSpaceTablePtr->FileSys[CmdPtr->TableEntryIndex].State = CmdPtr->TableEntryState;

            /* Notify cFE that we have modified the table data */
            CFE_TBL_Modified(FM_GlobalData.FreeSpaceTableHandle);

            /* Send command completion event (info) */
            CFE_EVS_SendEvent(FM_SET_TABLE_STATE_CMD_EID, CFE_EVS_INFORMATION,
               "%s command: index = %d, state = %d", CmdText, CmdPtr->TableEntryIndex, CmdPtr->TableEntryState);
        }
    }

    return(CommandResult);

} /* End of FM_SetTableStateCmd() */


/************************/
/*  End of File Comment */
/************************/

