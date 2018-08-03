/************************************************************************
** File:
**   $Id: cf_tbldefs.h 1.7.1.1 2015/03/06 15:30:36EST sstrege Exp  $
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
**  The CFS CFDP (CF) Application header file
**
** Notes:
**
** $Log: cf_tbldefs.h  $
** Revision 1.7.1.1 2015/03/06 15:30:36EST sstrege 
** Added copyright information
** Revision 1.7 2011/05/09 11:52:15EDT rmcgraw 
** DCR13317:1 Allow Destintaion path to be blank
** Revision 1.6 2010/11/01 16:09:32EDT rmcgraw 
** DCR12802:1 Changes for decoupling peer entity id from channel
** Revision 1.5 2010/10/25 11:21:53EDT rmcgraw 
** DCR12573:1 Changes to allow more than one incoming PDU MsgId
** Revision 1.4 2010/07/20 14:37:44EDT rmcgraw 
** Dcr11510:1 Remove Downlink buffer references
** Revision 1.3 2010/07/07 17:40:16EDT rmcgraw 
** DCR11510:1 Removed TmpPath
** Revision 1.2 2010/03/12 12:14:38EST rmcgraw 
** DCR11510:1 Initial check-in towards CF Version 1000
** Revision 1.1 2009/11/24 12:48:55EST rmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cf/fsw/src/project.pj
**
*************************************************************************/
#ifndef _cf_tbldefs_h_
#define _cf_tbldefs_h_


/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "cf_defs.h"
#include "cf_platform_cfg.h"


/*************************************************************************
** Type definitions
**************************************************************************/

/**  \brief CF Polling Directory Entry Format 
*/

typedef struct
{
    uint8   EntryInUse;
    uint8   EnableState;    
    uint8   Class;
    uint8   Priority;
    uint8   Preserve;
    uint8   Spare1;
    uint8   Spare2;
    uint8   Spare3;
    char    PeerEntityId[CF_MAX_CFG_VALUE_CHARS];/* 2 byte dotted-decimal string eg. "0.24"*/    
    char    SrcPath[OS_MAX_PATH_LEN];/* no spaces,fwd slash at end */
    char    DstPath[OS_MAX_PATH_LEN];/* no spaces */
    
} cf_polling_dir_entry_t;


/**  \brief CF Input Channel Entry Format 
*/

typedef struct
{ 
    uint16  IncomingPDUMsgId;
    uint8   OutChanForClass2Response;
    uint8   Spare;

} cf_in_channel_entry_t;


/**  \brief CF Output Channel Entry Format 
*/

typedef struct
{
    uint8   EntryInUse;
    uint8   DequeueEnable; 
    uint16  OutgoingPduMsgId;/**< \brief MsgId of the output PDU packet */
    uint32  PendingQDepth; /**< \brief Pending Queue Depth */      
    uint32  HistoryQDepth; /**< \brief History Queue Depth */      
    char    ChanName[OS_MAX_API_NAME]; /**< \brief Playback Channel Name */
    char    SemName[OS_MAX_API_NAME];/**< \brief Handshake Semaphore Name */

    cf_polling_dir_entry_t  PollDir[CF_MAX_POLLING_DIRS_PER_CHAN];

} cf_out_channel_entry_t;


typedef struct
{    
    char        TableIdString[OS_MAX_API_NAME];
    uint32      TableVersion;
    uint32      NumEngCyclesPerWakeup;
    uint32      NumWakeupsPerQueueChk;
    uint32      NumWakeupsPerPollDirChk;
    uint32      UplinkHistoryQDepth;
    uint32      Reserved1;
    uint32      Reserved2;

    char        AckTimeout[CF_MAX_CFG_VALUE_CHARS];
    char        AckLimit[CF_MAX_CFG_VALUE_CHARS];
    char        NakTimeout[CF_MAX_CFG_VALUE_CHARS];
    char        NakLimit[CF_MAX_CFG_VALUE_CHARS];
    char        InactivityTimeout[CF_MAX_CFG_VALUE_CHARS];
    char        OutgoingFileChunkSize[CF_MAX_CFG_VALUE_CHARS];
    char        SaveIncompleteFiles[CF_MAX_CFG_VALUE_CHARS];
    char        FlightEntityId[CF_MAX_CFG_VALUE_CHARS];/* 2 byte dotted-decimal string eg. "0.24"*/
    
    cf_in_channel_entry_t   InCh[CF_NUM_INPUT_CHANNELS];
    cf_out_channel_entry_t  OuCh[CF_MAX_PLAYBACK_CHANNELS];
    
} cf_config_table_t;


#endif      /* _cf_tbldefs_h_ */

/************************/
/*  End of File Comment */
/************************/
