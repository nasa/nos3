/************************************************************************
** File:
**   $Id: cf_verify.h 1.11.1.1 2015/03/06 15:30:44EST sstrege Exp  $
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
**  Define the CFS CF Application compile-time checks
**
** Notes:
**
** $Log: cf_verify.h  $
** Revision 1.11.1.1 2015/03/06 15:30:44EST sstrege 
** Added copyright information
** Revision 1.11 2011/05/19 15:32:06EDT rmcgraw 
** DCR15033:1 Add auto suspend processing
** Revision 1.10 2011/05/19 13:15:14EDT rmcgraw 
** DCR14532:1 Let user select fix or variable size outgoing PDU pkts
** Revision 1.9 2011/05/13 14:59:27EDT rmcgraw 
** DCR13439:1 Added platform config param CF_STARTUP_SYNC_TIMEOUT
** Revision 1.8 2010/10/25 11:21:52EDT rmcgraw 
** DCR12573:1 Changes to allow more than one incoming PDU MsgId
** Revision 1.7 2010/10/20 10:13:31EDT rmcgraw 
** DCR12982:1 Moved 4th digit in version to platform cfg file
** Revision 1.6 2010/08/06 18:45:58EDT rmcgraw 
** Dcr11510:1 Fixed cfg params with buffer sizes
** Revision 1.5 2010/08/04 15:17:38EDT rmcgraw 
** DCR11510:1 Changes prior to release
** Revision 1.4 2010/07/20 14:37:42EDT rmcgraw 
** Dcr11510:1 Remove Downlink buffer references
** Revision 1.3 2010/07/07 17:42:45EDT rmcgraw 
** DCR11510:1 Changed checks to match new platform cfg name Incoming pdu buf
** Revision 1.2 2010/04/26 10:14:47EDT rmcgraw 
** DCR11510:1 Added compile time checks
** Revision 1.1 2009/11/24 12:48:58EST rmcgraw 
** Initial revision
** Member added to CFS CF project
**
*************************************************************************/
#ifndef _cf_verify_h_
#define _cf_verify_h_


#ifndef CF_PIPE_DEPTH
    #error CF_PIPE_DEPTH must be defined!
#elif (CF_PIPE_DEPTH  <  1)
    #error CF_PIPE_DEPTH cannot be less than 1!
#elif (CF_PIPE_DEPTH  >  CFE_SB_MAX_PIPE_DEPTH)
    #error CF_PIPE_DEPTH cannot be greater than CFE_SB_MAX_PIPE_DEPTH!
#endif

#ifndef CF_PIPE_NAME
    #error CF_PIPE_NAME must be defined!
#endif


#ifndef CF_MAX_SIMULTANEOUS_TRANSACTIONS
    #error CF_MAX_SIMULTANEOUS_TRANSACTIONS must be defined!
#elif (CF_MAX_SIMULTANEOUS_TRANSACTIONS  <  1)
    #error CF_MAX_SIMULTANEOUS_TRANSACTIONS cannot be less than 1!
#endif


#ifndef CF_INCOMING_PDU_BUF_SIZE
    #error CF_INCOMING_PDU_BUF_SIZE must be defined!
#elif (CF_INCOMING_PDU_BUF_SIZE  <  1)
    #error CF_INCOMING_PDU_BUF_SIZE cannot be less than 1!
#elif (CF_INCOMING_PDU_BUF_SIZE  >  65535)
    #error CF_INCOMING_PDU_BUF_SIZE cannot be greater than 64K CCSDS Pkt Size Limit!
#endif
        

#ifndef CF_OUTGOING_PDU_BUF_SIZE
    #error CF_OUTGOING_PDU_BUF_SIZE must be defined!
#elif (CF_OUTGOING_PDU_BUF_SIZE  <  1)
    #error CF_OUTGOING_PDU_BUF_SIZE cannot be less than 1!
#elif (CF_OUTGOING_PDU_BUF_SIZE  <  CF_INCOMING_PDU_BUF_SIZE)
    #error CF_OUTGOING_PDU_BUF_SIZE cannot be less than CF_INCOMING_PDU_BUF_SIZE!
#elif (CF_OUTGOING_PDU_BUF_SIZE  >  65535)
    #error CF_OUTGOING_PDU_BUF_SIZE cannot be greater than 64K CCSDS Pkt Size Limit!    
#endif


#ifndef CF_ENGINE_TEMP_FILE_PREFIX
    #error CF_ENGINE_TEMP_FILE_PREFIX must be defined!
#endif


#ifndef CF_CONFIG_TABLE_NAME
    #error CF_CONFIG_TABLE_NAME must be defined!
#endif


#ifndef CF_CONFIG_TABLE_FILENAME
    #error CF_CONFIG_TABLE_FILENAME must be defined!
#endif


#ifndef CF_NUM_INPUT_CHANNELS
    #error CF_NUM_INPUT_CHANNELS must be defined!
#elif (CF_NUM_INPUT_CHANNELS  <  1)
    #error CF_NUM_INPUT_CHANNELS cannot be less than 1!
#elif (CF_NUM_INPUT_CHANNELS  >  255)
    #error CF_NUM_INPUT_CHANNELS cannot be greater than 255!
#endif


#ifndef CF_MAX_PLAYBACK_CHANNELS
    #error CF_MAX_PLAYBACK_CHANNELS must be defined!
#elif (CF_MAX_PLAYBACK_CHANNELS  <  1)
    #error CF_MAX_PLAYBACK_CHANNELS cannot be less than 1!
#elif (CF_MAX_PLAYBACK_CHANNELS  >  255)
    #error CF_MAX_PLAYBACK_CHANNELS cannot be greater than 255!
#endif


#ifndef CF_MAX_POLLING_DIRS_PER_CHAN
    #error CF_MAX_POLLING_DIRS_PER_CHAN must be defined!
#elif (CF_MAX_POLLING_DIRS_PER_CHAN  <  1)
    #error CF_MAX_POLLING_DIRS_PER_CHAN cannot be less than 1!
#elif (CF_MAX_POLLING_DIRS_PER_CHAN  >  255)
    #error CF_MAX_POLLING_DIRS_PER_CHAN cannot be greater than 255!
#endif


#ifndef CF_MEMORY_POOL_BYTES
    #error CF_MEMORY_POOL_BYTES must be defined!
#elif (CF_MEMORY_POOL_BYTES  <  256)
    #error CF_MEMORY_POOL_BYTES cannot be less than 256!
#elif (CF_MEMORY_POOL_BYTES  >  4294967296)
    #error CF_MEMORY_POOL_BYTES cannot be greater than 4294967296!
#elif ((CF_MEMORY_POOL_BYTES % 4) != 0)
    #error CF_MEMORY_POOL_BYTES must be a multiple of 4!
#endif


#ifndef CF_DEFAULT_QUEUE_INFO_FILENAME
    #error CF_DEFAULT_QUEUE_INFO_FILENAME must be defined!
#endif


#if CF_STARTUP_SYNC_TIMEOUT < 0
    #error CF_STARTUP_SYNC_TIMEOUT can not be less than 0
#elif CF_STARTUP_SYNC_TIMEOUT > 4294967295
    #error CF_STARTUP_SYNC_TIMEOUT can not exceed 4294967295
#endif


#if CF_SEND_FIXED_SIZE_PKTS < 0
    #error CF_SEND_FIXED_SIZE_PKTS can not be less than 0
#elif CF_SEND_FIXED_SIZE_PKTS > CFE_SB_MAX_SB_MSG_SIZE
    #error CF_SEND_FIXED_SIZE_PKTS can not exceed CFE_SB_MAX_SB_MSG_SIZE
#endif


#if CF_AUTOSUSPEND_MAX_TRANS < 1
    #error CF_AUTOSUSPEND_MAX_TRANS can not be less than 1
#elif CF_AUTOSUSPEND_MAX_TRANS > 4294967295
    #error CF_AUTOSUSPEND_MAX_TRANS can not exceed 4294967295
#endif


#ifndef CF_MISSION_REV
    #error CF_MISSION_REV must be defined!
#elif (CF_MISSION_REV < 0)
    #error CF_MISSION_REV must be greater than or equal to zero!
#endif


#endif /* _cf_verify_h_ */

/************************/
/*  End of File Comment */
/************************/
