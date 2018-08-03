/*
** File: cfe_es_perfids.h
**
** Purpose: This file contains the perf_ids
**
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement. 
**
** Design Notes:
**   Each performance id is used to identify something that needs to be
**   measured.  Performance ids are limited to the range of 0 to
**   CFE_ES_PERF_MAX_IDS - 1.  Any performance ids outside of this range
**   will be ignored and will be flagged as an error.  Note that
**   performance ids 0-31 are reserved for the cFE Core.
**
** References:
**
** $Log: cfe_es_perfids.h  $
** Revision 1.6 2012/01/13 12:32:23GMT-05:00 acudmore 
** Changed license text to reflect open source
** Revision 1.5 2012/01/06 14:06:54EST rmcgraw 
** DCR16100:2 changed pipe overflow id from 12 to 27
** Revision 1.4 2012/01/06 09:27:19GMT-05:00 rmcgraw 
** DCR16100:1 Added perf markers for msg lim and pipe overflow errors
** Revision 1.3 2010/09/21 15:04:06GMT-05:00 jmdagost 
** Make copyright symbol readable.
** Revision 1.2 2010/09/21 16:02:42EDT jmdagost 
** Change performance IDs from hexadecimal to decimal.
** Revision 1.1 2008/04/17 08:05:20EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/inc/project.pj
** Revision 1.9 2007/09/24 11:13:39EDT rjmcgraw 
** DCR5039 Updated comments
** Revision 1.8 2007/08/31 10:57:18EDT rjmcgraw
** Changes to eliminate perl script
** Revision 1.7 2007/04/24 08:59:04EDT njyanchik
** I added the changes tha LRO requested. They are specified in the DCR text.
** Revision 1.6 2007/04/09 14:43:30EDT rjmcgraw
** Changed names of performance ids to include CFE_
** Revision 1.5 2006/12/26 15:23:29EST rjmcgraw
** removed SBP performance id
** Revision 1.4 2006/11/20 15:17:00EST dlkobe
** Updated comments to make use of Doxygen utility
** Revision 1.3 2006/06/21 12:13:27GMT-05:00 kkaudra
** Added Markers for TIME ISRs
** Revision 1.2 2006/06/14 09:53:21EDT kkaudra
** moved perf config information to cfe_platform_cfg.h
** Revision 1.1 2006/06/13 15:39:08EDT rjmcgraw
** Initial revision
**
*/

#ifndef _cfe_es_perfids_
#define _cfe_es_perfids_


#define CFE_ES_PERF_EXIT_BIT                    31      /**< \brief bit (31) is reserved by the perf utilities */

/** \name cFE Performance Monitor IDs (Reserved IDs 0-31) */
/** \{ */
#define CFE_ES_MAIN_PERF_ID             1    /**< \brief Performance ID for Executive Services Task */
#define CFE_EVS_MAIN_PERF_ID            2    /**< \brief Performance ID for Events Services Task */
#define CFE_TBL_MAIN_PERF_ID            3    /**< \brief Performance ID for Table Services Task */
#define CFE_SB_MAIN_PERF_ID             4    /**< \brief Performance ID for Software Bus Services Task */
#define CFE_SB_MSG_LIM_PERF_ID          5    /**< \brief Performance ID for Software Bus Msg Limit Errors */
#define CFE_SB_PIPE_OFLOW_PERF_ID       27   /**< \brief Performance ID for Software Bus Pipe Overflow Errors */


#define CFE_TIME_MAIN_PERF_ID           6    /**< \brief Performance ID for Time Services Task */
#define CFE_TIME_TONE1HZISR_PERF_ID     7    /**< \brief Performance ID for 1 Hz Tone ISR */
#define CFE_TIME_LOCAL1HZISR_PERF_ID    8    /**< \brief Performance ID for 1 Hz Local ISR */

#define CFE_TIME_SENDMET_PERF_ID        9    /**< \brief Performance ID for Time ToneSendMET */
#define CFE_TIME_LOCAL1HZTASK_PERF_ID   10   /**< \brief Performance ID for 1 Hz Local Task */
#define CFE_TIME_TONE1HZTASK_PERF_ID    11   /**< \brief Performance ID for 1 Hz Tone Task */


/** \} */


#endif   /* _cfe_es_perfids_ */
