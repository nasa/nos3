/*
** $Id: fm_msg.h 1.29 2015/02/28 17:50:45EST sstrege Exp  $
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
**        Packet Header File
**
** Purpose: Specification for the CFS FM command and telemetry messages.
**
** Author: Susanne L. Strege, Code 582 NASA GSFC
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**
** $Log: fm_msg.h  $
** Revision 1.29 2015/02/28 17:50:45EST sstrege 
** Added copyright information
** Revision 1.28 2014/12/18 14:32:55EST lwalling 
** Added mutex semaphore protection when accessing FM_GlobalData.ChildQueueCount
** Revision 1.27 2014/12/04 17:52:10EST lwalling 
** Removed unused CommandWarnCounter
** Revision 1.26 2011/07/04 13:57:22EDT lwalling 
** Change all housekeeping values from 16 to 8 bits
** Revision 1.25 2011/07/04 13:38:13EDT lwalling 
** Change cmd counter from 16 to 8 bits
** Revision 1.24 2011/05/16 14:30:20EDT lwalling 
** Cleanup comments in description of housekeeping data structure
** Revision 1.23 2011/04/19 15:57:22EDT lwalling 
** Added overwrite argument to copy and move file commands
** Revision 1.22 2011/04/15 15:17:34EDT lwalling 
** Added current and previous child task command code to global data and housekeeping packet
** Revision 1.21 2010/03/08 15:49:39EST lwalling 
** Remove uint64 data type from free space packet
** Revision 1.20 2010/03/03 18:20:09EST lwalling 
** Changed WarnCounter to WarnCtr to match ASIST database
** Revision 1.19 2010/01/12 15:06:58EST lwalling 
** Remove references to fm_mission_cfg.h
** Revision 1.18 2009/11/17 13:40:52EST lwalling 
** Remove global open files list data structure
** Revision 1.17 2009/11/13 16:23:24EST lwalling 
** Modify macro names, add CRC arg to GetFileInfo cmd pkt, add SetTableEntryState cmd pkt
** Revision 1.16 2009/11/09 16:55:55EST lwalling 
** Cleanup doxygen comments, change structure names, add app global data
** Revision 1.15 2009/10/30 16:00:31EDT lwalling 
** Remove include fm_msgdefs.h, add HK request command packet definition
** Revision 1.14 2009/10/30 14:02:27EDT lwalling 
** Remove trailing white space from all lines
** Revision 1.13 2009/10/30 10:45:58EDT lwalling
** Add table structures, create command specific packet structures
** Revision 1.12 2009/10/29 11:42:26EDT lwalling
** Make common structure for open files list and open file telemetry packet, change open file to open files
** Revision 1.11 2009/10/27 17:31:14EDT lwalling
** Add a child task command warning counter
** Revision 1.10 2009/10/26 16:44:57EDT lwalling
** Add child task vars to housekeeping pkt, change some structure and variable names
** Revision 1.9 2009/10/26 11:31:02EDT lwalling
** Remove Close File command from FM application
** Revision 1.8 2009/10/23 14:49:08EDT lwalling
** Update event text and descriptions of event text
** Revision 1.7 2009/10/16 15:45:05EDT lwalling
** Update comments, descriptive text, command code names, structure names
** Revision 1.6 2009/10/09 17:23:50EDT lwalling
** Create command to generate file system free space packet, replace device table with free space table
** Revision 1.5 2009/10/08 16:20:23EDT lwalling
** Remove disk free space from HK telemetry
** Revision 1.4 2009/09/28 14:15:30EDT lwalling
** Create common filename verification functions
** Revision 1.3 2008/10/01 16:16:00EDT sstrege
** Updated FM_SourceUint16DataCmd_t to FM_SourceUint32DataCmd_t
** Revision 1.2 2008/06/20 16:21:38EDT slstrege
** Member moved from fsw/src/fm_msg.h in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/cfs_fm.pj to fm_msg.h in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/fsw/src/project.pj.
** Revision 1.1 2008/06/20 15:21:38ACT slstrege
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/cfs_fm.pj
*/

#ifndef _fm_msg_h_
#define _fm_msg_h_

#include "cfe.h"
#include "fm_platform_cfg.h"
#include "fm_defs.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM -- command packet structures                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
**  \brief Housekeeping Request command packet structure
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */

} FM_HousekeepingCmd_t;


/*
**  \brief No-Operation command packet structure
**
**  For command details see #FM_NOOP_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */

} FM_NoopCmd_t;


/*
**  \brief Reset Counters command packet structure
**
**  For command details see #FM_RESET_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */

} FM_ResetCmd_t;


/*
**  \brief Copy File command packet structure
**
**  For command details see #FM_COPY_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    uint16  Overwrite;                      /**< \brief Allow overwrite */
    char    Source[OS_MAX_PATH_LEN];        /**< \brief Source filename */
    char    Target[OS_MAX_PATH_LEN];        /**< \brief Target filename */

} FM_CopyFileCmd_t;


/*
**  \brief Move File command packet structure
**
**  For command details see #FM_MOVE_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    uint16  Overwrite;                      /**< \brief Allow overwrite */
    char    Source[OS_MAX_PATH_LEN];        /**< \brief Source filename */
    char    Target[OS_MAX_PATH_LEN];        /**< \brief Target filename */

} FM_MoveFileCmd_t;


/*
**  \brief Rename File command packet structure
**
**  For command details see #FM_RENAME_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    char    Source[OS_MAX_PATH_LEN];        /**< \brief Source filename */
    char    Target[OS_MAX_PATH_LEN];        /**< \brief Target filename */

} FM_RenameFileCmd_t;


/*
**  \brief Delete File command packet structure
**
**  For command details see #FM_DELETE_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    char    Filename[OS_MAX_PATH_LEN];      /**< \brief Delete filename */

} FM_DeleteFileCmd_t;


/*
**  \brief Delete All command packet structure
**
**  For command details see #FM_DELETE_ALL_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    char    Directory[OS_MAX_PATH_LEN];     /**< \brief Directory name */

} FM_DeleteAllCmd_t;


/*
**  \brief Decompress File command packet structure
**
**  For command details see #FM_DECOMPRESS_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    char    Source[OS_MAX_PATH_LEN];        /**< \brief Source filename */
    char    Target[OS_MAX_PATH_LEN];        /**< \brief Target filename */

} FM_DecompressCmd_t;


/*
**  \brief Concatenate Files command packet structure
**
**  For command details see #FM_CONCAT_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    char    Source1[OS_MAX_PATH_LEN];       /**< \brief Source 1 filename */
    char    Source2[OS_MAX_PATH_LEN];       /**< \brief Source 2 filename */
    char    Target[OS_MAX_PATH_LEN];        /**< \brief Target filename */

} FM_ConcatCmd_t;


/*
**  \brief Get File Info command packet structure
**
**  For command details see #FM_GET_FILE_INFO_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    char    Filename[OS_MAX_PATH_LEN];      /**< \brief Filename */
    uint32  FileInfoCRC;                    /**< \brief File info CRC method */

} FM_GetFileInfoCmd_t;


/*
**  \brief Get Open Files command packet structure
**
**  For command details see #FM_GET_OPEN_FILES_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */

} FM_GetOpenFilesCmd_t;


/*
**  \brief Create Directory command packet structure
**
**  For command details see #FM_CREATE_DIR_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    char    Directory[OS_MAX_PATH_LEN];     /**< \brief Directory name */

} FM_CreateDirCmd_t;


/*
**  \brief Delete Directory command packet structure
**
**  For command details see #FM_DELETE_DIR_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    char    Directory[OS_MAX_PATH_LEN];     /**< \brief Directory name */

} FM_DeleteDirCmd_t;


/*
**  \brief Get DIR List to File command packet structure
**
**  For command details see #FM_GET_DIR_FILE_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    char    Directory[OS_MAX_PATH_LEN];     /**< \brief Directory name */
    char    Filename[OS_MAX_PATH_LEN];      /**< \brief Filename */

} FM_GetDirFileCmd_t;


/*
**  \brief Get DIR List to Packet command packet structure
**
**  For command details see #FM_GET_DIR_PKT_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    char    Directory[OS_MAX_PATH_LEN];     /**< \brief Directory name */
    uint32  DirListOffset;                  /**< \brief Index of 1st dir entry to put in packet */

} FM_GetDirPktCmd_t;


/*
**  \brief Get Free Space command packet structure
**
**  For command details see #FM_GET_FREE_SPACE_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */

} FM_GetFreeSpaceCmd_t;


/*
**  \brief Set Table State command packet structure
**
**  For command details see #FM_SET_TABLE_STATE_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    uint32  TableEntryIndex;                /**< \brief Table entry index */
    uint32  TableEntryState;                /**< \brief New table entry state */

} FM_SetTableStateCmd_t;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM -- get directory listing telemetry structures                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \fmtlm Get Directory Listing entry structure
**/
typedef struct
{
    char    EntryName[OS_MAX_PATH_LEN];     /**< \fmtlmmnemonic \FM_DLFileName
                                                 \brief Directory Listing Filename */
    uint32  EntrySize;                      /**< \fmtlmmnemonic \FM_DLFileSize
                                                 \brief Directory Listing File Size */
    uint32  ModifyTime;                     /**< \fmtlmmnemonic \FM_DLModTime
                                                 \brief Directory Listing File Last Modification Times */
} FM_DirListEntry_t;

/**
**  \fmtlm Get Directory Listing telemetry packet
**/
typedef struct
{
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< \brief cFE SB tlm hdr */

    char    DirName[OS_MAX_PATH_LEN];       /**< \fmtlmmnemonic \FM_DirName
                                                 \brief Directory Name */
    uint32  TotalFiles;                     /**< \fmtlmmnemonic \FM_TotalFiles
                                                 \brief Number of files in the directory */
    uint32  PacketFiles;                    /**< \fmtlmmnemonic \FM_PacketFiles
                                                 \brief Number of files in this packet */
    uint32  FirstFile;                      /**< \fmtlmmnemonic \FM_FirstFile
                                                 \brief Index into directory files of first packet file */
    FM_DirListEntry_t  FileList[FM_DIR_LIST_PKT_ENTRIES];  /**< \fmtlmmnemonic \FM_DLFileList
                                                 \brief Directory listing file data */
} FM_DirListPkt_t;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM -- get directory listing to file structures                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \brief Get Directory Listing file statistics structure
**/
typedef struct
{
    char    DirName[OS_MAX_PATH_LEN];       /**< \brief Directory name */
    uint32  DirEntries;                     /**< \brief Number of entries in the directory */
    uint32  FileEntries;                    /**< \brief Number of entries written to output file */

} FM_DirListFileStats_t;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM -- get file information telemetry structure                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \fmtlm Get File Info telemetry packet
**/
typedef struct
{
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< \brief cFE SB tlm hdr */

    uint8   FileStatus;                     /**< \fmtlmmnemonic \FM_FileStatus
                                                 \brief Status indicating whether the file is open or closed */
    uint8   CRC_Computed;                   /**< \fmtlmmnemonic \FM_ComputeCRC
                                                 \brief Flag indicating whether a CRC was computed or not */
    uint8   Spare[2];                       /**< \fmtlmmnemonic \FM_InfoPad
                                                 \brief Structure padding */
    uint32  CRC;                            /**< \fmtlmmnemonic \FM_CRC
                                                 \brief CRC value if computed */
    uint32  FileSize;                       /**< \fmtlmmnemonic \FM_InfoFileSize
                                                 \brief File Size */
    uint32  LastModifiedTime;               /**< \fmtlmmnemonic \FM_ModTime
                                                 \brief Last Modification Time of File */
    char    Filename[OS_MAX_PATH_LEN];      /**< \fmtlmmnemonic \FM_InfoFileName
                                                 \brief Name of File */
} FM_FileInfoPkt_t;



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM -- get open files list telemetry structures                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \brief Get Open Files list entry structure
**/
typedef struct
{
	char    LogicalName[OS_MAX_PATH_LEN];   /**< \brief Logical filename */
    char    AppName[OS_MAX_API_NAME];       /**< \brief Application that opened file */

} FM_OpenFilesEntry_t;


/**
**  \fmtlm Get Open Files telemetry packet
**/
typedef struct
{
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< \brief cFE SB tlm hdr */

    uint32  NumOpenFiles;                   /**< \fmtlmmnemonic \FM_TotalOpenFiles
                                                 \brief Number of files opened via cFE */
    FM_OpenFilesEntry_t  OpenFilesList[OS_MAX_NUM_OPEN_FILES];  /**< \fmtlmmnemonic \FM_OpenFilesList
                                                 \brief List of files opened via cFE */
} FM_OpenFilesPkt_t;



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM -- get file system free space telemetry structures           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \brief Get Free Space list entry structure
**/
typedef struct
{
    uint32  FreeSpace_A;                    /**< \fmtlmmnemonic \FM_FreeSpace_A
                                                 \brief First 32 bit portion of a 64 bit value */
    uint32  FreeSpace_B;                    /**< \fmtlmmnemonic \FM_FreeSpace_B
                                                 \brief Second 32 bit portion of a 64 bit value */
    char    Name[OS_MAX_PATH_LEN];          /**< \fmtlmmnemonic \FM_PktFsName
                                                 \brief File system name */
} FM_FreeSpacePktEntry_t;


/**
**  \fmtlm Get Free Space telemetry packet
**/
typedef struct
{
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< \brief cFE SB tlm hdr */

    FM_FreeSpacePktEntry_t  FileSys[FM_TABLE_ENTRY_COUNT]; /**< \fmtlmmnemonic \FM_PktFsList
                                                 \brief Array of file system free space entries */
} FM_FreeSpacePkt_t;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM -- housekeeping telemetry structure                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
** \fmtlm Housekeeping telemetry packet
**/
typedef struct
{
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< \brief cFE SB tlm hdr */

    uint8   CommandCounter;		            /**< \fmtlmmnemonic \FM_CMDPC
                                                 \brief Application command counter */
    uint8   CommandErrCounter;			    /**< \fmtlmmnemonic \FM_CMDEC
                                                 \brief Application command error counter */
    uint8   Spare;                          /**< \brief Placeholder for unused command warning counter */

    uint8   NumOpenFiles;                   /**< \fmtlmmnemonic \FM_NumOpen
                                                 \brief Number of open files in the system */

    uint8   ChildCmdCounter;                /**< \fmtlmmnemonic \FM_ChildCMDPC
                                                 \brief Child task command counter */
    uint8   ChildCmdErrCounter;             /**< \fmtlmmnemonic \FM_ChildCMDEC
                                                 \brief Child task command error counter */
    uint8   ChildCmdWarnCounter;            /**< \fmtlmmnemonic \FM_ChildWarnCtr
                                                 \brief Child task command warning counter */

    uint8   ChildQueueCount;                /**< \fmtlmmnemonic \FM_ChildQueueCount
                                                 \brief Number of pending commands in queue */

    uint8   ChildCurrentCC;                 /**< \fmtlmmnemonic \FM_ChildCurrCC
                                                 \brief Command code currently executing */
    uint8   ChildPreviousCC;                /**< \fmtlmmnemonic \FM_ChildPrevCC
                                                 \brief Command code previously executed */

} FM_HousekeepingPkt_t;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM -- file system free space table structures                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \brief Get Free Space table entry
**/
typedef struct
{
    uint32  State;                          /**< \brief Table entry enable/disable state */
    char    Name[OS_MAX_PATH_LEN];          /**< \brief File system name = string */

} FM_TableEntry_t;


/**
**  \brief Get Free Space table definition
**/
typedef struct
{
    FM_TableEntry_t FileSys[FM_TABLE_ENTRY_COUNT]; /**< \brief One entry for each file system */

} FM_FreeSpaceTable_t;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM -- child task interface command queue entry                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \brief Child Task Interface command queue entry structure
**/
typedef struct
{
    uint32  CommandCode;                    /**< \brief Command code - identifies the command */
    uint32  DirListOffset;                  /**< \brief Starting entry for dir list commands */
    uint32  FileInfoState;                  /**< \brief File info state */
    uint32  FileInfoSize;                   /**< \brief File info size */
    uint32  FileInfoTime;                   /**< \brief File info time */
    uint32  FileInfoCRC;                    /**< \brief File info CRC method */
    char    Source1[OS_MAX_PATH_LEN];       /**< \brief First source file or directory name command argument */
    char    Source2[OS_MAX_PATH_LEN];       /**< \brief Second source filename command argument */
    char    Target[OS_MAX_PATH_LEN];        /**< \brief Target filename command argument */

} FM_ChildQueueEntry_t;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM -- application global data structure                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \fmtlm Application global data structure
**/
typedef struct
{
    FM_FreeSpaceTable_t *FreeSpaceTablePtr; /**< \brief File System Table Pointer */
    CFE_TBL_Handle_t FreeSpaceTableHandle;  /**< \brief File System Table Handle */

    CFE_SB_PipeId_t  CmdPipe;               /**< \brief cFE software bus command pipe */

    uint32  ChildTaskID;                    /**< \brief Child task ID */
    uint32  ChildSemaphore;                 /**< \brief Child task wakeup counting semaphore */
    uint32  ChildQueueCountSem;             /**< \brief Child queue counter mutex semaphore */

    uint8   ChildCmdCounter;                /**< \brief Child task command success counter */
    uint8   ChildCmdErrCounter;             /**< \brief Child task command error counter */
    uint8   ChildCmdWarnCounter;            /**< \brief Child task command warning counter */

    uint8   ChildWriteIndex;                /**< \brief Array index for next write to command args */
    uint8   ChildReadIndex;                 /**< \brief Array index for next read from command args */
    uint8   ChildQueueCount;                /**< \brief Number of pending commands in queue */

    uint8   CommandCounter;                 /**< \brief Application command success counter */
    uint8   CommandErrCounter;	            /**< \brief Application command error counter */
    uint8   Spare8a;                        /**< \brief Placeholder for unused command warning counter */

    uint8   ChildCurrentCC;                 /**< \brief Command code currently executing */
    uint8   ChildPreviousCC;                /**< \brief Command code previously executed */
    uint8   Spare8b;                        /**< \brief Structure alignment spare */

    uint32  FileStatTime;                   /**< \brief Modify time from most recent OS_stat */
    uint32  FileStatSize;                   /**< \brief File size from most recent OS_stat */

    FM_DirListFileStats_t DirListFileStats; /**< \brief Get dir list to file statistics structure */

    FM_DirListPkt_t DirListPkt;             /**< \brief Get dir list to packet telemetry packet */

    FM_FreeSpacePkt_t FreeSpacePkt;         /**< \brief Get free space telemetry packet */

    FM_FileInfoPkt_t FileInfoPkt;           /**< \brief Get file info telemetry packet */

    FM_OpenFilesPkt_t OpenFilesPkt;         /**< \brief Get open files telemetry packet */

    FM_HousekeepingPkt_t HousekeepingPkt;   /**< \brief Application housekeeping telemetry packet */

    char    ChildBuffer[FM_CHILD_FILE_BLOCK_SIZE]; /**< \brief Child task file I/O buffer */

    FM_ChildQueueEntry_t ChildQueue[FM_CHILD_QUEUE_DEPTH];  /**< \brief Child task command queue */

} FM_GlobalData_t;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM -- access to global data structure (defined in fm_app.c)     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

extern FM_GlobalData_t    FM_GlobalData;


#endif /* _fm_msg_h_ */

/************************/
/*  End of File Comment */
/************************/
