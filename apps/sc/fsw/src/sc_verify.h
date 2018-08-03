/*************************************************************************
 ** File:
 **   $Id: sc_verify.h 1.10 2015/03/02 12:58:36EST sstrege Exp  $
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
 **   Contains CFS Stored Command macros that run preprocessor checks
 **   on mission configurable parameters
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sc_verify.h  $
 **   Revision 1.10 2015/03/02 12:58:36EST sstrege 
 **   Added copyright information
 **   Revision 1.9 2011/09/26 13:53:39EDT lwalling 
 **   Remove verification of SC_SAVE_TO_CDS
 **   Revision 1.8 2011/09/23 14:28:00EDT lwalling 
 **   Made group commands conditional on configuration definition
 **   Revision 1.7 2010/10/08 13:20:03EDT lwalling 
 **   Move mission rev definition to platform config header file
 **   Revision 1.6 2010/04/05 11:56:00EDT lwalling 
 **   Add verification entries for Append ATS table definitions
 **   Revision 1.5 2010/03/11 16:28:34EST lwalling 
 **   Create table file name definition separate from table object name
 **   Revision 1.4 2010/03/11 10:13:05EST lwalling 
 **   Update verification tests and comments
 **   Revision 1.3 2009/01/26 14:47:16EST nyanchik 
 **   Check in of Unit test
 **   Revision 1.2 2009/01/05 08:27:00EST nyanchik 
 **   Check in after code review changes
 
 *************************************************************************/
#ifndef _sc_verify_
#define _sc_verify_

/*************************************************************************
 ** Includes
 *************************************************************************/

#include "cfe.h"
#include "sc_platform_cfg.h"

/*************************************************************************
 ** Macro Definitions
 *************************************************************************/

#ifndef SC_MAX_CMDS_PER_SEC
    #error SC_MAX_CMDS_PER_SEC must be defined!
#elif (SC_MAX_CMDS_PER_SEC > 65535)
    #error SC_MAX_CMDS_PER_SEC cannot be greater than 65535!
#elif (SC_MAX_CMDS_PER_SEC < 1)
    #error SC_MAX_CMDS_PER_SEC cannot be less than 1!
#endif 
 
#ifndef SC_NUMBER_OF_RTS
    #error SC_NUMBER_OF_RTS must be defined!
#elif (SC_NUMBER_OF_RTS > CFE_TBL_MAX_NUM_TABLES)
    #error SC_NUMBER_OF_RTS cannot be greater than CFE_TBL_MAX_NUM_TABLES!
#elif (SC_NUMBER_OF_RTS < 1)
    #error SC_NUMBER_OF_RTS cannot be less than 1!
#endif

/*
**  SC also has the following "dump only" tables..
**
**    RTS info table
**    RTS control block table
**    ATS info table
**    ATS control block table
**    ATS command status table
**
**  CFE_TBL_MAX_NUM_TABLES limits the sum of all tables from all apps.
*/
#if ((SC_NUMBER_OF_RTS + SC_NUMBER_OF_ATS + 5) > CFE_TBL_MAX_NUM_TABLES)
    #error Sum of all SC tables cannot be greater than CFE_TBL_MAX_NUM_TABLES!
#endif

#ifndef SC_ATS_BUFF_SIZE
    #error SC_ATS_BUFF_SIZE must be defined!
#elif (SC_ATS_BUFF_SIZE > 65535)
    #error SC_ATS_BUFF_SIZE cannot be greater than 65535!
#elif (SC_ATS_BUFF_SIZE < SC_PACKET_MIN_SIZE)
    #error SC_ATS_BUFF_SIZE must be at least big enough to hold one command (SC_PACKET_MAX_SIZE)!
    /* buf size = words, tbl size = bytes */
#elif ((SC_ATS_BUFF_SIZE * 2) > CFE_TBL_MAX_DBL_TABLE_SIZE)
    #error SC_ATS_BUFF_SIZE cannot be greater than CFE_TBL_MAX_DBL_TABLE_SIZE!
#endif 

#ifndef SC_APPEND_BUFF_SIZE
    #error SC_APPEND_BUFF_SIZE must be defined!
#elif (SC_APPEND_BUFF_SIZE > SC_ATS_BUFF_SIZE)
    #error SC_APPEND_BUFF_SIZE cannot be greater than SC_ATS_BUFF_SIZE!
#endif 

#ifndef SC_RTS_BUFF_SIZE
    #error SC_RTS_BUFF_SIZE must be defined!
#elif (SC_RTS_BUFF_SIZE > 65535) 
    #error  SC_RTS_BUFF_SIZE cannot be greater than 65535!
#elif (SC_RTS_BUFF_SIZE < SC_PACKET_MIN_SIZE)
    #error SC_RTS_BUFF_SIZE must be at least big enough to hold one command (SC_PACKET_MIN_SIZE)!
    /* buf size = words, tbl size = bytes */
#elif ((SC_RTS_BUFF_SIZE * 2) > CFE_TBL_MAX_SNGL_TABLE_SIZE) 
    #error  SC_RTS_BUFF_SIZE cannot be greater than CFE_TBL_MAX_SNGL_TABLE_SIZE!
#endif 

#ifndef SC_MAX_ATS_CMDS
    #error SC_MAX_ATS_CMDS must be defined!
#elif (SC_MAX_ATS_CMDS > 65535)
    #error  SC_MAX_ATS_CMDS cannot be greater than 65535!
#elif (SC_MAX_ATS_CMDS < 1)
    #error SC_MAX_ATS_CMDS cannot be less than 1!
#endif 

#ifndef SC_LAST_RTS_WITH_EVENTS
    #error SC_LAST_RTS_WITH_EVENTS must be defined!
#elif (SC_LAST_RTS_WITH_EVENTS > SC_NUMBER_OF_RTS)
    #error SC_LAST_RTS_WITH_EVENTS cannot be greater than SC_NUMBER_OF_RTS!
#elif (SC_LAST_RTS_WITH_EVENTS < 1)
    #error SC_LAST_RTS_WITH_EVENTS cannot be less than 1!
#endif 

#ifndef SC_PACKET_MIN_SIZE
    #error SC_PACKET_MIN_SIZE must be defined!
#elif (SC_PACKET_MIN_SIZE > CFE_SB_MAX_SB_MSG_SIZE)
    #error SC_PACKET_MIN_SIZE cannot be greater than CFE_SB_MAX_SB_MSG_SIZE!
    /* cannot use CFE_SB_CMD_HDR_SIZE in #if because it includes sizeof() */
#elif (SC_PACKET_MIN_SIZE < 8)
    #error SC_PACKET_MIN_SIZE cannot be less than CFE_SB_CMD_HDR_SIZE!
#endif 

#ifndef SC_PACKET_MAX_SIZE
    #error SC_PACKET_MAX_SIZE must be defined!
#elif (SC_PACKET_MAX_SIZE > CFE_SB_MAX_SB_MSG_SIZE) 
    #error SC_PACKET_MAX_SIZE cannot be greater than CFE_SB_MAX_SB_MSG_SIZE!
#elif (SC_PACKET_MAX_SIZE < SC_PACKET_MIN_SIZE)
    #error SC_PACKET_MAX_SIZE cannot be less than SC_PACKET_MIN_SIZE!
#endif 

#ifndef SC_PIPE_DEPTH
    #error SC_PIPE_DEPTH must be defined!
#elif (SC_PIPE_DEPTH > CFE_SB_MAX_PIPE_DEPTH)
    #error SC_PIPE_DEPTH cannot be greater than CFE_SB_MAX_PIPE_DEPTH!
#elif (SC_PIPE_DEPTH < 1)
    #error SC_PIPE_DEPTH cannot be less than 1!
#endif 

#ifndef SC_ATS_TABLE_NAME
    #error SC_ATS_TABLE_NAME must be defined!
#endif 

#ifndef SC_APPEND_TABLE_NAME
    #error SC_APPEND_TABLE_NAME must be defined!
#endif 

#ifndef SC_RTS_TABLE_NAME
    #error SC_RTS_TABLE_NAME must be defined!
#endif 

#ifndef SC_ATS_FILE_NAME
    #error SC_ATS_FILE_NAME must be defined!
#endif 

#ifndef SC_APPEND_FILE_NAME
    #error SC_APPEND_FILE_NAME must be defined!
#endif 

#ifndef SC_RTS_FILE_NAME
    #error SC_RTS_FILE_NAME must be defined!
#endif 

#ifndef SC_RTSINFO_TABLE_NAME
    #error SC_RTSINFO_TABLE_NAME must be defined!
#endif 

#ifndef SC_RTP_CTRL_TABLE_NAME
    #error SC_RTP_CTRL_TABLE_NAME must be defined!
#endif 

#ifndef SC_ATSINFO_TABLE_NAME
    #error SC_ATSINFO_TABLE_NAME must be defined!
#endif 

#ifndef SC_APPENDINFO_TABLE_NAME
    #error SC_APPENDINFO_TABLE_NAME must be defined!
#endif 

#ifndef SC_ATS_CTRL_TABLE_NAME
    #error SC_ATS_CTRL_TABLE_NAME must be defined!
#endif 

#ifndef SC_ATS_CMD_STAT_TABLE_NAME
    #error SC_ATS_CMD_STAT_TABLE_NAME must be defined!
#endif 

#ifndef SC_CONT_ON_FAILURE_START
    #error SC_CONT_ON_FAILURE_START must be defined!
#elif (SC_CONT_ON_FAILURE_START != TRUE)
  #if (SC_CONT_ON_FAILURE_START != FALSE)
    #error SC_CONT_ON_FAILURE_START must be either TRUE or FALSE!
  #endif 
#endif 

#ifndef SC_TIME_TO_USE
    #error SC_TIME_TO_USE must be defined!
#elif (SC_TIME_TO_USE != SC_USE_CFE_TIME)
  #if (SC_TIME_TO_USE != SC_USE_TAI)
    #if (SC_TIME_TO_USE != SC_USE_UTC)
      #error SC_TIME_TO_USE must be either SC_USE_CFE_TIME, SC_USE_TAI or SC_USE_UTC!
    #endif 
  #endif 
#endif 

#ifndef SC_ENABLE_GROUP_COMMANDS
    #error SC_ENABLE_GROUP_COMMANDS must be defined!
#elif (SC_ENABLE_GROUP_COMMANDS != TRUE)
  #if (SC_ENABLE_GROUP_COMMANDS != FALSE)
    #error SC_ENABLE_GROUP_COMMANDS must be either TRUE or FALSE!
  #endif 
#endif 

#ifndef SC_MISSION_REV
    #error SC_MISSION_REV must be defined!
#elif (SC_MISSION_REV < 0)
    #error SC_MISSION_REV must be greater than or equal to zero!
#endif 

#endif

/*_sc_verify_*/

/************************/
/*  End of File Comment */
/************************/
