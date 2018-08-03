/************************************************************************
** File:
**   $Id: cf_defs.h 1.17.1.1 2015/03/06 15:30:25EST sstrege Exp  $
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
** $Log: cf_defs.h  $
** Revision 1.17.1.1 2015/03/06 15:30:25EST sstrege 
** Added copyright information
** Revision 1.17 2011/05/17 09:25:06EDT rmcgraw 
** DCR14529:1 Added processing for GiveTake Cmd
** Revision 1.16 2011/03/14 17:30:16EDT rmcgraw 
** DCR14582:2 Change CF_MAX_TRANSID_CHARS in CF_FindActiveTransIdByName from 16 to 20
** Revision 1.15 2010/11/03 14:55:30EDT rmcgraw 
** DCR13220:1 Changed CF_MEM_BLOCK_SIZE_05 from 164 to 184
** Revision 1.14 2010/10/20 16:07:02EDT rmcgraw 
** DCR13054:1 Expanded max event filters at startup from four to eight
** Revision 1.13 2010/10/20 13:54:24EDT rmcgraw 
** DCR12490:1 Changed definition for CF_MAX_OUTGOING_CHUNK_SIZE
** Revision 1.12 2010/08/06 18:45:56EDT rmcgraw 
** Dcr11510:1 Fixed cfg params with buffer sizes
** Revision 1.11 2010/08/04 15:17:40EDT rmcgraw 
** DCR11510:1 Changes prior to release
** Revision 1.10 2010/07/20 14:37:46EDT rmcgraw 
** Dcr11510:1 Remove Downlink buffer references
** Revision 1.9 2010/07/07 17:37:24EDT rmcgraw 
** DCR11510:1 defined new cond codes and status values
** Revision 1.8 2010/06/17 10:09:26EDT rmcgraw 
** DCR11510:1 Change Transaction state values
** Revision 1.7 2010/06/11 16:13:13EDT rmcgraw 
** DCR11510:1 ZeroCopy, Un-hardcoded cmd/tlm input/output pdus
** Revision 1.6 2010/06/01 10:56:00EDT rmcgraw 
** DCR111510:1 CF_MAX_ERR_STRING_CHARS
** Revision 1.5 2010/04/23 15:33:30EDT rmcgraw 
** DCR11510:1 Set Mem pool block size to sizeof CF_QueueEntry_t, 164
** Revision 1.4 2010/04/23 08:39:19EDT rmcgraw 
** Dcr11510:1 Code Review Prep
** Revision 1.3 2010/03/26 15:30:25EDT rmcgraw 
** DCR11510 Various developmental changes
** Revision 1.2 2010/03/12 12:14:33EST rmcgraw 
** DCR11510:1 Initial check-in towards CF Version 1000
** Revision 1.1 2009/11/24 12:48:52EST rmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cf/fsw/src/project.pj
**
*************************************************************************/
#ifndef _cf_defs_h_
#define _cf_defs_h_



/*************************************************************************
** Macro definitions
**************************************************************************/
#define CF_SUCCESS            (0)  /**< \brief CF return code for success */
#define CF_ERROR              (-1) /**< \brief CF return code for general error */
#define CF_BAD_MSG_LENGTH_RC  (-2) /**< \brief CF return code for unexpected cmd length */

#define CF_INVALID              0xFFFFFFFF

#define CF_DONT_CARE            0
#define CF_UNKNOWN              0
#define CF_TRANS_SUCCESS        1
#define CF_TRANS_FAIL           2

#define CF_ENTRY_UNUSED         0
#define CF_ENTRY_IN_USE         1

#define CF_DISABLED             0
#define CF_ENABLED              1

#define CF_FALSE                0
#define CF_TRUE                 1

#define CF_CLOSED               0
#define CF_OPEN                 1

#define CF_FILE_NOT_ACTIVE      0
#define CF_FILE_IS_ACTIVE       1

#define CF_NOT_IN_PROGRESS      0
#define CF_IN_PROGRESS          1

#define CF_UP_ACTIVEQ           0
#define CF_UP_HISTORYQ          1

#define CF_PB_PENDINGQ          0
#define CF_PB_ACTIVEQ           1
#define CF_PB_HISTORYQ          2

#define CF_PENDINGQ             0
#define CF_ACTIVEQ              1
#define CF_HISTORYQ             2

#define CF_ENTRY_UNUSED         0
#define CF_ENTRY_IN_USE         1

#define CF_NOT_ISSUED           0
#define CF_WAS_ISSUED           1

#define CF_DELETE_FILE          0
#define CF_KEEP_FILE            1

#define CF_PLAYBACKFILECMD      1
#define CF_PLAYBACKDIRCMD       2
#define CF_POLLDIRECTORY        3

#define CF_ALL                  0
#define CF_UPLINK               1
#define CF_PLAYBACK             2

#define CF_INCOMING             1
#define CF_OUTGOING             2

#define CF_TLM                  0
#define CF_CMD                  1

#define CF_CLASS_1              1
#define CF_CLASS_2              2

#define CF_NUM_UPLINK_QUEUES    2
#define CF_QUEUES_PER_CHAN      3

#define CF_STAT_UNKNOWN         0
#define CF_STAT_SUCCESS         1
#define CF_STAT_CANCELLED       2
#define CF_STAT_ABANDON         3
#define CF_STAT_NO_META         4
#define CF_STAT_PENDING         5
#define CF_STAT_ALRDY_ACTIVE    6
#define CF_STAT_PUT_REQ_ISSUED  7  
#define CF_STAT_PUT_REQ_FAIL    8
#define CF_STAT_ACTIVE          9

#define CF_MAX_TRANSID_CHARS    20 /* 255.255_9999999 */
#define CF_MAX_CFG_VALUE_CHARS  16
#define CF_MAX_ERR_STRING_CHARS 32
#define CF_MAX_CFG_PARAM_CHARS  32
#define CF_MAX_EVENT_FILTERS     8

#define CF_GIVE_SEMAPHORE        0
#define CF_TAKE_SEMAPHORE        1

#define CF_MAX_CCSDS_HDR_BYTES  12
#define CF_PDU_HDR_BYTES        12
#define CF_MAX_MEMPOOL_BLK_SIZES 8
#define CF_PDUHDR_FIXED_FIELD_BYTES 4
#define CF_PDUHDR_PDUTYPE_BIT   4
#define CF_PDUHDR_DIRECTION_BIT 3
#define CF_FILEDATA_OFFSET_BYTES 4
#define CF_MAX_OUTGOING_CHUNK_SIZE  (CF_OUTGOING_PDU_BUF_SIZE - \
         (CF_PDU_HDR_BYTES + CF_FILEDATA_OFFSET_BYTES + CF_MAX_CCSDS_HDR_BYTES))


/**
**  \cfeescfg Define CF Memory Pool Block Sizes
**
**  \par Description:
**       CFDP Memory Pool Block Sizes
**
**  \par Limits
**       These sizes MUST be increasing and MUST be an integral multiple of 4.
**       The number of block sizes defined cannot exceed 
**       #CFE_ES_MAX_MEMPOOL_BLOCK_SIZES
*/
#define CF_MEM_BLOCK_SIZE_01              8
#define CF_MEM_BLOCK_SIZE_02             16
#define CF_MEM_BLOCK_SIZE_03             32
#define CF_MEM_BLOCK_SIZE_04             64
#define CF_MEM_BLOCK_SIZE_05            184
#define CF_MEM_BLOCK_SIZE_06            256
#define CF_MEM_BLOCK_SIZE_07            512
#define CF_MAX_BLOCK_SIZE       (CF_MEM_BLOCK_SIZE_07 + CF_MEM_BLOCK_SIZE_07)


#endif      /* _cf_defs_h_ */

/************************/
/*  End of File Comment */
/************************/
