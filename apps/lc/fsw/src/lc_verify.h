/*************************************************************************
** File:
**   $Id: lc_verify.h 1.2 2015/03/04 16:09:51EST sstrege Exp  $
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
**   Contains CFS Limit Checker (LC) macros that run preprocessor checks
**   on mission and platform configurable parameters
**
** Notes:
**
**   $Log: lc_verify.h  $
**   Revision 1.2 2015/03/04 16:09:51EST sstrege 
**   Added copyright information
**   Revision 1.1 2012/07/31 16:53:39EDT nschweis 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lcx/fsw/src/project.pj
**   Revision 1.3 2010/01/07 13:53:47EST lwalling 
**   Update LC configuration header files and configuration verification header file
**   Revision 1.2 2008/12/03 13:59:31EST dahardis 
**   Corrections from peer code review
**   Revision 1.1 2008/10/29 14:19:45EDT dahardison 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lc/fsw/src/project.pj
** 
*************************************************************************/
#ifndef _lc_verify_
#define _lc_verify_


/*************************************************************************
** Macro Definitions - defined in lc_mission_cfg.h
*************************************************************************/

  /*
  ** RTS request message ID 
  */  
  #ifndef LC_RTS_REQ_MID
    #error LC_RTS_REQ_MID must be defined!
  #elif LC_RTS_REQ_MID < 1
    #error LC_RTS_REQ_MID must be greater than zero
  #elif LC_RTS_REQ_MID > CFE_SB_HIGHEST_VALID_MSGID
    #error LC_RTS_REQ_MID must not exceed CFE_SB_HIGHEST_VALID_MSGID
  #endif 

  /*
  ** RTS request command code
  */  
  #ifndef LC_RTS_REQ_CC
    #error LC_RTS_REQ_CC must be defined!
  #elif LC_RTS_REQ_CC < 0
    #error LC_RTS_REQ_CC must not be less than zero
  #elif LC_RTS_REQ_CC > 127
    #error LC_RTS_REQ_CC must not exceed 127
  #endif 


/*************************************************************************
** Macro Definitions - defined in lc_platform_cfg.h
*************************************************************************/

  /*
  ** Application name
  */  
  #ifndef LC_APP_NAME
    #error LC_APP_NAME must be defined!
  #endif

  /*
  ** Command pipe depth
  */  
  #ifndef LC_PIPE_DEPTH
    #error LC_PIPE_DEPTH must be defined!
  #elif LC_PIPE_DEPTH < 1
    #error LC_PIPE_DEPTH must not be less than 1
  #elif LC_PIPE_DEPTH > 65535
    #error LC_PIPE_DEPTH must not exceed 65535
  #endif 

  /*
  ** Maximum number of watchpoints
  */  
  #ifndef LC_MAX_WATCHPOINTS
    #error LC_MAX_WATCHPOINTS must be defined!
  #elif LC_MAX_WATCHPOINTS < 1
    #error LC_MAX_WATCHPOINTS must not be less than 1
  #elif LC_MAX_WATCHPOINTS > 65520
    #error LC_MAX_WATCHPOINTS must not exceed 65520 (OxFFF0)
  #endif 

  /*
  ** Maximum number of actionpoints
  */  
  #ifndef LC_MAX_ACTIONPOINTS
    #error LC_MAX_ACTIONPOINTS must be defined!
  #elif LC_MAX_ACTIONPOINTS < 1
    #error LC_MAX_ACTIONPOINTS must not be less than 1
  #elif LC_MAX_ACTIONPOINTS > 65535
    #error LC_MAX_ACTIONPOINTS must not exceed 65535
  #endif 

  /*
  ** LC state after power-on reset
  */
  #ifndef LC_STATE_POWER_ON_RESET
    #error LC_STATE_POWER_ON_RESET must be defined!
  #elif (LC_STATE_POWER_ON_RESET != LC_STATE_ACTIVE)  && \
    (LC_STATE_POWER_ON_RESET != LC_STATE_PASSIVE) && \
    (LC_STATE_POWER_ON_RESET != LC_STATE_DISABLED)
    #error LC_STATE_POWER_ON_RESET must be defined as a supported enumerated type
  #endif

  /*
  ** LC state when CDS is restored
  */
  #ifndef LC_STATE_WHEN_CDS_RESTORED
    #error LC_STATE_WHEN_CDS_RESTORED must be defined!
  #elif (LC_STATE_WHEN_CDS_RESTORED != LC_STATE_ACTIVE)   && \
    (LC_STATE_WHEN_CDS_RESTORED != LC_STATE_PASSIVE)  && \
    (LC_STATE_WHEN_CDS_RESTORED != LC_STATE_DISABLED) && \
    (LC_STATE_WHEN_CDS_RESTORED != LC_STATE_FROM_CDS)
    #error LC_STATE_WHEN_CDS_RESTORED must be defined as a supported enumerated type
  #endif

  /*
  ** Default watchpoint definition table filename
  */  
  #ifndef LC_WDT_FILENAME
    #error LC_WDT_FILENAME must be defined!
  #endif

  /*
  ** Default actionpoint definition table filename
  */  
  #ifndef LC_ADT_FILENAME
    #error LC_ADT_FILENAME must be defined!
  #endif

  /*
  ** RPN equation buffer size (in 16 bit words)
  */  
  #ifndef LC_MAX_RPN_EQU_SIZE
    #error LC_MAX_RPN_EQU_SIZE must be defined!
  #elif LC_MAX_RPN_EQU_SIZE < 2
    #error LC_MAX_RPN_EQU_SIZE must not be less than 2
  #elif LC_MAX_RPN_EQU_SIZE > 32
    #error LC_MAX_RPN_EQU_SIZE must not exceed 32
  #endif

  /* 
  ** Maximum actionpoint event text string size
  */ 
  #ifndef LC_MAX_ACTION_TEXT
    #error LC_MAX_ACTION_TEXT must be defined!
  #elif LC_MAX_ACTION_TEXT < 0
    #error LC_MAX_ACTION_TEXT must not be less than zero
  #elif LC_MAX_ACTION_TEXT > CFE_EVS_MAX_MESSAGE_LENGTH
    #error LC_MAX_ACTION_TEXT must not exceed CFE_EVS_MAX_MESSAGE_LENGTH
  #endif

  /* Note: LC_AP_EVENT_TAIL_LEN is defined in lc_action.h */
  #ifndef LC_AP_EVENT_TAIL_LEN
    #error LC_AP_EVENT_TAIL_LEN must be defined!
  #elif LC_AP_EVENT_TAIL_LEN < 0
    #error LC_AP_EVENT_TAIL_LEN must not be less than zero
  #elif LC_AP_EVENT_TAIL_LEN > CFE_EVS_MAX_MESSAGE_LENGTH
    #error LC_AP_EVENT_TAIL_LEN must not exceed CFE_EVS_MAX_MESSAGE_LENGTH
  #endif

  #if (LC_MAX_ACTION_TEXT + LC_AP_EVENT_TAIL_LEN) > CFE_EVS_MAX_MESSAGE_LENGTH
    #error The sum of LC_MAX_ACTION_TEXT + LC_AP_EVENT_TAIL_LEN must not exceed CFE_EVS_MAX_MESSAGE_LENGTH
  #endif

  /*
  ** Maximum valid actionpoint definition table RTS ID 
  */  
  #ifndef LC_MAX_VALID_ADT_RTSID
    #error LC_MAX_VALID_ADT_RTSID must be defined!
  #elif LC_MAX_VALID_ADT_RTSID < 0
    #error LC_MAX_VALID_ADT_RTSID must not be less than zero
  #elif LC_MAX_VALID_ADT_RTSID > 65535
    #error LC_MAX_VALID_ADT_RTSID must not exceed 65535
  #endif 

  /*
  ** Application name
  */  
  #ifndef LC_APP_NAME
    #error LC_APP_NAME must be defined!
  #endif

#endif /*_lc_verify_*/

/************************/
/*  End of File Comment */
/************************/
