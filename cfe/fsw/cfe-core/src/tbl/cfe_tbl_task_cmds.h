/*
** $Id: cfe_tbl_task_cmds.h 1.4 2012/01/13 12:17:40GMT-05:00 acudmore Exp  $
**
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement.
**  
**
** Subsystem: cFE TBL Task Command Handler Interface Definition File
**
** Author: David Kobe (the Hammers Company, Inc.)
**
** Notes:
**
** $Log: cfe_tbl_task_cmds.h  $
** Revision 1.4 2012/01/13 12:17:40GMT-05:00 acudmore 
** Changed license text to reflect open source
** Revision 1.3 2010/10/25 15:00:34EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.2 2010/10/04 15:18:55EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.1 2008/04/17 08:05:37EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/tbl/project.pj
** Revision 1.6 2007/05/04 15:56:22EDT David Kobe (dlkobe) 
** Added Delete CDS command
** Revision 1.5 2006/10/31 12:21:44GMT-05:00 dlkobe 
** Removed all references to the TBL_MOD_DUMP_ONLY option command
** Revision 1.4 2006/06/08 13:25:57GMT-05:00 dlkobe 
** Added legal statement to TBL service source and header files
** Revision 1.3 2006/04/12 14:44:53GMT-05:00 dlkobe 
** Updated comments to conform to doxygen formatting
** Revision 1.2 2005/11/16 16:04:00GMT-05:00 dlkobe 
** Added Abort Load Command and fixed partial table loads
** Revision 1.1 2005/11/10 15:35:42GMT-05:00 dlkobe
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-REPOSITORY/cfe-core/tbl/project.pj
**
*/

#ifndef _cfe_tbl_task_cmds_
#define _cfe_tbl_task_cmds_

/*
** Required header files
*/
#include "cfe.h"

/*********************  Macro and Constant Type Definitions   ***************************/

typedef enum
{
    CFE_TBL_INC_ERR_CTR=-1,   /**< Error detected in (or while processing) message, increment command error counter */
    CFE_TBL_DONT_INC_CTR=0,   /**< No errors detected but don't increment command counter */
    CFE_TBL_INC_CMD_CTR=1     /**< No errors detected and increment command counter */
} CFE_TBL_CmdProcRet_t;

typedef CFE_TBL_CmdProcRet_t (*CFE_TBL_MsgProcFuncPtr_t)(const CFE_SB_MsgPayloadPtr_t Payload);

#define CFE_TBL_BAD_CMD_CODE  (-1) /**< Command Code found in Message does not match any in #CFE_TBL_CmdHandlerTbl */
#define CFE_TBL_BAD_MSG_ID    (-2) /**< Message ID found in Message does not match any in #CFE_TBL_CmdHandlerTbl */

/*
** Table task const data
*/

typedef enum
{
    CFE_TBL_TERM_MSGTYPE=0,   /**< \brief Command Handler Table Terminator Type */
    CFE_TBL_MSG_MSGTYPE,      /**< \brief Message Type (requires Message ID match) */
    CFE_TBL_CMD_MSGTYPE       /**< \brief Command Type (requires Message ID and Command Code match) */
} CFE_TBL_MsgType_t;

/**
** Data structure of a single record in #CFE_TBL_CmdHandlerTbl
*/
typedef struct {
    uint32                   MsgId;           /**< \brief Acceptable Message ID */
    uint32                   CmdCode;         /**< \brief Acceptable Command Code (if necessary) */
    uint32                   ExpectedLength;  /**< \brief Expected Message Length (in bytes) including message header */
    CFE_TBL_MsgProcFuncPtr_t MsgProcFuncPtr;  /**< \brief Pointer to function to handle message  */
    CFE_TBL_MsgType_t        MsgTypes;        /**< \brief Message Type (i.e. - with/without Cmd Code)   */
} CFE_TBL_CmdHandlerTblRec_t;


/* Command Message Processing Functions */
/*****************************************************************************/
/**
** \brief Process Housekeeping Request Message
**
** \par Description
**        Constructs a Housekeeping Packet (#CFE_TBL_HkPacket_t) from task data and sends it out
** 
** \par Assumptions, External Events, and Notes:
**          The message pointed to by MessagePtr has been identified as a Housekeeping Request Message
**
** \param[in] MessagePtr points to the message received via command pipe that needs processing
**
** \retval #CFE_TBL_DONT_INC_CTR \copydoc CFE_TBL_DONT_INC_CTR
******************************************************************************/
extern CFE_TBL_CmdProcRet_t CFE_TBL_HousekeepingCmd(const CFE_SB_MsgPayloadPtr_t MessagePtr);

/*****************************************************************************/
/**
** \brief Process NO OP Command Message
**
** \par Description
**        Responds to the NOOP command by issuing an Event Message
** 
** \par Assumptions, External Events, and Notes:
**          The message pointed to by MessagePtr has been identified as a NO OP Command Message
**
** \param[in] MessagePtr points to the message received via command pipe that needs processing
**
** \retval #CFE_TBL_INC_ERR_CTR  \copydoc CFE_TBL_INC_ERR_CTR
** \retval #CFE_TBL_INC_CMD_CTR  \copydoc CFE_TBL_INC_CMD_CTR
******************************************************************************/
extern CFE_TBL_CmdProcRet_t CFE_TBL_NoopCmd(const CFE_SB_MsgPayloadPtr_t MessagePtr);

/*****************************************************************************/
/**
** \brief Process Reset Counters Command Message
**
** \par Description
**        Resets command counters and validation request counters
** 
** \par Assumptions, External Events, and Notes:
**          The message pointed to by MessagePtr has been identified as a Reset Counters Command Message
**
** \param[in] MessagePtr points to the message received via command pipe that needs processing
**
** \retval #CFE_TBL_DONT_INC_CTR \copydoc CFE_TBL_DONT_INC_CTR
******************************************************************************/
extern CFE_TBL_CmdProcRet_t CFE_TBL_ResetCmd(const CFE_SB_MsgPayloadPtr_t MessagePtr);

/*****************************************************************************/
/**
** \brief Process Load Table Command Message
**
** \par Description
**        Locates the file specified in the command message and loads the contents of the file into
**        a buffer that is associated with the table specified within the file header.
** 
** \par Assumptions, External Events, and Notes:
**          The message pointed to by MessagePtr has been identified as a Load Table Command Message
**
** \param[in] MessagePtr points to the message received via command pipe that needs processing
**
** \retval #CFE_TBL_INC_ERR_CTR  \copydoc CFE_TBL_INC_ERR_CTR
** \retval #CFE_TBL_INC_CMD_CTR  \copydoc CFE_TBL_INC_CMD_CTR
******************************************************************************/
extern CFE_TBL_CmdProcRet_t CFE_TBL_LoadCmd(const CFE_SB_MsgPayloadPtr_t MessagePtr);

/*****************************************************************************/
/**
** \brief Process Dump Table Command Message
**
** \par Description
**        Locates the memory associated with the table identified in the command message and copies
**        the data contents to the command message specified file.
** 
** \par Assumptions, External Events, and Notes:
**          The message pointed to by MessagePtr has been identified as a Dump Table Command Message
**
** \param[in] MessagePtr points to the message received via command pipe that needs processing
**
** \retval #CFE_TBL_INC_ERR_CTR  \copydoc CFE_TBL_INC_ERR_CTR
** \retval #CFE_TBL_INC_CMD_CTR  \copydoc CFE_TBL_INC_CMD_CTR
******************************************************************************/
extern CFE_TBL_CmdProcRet_t CFE_TBL_DumpCmd(const CFE_SB_MsgPayloadPtr_t MessagePtr);

/*****************************************************************************/
/**
** \brief Process Validate Table Command Message
**
** \par Description
**        Computes a Data Integrity Check Value for the command message specified table and notifies
**        the table's parent Application, if it has an associated validation function, that a validation
**        of the buffer's contents is required.
** 
** \par Assumptions, External Events, and Notes:
**          The message pointed to by MessagePtr has been identified as a Validate Table Command Message
**
** \param[in] MessagePtr points to the message received via command pipe that needs processing
**
** \retval #CFE_TBL_INC_ERR_CTR  \copydoc CFE_TBL_INC_ERR_CTR
** \retval #CFE_TBL_INC_CMD_CTR  \copydoc CFE_TBL_INC_CMD_CTR
******************************************************************************/
extern CFE_TBL_CmdProcRet_t CFE_TBL_ValidateCmd(const CFE_SB_MsgPayloadPtr_t MessagePtr);

/*****************************************************************************/
/**
** \brief Process Activate Table Command Message
**
** \par Description
**        Notifies the table's owner Application that a new version of the table is pending and should
**        be used.
** 
** \par Assumptions, External Events, and Notes:
**          The message pointed to by MessagePtr has been identified as an Activate Table Command Message
**
** \param[in] MessagePtr points to the message received via command pipe that needs processing
**
** \retval #CFE_TBL_INC_ERR_CTR  \copydoc CFE_TBL_INC_ERR_CTR
** \retval #CFE_TBL_INC_CMD_CTR  \copydoc CFE_TBL_INC_CMD_CTR
******************************************************************************/
extern CFE_TBL_CmdProcRet_t CFE_TBL_ActivateCmd(const CFE_SB_MsgPayloadPtr_t MessagePtr);

/*****************************************************************************/
/**
** \brief Process Dump Table Registry Command Message
**
** \par Description
**        Copies the contents of the Table Registry to a command message specified file.
** 
** \par Assumptions, External Events, and Notes:
**          The message pointed to by MessagePtr has been identified as a Dump Table Registry Command Message
**
** \param[in] MessagePtr points to the message received via command pipe that needs processing
**
** \retval #CFE_TBL_INC_ERR_CTR  \copydoc CFE_TBL_INC_ERR_CTR
** \retval #CFE_TBL_INC_CMD_CTR  \copydoc CFE_TBL_INC_CMD_CTR
******************************************************************************/
extern CFE_TBL_CmdProcRet_t CFE_TBL_DumpRegCmd(const CFE_SB_MsgPayloadPtr_t MessagePtr);

/*****************************************************************************/
/**
** \brief Process Telemeter Table Registry Entry Command Message
**
** \par Description
**        Extracts the Table Registry information for a command message specified table and puts it into
**        a message that is sent out.
** 
** \par Assumptions, External Events, and Notes:
**          The message pointed to by MessagePtr has been identified as a Telemeter Table Registry Entry Command Message
**
** \param[in] MessagePtr points to the message received via command pipe that needs processing
**
** \retval #CFE_TBL_INC_ERR_CTR  \copydoc CFE_TBL_INC_ERR_CTR
** \retval #CFE_TBL_INC_CMD_CTR  \copydoc CFE_TBL_INC_CMD_CTR
******************************************************************************/
extern CFE_TBL_CmdProcRet_t CFE_TBL_TlmRegCmd(const CFE_SB_MsgPayloadPtr_t MessagePtr);

/*****************************************************************************/
/**
** \brief Delete Critical Table's CDS Command message
**
** \par Description
**        Deletes a Critical Data Store used to hold a Critical Table's image
** 
** \par Assumptions, External Events, and Notes:
**          The message pointed to by MessagePtr has been identified as a Delete CDS Command Message
**
** \param[in] MessagePtr points to the message received via command pipe that needs processing
**
** \retval #CFE_TBL_INC_ERR_CTR  \copydoc CFE_TBL_INC_ERR_CTR
** \retval #CFE_TBL_INC_CMD_CTR  \copydoc CFE_TBL_INC_CMD_CTR
******************************************************************************/
extern CFE_TBL_CmdProcRet_t CFE_TBL_DeleteCDSCmd(const CFE_SB_MsgPayloadPtr_t MessagePtr);

/*****************************************************************************/
/**
** \brief Process Abort Load Command message
**
** \par Description
**        Frees any resources associated with a previously loaded table.
** 
** \par Assumptions, External Events, and Notes:
**          The message pointed to by MessagePtr has been identified as an Abort Load Command Message
**
** \param[in] MessagePtr points to the message received via command pipe that needs processing
**
** \retval #CFE_TBL_INC_ERR_CTR  \copydoc CFE_TBL_INC_ERR_CTR
** \retval #CFE_TBL_INC_CMD_CTR  \copydoc CFE_TBL_INC_CMD_CTR
******************************************************************************/
extern CFE_TBL_CmdProcRet_t CFE_TBL_AbortLoadCmd(const CFE_SB_MsgPayloadPtr_t MessagePtr);


/*****************************************************************************/
/**
** \brief Output block of data to file with standard cFE Table Image Headers
**
** \par Description
**        Writes the specified block of data in memory to the specified file
**        with the standard cFE File and cFE Table Image Headers.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] DumpFilename    Character string containing the full path of the file
**                            to which the contents of the table are to be written
**
** \param[in] TableName       Name of table being dumped to a file
**
** \param[in] DumpDataAddr    Address of data buffer whose contents are to be written
**                            to the specified file
**
** \param[in] TblSizeInBytes  Size of block of data to be written to the file
**
** \retval #CFE_TBL_INC_ERR_CTR  \copydoc CFE_TBL_INC_ERR_CTR
** \retval #CFE_TBL_INC_CMD_CTR  \copydoc CFE_TBL_INC_CMD_CTR
******************************************************************************/
extern CFE_TBL_CmdProcRet_t CFE_TBL_DumpToFile( const char *DumpFilename, const char *TableName,
                                         void *DumpDataAddr, uint32 TblSizeInBytes);


/*****************************************************************************/
/**
** \brief Compares message with #CFE_TBL_CmdHandlerTbl to identify the message
**
** \par Description
**          Searches the Command Handler Table for an entry matching the
**          message ID and, if necessary, the Command Code.  If an entry
**          is not located, an error code is returned.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] MessageID message ID of command message received on command pipe
**
** \param[in] CommandCode command code from command message received on command pipe
**
** \retval #CFE_SUCCESS          \copydoc CFE_SUCCESS
** \retval #CFE_TBL_BAD_CMD_CODE \copydoc CFE_TBL_BAD_CMD_CODE
** \retval #CFE_TBL_BAD_MSG_ID   \copydoc CFE_TBL_BAD_MSG_ID
**
******************************************************************************/

extern int16 CFE_TBL_SearchCmdHndlrTbl(CFE_SB_MsgId_t MessageID, uint16 CommandCode);

/*****************************************************************************/
/**
** \brief Aborts load by freeing associated inactive buffers and sending event message
**
** \par Description
**        This function aborts the load for the table whose registry entry is identified
**        by the registry record pointer given as an argument.  Aborting the load consists
**        of freeing any associated inactive buffer and issuing an event message.
**
** \par Assumptions, External Events, and Notes:
**        The given registry record pointer is assumed to be valid.
**
** \param[in] RegRecPtr   Pointer to registry record entry for the table whose load is to be aborted
**
** \return None
******************************************************************************/
void CFE_TBL_AbortLoad(CFE_TBL_RegistryRec_t *RegRecPtr);



#endif  /* _cfe_tbl_task_cmds_ */
