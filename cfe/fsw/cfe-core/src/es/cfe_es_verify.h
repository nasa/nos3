/******************************************************************************
** File: cfe_es_verify.h
**
**
**
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement.
**
** Purpose:
**   This header file performs compile time checking for ES configuration
**   parameters.
**
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
**     The upper limits are somewhat arbitrary right now.
**
**  $Log: cfe_es_verify.h  $
**  Revision 1.4 2012/01/13 11:50:06GMT-05:00 acudmore 
**  Changed license text to reflect open source
**  Revision 1.3 2010/10/26 16:28:46EDT jmdagost 
**  Added CFE_ES_MAX_SHELL_CMD test (for < OS_MAX_CMD_LEN)
**  Revision 1.2 2009/07/28 18:04:06EDT jmdagost 
**  Increased max allowed number of registered CDS blocks
**  Revision 1.1 2008/04/17 08:05:10EDT ruperera 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/es/project.pj
**  Revision 1.10 2007/07/13 10:25:21EDT rjmcgraw 
**  DCR4462:1 Corrected wording in CFE_ES_PERF_DATA_BUFFER_SIZE checking
**  Revision 1.9 2007/05/31 10:11:34EDT apcudmore 
**  Removed CFE_ES_CPU_CONTEXT_SIZE from verify.h
**  Removed unused variable.
**  Revision 1.8 2007/05/24 15:09:23EDT apcudmore 
**  App-Decompression updates:
**    - Added verify macros for PERCENT_FREE config parameter
**    - Updated comments in config files
**    - Updated logic to turn off check if RAM_DISK_PERCENT_FREE is 0
**  Revision 1.7 2007/05/04 18:50:50EDT dlkobe 
**  Added CDS Memory Pool Parameter compile time verifications
**  Revision 1.6 2007/03/29 13:27:34EST apcudmore 
**  Updated ES configuration parameters and verification macros.
**
******************************************************************************/

#ifndef _cfe_es_verify_
#define _cfe_es_verify_

#if CFE_ES_MAX_APPLICATIONS  <  6
    #error CFE_ES_MAX_APPLICATIONS cannot be less than 6!
#elif CFE_EVS_MAX_APPLICATIONS  >  64
    #error CFE_ES_MAX_APPLICATIONS cannot be greater than 64!
#endif

#if CFE_ES_MAX_LIBRARIES  <  1
    #error CFE_ES_MAX_LIBRARIES cannot be less than 1!
#elif CFE_EVS_MAX_LIBRARIES  >  64
    #error CFE_ES_MAX_LIBRARIES cannot be greater than 64!
#endif

#if CFE_ES_ER_LOG_ENTRIES  <  10
    #error CFE_ES_ER_LOG_ENTRIES cannot be less than 10!
#elif CFE_ES_ER_LOG_ENTRIES  >  128
    #error CFE_ES_ER_LOG_ENTRIES cannot be greater than 128!
#endif

#if CFE_ES_SYSTEM_LOG_SIZE  <  512
    #error CFE_ES_SYSTEM_LOG_SIZE cannot be less than 512 Bytes!
#elif CFE_ES_SYSTEM_LOG_SIZE  >  16384
    #error CFE_ES_SYSTEM_LOG_SIZE cannot be greater than 16K Bytes!
#endif

#if CFE_ES_DEFAULT_STACK_SIZE  <  2048
    #error CFE_ES_DEFAULT_STACK_SIZE cannot be less than 2048 Bytes!
#elif CFE_ES_DEFAULT_STACK_SIZE  >  16384
    #error CFE_ES_DEFAULT_STACK_SIZE cannot be greater than 16K Bytes!
#endif

/*
** Number of entries in the ES Object table ( The table that controls core cFE startup )
*/
#if CFE_ES_OBJECT_TABLE_SIZE  <  15
    #error CFE_ES_OBJECT_TABLE_SIZE cannot be less than 15!
#elif CFE_ES_OBJECT_TABLE_SIZE  >  50
    #error CFE_ES_OBJECT_TABLE_SIZE cannot be greater than 50!
#endif

/*
** ES Application Control Scan Rate. 
*/
#if CFE_ES_APP_SCAN_RATE  <  100
    #error CFE_ES_APP_SCAN_RATE cannot be less than 100 milliseconds!
#elif CFE_ES_APP_SCAN_RATE  >  20000
    #error CFE_ES_APP_SCAN_RATE cannot be greater than 20 seconds!
#endif

/*
** ES Application Kill Timeout 
*/
#if CFE_ES_APP_KILL_TIMEOUT  <  1
    #error CFE_ES_APP_KILL_TIMEOUT cannot be less than 1!
#elif CFE_ES_APP_KILL_TIMEOUT  >  100
    #error CFE_ES_APP_KILL_TIMEOUT cannot be greater than 100!
#endif

/*
** ES / cFE RAM disk parameters 
*/
#if CFE_ES_RAM_DISK_SECTOR_SIZE  <  128
    #error CFE_ES_RAM_DISK_SECTOR_SIZE cannot be less than 128!
#elif CFE_ES_RAM_DISK_SECTOR_SIZE  >  4096
    #error CFE_ES_RAM_DISK_SECTOR_SIZE cannot be greater than 4096!
#endif

#if CFE_ES_RAM_DISK_NUM_SECTORS  <  128
    #error CFE_ES_RAM_DISK_NUM_SECTORS cannot be less than 128!
#elif CFE_ES_RAM_DISK_NUM_SECTORS  >  8192
    #error CFE_ES_RAM_DISK_NUM_SECTORS cannot be greater than 8192!
#endif

#if CFE_ES_RAM_DISK_PERCENT_RESERVED  < 0 
    #error CFE_ES_RAM_DISK_PERCENT_RESERVED cannot be less than 0!
#elif CFE_ES_RAM_DISK_PERCENT_RESERVED  > 75 
    #error CFE_ES_RAM_DISK_PERCENT_RESERVED cannot be greater than 75!
#endif

/*
** Critical data store size 
*/
#if CFE_ES_CDS_SIZE  <  ( 8 * 1024 )
    #error CFE_ES_CDS_SIZE cannot be less than 8Kbytes!
#elif CFE_ES_CDS_SIZE >  ( 1024 * 2048 )
    #error CFE_ES_CDS_SIZE cannot be greater than 2 Megabytes!
#endif

/*
** User Reserved Memory Size. 
*/
#if CFE_ES_USER_RESERVED_SIZE  <  ( 1 * 1024 )
    #error CFE_ES_USER_RESERVED_SIZE cannot be less than 1Kbytes!
#elif CFE_ES_USER_RESERVED_SIZE >  ( 1024 * 32768 )
    #error CFE_ES_USER_RESERVED_SIZE cannot be greater than 32 Megabytes!
#endif

/*
** ES Reset area size -- This should depend on 
**  the size of the data structures that are in this area.
*/
#if CFE_ES_RESET_AREA_SIZE  <  ( 150 * 1024 )
    #error CFE_ES_RESET_AREA_SIZE cannot be less than 150Kbytes!
#elif CFE_ES_RESET_AREA_SIZE >  ( 1024 * 2048 )
    #error CFE_ES_RESET_AREA_SIZE cannot be greater than 2 Megabytes!
#endif

/* 
** The size of a command to the OS that lies under the cFE 
*/
#if CFE_ES_MAX_SHELL_CMD  <  64
    #error CFE_ES_MAX_SHELL_CMD cannot be less than 64!
#elif CFE_ES_MAX_SHELL_CMD  >  128
    #error CFE_ES_MAX_SHELL_CMD cannot be greater than 128!
#endif

/* 
** The size of a command to the OS that lies under the cFE must avoid overflow 
*/
#if CFE_ES_MAX_SHELL_CMD  >  OS_MAX_CMD_LEN
    #error CFE_ES_MAX_SHELL_CMD cannot be greater than OS_MAX_CMD_LEN!
#endif

/* 
** The size of the chunks to break the output of the shell command into 
 * to send down in telemetry 
*/
#if CFE_ES_MAX_SHELL_PKT  <  32
    #error CFE_ES_MAX_SHELL_PKT cannot be less than 32!
#elif CFE_ES_MAX_SHELL_PKT  >  128
    #error CFE_ES_MAX_SHELL_PKT cannot be greater than 128!
#endif

/* 
** Syslog mode 
*/
#if CFE_ES_DEFAULT_SYSLOG_MODE  <  0
    #error CFE_ES_DEFAULT_SYSLOG_MODE cannot be less than 0!
#elif CFE_ES_DEFAULT_SYSLOG_MODE  >  1
    #error CFE_ES_DEFAULT_SYSLOG_MODE cannot be greater than 1!
#endif

/* 
** Maximum number of performance IDs 
*/
#if CFE_ES_PERF_MAX_IDS  <  32
    #error CFE_ES_PERF_MAX_IDS cannot be less than 32!
#elif CFE_ES_PERF_MAX_IDS  >  512
    #error CFE_ES_PERF_MAX_IDS cannot be greater than 512!
#endif

/* 
** Performance data buffer size
*/
#if CFE_ES_PERF_DATA_BUFFER_SIZE  <  1025
    #error CFE_ES_PERF_DATA_BUFFER_SIZE cannot be less than 1025 entries!
#elif CFE_ES_PERF_DATA_BUFFER_SIZE  >  (1024 * 1024 )
    #error CFE_ES_PERF_DATA_BUFFER_SIZE cannot be greater than 1048576 entries!
#endif

/* 
** Maximum number of Registered CDS blocks
*/
#if CFE_ES_CDS_MAX_NUM_ENTRIES  <  8
    #error CFE_ES_CDS_MAX_NUM_ENTRIES cannot be less than 8!
#elif CFE_ES_CDS_MAX_NUM_ENTRIES  >  32767
    #error CFE_ES_CDS_MAX_NUM_ENTRIES cannot be greater than 32767!
#endif

/* 
** Maximum number of processor resets before a power-on
*/
#if CFE_ES_MAX_PROCESSOR_RESETS  <  0
    #error CFE_ES_MAX_PROCESSOR_RESETS cannot be less than 0!
#elif CFE_ES_MAX_PROCESSOR_RESETS  >  1024
    #error CFE_ES_MAX_PROCESSOR_RESETS cannot be greater than 1024!
#endif

/*
**  Intermediate ES Memory Pool Block Sizes
*/
#if CFE_ES_MAX_BLOCK_SIZE < CFE_SB_MAX_SB_MSG_SIZE
    #error CFE_ES_MAX_BLOCK_SIZE must be larger than CFE_SB_MAX_SB_MSG_SIZE!
#endif

#if CFE_ES_MAX_BLOCK_SIZE < CFE_TBL_MAX_SNGL_TABLE_SIZE
    #error CFE_ES_MAX_BLOCK_SIZE must be larger than CFE_TBL_MAX_SNGL_TABLE_SIZE!
#endif

#if CFE_ES_MAX_BLOCK_SIZE < CFE_TBL_MAX_DBL_TABLE_SIZE
    #error CFE_ES_MAX_BLOCK_SIZE must be larger than CFE_TBL_MAX_DBL_TABLE_SIZE!
#endif

#if CFE_ES_MEM_BLOCK_SIZE_01 > CFE_ES_MEM_BLOCK_SIZE_02
    #error CFE_ES_MEM_BLOCK_SIZE_01 must be less than CFE_ES_MEM_BLOCK_SIZE_02
#endif

#if CFE_ES_MEM_BLOCK_SIZE_02 > CFE_ES_MEM_BLOCK_SIZE_03
    #error CFE_ES_MEM_BLOCK_SIZE_02 must be less than CFE_ES_MEM_BLOCK_SIZE_03
#endif

#if CFE_ES_MEM_BLOCK_SIZE_03 > CFE_ES_MEM_BLOCK_SIZE_04
    #error CFE_ES_MEM_BLOCK_SIZE_03 must be less than CFE_ES_MEM_BLOCK_SIZE_04
#endif

#if CFE_ES_MEM_BLOCK_SIZE_04 > CFE_ES_MEM_BLOCK_SIZE_05
    #error CFE_ES_MEM_BLOCK_SIZE_04 must be less than CFE_ES_MEM_BLOCK_SIZE_05
#endif

#if CFE_ES_MEM_BLOCK_SIZE_05 > CFE_ES_MEM_BLOCK_SIZE_06
    #error CFE_ES_MEM_BLOCK_SIZE_05 must be less than CFE_ES_MEM_BLOCK_SIZE_06
#endif

#if CFE_ES_MEM_BLOCK_SIZE_06 > CFE_ES_MEM_BLOCK_SIZE_07
    #error CFE_ES_MEM_BLOCK_SIZE_06 must be less than CFE_ES_MEM_BLOCK_SIZE_07
#endif

#if CFE_ES_MEM_BLOCK_SIZE_07 > CFE_ES_MEM_BLOCK_SIZE_08
    #error CFE_ES_MEM_BLOCK_SIZE_07 must be less than CFE_ES_MEM_BLOCK_SIZE_08
#endif

#if CFE_ES_MEM_BLOCK_SIZE_08 > CFE_ES_MEM_BLOCK_SIZE_09
    #error CFE_ES_MEM_BLOCK_SIZE_08 must be less than CFE_ES_MEM_BLOCK_SIZE_09
#endif

#if CFE_ES_MEM_BLOCK_SIZE_09 > CFE_ES_MEM_BLOCK_SIZE_10
    #error CFE_ES_MEM_BLOCK_SIZE_09 must be less than CFE_ES_MEM_BLOCK_SIZE_10
#endif

#if CFE_ES_MEM_BLOCK_SIZE_10 > CFE_ES_MEM_BLOCK_SIZE_11
    #error CFE_ES_MEM_BLOCK_SIZE_10 must be less than CFE_ES_MEM_BLOCK_SIZE_11
#endif

#if CFE_ES_MEM_BLOCK_SIZE_11 > CFE_ES_MEM_BLOCK_SIZE_12
    #error CFE_ES_MEM_BLOCK_SIZE_11 must be less than CFE_ES_MEM_BLOCK_SIZE_12
#endif

#if CFE_ES_MEM_BLOCK_SIZE_12 > CFE_ES_MEM_BLOCK_SIZE_13
    #error CFE_ES_MEM_BLOCK_SIZE_12 must be less than CFE_ES_MEM_BLOCK_SIZE_13
#endif

#if CFE_ES_MEM_BLOCK_SIZE_13 > CFE_ES_MEM_BLOCK_SIZE_14
    #error CFE_ES_MEM_BLOCK_SIZE_13 must be less than CFE_ES_MEM_BLOCK_SIZE_14
#endif

#if CFE_ES_MEM_BLOCK_SIZE_14 > CFE_ES_MEM_BLOCK_SIZE_15
    #error CFE_ES_MEM_BLOCK_SIZE_14 must be less than CFE_ES_MEM_BLOCK_SIZE_15
#endif

#if CFE_ES_MEM_BLOCK_SIZE_15 > CFE_ES_MEM_BLOCK_SIZE_16
    #error CFE_ES_MEM_BLOCK_SIZE_15 must be less than CFE_ES_MEM_BLOCK_SIZE_16
#endif

#if CFE_ES_MEM_BLOCK_SIZE_16 > CFE_ES_MAX_BLOCK_SIZE
    #error CFE_ES_MEM_BLOCK_SIZE_16 must be less than CFE_ES_MAX_BLOCK_SIZE
#endif

/*
**  Intermediate ES Critical Data Store Memory Pool Block Sizes
*/
#if CFE_ES_CDS_MEM_BLOCK_SIZE_01 > CFE_ES_CDS_MEM_BLOCK_SIZE_02
    #error CFE_ES_CDS_MEM_BLOCK_SIZE_01 must be less than CFE_ES_CDS_MEM_BLOCK_SIZE_02
#endif

#if CFE_ES_CDS_MEM_BLOCK_SIZE_02 > CFE_ES_CDS_MEM_BLOCK_SIZE_03
    #error CFE_ES_CDS_MEM_BLOCK_SIZE_02 must be less than CFE_ES_CDS_MEM_BLOCK_SIZE_03
#endif

#if CFE_ES_CDS_MEM_BLOCK_SIZE_03 > CFE_ES_CDS_MEM_BLOCK_SIZE_04
    #error CFE_ES_CDS_MEM_BLOCK_SIZE_03 must be less than CFE_ES_CDS_MEM_BLOCK_SIZE_04
#endif

#if CFE_ES_CDS_MEM_BLOCK_SIZE_04 > CFE_ES_CDS_MEM_BLOCK_SIZE_05
    #error CFE_ES_CDS_MEM_BLOCK_SIZE_04 must be less than CFE_ES_CDS_MEM_BLOCK_SIZE_05
#endif

#if CFE_ES_CDS_MEM_BLOCK_SIZE_05 > CFE_ES_CDS_MEM_BLOCK_SIZE_06
    #error CFE_ES_CDS_MEM_BLOCK_SIZE_05 must be less than CFE_ES_CDS_MEM_BLOCK_SIZE_06
#endif

#if CFE_ES_CDS_MEM_BLOCK_SIZE_06 > CFE_ES_CDS_MEM_BLOCK_SIZE_07
    #error CFE_ES_CDS_MEM_BLOCK_SIZE_06 must be less than CFE_ES_CDS_MEM_BLOCK_SIZE_07
#endif

#if CFE_ES_CDS_MEM_BLOCK_SIZE_07 > CFE_ES_CDS_MEM_BLOCK_SIZE_08
    #error CFE_ES_CDS_MEM_BLOCK_SIZE_07 must be less than CFE_ES_CDS_MEM_BLOCK_SIZE_08
#endif

#if CFE_ES_CDS_MEM_BLOCK_SIZE_08 > CFE_ES_CDS_MEM_BLOCK_SIZE_09
    #error CFE_ES_CDS_MEM_BLOCK_SIZE_08 must be less than CFE_ES_CDS_MEM_BLOCK_SIZE_09
#endif

#if CFE_ES_CDS_MEM_BLOCK_SIZE_09 > CFE_ES_CDS_MEM_BLOCK_SIZE_10
    #error CFE_ES_CDS_MEM_BLOCK_SIZE_09 must be less than CFE_ES_CDS_MEM_BLOCK_SIZE_10
#endif

#if CFE_ES_CDS_MEM_BLOCK_SIZE_10 > CFE_ES_CDS_MEM_BLOCK_SIZE_11
    #error CFE_ES_CDS_MEM_BLOCK_SIZE_10 must be less than CFE_ES_CDS_MEM_BLOCK_SIZE_11
#endif

#if CFE_ES_CDS_MEM_BLOCK_SIZE_11 > CFE_ES_CDS_MEM_BLOCK_SIZE_12
    #error CFE_ES_CDS_MEM_BLOCK_SIZE_11 must be less than CFE_ES_CDS_MEM_BLOCK_SIZE_12
#endif

#if CFE_ES_CDS_MEM_BLOCK_SIZE_12 > CFE_ES_CDS_MEM_BLOCK_SIZE_13
    #error CFE_ES_CDS_MEM_BLOCK_SIZE_12 must be less than CFE_ES_CDS_MEM_BLOCK_SIZE_13
#endif

#if CFE_ES_CDS_MEM_BLOCK_SIZE_13 > CFE_ES_CDS_MEM_BLOCK_SIZE_14
    #error CFE_ES_CDS_MEM_BLOCK_SIZE_13 must be less than CFE_ES_CDS_MEM_BLOCK_SIZE_14
#endif

#if CFE_ES_CDS_MEM_BLOCK_SIZE_14 > CFE_ES_CDS_MEM_BLOCK_SIZE_15
    #error CFE_ES_CDS_MEM_BLOCK_SIZE_14 must be less than CFE_ES_CDS_MEM_BLOCK_SIZE_15
#endif

#if CFE_ES_CDS_MEM_BLOCK_SIZE_15 > CFE_ES_CDS_MEM_BLOCK_SIZE_16
    #error CFE_ES_CDS_MEM_BLOCK_SIZE_15 must be less than CFE_ES_CDS_MEM_BLOCK_SIZE_16
#endif

#if CFE_ES_CDS_MEM_BLOCK_SIZE_16 > CFE_ES_CDS_MAX_BLOCK_SIZE
    #error CFE_ES_CDS_MEM_BLOCK_SIZE_16 must be less than CFE_ES_CDS_MAX_BLOCK_SIZE
#endif

#endif /* _cfe_es_verify_ */
/*****************************************************************************/
