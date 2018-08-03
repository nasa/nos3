/*
** $Id: fm_cmd_utils.c 1.36 2015/02/28 17:50:55EST sstrege Exp  $
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
** Title: File Manager (FM) Command Utility Functions
**
** Purpose: Provides file manager utility function definitions for
**          processing file manager commands
**
** Author: Susanne L. Strege, Code 582 NASA GSFC
**
** Notes:
**
** $Log: fm_cmd_utils.c  $
** Revision 1.36 2015/02/28 17:50:55EST sstrege 
** Added copyright information
** Revision 1.35 2014/12/18 14:32:54EST lwalling 
** Added mutex semaphore protection when accessing FM_GlobalData.ChildQueueCount
** Revision 1.34 2014/12/11 17:15:03EST lwalling 
** Remove unnecessary include of osapi-os-filesys.h
** Revision 1.33 2014/10/22 17:51:01EDT lwalling 
** Allow zero as a valid semaphore ID, use FM_CHILD_SEM_INVALID instead
** Revision 1.32 2014/06/09 16:59:51EDT lwalling 
** Change all binary semaphores to count semaphores
** Revision 1.31 2011/04/20 11:34:30EDT lwalling 
** Cast unsigned app name buffer to (char *), remove unused local variable
** Revision 1.30 2011/04/19 16:41:03EDT lwalling 
** Added function FM_VerifyOverwrite to validate overwrite command arguments
** Revision 1.29 2011/04/19 10:30:24EDT lwalling 
** Fail child task commands rather than execute them in context of parent task
** Revision 1.28 2010/01/12 16:35:28EST lwalling 
** Temp fix to use Binary instead of Counting semaphore
** Revision 1.27 2009/11/20 15:32:27EST lwalling 
** Remove return code and error events from FM_AppendPathSep
** Revision 1.26 2009/11/17 13:40:51EST lwalling 
** Remove global open files list data structure
** Revision 1.25 2009/11/13 16:32:46EST lwalling 
** Modify macro names, remove VerifyFileOpen function
** Revision 1.24 2009/11/09 16:54:53EST lwalling 
** Add FileInfo arg to GetState func, add test for lost semaphore to VerifyChild func
** Revision 1.23 2009/10/30 14:02:34EDT lwalling 
** Remove trailing white space from all lines
** Revision 1.22 2009/10/30 10:44:01EDT lwalling
** Remove detail from function prologs, modify directory list structure field names
** Revision 1.21 2009/10/29 11:42:25EDT lwalling
** Make common structure for open files list and open file telemetry packet, change open file to open files
** Revision 1.20 2009/10/26 16:43:34EDT lwalling
** Change some structure and variable names
** Revision 1.19 2009/10/26 11:31:02EDT lwalling
** Remove Close File command from FM application
** Revision 1.18 2009/10/23 14:40:05EDT lwalling
** Create FM child task to process slow commands, update event text, move slow util fncs to fm_child.c
** Revision 1.17 2009/10/16 15:43:46EDT lwalling
** Update event text, function names, arg names, comments, add no dir verify function
** Revision 1.16 2009/10/06 11:06:10EDT lwalling
** Clean up after create common filename verify functions
** Revision 1.15 2009/09/29 13:41:24EDT lwalling
** Perform tests for file open against current system list of open files, allow open files for copy/move/rename commands
** Revision 1.14 2009/09/28 15:29:56EDT lwalling
** Review and modify event text
** Revision 1.13 2009/09/28 14:15:27EDT lwalling
** Create common filename verification functions
** Revision 1.12 2009/09/14 16:58:35EDT lwalling
** Modify FM_DirListFileInit() to reference FM_DIRLIST_SUBTYPE from platform config file
** Revision 1.11 2009/09/14 16:08:12EDT lwalling
** Modified FM_IsValidPathname() to find string terminator or fail verification
** Revision 1.10 2009/09/10 13:04:01EDT lwalling
** Modified FM_GetOpenFileList() to remove call to OS_NameChange()
** Revision 1.9 2009/06/12 14:16:26EDT rmcgraw
** DCR82191:1 Changed OS_Mem function calls to CFE_PSP_Mem
** Revision 1.8 2009/01/07 12:39:52EST sstrege
** Fixed bug in DirListFileInit event
** Revision 1.7 2008/12/24 16:20:40EST sstrege
** Added directory check in IsValidDeleteFile function
** Revision 1.6 2008/12/22 15:45:46EST sstrege
** Updated IsValidDeleteFile utility function to accept Event Type as an input parameter
** Revision 1.5 2008/12/12 12:59:25EST sstrege
** Fixed bug in FM utility function FM_GetOpenFileList
** Revision 1.4 2008/10/06 11:32:04EDT sstrege
** Updated DirListFileInit function to write the directory listing statistics structure
** Revision 1.3 2008/09/30 18:36:30EDT sstrege
** Replaced Directory Listing File Header initialization code with call to CFS_FS_WriteHeader
** Revision 1.2 2008/06/20 16:21:24EDT slstrege
** Member moved from fsw/src/fm_cmd_utils.c in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/cfs_fm.pj to fm_cmd_utils.c in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/fsw/src/project.pj.
** Revision 1.1 2008/06/20 15:21:24ACT slstrege
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/cfs_fm.pj
*/

#include "cfe.h"
#include "fm_msg.h"
#include "fm_cmd_utils.h"
#include "fm_child.h"
#include "fm_perfids.h"
#include "fm_events.h"
#include "cfs_utils.h"

#include <string.h>
#include <ctype.h>


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM utility function -- verify command packet length             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean FM_IsValidCmdPktLength(CFE_SB_MsgPtr_t CmdPacket, uint16 ExpectedLength,
                                uint32 EventID, char *CmdText)
{
    boolean FunctionResult = TRUE;
    uint16 ActualLength = CFE_SB_GetTotalMsgLength(CmdPacket);

    /* Verify command packet length */
    if (ActualLength != ExpectedLength)
    {
        FunctionResult = FALSE;

        CFE_EVS_SendEvent(EventID, CFE_EVS_ERROR,
                         "%s error: invalid command packet length: expected = %d, actual = %d",
                          CmdText, ExpectedLength, ActualLength);
    }

    return(FunctionResult);

} /* FM_IsValidCmdPktLength */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM utility function -- verify state is not invalid              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean FM_VerifyOverwrite(uint16 Overwrite, uint32 EventID, char *CmdText)
{
    boolean FunctionResult = TRUE;

    /* Acceptable values are TRUE (one) and FALSE (zero) */
    if ((Overwrite != TRUE) && (Overwrite != FALSE))
    {
        CFE_EVS_SendEvent(EventID, CFE_EVS_ERROR,
                         "%s error: invalid overwrite = %d", CmdText, Overwrite);

        FunctionResult = FALSE;
    }

    return(FunctionResult);

} /* End FM_VerifyOverwrite */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM utility function -- get open files data                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

uint32 FM_GetOpenFilesData(FM_OpenFilesEntry_t *OpenFilesData)
{
    uint32 OpenFilesCount = 0;
    int32 FDTableIndex;
    OS_FDTableEntry FDTableEntry;
    CFE_ES_TaskInfo_t TaskInfo;

    /* Get system info for each file descriptor table entry */
    for (FDTableIndex = 0; FDTableIndex < OS_MAX_NUM_OPEN_FILES; FDTableIndex++)
    {
        OS_FDGetInfo(FDTableIndex, &FDTableEntry);

        /* If FD table entry is valid - then file is open */
        if (FDTableEntry.IsValid == TRUE)
        {
            /* Getting the list of filenames is optional */
            if (OpenFilesData != (FM_OpenFilesEntry_t *) NULL)
            {
                /* FDTableEntry.Path has logical filename saved when file was opened */
                strcpy(OpenFilesData[OpenFilesCount].LogicalName, FDTableEntry.Path);

                /* Get the name of the application that opened the file */
                CFE_PSP_MemSet(&TaskInfo, 0, sizeof(CFE_ES_TaskInfo_t));
                if (CFE_ES_GetTaskInfo(&TaskInfo, FDTableEntry.User) == CFE_SUCCESS)
                {
                    strcpy(OpenFilesData[OpenFilesCount].AppName, (char *) TaskInfo.AppName);
                }
            }

            /* File count is not optional */
            OpenFilesCount++;
        }
    }

    return(OpenFilesCount);

} /* End FM_GetOpenFilesData */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM utility function -- query filename state                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

uint32 FM_GetFilenameState(char *Filename, uint32 BufferSize, boolean FileInfoCmd)
{
    OS_FDTableEntry FDTableEntry;
    os_fstat_t FileStatus;
    uint32     FilenameState = FM_NAME_IS_INVALID;
    boolean    FilenameIsValid = FALSE;
    int32      StringLength = 0;
    uint32     i;


    /* Search Filename for a string terminator */
    for (StringLength = 0; StringLength < BufferSize; StringLength++)
    {
        if (Filename[StringLength] == '\0')
        {
            break;
        }
    }

    /* Verify that Filename is not empty and has a terminator */
    if ((StringLength > 0) && (StringLength < BufferSize))
    {
        /* Verify that the string characters are OK for a filename */
        FilenameIsValid = CFS_IsValidFilename(Filename, StringLength);
    }

    /* If Filename is valid, then determine its state */
    if (FilenameIsValid)
    {
        /* Check to see if Filename is in use */
        if (OS_stat(Filename, &FileStatus) == OS_SUCCESS)
        {
            /* Filename is in use, is it also a directory? */
            if (S_ISDIR(FileStatus.st_mode))
            {
                /* Filename is a directory */
                FilenameState = FM_NAME_IS_DIRECTORY;
            }
            else
            {
                /* Filename is a file, but is it open? */
                FilenameState = FM_NAME_IS_FILE_CLOSED;

                /* Search for Filename in current list of open files */
                for (i = 0; i < OS_MAX_NUM_OPEN_FILES; i++)
                {
                    /* Get system info for each file descriptor table entry */
                    OS_FDGetInfo(i, &FDTableEntry);

                    /* If the FD table entry is valid - then the file is open */
                    if (FDTableEntry.IsValid == TRUE)
                    {
                        if (strcmp(FDTableEntry.Path, Filename) == 0)
                        {
                            FilenameState = FM_NAME_IS_FILE_OPEN;
                            break;
                        }
                    }
                }
            }

            /* Save the last modify time and file size for File Info commands */
            if (FileInfoCmd)
            {
                FM_GlobalData.FileStatTime = FileStatus.st_mtime;
                FM_GlobalData.FileStatSize = FileStatus.st_size;
            }
        }
        else
        {
            /* Cannot get file stat - therefore does not exist */
            FilenameState = FM_NAME_IS_NOT_IN_USE;

            /* Save the last modify time and file size for File Info commands */
            if (FileInfoCmd)
            {
                FM_GlobalData.FileStatSize = 0;
                FM_GlobalData.FileStatTime = 0;
            }
        }
    }

    return(FilenameState);

} /* End FM_GetFilenameState */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM utility function -- verify state is not invalid              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

uint32 FM_VerifyNameValid(char *Name, uint32 BufferSize, uint32 EventID, char *CmdText)
{
    uint32  FilenameState;

    /* Looking for filename state != FM_NAME_IS_INVALID */
    FilenameState = FM_GetFilenameState(Name, BufferSize, TRUE);

    if (FilenameState == FM_NAME_IS_INVALID)
    {
        /* Insert a terminator in case the invalid string did not have one */
        Name[BufferSize - 1] = '\0';
        CFE_EVS_SendEvent(EventID, CFE_EVS_ERROR,
                         "%s error: invalid name: name = %s", CmdText, Name);
    }

    return(FilenameState);

} /* End FM_VerifyNameValid */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM utility function -- verify state is closed file              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean FM_VerifyFileClosed(char *Filename, uint32 BufferSize, uint32 EventID, char *CmdText)
{
    boolean Result = FALSE;
    uint32  FilenameState;

    /* Looking for filename state = file (closed) */
    FilenameState = FM_GetFilenameState(Filename, BufferSize, FALSE);

    if (FilenameState == FM_NAME_IS_INVALID)
    {
        /* Insert a terminator in case the invalid string did not have one */
        Filename[BufferSize - 1] = '\0';
        CFE_EVS_SendEvent(EventID, CFE_EVS_ERROR,
                         "%s error: filename is invalid: name = %s", CmdText, Filename);
    }
    else if (FilenameState == FM_NAME_IS_NOT_IN_USE)
    {
        CFE_EVS_SendEvent(EventID, CFE_EVS_ERROR,
                         "%s error: file does not exist: name = %s", CmdText, Filename);
    }
    else if (FilenameState == FM_NAME_IS_FILE_OPEN)
    {
        CFE_EVS_SendEvent(EventID, CFE_EVS_ERROR,
                         "%s error: file is already open: name = %s", CmdText, Filename);
    }
    else if (FilenameState == FM_NAME_IS_FILE_CLOSED)
    {
        Result = TRUE;
    }
    else if (FilenameState == FM_NAME_IS_DIRECTORY)
    {
        CFE_EVS_SendEvent(EventID, CFE_EVS_ERROR,
                         "%s error: filename is a directory: name = %s", CmdText, Filename);
    }

    return(Result);

} /* End FM_VerifyFileClosed */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM utility function -- verify state is open or closed file      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean FM_VerifyFileExists(char *Filename, uint32 BufferSize, uint32 EventID, char *CmdText)
{
    boolean Result = FALSE;
    uint32  FilenameState;

    /* Looking for filename state = file (open or closed) */
    FilenameState = FM_GetFilenameState(Filename, BufferSize, FALSE);

    if (FilenameState == FM_NAME_IS_INVALID)
    {
        /* Insert a terminator in case the invalid string did not have one */
        Filename[BufferSize - 1] = '\0';
        CFE_EVS_SendEvent(EventID, CFE_EVS_ERROR,
                         "%s error: filename is invalid: name = %s", CmdText, Filename);
    }
    else if (FilenameState == FM_NAME_IS_NOT_IN_USE)
    {
        CFE_EVS_SendEvent(EventID, CFE_EVS_ERROR,
                         "%s error: file does not exist: name = %s", CmdText, Filename);
    }
    else if ((FilenameState == FM_NAME_IS_FILE_OPEN) ||
             (FilenameState == FM_NAME_IS_FILE_CLOSED))
    {
        Result = TRUE;
    }
    else if (FilenameState == FM_NAME_IS_DIRECTORY)
    {
        CFE_EVS_SendEvent(EventID, CFE_EVS_ERROR,
                         "%s error: filename is a directory: name = %s", CmdText, Filename);
    }

    return(Result);

} /* End FM_VerifyFileExists */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM utility function -- verify state is unused                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean FM_VerifyFileNoExist(char *Filename, uint32 BufferSize, uint32 EventID, char *CmdText)
{
    boolean Result = FALSE;
    uint32  FilenameState;

    /* Looking for filename state = not in use */
    FilenameState = FM_GetFilenameState(Filename, BufferSize, FALSE);

    if (FilenameState == FM_NAME_IS_INVALID)
    {
        /* Insert a terminator in case the invalid string did not have one */
        Filename[BufferSize - 1] = '\0';
        CFE_EVS_SendEvent(EventID, CFE_EVS_ERROR,
                         "%s error: filename is invalid: name = %s", CmdText, Filename);
    }
    else if (FilenameState == FM_NAME_IS_NOT_IN_USE)
    {
        Result = TRUE;
    }
    else if ((FilenameState == FM_NAME_IS_FILE_OPEN) ||
             (FilenameState == FM_NAME_IS_FILE_CLOSED))
    {
        CFE_EVS_SendEvent(EventID, CFE_EVS_ERROR,
                         "%s error: file already exists: name = %s", CmdText, Filename);
    }
    else if (FilenameState == FM_NAME_IS_DIRECTORY)
    {
        CFE_EVS_SendEvent(EventID, CFE_EVS_ERROR,
                         "%s error: filename is a directory: name = %s", CmdText, Filename);
    }

    return(Result);

} /* End FM_VerifyFileNoExist */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM utility function -- verify state is unused or closed file    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean FM_VerifyFileNotOpen(char *Filename, uint32 BufferSize, uint32 EventID, char *CmdText)
{
    boolean Result = FALSE;
    uint32  FilenameState;

    /* Looking for filename state = file (closed) or name not in use */
    FilenameState = FM_GetFilenameState(Filename, BufferSize, FALSE);

    if (FilenameState == FM_NAME_IS_INVALID)
    {
        /* Insert a terminator in case the invalid string did not have one */
        Filename[BufferSize - 1] = '\0';
        CFE_EVS_SendEvent(EventID, CFE_EVS_ERROR,
                         "%s error: filename is invalid: name = %s", CmdText, Filename);
    }
    else if (FilenameState == FM_NAME_IS_NOT_IN_USE)
    {
        Result = TRUE;
    }
    else if (FilenameState == FM_NAME_IS_FILE_OPEN)
    {
        CFE_EVS_SendEvent(EventID, CFE_EVS_ERROR,
                         "%s error: file exists as an open file: name = %s", CmdText, Filename);
    }
    else if (FilenameState == FM_NAME_IS_FILE_CLOSED)
    {
        Result = TRUE;
    }
    else if (FilenameState == FM_NAME_IS_DIRECTORY)
    {
        CFE_EVS_SendEvent(EventID, CFE_EVS_ERROR,
                         "%s error: filename is a directory: name = %s", CmdText, Filename);
    }

    return(Result);

} /* End FM_VerifyFileNotOpen */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM utility function -- verify state is directory                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean FM_VerifyDirExists(char *Directory, uint32 BufferSize, uint32 EventID, char *CmdText)
{
    boolean Result = FALSE;
    uint32  FilenameState;

    /* Looking for filename state = directory */
    FilenameState = FM_GetFilenameState(Directory, BufferSize, FALSE);

    if (FilenameState == FM_NAME_IS_INVALID)
    {
        /* Insert a terminator in case the invalid string did not have one */
        Directory[BufferSize - 1] = '\0';
        CFE_EVS_SendEvent(EventID, CFE_EVS_ERROR,
                         "%s error: directory name is invalid: name = %s", CmdText, Directory);
    }
    else if (FilenameState == FM_NAME_IS_NOT_IN_USE)
    {
        CFE_EVS_SendEvent(EventID, CFE_EVS_ERROR,
                         "%s error: directory does not exist: name = %s", CmdText, Directory);
    }
    else if ((FilenameState == FM_NAME_IS_FILE_OPEN) ||
             (FilenameState == FM_NAME_IS_FILE_CLOSED))
    {
        CFE_EVS_SendEvent(EventID, CFE_EVS_ERROR,
                         "%s error: directory name exists as a file: name %s", CmdText, Directory);
    }
    else if (FilenameState == FM_NAME_IS_DIRECTORY)
    {
        Result = TRUE;
    }

    return(Result);

} /* End FM_VerifyDirExists */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM utility function -- verify state is unused                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean FM_VerifyDirNoExist(char *Name, uint32 BufferSize, uint32 EventID, char *CmdText)
{
    boolean Result = FALSE;
    uint32  FilenameState;

    /* Looking for filename state = unused */
    FilenameState = FM_GetFilenameState(Name, BufferSize, FALSE);

    if (FilenameState == FM_NAME_IS_INVALID)
    {
        /* Insert a terminator in case the invalid string did not have one */
        Name[BufferSize - 1] = '\0';
        CFE_EVS_SendEvent(EventID, CFE_EVS_ERROR,
                         "%s error: directory name is invalid: name = %s", CmdText, Name);
    }
    else if (FilenameState == FM_NAME_IS_NOT_IN_USE)
    {
        Result = TRUE;
    }
    else if ((FilenameState == FM_NAME_IS_FILE_OPEN) ||
             (FilenameState == FM_NAME_IS_FILE_CLOSED))
    {
        CFE_EVS_SendEvent(EventID, CFE_EVS_ERROR,
                         "%s error: directory name exists as a file: name = %s", CmdText, Name);
    }
    else if (FilenameState == FM_NAME_IS_DIRECTORY)
    {
        CFE_EVS_SendEvent(EventID, CFE_EVS_ERROR,
                         "%s error: directory already exists: name = %s", CmdText, Name);
    }

    return(Result);

} /* End FM_VerifyDirNoExist */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM utility function -- verify child task interface is alive     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean FM_VerifyChildTask(uint32 EventID, char *CmdText)
{
    boolean Result = FALSE;

    /* Copy of child queue count that child task cannot change */
    uint8 LocalQueueCount = FM_GlobalData.ChildQueueCount;

    /* Verify child task is active and queue interface is healthy */
    if (FM_GlobalData.ChildSemaphore == FM_CHILD_SEM_INVALID)
    {
        CFE_EVS_SendEvent(EventID, CFE_EVS_ERROR,
                         "%s error: child task is disabled", CmdText);

        /* Child task disabled - cannot add another command */
        Result = FALSE;
    }
    else if (LocalQueueCount == FM_CHILD_QUEUE_DEPTH)
    {
        CFE_EVS_SendEvent(EventID, CFE_EVS_ERROR,
                         "%s error: child task queue is full", CmdText);

        /* Queue full - cannot add another command */
        Result = FALSE;
    }
    else if ((LocalQueueCount > FM_CHILD_QUEUE_DEPTH) ||
             (FM_GlobalData.ChildWriteIndex >= FM_CHILD_QUEUE_DEPTH))
    {
        CFE_EVS_SendEvent(EventID, CFE_EVS_ERROR,
                         "%s error: child task interface is broken: count = %d, index = %d",
                          CmdText, LocalQueueCount, FM_GlobalData.ChildWriteIndex);

        /* Queue broken - cannot add another command */
        Result = FALSE;
    }
    else
    {
        CFE_PSP_MemSet(&FM_GlobalData.ChildQueue[FM_GlobalData.ChildWriteIndex],
                       0, sizeof(FM_ChildQueueEntry_t));

        /* OK to add another command to the queue */
        Result = TRUE;
    }

    return(Result);

} /* End FM_VerifyChildTask */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM utility function -- invoke child task command processor      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_InvokeChildTask(void)
{

    /* Update callers queue index */
    FM_GlobalData.ChildWriteIndex++;

    if (FM_GlobalData.ChildWriteIndex >= FM_CHILD_QUEUE_DEPTH)
    {
        FM_GlobalData.ChildWriteIndex = 0;
    }

    /* Prevent parent/child updating queue counter at same time */
    OS_MutSemTake(FM_GlobalData.ChildQueueCountSem);
    FM_GlobalData.ChildQueueCount++;
    OS_MutSemGive(FM_GlobalData.ChildQueueCountSem);

    /* Does the child task still have a semaphore? */
    if (FM_GlobalData.ChildSemaphore != FM_CHILD_SEM_INVALID)
    {
        /* Signal child task to call command handler */
        OS_CountSemGive(FM_GlobalData.ChildSemaphore);
    }

    return;

} /* End of FM_InvokeChildTask */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM utility function -- add path separator to directory name     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_AppendPathSep(char *Directory, uint32 BufferSize)
{
    uint32 StringLength;

    /*
    **  Previous verification tests ensure that the length of
    **   the string is both non-zero and less than the size
    **   of the string buffer.
    */
    StringLength = strlen(Directory);

    /* Do nothing if string already ends with a path separator */
    if (Directory[StringLength - 1] != '/')
    {
        /* Verify that string buffer has room for a path separator */
        if (StringLength < (BufferSize - 1))
        {
            strcat(Directory, "/");
        }
    }

    return;

} /* End of FM_AppendPathSep */


/************************/
/*  End of File Comment */
/************************/
