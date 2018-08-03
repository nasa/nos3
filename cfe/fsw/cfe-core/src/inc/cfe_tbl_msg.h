/*
** $Id: cfe_tbl_msg.h 1.9 2014/08/19 13:49:24GMT-05:00 sstrege Exp  $
**
**      Copyright (c) 2004-2006, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
** Purpose:  cFE Table Services (TBL) SB message definitions header file
**
** Author:   D.Kobe/Hammers
**
** Notes:
**
** $Log: cfe_tbl_msg.h  $
** Revision 1.9 2014/08/19 13:49:24GMT-05:00 sstrege 
** Fixed doxygen warning
** Revision 1.8 2014/06/09 18:27:05EDT lwalling 
** Removed pad bytes from TBL housekeeping packet
** Revision 1.7 2014/06/09 10:30:25EDT lwalling 
** Add LastTableLoaded to TBL housekeeping structure
** Revision 1.6 2010/10/27 16:33:44EDT dkobe 
** Added CRC to Registry Entry Telemetry Message
** Revision 1.5 2010/10/27 13:54:24EDT dkobe 
** Added command message data structure for TBL notification messages
** Revision 1.4 2010/10/25 15:01:11EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.3 2010/10/04 15:24:58EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.2 2008/12/08 12:07:10EST dkobe 
** Updates to correct doxygen errors
** Revision 1.1 2008/04/17 08:05:25EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/inc/project.pj
** Revision 1.20 2007/09/27 15:53:53EDT David Kobe (dlkobe) 
** Added relevant "See Also" doxygen comment references.
** Revision 1.19 2007/07/05 15:39:17EDT dlkobe 
** Added Critical Table Flag to Tbl Registry Record Tlm Command
** Revision 1.18 2007/06/07 13:07:46EDT dlkobe 
** Added Command and Telemetry doxygen comments to TIME, EVS and SB
** Revision 1.17 2007/06/02 10:10:11EDT dlkobe 
** Added doxygen comments for User's Guides
** Revision 1.16 2007/05/25 16:24:49EDT dlkobe 
** Continued updating doxygen comments
** Revision 1.14 2007/05/16 15:13:39EDT dlkobe 
** Added MemPoolHandle members to HK packet
** Revision 1.13 2007/05/04 15:55:06EDT dlkobe 
** Added Delete CDS Command
** Revision 1.12 2006/11/10 15:14:35GMT-05:00 dlkobe 
** Updated comments to conform with doxygen
** Revision 1.11 2006/10/31 12:21:44GMT-05:00 dlkobe 
** Removed all references to the TBL_MOD_DUMP_ONLY option command
**
*/

/*
** Ensure that header is included only once...
*/
#ifndef _cfe_tbl_msg_
#define _cfe_tbl_msg_

/*
** Required header files...
*/
#include "cfe.h"

/*************************************************************************/

/*
** Table task command packet command codes
*/
/** \name Table Services Command Codes */
/** \{ */

/** \cfetblcmd Table No-Op
**
**  \par Description
**       This command performs no other function than to increment the 
**       command execution counter. The command may be used to verify 
**       general aliveness of the Table Services task.
**
**  \cfecmdmnemonic \TBL_NOOP
**
**  \par Command Structure
**       #CFE_TBL_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the 
**       following telemetry:
**       - \b \c \TBL_CMDPC - command execution counter will 
**         increment
**       - The #CFE_TBL_NOOP_INF_EID informational event message will 
**         be generated
**
**  \par Error Conditions
**       There are no error conditions for this command. If the Table 
**       Services receives the command, the event is sent (although it 
**       may be filtered by EVS) and the counter is incremented 
**       unconditionally.
**
**  \par Criticality
**       None
**
**  \sa 
*/
#define CFE_TBL_NOOP_CC         0    

/** \cfetblcmd Table Reset Counters
**
**  \par Description
**       This command resets the following counters within the Table 
**       Services housekeeping telemetry:
**       - Command Execution Counter (\TBL_CMDPC)
**       - Command Error Counter (\TBL_CMDEC)
**       - Successful Table Validations Counter (\TBL_VALSUCCESSCTR)
**       - Failed Table Validations Counter (\TBL_VALFAILEDCTR)
**       - Number of Table Validations Requested (\TBL_VALREQCTR)
**
**  \cfecmdmnemonic \TBL_RESETCTRS
**
**  \par Command Structure
**       #CFE_TBL_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \TBL_CMDPC - command execution counter will 
**         increment
**       - The #CFE_TBL_RESET_INF_EID debug event message will be 
**         generated
**
**  \par Error Conditions
**       There are no error conditions for this command. If the Table 
**       Services receives the command, the event is sent (although it 
**       may be filtered by EVS) and the counter is incremented 
**       unconditionally.
**
**  \par Criticality
**       This command is not inherently dangerous.  However, it is 
**       possible for ground systems and on-board safing procedures 
**       to be designed such that they react to changes in the counter 
**       values that are reset by this command.
**
**  \sa 
*/
#define CFE_TBL_RESET_CC        1

/** \cfetblcmd Load Table
**
**  \par Description
**       This command loads the contents of the specified file into 
**       an inactive buffer for the table specified within the file.
**
**  \cfecmdmnemonic \TBL_LOAD
**
**  \par Command Structure
**       #CFE_TBL_LoadCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \TBL_CMDPC - command execution counter will 
**         increment
**       - The #CFE_TBL_FILE_LOADED_INF_EID informational event message 
**         will be generated
**
**  \par Error Conditions
**       This command can fail for the following reasons:
**       - Table name found in table image file's table header is not found 
**         in table registry (ie - The table associated with the table image 
**         in the file has not been registered by an application).
**       - The table image file's header indicates the file contains 'x' 
**         number of bytes of data but the file contains less.
**       - No working buffers are available for the load.  This would indicate 
**         that too many single-buffered table loads are in progress at the same 
**         time.
**       - The table image file's header indicates the data to be loaded is 
**         beyond the size of the table.  Either the number of bytes in the 
**         file are too many or the starting offset into the table is too high.
**       - The table image file's header indicates there is no data in the 
**         file (ie - Number of bytes to load is zero).
**       - An attempt is being made to load an uninitialized table with a file 
**         containing only a partial table image.
**       - The table image file was unable to be opened.  Either the file does 
**         not exist at the specified location, the filename is in error, or 
**         the file system has been corrupted.
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \TBL_CMDEC - command error counter will increment
**       - Command specific error event messages are issued for all error cases
**
**  \par Criticality
**       This command is not inherently dangerous.  It is performing the first 
**       step of loading a table and can be aborted (using the Abort Table Load 
**       command described below) without affecting the contents of the active 
**       table image.
**
**  \sa  #CFE_TBL_DUMP_CC, #CFE_TBL_VALIDATE_CC, #CFE_TBL_ACTIVATE_CC, #CFE_TBL_ABORT_LOAD_CC
*/
#define CFE_TBL_LOAD_CC         2

/** \cfetblcmd Dump Table
**
**  \par Description
**       This command will cause the Table Services to put the contents 
**       of the specified table buffer into the command specified file. 
**
**  \cfecmdmnemonic \TBL_DUMP
**
**  \par Command Structure
**       #CFE_TBL_DumpCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the 
**       following telemetry:
**       - \b \c \TBL_CMDPC - command execution counter will 
**         increment
**       - Either the #CFE_TBL_OVERWRITE_DUMP_INF_EID OR the
**         #CFE_TBL_WRITE_DUMP_INF_EID informational event message will 
**         be generated
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - A single buffered table's inactive buffer was requested to be 
**         dumped and no such buffer is currently allocated.
**       - Error occurred during write operation to file. Possible causes 
**         might be insufficient space in the file system or the filename 
**         or file path is improperly specified.
**       - The specified table name was not found in the table registry.
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \TBL_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error 
**         cases
**
**  \par Criticality
**       This command is not inherently dangerous.  It will create a new 
**       file in the file system and could, if performed repeatedly without 
**       sufficient file management by the operator, fill the file system.
**
**  \sa  #CFE_TBL_LOAD_CC, #CFE_TBL_VALIDATE_CC, #CFE_TBL_ACTIVATE_CC, #CFE_TBL_ABORT_LOAD_CC
*/
#define CFE_TBL_DUMP_CC         3

/** \cfetblcmd Validate Table
**
**  \par Description
**       This command will cause Table Services to calculate the Data Integrity 
**       Value for the specified table and to notify the owning application that 
**       the table's validation function should be executed.  The results of both 
**       the Data Integrity Value computation and the validation function are 
**       reported in Table Services Housekeeping Telemetry.  
**
**  \cfecmdmnemonic \TBL_VALIDATE
**
**  \par Command Structure
**       #CFE_TBL_ValidateCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the following 
**       telemetry:
**       - \b \c \TBL_CMDPC -  command execution counter will increment
**       - \b \c \TBL_VALREQCTR - table validation request counter will increment
**       - \b \c \TBL_LASTVALCRC - calculated data integrity value will be updated
**       - The #CFE_TBL_VAL_REQ_MADE_INF_EID debug event message (indicating the 
**         application is being notified of a validation request)
**       
**       If the specified table has an associated validation function, then the 
**       following telemetry will also change:
**       - Either \b \c \TBL_VALSUCCESSCTR OR \b \c \TBL_VALFAILEDCTR will 
**         increment
**       - \b \c \TBL_VALCOMPLTDCTR - table validations performed counter will 
**         increment
**       - \b \c \TBL_LASTVALS - table validation function return status will 
**         update
**       - The #CFE_TBL_VALIDATION_INF_EID informational event message (indicating the 
**         validation function return status) will be generated
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - A single buffered table's inactive buffer was requested to be dumped 
**         and no such buffer is currently allocated.
**       - Too many validations have been requested simultaneously.  The operator 
**         must wait for one or more applications to perform their table validation 
**         functions before trying again.
**       - The specified table name was not found in the table registry.
**       
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \TBL_CMDEC - command error counter will increment
**       - Command specific error event message are issued for all error cases
**
**  \par Criticality
**       The success or failure of a table validation does not have any immediate 
**       impact on table contents.  The results are sent to the operator in telemetry 
**       and the operator must determine whether the results are acceptable and send a 
**       command to activate the validated table image.
**
**  \sa  #CFE_TBL_LOAD_CC, #CFE_TBL_DUMP_CC, #CFE_TBL_ACTIVATE_CC, #CFE_TBL_ABORT_LOAD_CC
*/
#define CFE_TBL_VALIDATE_CC     4

/** \cfetblcmd Activate Table
**
**  \par Description
**       This command will cause Table Services to notify a table's owner 
**       that an update is pending.  The owning application will then update 
**       the contents of the active table buffer with the contents of the 
**       associated inactive table buffer at a time of their convenience. 
**
**  \cfecmdmnemonic \TBL_ACTIVATE
**
**  \par Command Structure
**       #CFE_TBL_ActivateCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the 
**       following telemetry:
**       - \b \c \TBL_CMDPC - command execution counter will 
**         increment
**       - The #CFE_TBL_UPDATE_SUCCESS_INF_EID informational event message 
**         will be generated
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - A single buffered table's inactive buffer was requested to be 
**         dumped and no such buffer is currently allocated.
**       - The specified table name was not found in the table registry.
**       
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \TBL_CMDEC - command error counter will increment
**       - Command specific error event message are issued for all 
**         error cases
**
**  \par Criticality
**       This command will cause the contents of the specified table to be 
**       updated with the contents in the inactive table buffer.
**
**  \sa  #CFE_TBL_LOAD_CC, #CFE_TBL_DUMP_CC, #CFE_TBL_VALIDATE_CC, #CFE_TBL_ABORT_LOAD_CC
*/
#define CFE_TBL_ACTIVATE_CC     5

/** \cfetblcmd Dump Table Registry
**
**  \par Description
**       This command will cause Table Services to write some of the 
**       contents of the Table Registry to the command specified file.  
**       This allows the operator to see the current state and configuration 
**       of all tables that have been registered with the cFE.  
**
**  \cfecmdmnemonic \TBL_WRITEREG2FILE
**
**  \par Command Structure
**       #CFE_TBL_DumpRegCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the 
**       following telemetry:
**       - \b \c \TBL_CMDPC -  command execution counter will 
**         increment
**       - The generation of either #CFE_TBL_OVERWRITE_REG_DUMP_INF_EID 
**         or #CFE_TBL_WRITE_REG_DUMP_INF_EID debug event messages
**       - The specified file should appear (or be updated) at the 
**         specified location in the file system
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Error occurred during write operation to file. Possible 
**         causes might be insufficient space in the file system 
**         or the filename or file path is improperly specified.
**       
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \TBL_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**       This command is not inherently dangerous.  It will create a new 
**       file in the file system and could, if performed repeatedly without 
**       sufficient file management by the operator, fill the file system.
**
**  \sa  #CFE_TBL_TLM_REG_CC
*/
#define CFE_TBL_DUMP_REG_CC     6

/** \cfetblcmd Telemeter One Table Registry Entry
**
**  \par Description
**       This command will cause Table Services to telemeter the contents 
**       of the Table Registry for the command specified table.  
**
**  \cfecmdmnemonic \TBL_TLMREG
**
**  \par Command Structure
**       #CFE_TBL_TlmRegCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the 
**       following telemetry:
**       - \b \c \TBL_CMDPC -  command execution counter will increment
**       - Receipt of a Table Registry Info Packet (see #CFE_TBL_TblRegPacket_t)
**       - The #CFE_TBL_TLM_REG_CMD_INF_EID debug event message will 
**         be generated
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The specified table name was not found in the table registry.
**       
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \TBL_CMDEC - command error counter will increment
**       - Error specific event message
**
**  \par Criticality
**       This command is not inherently dangerous.  It will generate 
**       additional telemetry.
**
**  \sa  #CFE_TBL_DUMP_REG_CC
*/
#define CFE_TBL_TLM_REG_CC      7

/** \cfetblcmd Delete Critical Table from Critical Data Store
**
**  \par Description
**       This command will delete the Critical Data Store (CDS) associated
**       with the specified Critical Table.  Note that any table still
**       present in the Table Registry is unable to be deleted from
**       the Critical Data Store.  All Applications that are accessing
**       the critical table must release and unregister their access
**       before the CDS can be deleted.
**
**  \cfecmdmnemonic \TBL_DELETECDS
**
**  \par Command Structure
**       #CFE_TBL_DelCDSCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the 
**       following telemetry:
**       - \b \c \TBL_CMDPC - command execution counter will 
**         increment
**       - The #CFE_TBL_CDS_DELETED_INFO_EID informational event message 
**         will be generated
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The specified table name was not found in the critical data 
**         store registry
**       - The specified table name WAS found in the table registry (all
**         registrations/sharing of the table must be unregistered before
**         the table's CDS can be deleted)
**       - The table's owning application is still active
**       
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \TBL_CMDEC - command error counter will increment
**       - Error specific event message
**
**  \par Criticality
**       This command will cause the loss of the specified table's contents
**       before the owning Application was terminated.
**
**  \sa  #CFE_ES_DUMP_CDS_REG_CC, #CFE_ES_DELETE_CDS_CC
*/
#define CFE_TBL_DELETE_CDS_CC   8

/** \cfetblcmd Abort Table Load
**
**  \par Description
**       This command will cause Table Services to discard the contents of 
**       a table buffer that was previously loaded with the data in a file 
**       as specified by a Table Load command.  For single buffered tables, 
**       the allocated shared working buffer is freed and becomes available 
**       for other Table Load commands. 
**
**  \cfecmdmnemonic \TBL_LOADABORT
**
**  \par Command Structure
**       #CFE_TBL_AbortLdCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the 
**       following telemetry:
**       - \b \c \TBL_CMDPC -  command execution counter will increment
**       - The #CFE_TBL_LOAD_ABORT_INF_EID informational event message
**         is generated
**       - If the load was aborted for a single buffered table, the 
**         \b \c \TBL_NUMFREESHRBUF telemetry point should increment
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The specified table name was not found in the table registry.
**       - The specified table did not have a load in progress to be aborted.
**       
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \TBL_CMDEC - command error counter will increment
**       - Error specific event message
**
**  \par Criticality
**       This command will cause the loss of data put into an inactive table buffer.
**
**  \sa  #CFE_TBL_LOAD_CC, #CFE_TBL_DUMP_CC, #CFE_TBL_VALIDATE_CC, #CFE_TBL_ACTIVATE_CC
*/
#define CFE_TBL_ABORT_LOAD_CC   9
/** \} */

/*
** Validate Command Buffer Selection Parameters
*/
/** \name Validate and Dump Command Buffer Selection Parameters */
/** \{ */
#define CFE_TBL_INACTIVE_BUFFER 0
#define CFE_TBL_ACTIVE_BUFFER   1
/** \} */

/*************************************************************************/

/********************************/
/* Command Message Data Formats */
/********************************/
/**
** \brief Generic "no arguments" command
**
** This command structure is used for commands that do not have any parameters.
** This includes:
** -# The Housekeeping Request Message
** -# The No-Op Command (For details, see #CFE_TBL_NOOP_CC)
** -# The Reset Counters Command (For details, see #CFE_TBL_RESET_CC)
*/
typedef struct
{
    uint8                 CmdHeader[CFE_SB_CMD_HDR_SIZE];   /**< \brief cFE Software Bus Command Message Header */

} CFE_TBL_NoArgsCmd_t;

/**
** \brief Load Table Command
**
** For command details, see #CFE_TBL_LOAD_CC
**
**/
typedef struct
{
    char                  LoadFilename[OS_MAX_PATH_LEN];  /**< \brief Filename (and path) of data to be loaded */
                                                          /**< ASCII Character string containing full path 
                                                               filename for file to be loaded */
} CFE_TBL_LoadCmd_Payload_t;

typedef struct
{
    uint8                       CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE Software Bus Command Message Header */
    CFE_TBL_LoadCmd_Payload_t   Payload;
} CFE_TBL_LoadCmd_t;

/**
** \brief Dump Table Command
**
** For command details, see #CFE_TBL_DUMP_CC
*/
typedef struct
{
    uint16                ActiveTblFlag;                        /**< \brief #CFE_TBL_INACTIVE_BUFFER=Inactive Table, 
                                                                            #CFE_TBL_ACTIVE_BUFFER=Active Table */
                                                                /**< Selects either the "Inactive" 
                                                                     (#CFE_TBL_INACTIVE_BUFFER) buffer or the 
                                                                     "Active" (#CFE_TBL_ACTIVE_BUFFER) buffer 
                                                                     to be dumped */
    char                  TableName[CFE_TBL_MAX_FULL_NAME_LEN]; /**< \brief Full name of table to be dumped */
                                                                /**< ASCII string containing full table name 
                                                                     identifier of table to be dumped */
    char                  DumpFilename[OS_MAX_PATH_LEN];        /**< \brief Full Filename where data is to be written */
                                                                /**< ASCII string containing full path of filename 
                                                                     where data is to be dumped */
} CFE_TBL_DumpCmd_Payload_t;

typedef struct
{
    uint8                       CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE Software Bus Command Message Header */
    CFE_TBL_DumpCmd_Payload_t   Payload;
} CFE_TBL_DumpCmd_t;

/**
** \brief Validate Table Command
**
** For command details, see #CFE_TBL_VALIDATE_CC
*/
typedef struct
{
    uint16                ActiveTblFlag;                         /**< \brief #CFE_TBL_INACTIVE_BUFFER=Inactive Table, 
                                                                             #CFE_TBL_ACTIVE_BUFFER=Active Table */
                                                                 /**< Selects either the "Inactive" 
                                                                      (#CFE_TBL_INACTIVE_BUFFER) buffer or the 
                                                                      "Active" (#CFE_TBL_ACTIVE_BUFFER) buffer 
                                                                      to be validated */
    char                  TableName[CFE_TBL_MAX_FULL_NAME_LEN];  /**< \brief Full Name of Table to be validated */
                                                                 /**< ASCII string containing full table name 
                                                                      identifier of table to be validated */
} CFE_TBL_ValidateCmd_Payload_t;

typedef struct
{
    uint8                           CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE Software Bus Command Message Header */
    CFE_TBL_ValidateCmd_Payload_t   Payload;
} CFE_TBL_ValidateCmd_t;

/**
** \brief Activate Table Command
**
** For command details, see #CFE_TBL_ACTIVATE_CC
*/
typedef struct
{
    char                  TableName[CFE_TBL_MAX_FULL_NAME_LEN];  /**< \brief Full Name of Table to be activated */
                                                                 /**< ASCII string containing full table name 
                                                                      identifier of table to be activated */
} CFE_TBL_ActivateCmd_Payload_t;

typedef struct
{
    uint8                           CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE Software Bus Command Message Header */
    CFE_TBL_ActivateCmd_Payload_t   Payload;
} CFE_TBL_ActivateCmd_t;

/**
** \brief Dump Registry Command
**
** For command details, see #CFE_TBL_DUMP_REG_CC
*/
typedef struct
{
    char                  DumpFilename[OS_MAX_PATH_LEN];        /**< \brief Full Filename where dumped data is to be 
                                                                     written */
                                                                /**< ASCII string containing full path of filename 
                                                                     where registry is to be dumped */
} CFE_TBL_DumpRegCmd_Payload_t;

typedef struct
{
    uint8                           CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE Software Bus Command Message Header */
    CFE_TBL_DumpRegCmd_Payload_t    Payload;
} CFE_TBL_DumpRegCmd_t;

/**
** \brief Telemeter Table Registry Entry Command
**
** For command details, see #CFE_TBL_TLM_REG_CC
*/
typedef struct
{
    char                  TableName[CFE_TBL_MAX_FULL_NAME_LEN];  /**< \brief Full Name of Table whose registry entry 
                                                                      is to be telemetered */
                                                                 /**< ASCII string containing full table name 
                                                                      identifier of table whose registry entry is
                                                                      to be telemetered via #CFE_TBL_TblRegPacket_t */
} CFE_TBL_TlmRegCmd_Payload_t;

typedef struct
{
    uint8                       CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE Software Bus Command Message Header */
    CFE_TBL_TlmRegCmd_Payload_t Payload;
} CFE_TBL_TlmRegCmd_t;

/**
** \brief Delete Critical Table CDS Command
**
** For command details, see #CFE_TBL_DELETE_CDS_CC
*/
typedef struct
{
    char                  TableName[CFE_TBL_MAX_FULL_NAME_LEN];  /**< \brief Full Name of Table whose CDS is 
                                                                      to be deleted */
                                                                 /**< ASCII string containing full table name 
                                                                      identifier of a critical table whose 
                                                                      CDS is to be deleted */
} CFE_TBL_DelCDSCmd_Payload_t;

typedef struct
{
    uint8                       CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE Software Bus Command Message Header */
    CFE_TBL_DelCDSCmd_Payload_t Payload;
} CFE_TBL_DelCDSCmd_t;

/**
** \brief Abort Load Command
**
** For command details, see #CFE_TBL_ABORT_LOAD_CC
*/
typedef struct
{
    char                  TableName[CFE_TBL_MAX_FULL_NAME_LEN];  /**< \brief Full Name of Table whose load is to be aborted */
                                                                 /**< ASCII string containing full table name 
                                                                      identifier of a table whose load is to be aborted */
} CFE_TBL_AbortLdCmd_Payload_t;

typedef struct
{
    uint8                           CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE Software Bus Command Message Header */
    CFE_TBL_AbortLdCmd_Payload_t    Payload;
} CFE_TBL_AbortLdCmd_t;


/*************************************************************************/
/******************************************/
/* Generated Command Message Data Formats */
/******************************************/
/**
** \brief Table Management Notification Message
**
** \par Description
**      Whenever an application that owns a table calls the #CFE_TBL_NotifyByMessage API
**      following the table registration, Table services will generate the following
**      command message with the application specified message ID, command code and
**      parameter whenever the table requires management (e.g. - loads and validations).
*/
typedef struct
{
    uint32                Parameter;                             /**< \brief Application specified command parameter */
} CFE_TBL_NotifyCmd_Payload_t;

typedef struct
{
    uint8                       CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE Software Bus Command Message Header */
    CFE_TBL_NotifyCmd_Payload_t Payload;
} CFE_TBL_NotifyCmd_t;

/*************************************************************************/
/**********************************/
/* Telemetry Message Data Formats */
/**********************************/
/** 
**  \cfetbltlm Table Services Housekeeping Packet
**/
typedef struct
{
    /*
    ** Task command interface counters...
    */
    uint8                 CmdCounter;                           /**< \cfetlmmnemonic \TBL_CMDPC 
                                                                     \brief Count of valid commands received */
    uint8                 ErrCounter;                           /**< \cfetlmmnemonic \TBL_CMDEC 
                                                                     \brief Count of invalid commands received */

    /*
    ** Table Registry Statistics
    */
    uint16                NumTables;                            /**< \cfetlmmnemonic \TBL_NUMTABLES 
                                                                     \brief Number of Tables Registered */
    uint16                NumLoadPending;                       /**< \cfetlmmnemonic \TBL_NUMUPDATESPEND 
                                                                     \brief Number of Tables pending on Applications for their update */

    /*
    ** Last Table Validation Results
    */
    uint16                ValidationCtr;                        /**< \cfetlmmnemonic \TBL_VALCOMPLTDCTR 
                                                                     \brief Number of completed table validations */
    uint32                LastValCrc;                           /**< \cfetlmmnemonic \TBL_LASTVALCRC 
                                                                     \brief Data Integrity Value computed for last table validated */
    int32                 LastValStatus;                        /**< \cfetlmmnemonic \TBL_LASTVALS 
                                                                     \brief Returned status from validation function for last table validated */
    boolean               ActiveBuffer;                         /**< \cfetlmmnemonic \TBL_LASTVALBUF 
                                                                     \brief Indicator of whether table buffer validated was 0=Inactive, 1=Active */ 
    char                  LastValTableName[CFE_TBL_MAX_FULL_NAME_LEN]; /**< \cfetlmmnemonic \TBL_LASTVALTBLNAME 
                                                                            \brief Name of last table validated */
    uint8                 SuccessValCtr;                        /**< \cfetlmmnemonic \TBL_VALSUCCESSCTR 
                                                                     \brief Total number of successful table validations */
    uint8                 FailedValCtr;                         /**< \cfetlmmnemonic \TBL_VALFAILEDCTR 
                                                                     \brief Total number of unsuccessful table validations */
    uint8                 NumValRequests;                       /**< \cfetlmmnemonic \TBL_VALREQCTR 
                                                                     \brief Number of times Table Services has requested validations from Apps */
    
    /*
    ** Ground system interface information
    */
    uint8                 NumFreeSharedBufs;                    /**< \cfetlmmnemonic \TBL_NUMFREESHRBUF 
                                                                     \brief Number of free Shared Working Buffers */
    uint8                 ByteAlignPad1;                        /**< \cfetlmmnemonic \TBL_BYTEALIGNPAD1 
                                                                     \brief Spare byte to ensure longword alignment */
    CFE_ES_MemHandle_t    MemPoolHandle;                        /**< \cfetlmmnemonic \TBL_MEMPOOLHANDLE 
                                                                     \brief Handle to TBL's memory pool */
    CFE_TIME_SysTime_t    LastUpdateTime;                       /**< \cfetlmmnemonic \TBL_LASTUPDTIME 
                                                                     \brief Time of last table update */
    char                  LastUpdatedTbl[CFE_TBL_MAX_FULL_NAME_LEN]; /**< \cfetlmmnemonic \TBL_LASTUPDTBLNAME 
                                                                          \brief Name of the last table updated */
    char                  LastFileLoaded[OS_MAX_PATH_LEN];      /**< \cfetlmmnemonic \TBL_LASTFILELOADED 
                                                                     \brief Path and Name of last table image file loaded */
    char                  LastFileDumped[OS_MAX_PATH_LEN];      /**< \cfetlmmnemonic \TBL_LASTFILEDUMPED 
                                                                     \brief Path and Name of last file dumped to */
    char                  LastTableLoaded[CFE_TBL_MAX_FULL_NAME_LEN]; /**< \cfetlmmnemonic \TBL_LASTTABLELOADED 
                                                                          \brief Name of the last table loaded */
} CFE_TBL_HkPacket_Payload_t;

typedef struct
{
    uint8                       TlmHeader[CFE_SB_TLM_HDR_SIZE];       /**< \brief cFE Software Bus Telemetry Message Header */
    CFE_TBL_HkPacket_Payload_t  Payload;
} CFE_TBL_HkPacket_t;


/** 
**  \cfetbltlm Table Registry Info Packet
**/
typedef struct
{
    uint32                      Size;                           /**< \cfetlmmnemonic \TBL_SIZE 
                                                                     \brief Size, in bytes, of Table */
    uint32                      Crc;                            /**< \cfetlmmnemonic \TBL_CRC 
                                                                     \brief Most recently calculated CRC of Table */
    cpuaddr                     ActiveBufferAddr;               /**< \cfetlmmnemonic \TBL_ACTBUFADD
                                                                     \brief Address of Active Buffer */
    cpuaddr                     InactiveBufferAddr;             /**< \cfetlmmnemonic \TBL_IACTBUFADD
                                                                     \brief Address of Inactive Buffer */
    cpuaddr                     ValidationFuncPtr;              /**< \cfetlmmnemonic \TBL_VALFUNCPTR
                                                                     \brief Ptr to Owner App's function that validates tbl contents */
    CFE_TIME_SysTime_t          TimeOfLastUpdate;               /**< \cfetlmmnemonic \TBL_TIMELASTUPD 
                                                                     \brief Time when Table was last updated */
    uint32                      FileCreateTimeSecs;             /**< \cfetlmmnemonic \TBL_FILECSECONDS 
                                                                     \brief File creation time from last file loaded into table */
    uint32                      FileCreateTimeSubSecs;          /**< \cfetlmmnemonic \TBL_FILECSUBSECONDS 
                                                                     \brief File creation time from last file loaded into table */
    boolean                     TableLoadedOnce;                /**< \cfetlmmnemonic \TBL_LOADEDONCE 
                                                                     \brief Flag indicating whether table has been loaded once or not */
    boolean                     LoadPending;                    /**< \cfetlmmnemonic \TBL_UPDATEPNDNG 
                                                                     \brief Flag indicating an inactive buffer is ready to be copied */
    boolean                     DumpOnly;                       /**< \cfetlmmnemonic \TBL_DUMPONLY 
                                                                     \brief Flag indicating Table is NOT to be loaded */
    boolean                     DblBuffered;                    /**< \cfetlmmnemonic \TBL_DBLBUFFERED 
                                                                     \brief Flag indicating Table has a dedicated inactive buffer */
    char                        Name[CFE_TBL_MAX_FULL_NAME_LEN];/**< \cfetlmmnemonic \TBL_NAME 
                                                                     \brief Processor specific table name */
    char                        LastFileLoaded[OS_MAX_PATH_LEN];/**< \cfetlmmnemonic \TBL_LASTFILEUPD 
                                                                     \brief Filename of last file loaded into table */
    char                        OwnerAppName[OS_MAX_API_NAME];  /**< \cfetlmmnemonic \TBL_OWNERAPP 
                                                                     \brief Name of owning application */
    boolean                     Critical;                       /**< \cfetlmmnemonic \TBL_CRITICAL
                                                                     \brief Indicates whether table is Critical or not */
    uint8                       ByteAlign4;                     /**< \cfetlmmnemonic \TBL_SPARE4
                                                                     \brief Spare byte to maintain byte alignment */
} CFE_TBL_TblRegPacket_Payload_t;

typedef struct
{
    uint8                           TlmHeader[CFE_SB_TLM_HDR_SIZE];       /**< \brief cFE Software Bus Telemetry Message Header */
    CFE_TBL_TblRegPacket_Payload_t  Payload;
} CFE_TBL_TblRegPacket_t;

/*************************************************************************/

#endif /* _cfe_tbl_msg_ */

/************************/
/*  End of File Comment */
/************************/
