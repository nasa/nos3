/******************************************************************************
** File: cfe_mission_cfg.h
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
** Purpose:
**   This header file contains the mission configuration parameters and
**   typedefs with mission scope.
**
** Author:   R.McGraw/SSI
**
** Notes:
**
** $Log: cfe_mission_cfg.h  $
** Revision 1.1.1.3 2015/03/04 14:58:31EST sstrege 
** Added copyright information
** Revision 1.1.1.2 2008/10/17 15:48:23EDT rjmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hk/fsw/unit_test/cfe_hdr_files/project.pj
** Revision 1.2 2008/07/07 14:09:42EDT apcudmore 
** Removed FS Message ID defines from system config files.
** Revision 1.1 2008/04/17 08:02:33EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/build/inc/project.pj
** Revision 1.21 2007/09/19 15:21:16EDT rjmcgraw 
** DCR4488 Removed all SB network message numbers and message ids
** Revision 1.20 2007/09/12 16:06:11EDT David Kobe (dlkobe)
** Moved the definitions of CFE_ES_CRC_xx to the cfe_mission_cfg.h file and deleted TBL Services
** CRC #define statement.
** Revision 1.19 2007/09/12 09:46:03EDT David Kobe (dlkobe)
** Added doxygen comments to commands and some configuration parameters
** Revision 1.18 2007/06/08 11:12:47EDT rjmcgraw
** Added doxygen comments
** Revision 1.17 2007/05/04 20:07:53EDT dlkobe
** Added command to telemeter memory pool statistics
** Revision 1.16 2007/04/19 15:43:48EDT rjmcgraw
** Removed SBP related #defines
** Revision 1.15 2007/02/23 09:11:33EST njyanchik
** Added CFE_ES_SHELL_TLM_MSG
** Revision 1.14 2006/12/28 16:25:40EST rjmcgraw
** Added msg numbers for new SB subscription pkts
** Revision 1.13 2006/12/26 15:13:37EST rjmcgraw
** Comment changes in networking defines
** Revision 1.12 2006/11/28 11:28:30EST mobartholomew
** Updated cfe_mission_cfg to reference valid CRC type
** Revision 1.11 2006/11/09 15:30:07EST rjmcgraw
** Added #define for CFE_SPACECRAFT_ID
** Revision 1.10 2006/10/30 12:53:13EST dlkobe
** Baseline Critical Data Store Implementation
**
******************************************************************************/

#ifndef _cfe_mission_cfg_
#define _cfe_mission_cfg_


/**
**  \cfemissioncfg Spacecraft ID
**
**  \par Description:
**      This defines the value that is returned by the call to
**      OS_BSPGetSpacecraftId.
**
**  \par Limits
**       The cFE does not place a limit on this configuration paramter.
**       CCSDS allocates 8 bits for this field in the standard VCDU.
*/
#define CFE_SPACECRAFT_ID       0x42


/**
**  \cfemissioncfg Spacecraft ID
**
**  \par Description:
**      Dictates the message format used by the cFE.
**
**  \par Limits
**      As of cFE Build 4.1.0, CCSDS is the only format supported by the cFE
*/
#define MESSAGE_FORMAT_IS_CCSDS


/**
**  \cfesbcfg Maximum SB Message Size
**
**  \par Description:
**      The following definition dictates the maximum message size allowed on
**      the software bus. SB checks the pkt length field in the header of all
**      messages sent. If the pkt length field indicates the message is larger
**      than this define, SB sends an event and rejects the send.
**
**
**  \par Limits
**      The maximum msg size(including headers)the cFE can support is 32768.
*/
#define CFE_SB_MAX_SB_MSG_SIZE         32768


/**
**  \cfetimecfg Default Time Format
**
**  \par Description:
**      The following definition selects either UTC or TAI as the default
**      (mission specific) time format.  Although it is possible for an
**      application to request time in a specific format, most callers
**      should use CFE_TIME_GetTime(), which returns time in the default
**      format.  This avoids having to modify each individual caller
**      when the default choice is changed.
**
**
**  \par Limits
**      define only CFE_TIME_CFG_DEFAULT_TAI or CFE_TIME_CFG_DEFAULT_UTC
*/
#define CFE_TIME_CFG_DEFAULT_TAI
/*#define CFE_TIME_CFG_DEFAULT_UTC*/


/**
**  \cfetimecfg Default Time Format
**
**  \par Description:
**      The following definition enables the use of a simulated time at
**      the tone signal using a software bus message.
**
**
**  \par Limits
**      Not Applicable
*/
#define CFE_TIME_CFG_FAKE_TONE


/**
**  \cfetimecfg Default Time and Tone Order
**
**  \par Description:
**      Time Services may be configured to expect the time at the tone
**      data packet to either precede or follow the tone signal.  If the
**      time at the tone data packet follows the tone signal, then the
**      data within the packet describes what the time "was" at the tone.
**      If the time at the tone data packet precedes the tone signal, then
**      the data within the packet describes what the time "will be" at
**      the tone.  One, and only one, of the following symbols must be defined:
**      - #CFE_TIME_AT_TONE_WAS
**      - #CFE_TIME_AT_TONE_WILL_BE
**      Note: If Time Services is defined as using a simulated tone signal
**            (see #CFE_TIME_CFG_FAKE_TONE above), then the tone data packet
**            must follow the tone signal.
**
**  \par Limits
**      define only CFE_TIME_AT_TONE_WAS or CFE_TIME_AT_TONE_WILL_BE
*/
#define CFE_TIME_AT_TONE_WAS
/*#define CFE_TIME_AT_TONE_WILL_BE*/

/**
**  \cfetimecfg Min and Max Time Elapsed
**
**  \par Description:
**      Based on the definition of Time and Tone Order
**      (CFE_TIME_AT_TONE_WAS/WILL_BE) either the "time at the tone" signal or
**      data packet will follow the other. This definition sets the valid window
**      of time for the second of the pair to lag behind the first. Time
**      Services will invalidate both the tone and packet if the second does not
**      arrive within this window following the first.
**
**      For example, if the data packet follows the tone, it might be valid for
**      the data packet to arrive between zero and 100,000 micro-seconds after
**      the tone.  But, if the tone follows the the packet, it might be valid
**      only if the packet arrived between 200,000 and 700,000 micro-seconds
**      before the tone.
**
**      Note: units are in micro-seconds
**
**
**  \par Limits
**       0 to 999,999 decimal
*/
#define CFE_TIME_MIN_ELAPSED          0
#define CFE_TIME_MAX_ELAPSED     200000


/**
**  \cfetimecfg Default Time Values
**
**  \par Description:
**      Default time values are provided to avoid problems due to time
**      calculations performed after startup but before commands can be
**      processed.  For example, if the default time format is UTC then
**      it is important that the sum of MET and STCF always exceed the
**      value of Leap Seconds to prevent the UTC time calculation
**     <tt>(time = MET + STCF - Leap Seconds) </tt> from resulting in a negative
**     (very large) number.<BR><BR>
**     Some past missions have also created known (albeit wrong) default
**     timestamps.  For example, assume the epoch is defined as Jan 1, 1970
**     and further assume the default time values are set to create a timestamp
**     of Jan 1, 2000.  Even though the year 2000 timestamps are wrong, it
**     may be of value to keep the time within some sort of bounds acceptable
**     to the software.<BR><BR>
**     Note: Sub-second units are in micro-seconds (0 to 999,999) and
**           all values must be defined
**
**  \par Limits
**       Not Applicable
*/
#define CFE_TIME_DEF_MET_SECS      1000
#define CFE_TIME_DEF_MET_SUBS         0

#define CFE_TIME_DEF_STCF_SECS  1000000
#define CFE_TIME_DEF_STCF_SUBS        0

#define CFE_TIME_DEF_LEAPS           32

#define CFE_TIME_DEF_DELAY_SECS       0
#define CFE_TIME_DEF_DELAY_SUBS    1000


/**
**  \cfetimecfg Default EPOCH Values
**
**  \par Description:
**      Default ground time epoch values
**      Note: these values are used only by the CFE_TIME_Print() API function
**
**  \par Limits
**      Year - must be within 136 years
**      Day - Jan 1 = 1, Feb 1 = 32, etc.
**      Hour - 0 to 23
**      Minute - 0 to 59
**      Second - 0 to 59
*/
#define CFE_TIME_EPOCH_YEAR        1980
#define CFE_TIME_EPOCH_DAY            1
#define CFE_TIME_EPOCH_HOUR           0
#define CFE_TIME_EPOCH_MINUTE         0
#define CFE_TIME_EPOCH_SECOND         0


/**
**  \cfetimecfg Time File System Factor
**
**  \par Description:
**      Define the s/c vs file system time conversion constant...
**
**      Note: this value is intended for use only by CFE TIME API functions to
**      convert time values based on the ground system epoch (s/c time) to
**      and from time values based on the file system epoch (fs time).
**
**      FS time  = S/C time + factor
**      S/C time = FS time - factor
**
**      Worksheet:
**
**      S/C epoch = Jan 1, 2005  (LRO ground system epoch)
**      FS epoch  = Jan 1, 1980  (vxWorks DOS file system epoch)
**
**      Delta = 25 years, 0 days, 0 hours, 0 minutes, 0 seconds
**
**      Leap years = 1980, 1984, 1988, 1992, 1996, 2000, 2004
**      (divisible by 4 -- except if by 100 -- unless also by 400)
**
**      1 year   =  31,536,000 seconds
**      1 day    =      86,400 seconds
**      1 hour   =       3,600 seconds
**      1 minute =          60 seconds
**
**      25 years = 788,400,000 seconds
**      7 extra leap days = 604,800 seconds
**
**      total delta = 789,004,800 seconds
**
**  \par Limits
**      Not Applicable
*/
#define CFE_TIME_FS_FACTOR  789004800


/**
**  \cfeescfg Maximum Length of CDS Name
**
**  \par Description:
**      Indicates the maximum length (in characers) of the CDS name ('CDSName')
**      portion of a Full CDS Name of the following form:
**      "ApplicationName.CDSName"
**
**
**  \par Limits
**      Not Applicable
*/
#define CFE_ES_CDS_MAX_NAME_LENGTH         16



/**
**  \cfeevscfg Maximum Event Message Length
**
**  \par Description:
**      Indicates the maximum length (in characers) of the formatted text
**      string portion of an event message
**
**  \par Limits
**      Not Applicable
*/
#define CFE_EVS_MAX_MESSAGE_LENGTH     122


/** \name Checksum/CRC algorithm identifiers */
/** \{ */
#define CFE_ES_CRC_8   1  /**< \brief CRC ( 8 bit additive - returns 32 bit total) (Currently not implemented) */
#define CFE_ES_CRC_16  2  /**< \brief CRC (16 bit additive - returns 32 bit total) */
#define CFE_ES_CRC_32  3  /**< \brief CRC (32 bit additive - returns 32 bit total) (Currently not implemented) */
/** \} */

/**
**  \cfeescfg Mission Default CRC algorithm
**
**  \par Description:
**      Indicates the which CRC algorithm should be used as the default
**      for verifying the contents of Critical Data Stores and when calculating
**      Table Image data integrity values.
**
**  \par Limits
**      Currently only CFE_ES_CRC_16 is supported (see #CFE_ES_CRC_16)
*/
#define CFE_ES_DEFAULT_CRC  CFE_ES_CRC_16


/**
**  \cfetblcfg Maximum Table Name Length
**
**  \par Description:
**      Indicates the maximum length (in characers) of the table name
**      ('TblName') portion of a Full Table Name of the following
**      form: "ApplicationName.TblName"
**
**  \par Limits
**      Not Applicable
*/
#define CFE_TBL_MAX_NAME_LENGTH         16


/**
**  \cfemissioncfg cFE Message ID Base Numbers
**
**  \par Description:
**      Message Id base numbers for the cFE messages
**      NOTE: cFE MsgIds are the sum of the base numbers and the portable msg
**            numbers.
**
**  \par Limits
**      Must be less than CFE_SB_HIGHEST_VALID_MSGID
*/
#define CFE_CMD_MID_BASE_CPU1   0x1800
#define CFE_TLM_MID_BASE_CPU1   0x0800

#define CFE_CMD_MID_BASE_CPU2   0x1820
#define CFE_TLM_MID_BASE_CPU2   0x0820

#define CFE_CMD_MID_BASE_CPU3   0x1840
#define CFE_TLM_MID_BASE_CPU3   0x0840

#define CFE_CMD_MID_BASE_GLOB   0x1860
#define CFE_TLM_MID_BASE_GLOB   0x0860



/**
**  \cfemissioncfg cFE Portable Message Numbers for Commands
**
**  \par Description:
**      Portable message numbers for the cFE command messages
**      NOTE: cFE MsgIds are the sum of the base numbers and the portable msg
**            numbers.
**
**  \par Limits
**      Not Applicable
*/
#define CFE_EVS_CMD_MSG         1
    /* Offset 2 is available */
#define CFE_SB_CMD_MSG          3
#define CFE_TBL_CMD_MSG         4
#define CFE_TIME_CMD_MSG        5
#define CFE_ES_CMD_MSG          6

#define CFE_ES_SEND_HK_MSG      8
#define CFE_EVS_SEND_HK_MSG     9
    /* Offset 10 is available */
#define CFE_SB_SEND_HK_MSG      11
#define CFE_TBL_SEND_HK_MSG     12
#define CFE_TIME_SEND_HK_MSG    13

#define CFE_TIME_TONE_CMD_MSG   16
#define CFE_TIME_1HZ_CMD_MSG    17


/**
**  \cfemissioncfg cFE Portable Message Numbers for Global Messages
**
**  \par Description:
**      Portable message numbers for the cFE global messages
**      NOTE: cFE MsgIds are the sum of the base numbers and the portable msg
**            numbers.
**
**  \par Limits
**      Not Applicable
*/
#define CFE_TIME_DATA_CMD_MSG   0
#define CFE_TIME_FAKE_CMD_MSG   1
#define CFE_TIME_SEND_CMD_MSG   2


/**
**  \cfemissioncfg cFE Portable Message Numbers for Telemetry
**
**  \par Description:
**      Portable message numbers for the cFE telemetry messages
**      NOTE: cFE MsgIds are the sum of the base numbers and the portable msg
**            numbers.
**
**  \par Limits
**      Not Applicable
*/
#define CFE_ES_HK_TLM_MSG       0
#define CFE_EVS_HK_TLM_MSG      1
   /* Offset 2 is available */
#define CFE_SB_HK_TLM_MSG       3
#define CFE_TBL_HK_TLM_MSG      4
#define CFE_TIME_HK_TLM_MSG     5
#define CFE_TIME_DIAG_TLM_MSG   6

#define CFE_EVS_EVENT_MSG_MSG   8
#define CFE_SB_STATS_TLM_MSG    10
#define CFE_ES_APP_TLM_MSG      11
#define CFE_TBL_REG_TLM_MSG     12
#define CFE_SB_ALLSUBS_TLM_MSG  13
#define CFE_SB_ONESUB_TLM_MSG   14
#define CFE_ES_SHELL_TLM_MSG    15
#define CFE_ES_MEMSTATS_TLM_MSG 16

#endif
