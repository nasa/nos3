/******************************************************************************
** File: cfe_sb_verify.h
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
**
** Purpose:
**      This header file performs compile time checking for SB configuration
**      parameters.
**
** Author:   R.McGraw/SSI
**
** $Log: cfe_sb_verify.h  $
** Revision 1.11 2012/04/25 09:12:51GMT-05:00 rmcgraw 
** DCR10525:1 Increased check on CFE_SB_MAX_SB_MSG_SIZE from 32768 to 65535
** Revision 1.10 2012/01/13 12:15:14EST acudmore 
** Changed license text to reflect open source
** Revision 1.9 2011/09/08 12:33:21EDT aschoeni 
** Reduced BUF_MEMORY_BYTES upper limit to correct range
** Revision 1.8 2011/02/03 15:21:53EST lwalling 
** Added verification for mission config parameter CFE_SB_PACKET_TIME_FORMAT
** Revision 1.7 2010/11/08 14:58:26EST aschoeni 
** added CFE_SB_DEFAULT_MSG_LIMIT
** Revision 1.6 2010/11/04 16:41:54EDT aschoeni 
** Added optional sender information storage
** Revision 1.5 2010/10/25 15:00:52EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.4 2010/10/04 15:21:14EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.3 2009/02/26 17:51:13EST rmcgraw 
** DCR6805:1 Added checks to correspond with SB cfg param upper and lower limits
** Revision 1.2 2009/02/11 14:23:46EST rmcgraw 
** DCR6269:1 Added SB mem pool blk size checks
** Revision 1.1 2008/04/17 08:05:33EDT ruperera 
** Initial revision
** Member added to cfe project on tlserver3
** Revision 1.8 2007/04/19 15:50:38EDT rjmcgraw 
** DCR3052:7 Removed sbp networking items from cfe
** Revision 1.7 2007/03/26 14:36:15EST rjmcgraw 
** Added check on CFE_SB_HIGHEST_VALID_MSGID
** Revision 1.6 2006/12/26 15:47:00EST rjmcgraw 
** Removed reference to SB_INCLUDE_NETWORK
** Revision 1.5 2006/06/12 15:26:06EDT rjmcgraw
** Added legal statement
** Revision 1.4 2005/09/08 09:21:24EDT rmcgraw
** Name change:CFE_SB_MAX_SYS_PKT_SIZE to CFE_SB_MAX_SB_MSG_SIZE
** Revision 1.3 2005/09/07 15:45:42EDT rmcgraw
** Removed dollar signs in comment
**
******************************************************************************/

#ifndef _cfe_sb_verify_
#define _cfe_sb_verify_


#if CFE_SB_MAX_MSG_IDS < 1
    #error CFE_SB_MAX_MSG_IDS cannot be less than 1!
#endif

#if CFE_SB_MAX_MSG_IDS > 1024
    #error CFE_SB_MAX_MSG_IDS cannot be greater than 1024!
#endif

#if CFE_SB_MAX_PIPES < 1
    #error CFE_SB_MAX_PIPES cannot be less than 1!
#endif

#if CFE_SB_MAX_PIPES > 255
    #error CFE_SB_MAX_PIPES cannot be greater than 255!
#endif

#if CFE_SB_MAX_PIPES > OS_MAX_QUEUES
    #error CFE_SB_MAX_PIPES cannot be greater than OS_MAX_QUEUES!
#endif

#if CFE_SB_MAX_DEST_PER_PKT < 1
    #error CFE_SB_MAX_DEST_PER_PKT cannot be less than 1!
#endif

#if CFE_SB_MAX_DEST_PER_PKT > 64
    #error CFE_SB_MAX_DEST_PER_PKT cannot be greater than 64!
#endif


#if CFE_SB_MAX_PIPE_DEPTH < 1
    #error CFE_SB_MAX_PIPE_DEPTH cannot be less than 1!
#endif

#if CFE_SB_MAX_PIPE_DEPTH > 65535
    #error CFE_SB_MAX_PIPE_DEPTH cannot be set greater than 65535!
#endif

#if CFE_SB_HIGHEST_VALID_MSGID < 1
  #error CFE_SB_HIGHEST_VALID_MSGID cannot be less than 1!
#endif

#if CFE_SB_HIGHEST_VALID_MSGID > 0xFFFF
  #error CFE_SB_HIGHEST_VALID_MSGID cannot be greater than 0xFFFF!
#endif

#if CFE_SB_BUF_MEMORY_BYTES < 512
    #error CFE_SB_BUF_MEMORY_BYTES cannot be less than 512 bytes!
#endif

#if CFE_SB_BUF_MEMORY_BYTES > 4294967295
    #error CFE_SB_BUF_MEMORY_BYTES cannot be greater than 4294967295 bytes!
#endif

#ifndef MESSAGE_FORMAT_IS_CCSDS
    #error There is no message format (MESSAGE_FORMAT_IS_CCSDS) specified!
#endif

#ifndef CFE_SB_PACKET_TIME_FORMAT
    #error CFE_SB_PACKET_TIME_FORMAT must be defined!
#elif ((CFE_SB_PACKET_TIME_FORMAT != CFE_SB_TIME_32_16_SUBS) && \
       (CFE_SB_PACKET_TIME_FORMAT != CFE_SB_TIME_32_32_SUBS) && \
       (CFE_SB_PACKET_TIME_FORMAT != CFE_SB_TIME_32_32_M_20))
    #error CFE_SB_PACKET_TIME_FORMAT must be CFE_SB_TIME_32_16_SUBS or CFE_SB_TIME_32_32_SUBS or CFE_SB_TIME_32_32_M_20!
#endif

#if CFE_SB_MAX_SB_MSG_SIZE < 6
     #error CFE_SB_MAX_SB_MSG_SIZE cannot be less than 6 (CCSDS Primary Hdr Size)!
#endif
     
#if CFE_SB_MAX_SB_MSG_SIZE > 65535
    #error CFE_SB_MAX_SB_MSG_SIZE cannot be set greater than 65535!
#endif


/*
**  SB Memory Pool Block Sizes
*/
#if CFE_SB_MAX_BLOCK_SIZE < CFE_SB_MAX_SB_MSG_SIZE
    #error CFE_SB_MAX_BLOCK_SIZE must be > or = to CFE_SB_MAX_SB_MSG_SIZE!
#endif

#if CFE_SB_MEM_BLOCK_SIZE_01 > CFE_SB_MEM_BLOCK_SIZE_02
    #error CFE_SB_MEM_BLOCK_SIZE_01 must be less than CFE_SB_MEM_BLOCK_SIZE_02
#endif

#if CFE_SB_MEM_BLOCK_SIZE_02 > CFE_SB_MEM_BLOCK_SIZE_03
    #error CFE_SB_MEM_BLOCK_SIZE_02 must be less than CFE_SB_MEM_BLOCK_SIZE_03
#endif

#if CFE_SB_MEM_BLOCK_SIZE_03 > CFE_SB_MEM_BLOCK_SIZE_04
    #error CFE_SB_MEM_BLOCK_SIZE_03 must be less than CFE_SB_MEM_BLOCK_SIZE_04
#endif

#if CFE_SB_MEM_BLOCK_SIZE_04 > CFE_SB_MEM_BLOCK_SIZE_05
    #error CFE_SB_MEM_BLOCK_SIZE_04 must be less than CFE_SB_MEM_BLOCK_SIZE_05
#endif

#if CFE_SB_MEM_BLOCK_SIZE_05 > CFE_SB_MEM_BLOCK_SIZE_06
    #error CFE_SB_MEM_BLOCK_SIZE_05 must be less than CFE_SB_MEM_BLOCK_SIZE_06
#endif

#if CFE_SB_MEM_BLOCK_SIZE_06 > CFE_SB_MEM_BLOCK_SIZE_07
    #error CFE_SB_MEM_BLOCK_SIZE_06 must be less than CFE_SB_MEM_BLOCK_SIZE_07
#endif

#if CFE_SB_MEM_BLOCK_SIZE_07 > CFE_SB_MEM_BLOCK_SIZE_08
    #error CFE_SB_MEM_BLOCK_SIZE_07 must be less than CFE_SB_MEM_BLOCK_SIZE_08
#endif

#if CFE_SB_MEM_BLOCK_SIZE_08 > CFE_SB_MEM_BLOCK_SIZE_09
    #error CFE_SB_MEM_BLOCK_SIZE_08 must be less than CFE_SB_MEM_BLOCK_SIZE_09
#endif

#if CFE_SB_MEM_BLOCK_SIZE_09 > CFE_SB_MEM_BLOCK_SIZE_10
    #error CFE_SB_MEM_BLOCK_SIZE_09 must be less than CFE_SB_MEM_BLOCK_SIZE_10
#endif

#if CFE_SB_MEM_BLOCK_SIZE_10 > CFE_SB_MEM_BLOCK_SIZE_11
    #error CFE_SB_MEM_BLOCK_SIZE_10 must be less than CFE_SB_MEM_BLOCK_SIZE_11
#endif

#if CFE_SB_MEM_BLOCK_SIZE_11 > CFE_SB_MEM_BLOCK_SIZE_12
    #error CFE_SB_MEM_BLOCK_SIZE_11 must be less than CFE_SB_MEM_BLOCK_SIZE_12
#endif

#if CFE_SB_MEM_BLOCK_SIZE_12 > CFE_SB_MEM_BLOCK_SIZE_13
    #error CFE_SB_MEM_BLOCK_SIZE_12 must be less than CFE_SB_MEM_BLOCK_SIZE_13
#endif

#if CFE_SB_MEM_BLOCK_SIZE_13 > CFE_SB_MEM_BLOCK_SIZE_14
    #error CFE_SB_MEM_BLOCK_SIZE_13 must be less than CFE_SB_MEM_BLOCK_SIZE_14
#endif

#if CFE_SB_MEM_BLOCK_SIZE_14 > CFE_SB_MEM_BLOCK_SIZE_15
    #error CFE_SB_MEM_BLOCK_SIZE_14 must be less than CFE_SB_MEM_BLOCK_SIZE_15
#endif

#if CFE_SB_MEM_BLOCK_SIZE_15 > CFE_SB_MEM_BLOCK_SIZE_16
    #error CFE_SB_MEM_BLOCK_SIZE_15 must be less than CFE_SB_MEM_BLOCK_SIZE_16
#endif

#if CFE_SB_MEM_BLOCK_SIZE_16 >= CFE_SB_MAX_BLOCK_SIZE
    #error CFE_SB_MEM_BLOCK_SIZE_16 must be less than CFE_SB_MAX_BLOCK_SIZE
#endif

#if CFE_SB_DEFAULT_REPORT_SENDER < 0
    #error CFE_SB_DEFAULT_REPORT_SENDER cannot be less than 0!
#endif

#if CFE_SB_DEFAULT_REPORT_SENDER > 1
    #error CFE_SB_DEFAULT_REPORT_SENDER cannot be greater than 1!
#endif

#if CFE_SB_DEFAULT_MSG_LIMIT < 4
    #error CFE_SB_DEFAULT_MSG_LIMIT cannot be less than 4!
#endif

#if CFE_SB_DEFAULT_MSG_LIMIT > 65535
    #error CFE_SB_DEFAULT_MSG_LIMIT cannot be greater than 65535!
#endif

#endif /* _cfe_sb_verify_ */
/*****************************************************************************/
