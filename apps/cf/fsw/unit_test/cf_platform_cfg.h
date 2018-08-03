/************************************************************************
** File:
**   $Id: cf_platform_cfg.h 1.3.1.1 2015/03/06 15:30:42EST sstrege Exp  $
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
**  The CFS CF Application platform configuration header file
**
** Notes:
**
** $Log: cf_platform_cfg.h  $
** Revision 1.3.1.1 2015/03/06 15:30:42EST sstrege 
** Added copyright information
** Revision 1.3 2011/05/19 15:32:03EDT rmcgraw 
** DCR15033:1 Add auto suspend processing
** Revision 1.14 2011/05/19 13:15:13EDT rmcgraw 
** DCR14532:1 Let user select fix or variable size outgoing PDU pkts
** Revision 1.13 2011/05/13 14:59:26EDT rmcgraw 
** DCR13439:1 Added platform config param CF_STARTUP_SYNC_TIMEOUT
** Revision 1.12 2010/11/04 11:37:45EDT rmcgraw 
** Dcr13051:1 Wrap OS_printfs in platform cfg CF_DEBUG
** Revision 1.11 2010/10/25 11:21:51EDT rmcgraw 
** DCR12573:1 Changes to allow more than one incoming PDU MsgId
** Revision 1.10 2010/10/20 16:07:01EDT rmcgraw 
** DCR13054:1 Expanded max event filters at startup from four to eight
** Revision 1.9 2010/10/20 10:13:31EDT rmcgraw 
** DCR12982:1 Moved 4th digit in version to platform cfg file
** Revision 1.8 2010/08/06 18:45:56EDT rmcgraw 
** Dcr11510:1 Fixed cfg params with buffer sizes
** Revision 1.7 2010/08/04 15:16:09EDT rmcgraw 
** DCR11510:1 Added Event Filtering
** Revision 1.6 2010/07/20 14:37:40EDT rmcgraw 
** Dcr11510:1 Remove Downlink buffer references
** Revision 1.5 2010/07/07 17:25:12EDT rmcgraw 
** DCR11510:1 Change incoming pdu data buf to incoming pdu buffer and corrected 
**     the comments
** Revision 1.4 2010/04/27 09:06:54EDT rmcgraw 
** DCR11510:1 Comment changes
** Revision 1.3 2010/04/23 08:39:16EDT rmcgraw 
** Dcr11510:1 Code Review Prep
** Revision 1.2 2010/03/12 12:14:39EST rmcgraw 
** DCR11510:1 Initial check-in towards CF Version 1000
** Revision 1.1 2009/11/24 12:47:36EST rmcgraw 
** Initial revision
** Member added to CFS CF project
**
*************************************************************************/
#ifndef _cf_platform_cfg_h_
#define _cf_platform_cfg_h_

/*************************************************************************
** Macro definitions
**************************************************************************/

/**
**  \cfcfg Application Pipe Depth 
**
**  \par Description:
**       Dictates the pipe depth of the cf command pipe.
**
**  \par Limits:
**		 The minimum size of this paramater is 1
**       The maximum size dictated by cFE platform configuration 
**		 parameter is CFE_SB_MAX_PIPE_DEPTH
*/
#define CF_PIPE_DEPTH                       40


/**
**  \cfcfg Application Pipe Name 
**
**  \par Description:
**       Dictates the pipe name of the cf command pipe.
**
**  \par Limits:
**
*/
#define CF_PIPE_NAME                        "CF_CMD_PIPE"


/**
**  \cfcfg Maximum Simulataneous Transactions 
**
**  \par Description:
**      Dictates max number of transactions (uplink and downlink)
**      that can be in progress at any given time.
**
**  \par Limits:
**
*/
#define CF_MAX_SIMULTANEOUS_TRANSACTIONS    100



/**
**  \cfcfg Uplink PDU Data Buffer Size 
**
**  \par Description:
**      This parameter sets the statically allocated size (in bytes) of the
**      incoming PDU buffer. This buffer will be used to hold the pdu hdr and 
**      data portion of the incoming PDUs. Incoming PDUs are enclosed in a CCSDS
**      packet. This parameter should not include the size of the CCSDS pkt hdr.
**
**  \par Limits:
**      Must be greater than or equal to the sum of the ground engine parameter 
**      outgoing-file-chunk-size, pdu hdr size and the 4 bytes of 'offset' in
**      file-data pdus. Upper limit of 64K derived from 16 bit PDU header field 
**      named 'PDU Data Field Length'.
**      This parameter must be less-than or equal-to the outgoing pdu buffer.
**      
**
*/
#define CF_INCOMING_PDU_BUF_SIZE            512



/**
**  \cfcfg Outgoing PDU Data Buffer Size 
**
**  \par Description:
**      This parameter sets the statically allocated size (in bytes) of the
**      outgoing PDU buffer. This buffer will be used to hold the pdu hdr and 
**      data portion of the outgoing PDUs. Outgoing PDUs are enclosed in a CCSDS
**      packet. This parameter should not include the size of the CCSDS pkt hdr.
**
**  \par Limits:
**      This parameter will put an upper limit on the table parameter 
**      'outgoing_file_chunk_size'. The max 'outgoing_file_chunk_size' allowed
**      will be CF_OUTGOING_PDU_BUF_SIZE - (12 + 4) The 12 and 4 are pdu hdr   
**      size and offset field in file-data pdu, respectively.
**      This parameter has an upper limit of 64K derived from 16 bit PDU header 
**      field named 'PDU Data Field Length'.
**      This parameter must be greater-than or equal-to the incoming pdu buffer.
**      
**
*/
#define CF_OUTGOING_PDU_BUF_SIZE            2048


/**
**  \cfcfg Path name and file prefix of the engine temp files
**
**  \par Description:
**      The receiving engine constructs all files in a temporary file. This 
**      parameter specifies the path and base filename of the temporary files.
**      The engine appends a sequence number to this parameter to get a complete
**      filename.
**
**  \par Limits:
**      - The length of this string, including the NULL terminator cannot exceed 
**          the #OS_MAX_PATH_LEN value. 
**      - The last character should not be a slash.
**
*/
#define CF_ENGINE_TEMP_FILE_PREFIX          "/ram/cftmp"

/**
**  \cfcfg Name of the CF Configuration Table 
**
**  \par Description:
**       This parameter defines the name of the CF Configuration Table. 
**
**  \par Limits
**       The length of this string, including the NULL terminator cannot exceed 
**       the #OS_MAX_PATH_LEN value.
*/
#define CF_CONFIG_TABLE_NAME                "ConfigTable"


/**
**  \cfcfg CF Configuration Table Filename
**
**  \par Description:
**       The value of this constant defines the filename of the CF Config Table
**
**  \par Limits
**       The length of this string, including the NULL terminator cannot exceed 
**       the #OS_MAX_PATH_LEN value.
*/
#define CF_CONFIG_TABLE_FILENAME            "/cf/cf_cfgtable.tbl"


/**
**  \cfcfg Number of Input Channels
**
**  \par Description:
**      Defines the number of input channels  
**      defined in the configuration table. Input channels were added to the 
**      design to support class 2 file receives from multiple peers. It is 
**      necessary for the code to know what output channel should be used for 
**      responses (ACK-EOF,NAK, etc) of incoming, class 2 transactions.
**      Each input channel has a dedicated MsgId for incoming PDUs and an output
**      channel for responses of class 2, file-receive transactions.
**
**  \par Limits
**      Lower Limit of 1, Upper limit of 255.
**
*/
#define CF_NUM_INPUT_CHANNELS               1


/**
**  \cfcfg Max Number of Playback Output Channels
**
**  \par Description:
**      Defines the max number of playback output channels that may ever be 
**      defined in the configuration table. Refer to the configuration table for
**      more details about playback output channels.
**
**  \par Limits
**      Lower Limit of 1, Upper limit of 255.
**
**  \par Notes:
**      The CF configuration table must have an entry for this number of 
**      playback channels, but some may be marked as not-in-use. This saves 
**      having to recompile and reload a new CF Application when a playback 
**      channel is added.
*/
#define CF_MAX_PLAYBACK_CHANNELS            2


/**
**  \cfcfg Max Number of Polling(Hot) Directories per Playback Output Channel
**
**  \par Description:
**      Defines the max number of polling directories that may ever be defined
**      in the configuration table. A polling directory is a directory that
**      is periodically checked for playback files. Files found in the polling
**      directory are immediately placed on the playback pending queue for 
**      downlink.
**
**  \par Limits:
**      Lower limit of 1, Upper limit of 255.
**
**  \par Notes:
**      The CF configuration table must have an entry for this number of polling   
**      directories, but some may be marked as not-in-use. This saves having to  
**      recompile and reload a new CF Application when a polling directory is 
**      added.
**
*/
#define CF_MAX_POLLING_DIRS_PER_CHAN        8


/**
**  \cfcfg Number of bytes in the CF Memory Pool 
**
**  \par Description:
**      The CF memory pool contains the memory needed to hold information for 
**      each transaction. The info for each transaction is defined by a
**      CF_QueueEntry_t. The number of CF_QueueEntry_t's needed is based on:
**      
**      UplinkHistoryQDepth + CF_MAX_SIMULTANEOUS_TRANSACTIONS +        
**      ((CF_MAX_PLAYBACK_CHANNELS * (PendingQDepth + HistoryQDepth))
**
**      Lower case variables are defined in config table, upper case params are
**      defined in platform config file (cf_platform_cfg.h)
**
**      See CF Housekeeping page for memory utilization details
**
**
**  \par Limits
**       Lower Limit of 256, Upper limit of 4 Gigabytes
*/
#define CF_MEMORY_POOL_BYTES                32768


/**
**  \cfcfg Default Queue Information Filename
**
**  \par Description:
**       The value of this constant defines the filename used to store the CF
**       queue information.  This filename is used only when no filename is
**       specified in the command.
**
**  \par Limits
**       The length of each string, including the NULL terminator cannot exceed 
**       the OS_MAX_PATH_LEN value.
*/
#define CF_DEFAULT_QUEUE_INFO_FILENAME      "/ram/cf_queue_info.dat"


/**
**  \cfcfg CF Event Filtering
**
**  \par Description:
**       This group of configuration parameters dictates what CF events will be
**       filtered through EVS. The filtering will begin after the CF app 
**       initializes and stay in effect until changed via EVS command.
**       Mark all unused event Id values and mask values to zero
**       eg.    #define CF_FILTERED_EVENT1          0
**              #define CF_FILTER_MASK1             0
**       To filter the event, set the mask value to CFE_EVS_FIRST_ONE_STOP
**       To disable filtering of the event, set mask value to CFE_EVS_NO_FILTER
**
**  \par Limits
**       These parameters have a lower limit of 0 and an upper limit of 65535.
*/

#define CF_FILTERED_EVENT1                  CF_IN_TRANS_START_EID
#define CF_FILTER_MASK1                     CFE_EVS_NO_FILTER

#define CF_FILTERED_EVENT2                  CF_IN_TRANS_OK_EID
#define CF_FILTER_MASK2                     CFE_EVS_NO_FILTER

#define CF_FILTERED_EVENT3                  CF_OUT_TRANS_START_EID
#define CF_FILTER_MASK3                     CFE_EVS_NO_FILTER

#define CF_FILTERED_EVENT4                  CF_OUT_TRANS_OK_EID
#define CF_FILTER_MASK4                     CFE_EVS_NO_FILTER

#define CF_FILTERED_EVENT5                  0
#define CF_FILTER_MASK5                     CFE_EVS_NO_FILTER

#define CF_FILTERED_EVENT6                  0
#define CF_FILTER_MASK6                     CFE_EVS_NO_FILTER

#define CF_FILTERED_EVENT7                  0
#define CF_FILTER_MASK7                     CFE_EVS_NO_FILTER

#define CF_FILTERED_EVENT8                  0
#define CF_FILTER_MASK8                     CFE_EVS_NO_FILTER



/**
**  \cfcfg Time to wait for all apps to be started (in milliseconds)
**
**  \par Description:
**       Dictates the timeout for the #CFE_ES_WaitForStartupSync call that
**       CF uses to ensure that TO or the downlink App has completed it's
**       initialization which includes creating the semaphore needed by CF.
**
**  \par Limits
**       This parameter can't be larger than an unsigned 32 bit
**       integer (4294967295).
**
**       This should be greater than or equal to the Startup Sync timeout for
**       any application in the Application Monitor Table.
*/
#define CF_STARTUP_SYNC_TIMEOUT   65000


/**
**  \cfcfg Use fixed size packets (for outgoing PDUs) or not.
**
**  \par Description:
**       When sending PDUs, CF can be configured to place the PDUs in fixed-size
**       pkts or let the PDU size determine the pkt size. The value defined 
**       must correspond to the CCSDS Total Message size which includes the PDU
**       header, the CCSDS header and data.
**       Set this value to 0 for variable pkt sizes.
**
**  \par Limits
**       This parameter can't be larger than CFE_SB_MAX_SB_MSG_SIZE (typically
**       set to 32K or 64K bytes)
**
**       If non-zero, this should be greater than or equal to the size needed to 
**       hold the largest PDU expected to be sent by the engine (typically a 
**       file data PDU which is derived from the CF table cfg param 
**       "OutgoingFileChunkSize").
*/
#define CF_SEND_FIXED_SIZE_PKTS   0


/**
**  \cfcfg Auto-Suspend, max transactions to suspend
**
**  \par Description:
**       When auto suspend is enabled, after EOF is sent the transaction number
**       is logged in a buffer. The buffer size is defined in this parameter.
**       After the following wakeup cmd is received, cF will check this buffer 
**       for transactions to suspend. They cannot be suspended at the time the
**       EOF is sent because the engine is not designed to re-entrant.
**
**  \par Limits
**       This parameter must be greater than zero and can't be larger than an 
**       unsigned 32 bit integer (4294967295).
*/
#define CF_AUTOSUSPEND_MAX_TRANS  1 




/** \cfcfg Mission specific version number for CF application
**  
**  \par Description:
**       An application version number consists of four parts:
**       major version number, minor version number, revision
**       number and mission specific revision number. The mission
**       specific revision number is defined here and the other
**       parts are defined in "cf_version.h".
**
**  \par Limits:
**       Must be defined as a numeric value that is greater than
**       or equal to zero.
*/
#define CF_MISSION_REV      0


/** \cfcfg Compile-time debug switch for CF application
**  
**  \par Description:
**      CF_DEBUG should NOT be defined under normal conditions. It is to be used 
**      as a safety net during development, when a uart terminal is connected to
**      the processor. When the code is compiled with CF_DEBUG defined, the code 
**      will issue OS_printfs in areas that would otherwise be quiet.
**
**  \par Limits:
**       Must be defined or commented out.
*/
/* #define CF_DEBUG */



#endif /* _cf_platform_cfg_h_ */

/************************/
/*  End of File Comment */
/************************/
