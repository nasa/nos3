/*
** $Id: fm_child.h 1.6 2015/02/28 17:50:40EST sstrege Exp  $
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
** Title: CFS File Manager (FM) Child Task Header File
**
** Purpose: Prototypes for child task functions.
**
** Author: Scott Walling (Microtel)
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**
** $Log: fm_child.h  $
** Revision 1.6 2015/02/28 17:50:40EST sstrege 
** Added copyright information
** Revision 1.5 2012/05/02 10:34:51EDT acudmore 
** FM Delete All Files command fix.
** Revision 1.4 2011/07/04 16:13:34EDT lwalling 
** Add child task prototypes for move, rename, delete, create dir and delete dir command handlers
** Revision 1.3 2011/04/19 10:11:14EDT lwalling 
** Add prototype for FM_ChildLoop(), modify description of FM_ChildTask() and FM_ChildProcess()
** Revision 1.2 2010/01/13 15:21:57EST lwalling 
** Remove second command completion event from GetDirToFile
** Revision 1.1 2009/11/09 16:47:45EST lwalling 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/fsw/src/project.pj
*/

#ifndef _fm_child_h_
#define _fm_child_h_

#include "cfe.h"
#include "fm_msg.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM child task global function prototypes                        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \brief Child Task Initialization Function
**
**  \par Description
**       This function is invoked during FM application startup initialization to
**       create and initialize the FM Child Task.  The purpose for the child task
**       is to process FM application commands that take too long to execute within
**       the main task.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  (none)
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS     \endcode
**  \retstmt Error return codes from #OS_CountSemCreate      \endcode
**  \retstmt Error return codes from #CFE_ES_CreateChildTask \endcode
**  \endreturns
**
**  \sa #FM_AppInit
**/
int32 FM_ChildInit(void);


/**
**  \brief Child Task Entry Point Function
**
**  \par Description
**       This function is the entry point for the FM application child task.  The
**       function registers with CFE as a child task, creates the semaphore to
**       interface with the parent task and calls the child task main loop function.
**       Should the main loop function return due to a breakdown in the interface
**       handshake with the parent task, this function will self delete as a child
**       task with CFE. There is no return from #CFE_ES_DeleteChildTask.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  (none)
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**
**  \sa #CFE_ES_DeleteChildTask, #FM_ChildLoop
**/
void FM_ChildTask(void);


/**
**  \brief Child Task Main Loop Processor Function
**
**  \par Description
**       This function is the main loop for the FM application child task.  The
**       function waits indefinitely for the parent task to grant the handshake
**       semaphore, which is the signal that there are fresh command arguments in
**       the child task handshake queue.  The function will remain in this loop
**       until the child task is terminated by the CFE, or until a fatal error
**       occurs which causes the child task to terminate itself.  Fatal errors are
**       defined as any error returned by #OS_CountSemTake or if the handshake
**       queue is empty, or if the read index for the handshake queue is invalid.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  (none)
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**
**  \sa #FM_ChildQueueEntry_t, #FM_ChildProcess
**/
void FM_ChildLoop(void);


/**
**  \brief Child Task Command Queue Processor Function
**
**  \par Description
**       This function routes control to the appropriate child task command
**       handler.  After the command handler has finished, this function then
**       updates the queue access variables to point to the next queue entry.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  (none)
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**
**  \sa #FM_ChildQueueEntry_t, #FM_ChildTask
**/
void FM_ChildProcess(void);


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM child task command handlers                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \brief Child Task Copy File Command Handler
**
**  \par Description
**       This function is invoked when the FM child task has been granted the child
**       task handshake semaphore and the child task command queue contains arguments
**       that signal a copy file command.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in] CmdArgsPtr - A pointer to an entry in the child task handshake command
**       queue which contains the arguments necessary to process this command.
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**
**  \sa #FM_ChildQueueEntry_t, #FM_Copy, #FM_CopyFileCmd_t
**/
void FM_ChildCopyCmd(FM_ChildQueueEntry_t *CmdArgs);


/**
**  \brief Child Task Move File Command Handler
**
**  \par Description
**       This function is invoked when the FM child task has been granted the child
**       task handshake semaphore and the child task command queue contains arguments
**       that signal a move file command.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in] CmdArgsPtr - A pointer to an entry in the child task handshake command
**       queue which contains the arguments necessary to process this command.
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**
**  \sa #FM_ChildQueueEntry_t, #FM_Move, #FM_MoveFileCmd_t
**/
void FM_ChildMoveCmd(FM_ChildQueueEntry_t *CmdArgs);


/**
**  \brief Child Task Rename File Command Handler
**
**  \par Description
**       This function is invoked when the FM child task has been granted the child
**       task handshake semaphore and the child task command queue contains arguments
**       that signal a rename file command.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in] CmdArgsPtr - A pointer to an entry in the child task handshake command
**       queue which contains the arguments necessary to process this command.
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**
**  \sa #FM_ChildQueueEntry_t, #FM_Rename, #FM_RenameFileCmd_t
**/
void FM_ChildRenameCmd(FM_ChildQueueEntry_t *CmdArgs);


/**
**  \brief Child Task Delete File Command Handler
**
**  \par Description
**       This function is invoked when the FM child task has been granted the child
**       task handshake semaphore and the child task command queue contains arguments
**       that signal a delete file command.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in] CmdArgsPtr - A pointer to an entry in the child task handshake command
**       queue which contains the arguments necessary to process this command.
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**
**  \sa #FM_ChildQueueEntry_t, #FM_Delete, #FM_DeleteFileCmd_t
**/
void FM_ChildDeleteCmd(FM_ChildQueueEntry_t *CmdArgs);


/**
**  \brief Child Task Delete All Files Command Handler
**
**  \par Description
**       This function is invoked when the FM child task has been granted the child
**       task handshake semaphore and the child task command queue contains arguments
**       that signal a delete all files from a directory command.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in] CmdArgsPtr - A pointer to an entry in the child task handshake command
**       queue which contains the arguments necessary to process this command.
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**
**  \sa #FM_ChildQueueEntry_t, #FM_DeleteAll, #FM_DeleteAllCmd_t
**/
void FM_ChildDeleteAllCmd(FM_ChildQueueEntry_t *CmdArgs);


/**
**  \brief Child Task Decompress File Command Handler
**
**  \par Description
**       This function is invoked when the FM child task has been granted the child
**       task handshake semaphore and the child task command queue contains arguments
**       that signal a decompress file command.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in] CmdArgsPtr - A pointer to an entry in the child task handshake command
**       queue which contains the arguments necessary to process this command.
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**
**  \sa #FM_ChildQueueEntry_t, #FM_Decompress, #FM_DecompressCmd_t
**/
void FM_ChildDecompressCmd(FM_ChildQueueEntry_t *CmdArgs);


/**
**  \brief Child Task Concatenate Files Command Handler
**
**  \par Description
**       This function is invoked when the FM child task has been granted the child
**       task handshake semaphore and the child task command queue contains arguments
**       that signal a concatenate files command.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in] CmdArgsPtr - A pointer to an entry in the child task handshake command
**       queue which contains the arguments necessary to process this command.
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**
**  \sa #FM_ChildQueueEntry_t, #FM_Concat, #FM_ConcatCmd_t
**/
void FM_ChildConcatCmd(FM_ChildQueueEntry_t *CmdArgs);


/**
**  \brief Child Task Get File Info Command Handler
**
**  \par Description
**       This function is invoked when the FM child task has been granted the child
**       task handshake semaphore and the child task command queue contains arguments
**       that signal a get file info command.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in] CmdArgsPtr - A pointer to an entry in the child task handshake command
**       queue which contains the arguments necessary to process this command.
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**
**  \sa #FM_ChildQueueEntry_t, #FM_GetFileInfo, #FM_GetFileInfoCmd_t
**/
void FM_ChildFileInfoCmd(FM_ChildQueueEntry_t *CmdArgs);


/**
**  \brief Child Task Create Directory Command Handler
**
**  \par Description
**       This function is invoked when the FM child task has been granted the child
**       task handshake semaphore and the child task command queue contains arguments
**       that signal a create directory command.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in] CmdArgsPtr - A pointer to an entry in the child task handshake command
**       queue which contains the arguments necessary to process this command.
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**
**  \sa #FM_ChildQueueEntry_t, #FM_CreateDir, #FM_CreateDirCmd_t
**/
void FM_ChildCreateDirCmd(FM_ChildQueueEntry_t *CmdArgs);


/**
**  \brief Child Task Delete Directory Command Handler
**
**  \par Description
**       This function is invoked when the FM child task has been granted the child
**       task handshake semaphore and the child task command queue contains arguments
**       that signal a delete directory command.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in] CmdArgsPtr - A pointer to an entry in the child task handshake command
**       queue which contains the arguments necessary to process this command.
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**
**  \sa #FM_ChildQueueEntry_t, #FM_DeleteDir, #FM_DeleteDirCmd_t
**/
void FM_ChildDeleteDirCmd(FM_ChildQueueEntry_t *CmdArgs);


/**
**  \brief Child Task Get Dir List to File Command Handler
**
**  \par Description
**       This function is invoked when the FM child task has been granted the child
**       task handshake semaphore and the child task command queue contains arguments
**       that signal a get directory listing to a file command.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in] CmdArgsPtr - A pointer to an entry in the child task handshake command
**       queue which contains the arguments necessary to process this command.
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**
**  \sa #FM_ChildQueueEntry_t, #FM_GetDirFile, #FM_GetDirFileCmd_t
**/
void FM_ChildDirListFileCmd(FM_ChildQueueEntry_t *CmdArgs);


/**
**  \brief Child Task Get Dir List to Packet Command Handler
**
**  \par Description
**       This function is invoked when the FM child task has been granted the child
**       task handshake semaphore and the child task command queue contains arguments
**       that signal a get directory listing to a packet command.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in] CmdArgsPtr - A pointer to an entry in the child task handshake command
**       queue which contains the arguments necessary to process this command.
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**
**  \sa #FM_ChildQueueEntry_t, #FM_GetDirPkt, #FM_GetDirPktCmd_t
**/
void FM_ChildDirListPktCmd(FM_ChildQueueEntry_t *CmdArgs);


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM child task utility functions                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \brief Child Task Get Dir List to File Initialization Function
**
**  \par Description
**       This function creates the output file and then writes both the CFE file header
**       and a blank copy of the directory list statistics structure to the output file.
**       At the end of the command, software will re-write the statistics structure,
**       this time with up to date values.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [out] FileHandlePtr - A pointer to a file handle variable which is modified to
**       contain the newly created output file handle.
**  \param [in] Directory - A pointer to a buffer containing the directory name.
**  \param [in] Filename - A pointer to a buffer containing the output filename.
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Error return codes from #OS_creat           \endcode
**  \retstmt Error return codes from #CFE_FS_WriteHeader \endcode
**  \retstmt Error return codes from #OS_write           \endcode
**  \endreturns
**
**  \sa #FM_GetDirFile
**/
boolean FM_ChildDirListFileInit(int32 *FileHandlePtr, char *Directory, char *Filename);


/**
**  \brief Child Task Get Dir List to File Loop Processor Function
**
**  \par Description
**       This function reads each directory entry, determines the last modify time
**       and the size for each entry, and writes the entry data to the output file.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in] DirPtr - Directory pointer, a handle used to read directory entries.
**  \param [in] FileHandle - Output file handle.
**  \param [in] Directory - Pointer to a buffer containing the directory name.
**  \param [in] DirWithSep - Pointer to directory name with path separator appended.
**  \param [in] Filename - Pointer to a buffer containing the output filename.
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**
**  \sa #FM_GetDirFile
**/
void FM_ChildDirListFileLoop(os_dirp_t DirPtr, int32 FileHandle,
                             char *Directory, char *DirWithSep, char *Filename);


/**
**  \brief Child Task File Size and Time Utility Function
**
**  \par Description
**       This function is invoked to query the last modify time and current size for
**       each directory entry when processing either the Get Directory List to File
**       or Get Directory List to Packet commands.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in] Filename - Pointer to the combined directory and entry names.
**  \param [in] FileSize - Pointer to the number containing the current entry size.
**  \param [in] FileTime - Pointer to the number containing the last modify time.
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Error return codes from #OS_stat            \endcode
**  \endreturns
**
**  \sa #FM_GetDirFile, #FM_GetDirPkt
**/
int32 FM_ChildSizeAndTime(const char *Filename, uint32 *FileSize, uint32 *FileTime);


#endif /* _fm_child_h_ */

/************************/
/*  End of File Comment */
/************************/

