/*
** $Id: fm_msgdefs.h 1.19 2015/02/28 17:50:47EST sstrege Exp  $
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
** Title: CFS File Manager (FM) Application Command and Telemetry
**        Packet Definitions Header File
**
** Purpose: Specification for the CFS FM command and telemetry message
**          macro definitions.
**
** Author: Susanne L. Strege, Code 582 NASA GSFC
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**
** $Log: fm_msgdefs.h  $
** Revision 1.19 2015/02/28 17:50:47EST sstrege 
** Added copyright information
** Revision 1.18 2011/05/31 17:08:12EDT lwalling 
** Added definition for delete internal command - FM_DELETE_INT_CC
** Revision 1.17 2011/04/19 15:56:39EDT lwalling 
** Added overwrite argument to copy and move file commands
** Revision 1.16 2010/03/04 10:43:09EST lwalling 
** Corrected several Doxygen event ID references
** Revision 1.15 2010/03/03 18:19:03EST lwalling 
** Changed some Doxygen symbols, WarnCounter to WarnCtr, event ID macro names
** Revision 1.14 2009/11/13 16:24:08EST lwalling 
** Modify macro names, add SetTableEntryState cmd code
** Revision 1.13 2009/11/09 17:02:14EST lwalling 
** Move value defs to fm_defs.h, fix source indents, add process func, add size/time func
** Revision 1.12 2009/10/30 14:02:28EDT lwalling 
** Remove trailing white space from all lines
** Revision 1.11 2009/10/28 16:40:58EDT lwalling
** Complete effort to replace the use of phrase device table with file system free space table
** Revision 1.10 2009/10/26 11:31:01EDT lwalling
** Remove Close File command from FM application
** Revision 1.9 2009/10/16 15:50:44EDT lwalling
** Update command code names, comments, descriptive text
** Revision 1.8 2009/10/09 17:23:55EDT lwalling
** Create command to generate file system free space packet, replace device table with free space table
** Revision 1.7 2009/04/18 12:42:02EDT dkobe
** Corrected DirList message type in doxygen comment
** Revision 1.6 2009/04/18 12:38:17EDT dkobe
** Corrected typos in doxygen comments
** Revision 1.5 2009/01/07 12:40:43EST sstrege
** Update user guide notes for DirListFile command
** Revision 1.4 2008/11/30 16:10:48EST sstrege
** Updated DirListTlm doxygen alias to include FM prefix
** Revision 1.3 2008/10/01 16:18:14EDT sstrege
** Updated FM_DIR_LIST_MSG_CC documentation to reference new FM_SourceUint32DataCmd_t
** Revision 1.2 2008/06/20 16:21:40EDT slstrege
** Member moved from fsw/src/fm_msgdefs.h in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/cfs_fm.pj to fm_msgdefs.h in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/fsw/src/project.pj.
** Revision 1.1 2008/06/20 15:21:40ACT slstrege
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/cfs_fm.pj
*/

#ifndef _fm_msgdefs_h_
#define _fm_msgdefs_h_

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM command packet command codes                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/** \fmcmd No Operation
**
**  \par Description
**       This command performs no operation other than to generate an
**       informational event that also contains software version data.
**       The command is most often used as a general aliveness test by
**       demonstrating that the application can receive commands and
**       generate telemetry.
**
**  \fmcmdmnemonic \FM_Noop
**
**  \par Command Packet Structure
**       #FM_NoopCmd_t
**
**  \par Command Success Verification
**       - Informational event #FM_NOOP_CMD_EID will be sent
**       - Command success counter /FM_CMDPC will increment
**
**  \par Command Error Conditions
**       - Invalid command packet length
**
**  \par Command Failure Verification
**       - Command error counter /FM_CMDEC will increment
**       - Error event #FM_NOOP_PKT_ERR_EID will be sent
**
**  \par Criticality
**       - There are no critical issues related to this command.
*/
#define FM_NOOP_CC              0


/** \fmcmd Reset Counters
**
**  \par Description
**       This command resets the following housekeeping telemetry:
**       - Command success counter /FM_CMDPC
**       - Command error counter /FM_CMDEC
**       - Command warning counter /FM_WarnCtr
**       - Child cmd success counter /FM_ChildCMDPC
**       - Child cmd error counter /FM_ChildCMDEC
**       - Child cmd warning counter /FM_ChildWarnCtr
**
**  \fmcmdmnemonic \FM_ResetCtrs
**
**  \par Command Packet Structure
**       #FM_ResetCmd_t
**
**  \par Command Success Verification
**       - Command counters will be set to zero (see description)
**       - Debug event #FM_RESET_CMD_EID will be sent
**
**  \par Command Error Conditions
**       - Invalid command packet length
**
**  \par Command Failure Verification
**       - Command error counter /FM_CMDEC will increment
**       - Error event #FM_RESET_PKT_ERR_EID will be sent
**
**  \par Criticality
**       - There are no critical issues related to this command.
**
*/
#define FM_RESET_CC             1


/** \fmcmd Copy File
**
**  \par Description
**       This command copies the source file to the target file.
**       The source must be an existing file and the target must not be a
**       directory name.
**       If the Overwrite command argument is TRUE, then the target may be
**       an existing file, provided that the file is closed.
**       If the Overwrite command argument is FALSE, then the target must not exist.
**       The source and target may be on different file systems.
**       Because of the possibility that this command might take a very long time
**       to complete, command argument validation will be done immediately but
**       copying the file will be performed by a lower priority child task.
**       As such, the command result for this function only refers to the result
**       of command argument verification and being able to place the command on
**       the child task interface queue.
**
**  \fmcmdmnemonic \FM_Copy
**
**  \par Command Packet Structure
**       #FM_CopyFileCmd_t
**
**  \par Command Success Verification
**       - Command success counter /FM_CMDPC will increment after validation
**       - Child cmd success counter /FM_ChildCMDPC will increment after completion
**       - Debug event #FM_COPY_CMD_EID will be sent
**
**  \par Command Error Conditions
**       - Invalid command packet length
**       - Overwrite is not TRUE (one) or FALSE (zero)
**       - Source filename is invalid
**       - Source file does not exist
**       - Source filename is a directory
**       - Target filename is invalid
**       - Target file already exists
**       - Target filename is a directory
**       - Child task interface queue is full
**       - Child task interface logic is broken
**       - Failure of OS copy function
**
**  \par Command Failure Verification
**       - Command error counter /FM_CMDEC may increment
**       - Child cmd error counter /FM_ChildCMDEC may increment
**       - Error event #FM_COPY_PKT_ERR_EID may be sent
**       - Error event #FM_COPY_OVR_ERR_EID may be sent
**       - Error event #FM_COPY_SRC_ERR_EID may be sent
**       - Error event #FM_COPY_TGT_ERR_EID may be sent
**       - Error event #FM_COPY_CHILD_ERR_EID may be sent
**       - Error event #FM_COPY_OS_ERR_EID may be sent
**
**  \par Criticality
**       Copying files may consume file space needed by other
**       critical tasks.  Also, copying very large files may
**       consume more CPU resource than anticipated.
**
**  \sa #FM_MOVE_CC, #FM_RENAME_CC
*/
#define FM_COPY_CC              2


/** \fmcmd Move File
**
**  \par Description
**       This command moves the source file to the target file.
**       The source must be an existing file and the target must not be a
**       directory name.
**       If the Overwrite command argument is TRUE, then the target may be
**       an existing file, provided that the file is closed.
**       If the Overwrite command argument is FALSE, then the target must not exist.
**       Source and target must both be on the same file system.
**       The move command does not actually move any file data.
**       The command modifies the file system directory structure to
**       create a different file entry for the same file data.
**       If the user wishes to move a file across file systems, he
**       must first copy the file and then delete the original.
**
**  \fmcmdmnemonic \FM_Move
**
**  \par Command Packet Structure
**       #FM_MoveFileCmd_t
**
**  \par Command Success Verification
**       - Command success counter /FM_CMDPC will increment
**       - Debug event #FM_MOVE_CMD_EID will be sent
**
**  \par Command Error Conditions
**       - Invalid command packet length
**       - Overwrite is not TRUE (one) or FALSE (zero)
**       - Source filename is invalid
**       - Source file does not exist
**       - Source filename is a directory
**       - Target filename is invalid
**       - Target file already exists
**       - Target filename is a directory
**       - Failure of OS move function
**
**  \par Command Failure Verification
**       - Command error counter /FM_CMDEC will increment
**       - Error event #FM_MOVE_PKT_ERR_EID may be sent
**       - Error event #FM_MOVE_OVR_ERR_EID may be sent
**       - Error event #FM_MOVE_SRC_ERR_EID may be sent
**       - Error event #FM_MOVE_TGT_ERR_EID may be sent
**       - Error event #FM_MOVE_OS_ERR_EID may be sent
**
**  \par Criticality
**       - There are no critical issues related to this command.
**
**  \sa #FM_COPY_CC, #FM_RENAME_CC
*/
#define FM_MOVE_CC              3


/** \fmcmd Rename File
**
**  \par Description
**       This command renames the source file to the target file.
**       Source must be an existing file and target must not exist.
**       Source and target must both be on the same file system.
**       The rename command does not actually move any file data.
**       The command modifies the file system directory structure to
**       create a different file entry for the same file data.
**       If the user wishes to rename a file across file systems, he
**       must first copy the file and then delete the original.
**
**  \fmcmdmnemonic \FM_Rename
**
**  \par Command Packet Structure
**       #FM_RenameFileCmd_t
**
**  \par Command Success Verification
**       - Command success counter /FM_CMDPC will increment
**       - Debug event #FM_RENAME_CMD_EID will be sent
**
**  \par Command Error Conditions
**       - Invalid command packet length
**       - Source filename is invalid
**       - Source file does not exist
**       - Source filename is a directory
**       - Target filename is invalid
**       - Target file already exists
**       - Target filename is a directory
**       - Failure of OS rename function
**
**  \par Command Failure Verification
**       - Command error counter /FM_CMDEC will increment
**       - Error event #FM_RENAME_PKT_ERR_EID may be sent
**       - Error event #FM_RENAME_SRC_ERR_EID may be sent
**       - Error event #FM_RENAME_TGT_ERR_EID may be sent
**       - Error event #FM_RENAME_OS_ERR_EID may be sent
**
**  \par Criticality
**       - There are no critical issues related to this command.
**
**  \sa #FM_COPY_CC, #FM_MOVE_CC
*/
#define FM_RENAME_CC            4


/** \fmcmd Delete File
**
**  \par Description
**       This command deletes the source file.
**       Source must be an existing file that is not open.
**
**  \fmcmdmnemonic \FM_Delete
**
**  \par Command Packet Structure
**       #FM_DeleteFileCmd_t
**
**  \par Command Success Verification
**       - Command success counter /FM_CMDPC will increment
**       - Debug event #FM_DELETE_CMD_EID will be sent
**
**  \par Command Error Conditions
**       - Invalid command packet length
**       - Filename is invalid
**       - File does not exist
**       - File is open
**       - Filename is a directory
**       - Failure of OS delete function
**
**  \par Command Failure Verification
**       - Command error counter /FM_CMDEC will increment
**       - Error event #FM_DELETE_PKT_ERR_EID may be sent
**       - Error event #FM_DELETE_SRC_ERR_EID may be sent
**       - Error event #FM_DELETE_OS_ERR_EID may be sent
**
**  \par Criticality
**       The FM application does not provide a method to restore deleted
**       files.  Critical data may be lost when deleting files.
**
**  \sa #FM_DELETE_ALL_CC, #FM_DELETE_DIR_CC
*/
#define FM_DELETE_CC            5


/** \fmcmd Delete All Files
**
**  \par Description
**       This command deletes all files in the source directory.
**       Source must be an existing directory.
**       Open files and sub-directories are not deleted.
**       Because of the possibility that this command might take a very long time
**       to complete, command argument validation will be done immediately but
**       reading the directory and deleting each file will be performed by a
**       lower priority child task.
**       As such, the return value for this function only refers to the result
**       of command argument verification and being able to place the command on
**       the child task interface queue.
**
**  \fmcmdmnemonic \FM_DeleteAll
**
**  \par Command Packet Structure
**       #FM_DeleteAllCmd_t
**
**  \par Command Success Verification
**       - Command success counter /FM_CMDPC will increment after validation
**       - Child cmd success counter /FM_ChildCMDPC will increment after completion
**       - Debug event #FM_DELETE_ALL_CMD_EID will be sent
**
**  \par Command Error Conditions
**       - Invalid command packet length
**       - Invalid directory name
**       - Directory does not exist
**       - Directory name + separator + filename is too long
**       - Failure of OS delete function
**
**  \par Command Failure Verification
**       - Command error counter /FM_CMDEC may increment
**       - Child cmd error counter /FM_ChildCMDEC may increment
**       - Error event #FM_DELETE_ALL_PKT_ERR_EID may be sent
**       - Error event #FM_DELETE_ALL_SRC_ERR_EID may be sent
**       - Error event #FM_DELETE_ALL_CHILD_ERR_EID may be sent
**       - Error event #FM_DELETE_ALL_OS_ERR_EID may be sent
**
**  \par Command Warning Conditions
**       - Directory entry is not a file (sub-directory)
**       - Directory entry is an open file
**
**  \par Command Warning Verification
**       - Warning counter /FM_WarnCtr will increment
**       - Informational event #FM_DELETE_ALL_WARNING_EID will be sent
**
**  \par Criticality
**       The FM application does not provide a method to restore deleted
**       files.  Critical data may be lost when deleting files.  Also,
**       deleting a very large number of files may consume more CPU resource
**       than anticipated.
**
**  \sa #FM_DELETE_CC, #FM_DELETE_DIR_CC
*/
#define FM_DELETE_ALL_CC        7


/** \fmcmd Decompress File
**
**  \par Description
**       This command invokes a CFE function to decompress the source
**       file into the target file.
**       Source must be an existing file and target must not exist.
**       Source and target may be on different file systems.
**       Because of the possibility that this command might take a very long time
**       to complete, command argument validation will be done immediately but
**       decompressing the source file into the target file will be performed by
**       a lower priority child task.
**       As such, the return value for this function only refers to the result
**       of command argument verification and being able to place the command on
**       the child task interface queue.
**
**  \fmcmdmnemonic \FM_Decompress
**
**  \par Command Packet Structure
**       #FM_DecompressCmd_t
**
**  \par Command Success Verification
**       - Command success counter /FM_CMDPC will increment after validation
**       - Child cmd success counter /FM_ChildCMDPC will increment after completion
**       - Debug event #FM_DECOM_CMD_EID will be sent
**
**  \par Command Error Conditions
**       - Invalid command packet length
**       - Invalid source filename
**       - Source file does not exist
**       - Invalid target filename
**       - Target file does exist
**       - Failure of CFE_FS_Decompress function
**
**  \par Command Failure Verification
**       - Command error counter /FM_CMDEC may increment
**       - Child cmd error counter /FM_ChildCMDEC may increment
**       - Error event #FM_DECOM_PKT_ERR_EID may be sent
**       - Error event #FM_DECOM_SRC_ERR_EID may be sent
**       - Error event #FM_DECOM_TGT_ERR_EID may be sent
**       - Error event #FM_DECOM_CFE_ERR_EID may be sent
**
**  \par Criticality
**       Decompressing a very large file may consume more CPU resource than
**       anticipated.
*/
#define FM_DECOMPRESS_CC        8


/** \fmcmd Concatenate Files
**
**  \par Description
**       This command concatenates two source files into the target
**       file.
**       Sources must both be existing files and target must not exist.
**       Sources and target may be on different file systems.
**       Because of the possibility that this command might take a very long time
**       to complete, command argument validation will be done immediately but
**       copying the first source file to the target file and then appending the
**       second source file to the target file will be performed by a lower priority
**       child task.
**       As such, the return value for this function only refers to the result
**       of command argument verification and being able to place the command on
**       the child task interface queue.
**
**  \fmcmdmnemonic \FM_Concat
**
**  \par Command Packet Structure
**       #FM_ConcatCmd_t
**
**  \par Command Success Verification
**       - Command success counter /FM_CMDPC will increment after validation
**       - Child cmd success counter /FM_ChildCMDPC will increment after completion
**       - Debug event #FM_CONCAT_CMD_EID will be sent
**
**  \par Command Error Conditions
**       - Invalid command packet length
**       - Invalid source filename
**       - Source file does not exist
**       - Invalid target filename
**       - Target file does exist
**       - Failure of OS function (copy, open, read, write, etc.)
**
**  \par Command Failure Verification
**       - Command error counter /FM_CMDEC may increment
**       - Child cmd error counter /FM_ChildCMDEC may increment
**       - Error event #FM_CONCAT_PKT_ERR_EID may be sent
**       - Error event #FM_CONCAT_SRC1_ERR_EID may be sent
**       - Error event #FM_CONCAT_SRC2_ERR_EID may be sent
**       - Error event #FM_CONCAT_TGT_ERR_EID may be sent
**       - Error event #FM_CONCAT_OS_ERR_EID may be sent
**
**  \par Criticality
**       Concatenating very large files may consume more CPU resource
**       than anticipated.
**
**  \sa #FM_COPY_CC
*/
#define FM_CONCAT_CC            9


/** \fmcmd Get File Information
**
**  \par Description
**       This command creates an FM file information telemetry packet
**       for the source file.
**       The file information packet includes status that indicates
**       whether source is a file that is open or closed, a directory,
**       or does not exist.
**       The file information data also includes a CRC, file size,
**       last modify time and the source name.
**       Because of the possibility that this command might take a very long time
**       to complete, command argument validation will be done immediately but
**       collecting the status data and calculating the CRC will be performed by
**       a lower priority child task.
**       As such, the return value for this function only refers to the result
**       of command argument verification and being able to place the command on
**       the child task interface queue.
**
**  \fmcmdmnemonic \FM_GetFileInfo
**
**  \par Command Packet Structure
**       #FM_GetFileInfoCmd_t
**
**  \par Command Success Verification
**       - Command success counter /FM_CMDPC will increment after validation
**       - Child cmd success counter /FM_ChildCMDPC will increment after completion
**       - Debug event #FM_GET_FILE_INFO_CMD_EID will be sent
**
**  \par Command Error Conditions
**       - Invalid command packet length
**       - Invalid source filename
**       - Failure of OS_stat function
**
**  \par Command Failure Verification
**       - Command error counter /FM_CMDEC may increment
**       - Child cmd error counter /FM_ChildCMDEC may increment
**       - Error event #FM_GET_FILE_INFO_PKT_ERR_EID may be sent
**       - Error event #FM_GET_FILE_INFO_SRC_ERR_EID may be sent
**       - Error event #FM_GET_FILE_INFO_CHILD_ERR_EID may be sent
**
**  \par Criticality
**       Calculating the CRC for a very large file may consume more CPU resource than
**       anticipated.
**
**  \sa #FM_GET_OPEN_FILES_CC, #FM_GET_DIR_FILE_CC, #FM_GET_DIR_PKT_CC
*/
#define FM_GET_FILE_INFO_CC     10


/** \fmcmd Get Open Files Listing
**
**  \par Description
**       This command creates an FM open files telemetry packet.
**       The open files packet includes the number of open files and
**       for each open file, the name of the file and the name of the
**       application that has the file opened.
**
**  \fmcmdmnemonic \FM_GetOpenFiles
**
**  \par Command Packet Structure
**       #FM_GetOpenFilesCmd_t
**
**  \par Command Success Verification
**       - Command success counter /FM_CMDPC will increment
**       - Debug event #FM_GET_OPEN_FILES_CMD_EID will be sent
**
**  \par Command Error Conditions
**       - Invalid command packet length
**
**  \par Command Failure Verification
**       - Command error counter /FM_CMDEC will increment
**       - Error event #FM_GET_OPEN_FILES_PKT_ERR_EID will be sent
**
**  \par Criticality
**       - There are no critical issues related to this command.
**
**  \sa #FM_GET_FILE_INFO_CC, #FM_GET_DIR_FILE_CC, #FM_GET_DIR_PKT_CC
*/
#define FM_GET_OPEN_FILES_CC    11


/** \fmcmd Create Directory
**
**  \par Description
**       This command creates the source directory.
**       Source must be a valid directory name that does not exist.
**
**  \fmcmdmnemonic \FM_CreateDir
**
**  \par Command Packet Structure
**       #FM_CreateDirCmd_t
**
**  \par Command Success Verification
**       - Command success counter /FM_CMDPC will increment
**       - Debug event #FM_CREATE_DIR_CMD_EID will be sent
**
**  \par Command Error Conditions
**       - Invalid command packet length
**       - Invalid directory name
**       - Directory name already exists
**       - Failure of OS_mkdir function
**
**  \par Command Failure Verification
**       - Command error counter /FM_CMDEC will increment
**       - Error event #FM_CREATE_DIR_PKT_ERR_EID may be sent
**       - Error event #FM_CREATE_DIR_SRC_ERR_EID may be sent
**       - Error event #FM_CREATE_DIR_OS_ERR_EID may be sent
**
**  \par Criticality
**       - There are no critical issues related to this command.
**
**  \sa #FM_DELETE_DIR_CC
*/
#define FM_CREATE_DIR_CC        12


/** \fmcmd Remove Directory
**
**  \par Description
**       This command deletes the source directory, it does
**       not delete the contents of the directory.
**       Source must be a valid directory name that exists.
**
**  \fmcmdmnemonic \FM_DeleteDir
**
**  \par Command Packet Structure
**       #FM_DeleteDirCmd_t
**
**  \par Command Success Verification
**       - Command success counter /FM_CMDPC will increment
**       - Debug event #FM_DELETE_DIR_CMD_EID will be sent
**
**  \par Command Error Conditions
**       - Invalid command packet length
**       - Invalid directory name
**       - Directory does not exist
**       - Directory is not empty
**       - Failure of OS function (OS_opendir, OS_rmdir)
**
**  \par Command Failure Verification
**       - Command error counter /FM_CMDEC will increment
**       - Error event #FM_DELETE_DIR_PKT_ERR_EID may be sent
**       - Error event #FM_DELETE_DIR_SRC_ERR_EID may be sent
**       - Error event #FM_DELETE_DIR_EMPTY_ERR_EID may be sent
**       - Error event #FM_DELETE_DIR_OS_ERR_EID may be sent
**
**  \par Criticality
**       The unexpected loss of a directory may affect a critical
**       tasks ability to store data.
**
**  \sa #FM_CREATE_DIR_CC
*/
#define FM_DELETE_DIR_CC        13


/** \fmcmd Get Directory Listing to a File
**
**  \par Description
**       This command writes a listing of the contents of the source
**       directory to the target file.
**       If the target filename buffer is empty, then the default
**       target filename #FM_DIR_LIST_FILE_DEFNAME is used.
**       The command will overwrite a previous copy of the target
**       file, if one exists.
**       Because of the possibility that this command might take a very long time
**       to complete, command argument validation will be done immediately but
**       reading the directory will be performed by a lower priority child task.
**       As such, the return value for this function only refers to the result
**       of command argument verification and being able to place the command on
**       the child task interface queue.
**
**  \fmcmdmnemonic \FM_GetDirFile
**
**  \par Command Packet Structure
**       #FM_GetDirFileCmd_t
**
**  \par Command Success Verification
**       - Command success counter /FM_CMDPC will increment after validation
**       - Child cmd success counter /FM_ChildCMDPC will increment after completion
**       - Debug event #FM_GET_DIR_FILE_CMD_EID will be sent
**
**  \par Command Error Conditions
**       - Invalid command packet length
**       - Invalid source directory name
**       - Source directory does not exist
**       - Directory name + separator is too long
**       - Directory name + directory entry is too long
**       - Invalid target filename
**       - Target file is already open
**       - Failure of OS function (OS_opendir, OS_creat, OS_write)
**
**  \par Command Failure Verification
**       - Command error counter /FM_CMDEC may increment
**       - Child cmd error counter /FM_ChildCMDEC may increment
**       - Error event #FM_GET_DIR_FILE_PKT_ERR_EID may be sent
**       - Error event #FM_GET_DIR_FILE_SRC_ERR_EID may be sent
**       - Error event #FM_GET_DIR_FILE_TGT_ERR_EID may be sent
**       - Error event #FM_GET_DIR_FILE_WARNING_EID may be sent
**       - Error event #FM_GET_DIR_FILE_OS_ERR_EID may be sent
**
**  \par Criticality
**       Reading a directory that contains thousands of files may consume more CPU
**       resource than anticipated.
**
**  \sa #FM_GET_DIR_PKT_CC
*/
#define FM_GET_DIR_FILE_CC      14


/** \fmcmd Get Directory Listing to a Packet
**
**  \par Description
**       This command creates a telemetry packet #FM_DirListPkt_t that
**       contains a listing of the entries in the specified directory.
**       Since the packet will likely hold fewer entries than will be
**       possible in a directory, the command also provides an index
**       argument to define which entry in the directory is the first
**       entry reported in the telemetry packet.
**       After reading the directory list and skipping entries until
**       reaching the index of the first entry reported, the remaining
**       entries in the packet are filled sequentially until either
**       the packet is full or until there are no more entries in the
**       directory.
**       The first entry index is zero based - thus, when the first
**       entry index is zero the first directory entry will be the
**       first packet entry.
**       The number of entries per packet #FM_DIR_LIST_PKT_ENTRIES
**       is a platform configuration definition.
**       Because of the possibility that this command might take a very long time
**       to complete, command argument validation will be done immediately but
**       reading the directory will be performed by a lower priority child task.
**       As such, the return value for this function only refers to the result
**       of command argument verification and being able to place the command on
**       the child task interface queue.
**
**  \fmcmdmnemonic \FM_GetDirPkt
**
**  \par Command Packet Structure
**       #FM_GetDirPktCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - Command success counter /FM_CMDPC will increment after validation
**       - Child cmd success counter /FM_ChildCMDPC will increment after completion
**       - The #FM_DirListPkt_t telemetry packet will be sent
**       - The #FM_GET_DIR_PKT_CMD_EID debug event will be sent
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - OS error received opening directory
**       - OS error received requesting directory size
**       - OS error received closing directory
**       - Invalid directory pathname received
**       - Command packet length not as expected
**
**  \par Evidence of failure may be found in the following telemetry:
**       - Command error counter /FM_CMDEC may increment
**       - Child cmd error counter /FM_ChildCMDEC may increment
**       - Error event #FM_GET_DIR_PKT_PKT_ERR_EID may be sent
**       - Error event #FM_GET_DIR_PKT_SRC_ERR_EID may be sent
**       - Error event #FM_GET_DIR_PKT_CHILD_ERR_EID may be sent
**       - Error event #FM_GET_DIR_PKT_OS_ERR_EID may be sent
**
**  \par Criticality
**       Reading a directory that contains thousands of files may consume more CPU
**       resource than anticipated.
**
**  \sa #FM_GET_DIR_FILE_CC
*/
#define FM_GET_DIR_PKT_CC       15


/** \fmcmd Get Free Space
**
**  \par Description
**       This command queries the amount of free space for each of the
**       enabled entries in the file system free space table.  The data
**       is then placed in a telemetry packet and sent to ground.
**
**  \fmcmdmnemonic \FM_GetFreeSpace
**
**  \par Command Packet Structure
**       #FM_GetFreeSpaceCmd_t
**
**  \par Evidence of success may be found in the following telemetry:
**       - Command success counter /FM_CMDPC will increment
**       - Debug event #FM_GET_FREE_SPACE_CMD_EID will be sent
**       - Telemetry packet #FM_FreeSpacePkt_t will be sent
**
**  \par Error Conditions
**       - Invalid command packet length
**
**  \par Evidence of failure may be found in the following telemetry:
**       - Command error counter /FM_CMDEC will increment
**       - Error event #FM_GET_FREE_SPACE_PKT_ERR_EID will be sent
**
**  \par Criticality
**       - There are no critical issues related to this command.
*/
#define FM_GET_FREE_SPACE_CC    16


/** \fmcmd Set Free Space Table Entry State
**
**  \par Description
**       This command enables or disables a single entry in the FM
**       file system free space table.  Only table entries that are
**       currently enabled or disabled may be modified by command.
**       Unused table entries cannot be modified.
**
**  \fmcmdmnemonic \FM_SetTableState
**
**  \par Command Packet Structure
**       #FM_SetTableStateCmd_t
**
**  \par Evidence of success may be found in the following telemetry:
**       - Command success counter /FM_CMDPC will increment
**       - Informational event #FM_SET_TABLE_STATE_CMD_EID will be sent
**
**  \par Error Conditions
**       - Invalid command packet length
**       - FM file system free space table has not yet been loaded
**       - Invalid command argument, table entry index arg is out of range
**       - Invalid command argument, entry state arg is not enable/disable
**       - Invalid current table entry state, entry is unused
**
**  \par Evidence of failure may be found in the following telemetry:
**       - Command error counter /FM_CMDEC will increment
**       - Error event #FM_SET_TABLE_STATE_PKT_ERR_EID may be sent
**       - Error event #FM_SET_TABLE_STATE_TBL_ERR_EID may be sent
**       - Error event #FM_SET_TABLE_STATE_ARG_ERR_EID may be sent
**       - Error event #FM_SET_TABLE_STATE_UNUSED_ERR_EID may be sent
**
**  \par Criticality
**       - There are no critical issues related to this command.
*/
#define FM_SET_TABLE_STATE_CC   17


/** \fmcmd Delete File (internal)
**
**  \par Description
**       This is a special version of the #FM_DELETE_CC command for
**       use when the command is sent by another application, rather
**       than from the ground.  This version of the command will not
**       generate a success event, nor will the command increment the
**       command success counter.  The intent is to avoid confusion
**       resulting from telemetry representing the results of delete
**       commands sent by other applications and those sent from the
**       ground.  Refer to #FM_DELETE_CC command for use details.
**
**  \sa #FM_DELETE_CC
*/
#define FM_DELETE_INT_CC        18


#endif /* _fm_msgdefs_h_ */

/************************/
/*  End of File Comment */
/************************/
