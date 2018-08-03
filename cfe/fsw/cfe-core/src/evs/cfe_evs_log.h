/*
**
**  Filename: cfe_evs_log.h
**    $Id: cfe_evs_log.h 1.5 2012/01/13 12:00:54GMT-05:00 acudmore Exp  $
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
**
**  Title:    Event Services API Log Control Interfaces.
**
**  Purpose: 
**            Unit specification for the event services log control interfaces.
**
**  Contents:
**       I.  macro and constant type definitions 
**      II.  EVM internal structures
**     III.  function prototypes
**
**  Design Notes: 
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**
**
**  Notes:
**
**
**  $Date: 2012/01/13 12:00:54GMT-05:00 $ 
**  $Revision: 1.5 $
**  $Log: cfe_evs_log.h  $
**  Revision 1.5 2012/01/13 12:00:54GMT-05:00 acudmore 
**  Changed license text to reflect open source
**  Revision 1.4 2011/05/23 15:51:06EDT lwalling 
**  Update list of included header files and function prototypes, delete unused macro definition
**  Revision 1.3 2011/04/05 16:33:43EDT lwalling 
**  Optimize EVS use of string functions, plus other performance improvements
**  Revision 1.2 2010/10/04 17:08:19EDT jmdagost 
**  Cleaned up copyright symbol.
**  Revision 1.1 2008/04/17 08:05:13EDT ruperera 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/evs/project.pj
**  Revision 1.10 2007/07/18 14:09:55EDT njyanchik 
**  I moved: CFE_EVS_DEBUG_BIT, CFE_EVS_INFORMATION_BIT, CFE_EVS_ERROR_BIT, CFE_EVS_CRITICAL_BIT, CFE_EVS_PORT1_BIT, CFE_EVS_PORT2_BIT, CFE_EVS_PORT3_BIT, CFE_EVS_PORT4_BIT, CFE_EVS_SHORT_FORMAT, CFE_EVS_LONG_FORMAT as well as 
**  
**  CFE_EVS_LOG_OVERWRITE and
**  CFE_EVS_LOG_DISCARD
**  
**  to cfe_evs_msg.h  because they are needed for commands
**  Revision 1.9 2007/05/14 10:26:02EDT apcudmore 
**  Preserve the EVS Local event log on a processor restart:
**  - moved the EVS Log to the ES reset data structure
**  - Added a couple of EVS Log related variables into the EVS_Log_t structure
**  - Added a routine to the EVS early init to get a pointer to the EVS Log 
**  - Added code to the EVS housekeeping service routine to copy Log variables into TLM
**  Revision 1.8 2007/03/02 10:59:57EST njyanchik 
**  The names of the command functions in cfe_evs_task.c didn't start with "CFE_", so that has been 
**  fixed.
**  Revision 1.7 2006/10/20 16:27:06EDT kkaudra 
**  Added cmd parameter for filenames
**  Revision 1.6 2006/06/08 14:14:52EDT njyanchik 
**  I added the appropriate legal headers to all of the evs files
**  Revision 1.5 2005/10/06 13:37:06EDT slstrege 
**  Delta code review actions
**  Revision 1.4 2005/07/29 19:51:44EDT sstrege 
**  EVS Code Review Action Items
**  Revision 1.3 2005/07/22 17:55:07EDT sstrege 
**  EVS Code Review Action Items
**  Revision 1.2 2005/07/12 12:29:49EDT sstrege 
**  Changed file path names to reflect new OSAL
**  Revision 1.1 2005/06/09 10:57:48EDT rperera 
**  Initial revision
**  Member added to project d:/mksdata/MKS-CFE-REPOSITORY/cfe-core/evs/project.pj
**  Revision 1.8  2005/04/27 16:26:07  sstrege
**  Modified command functions to return command status
**
**  Revision 1.7  2005/04/21 16:43:02  sstrege
**  Changed log file name to include path
**
**  Revision 1.5  2005/04/08 21:44:43  sstrege
**  fixed compiler errors
**
**  Revision 1.4  2005/04/08 20:47:53  sstrege
**  moved configuration parameters to cfe platform config file
**
**  Revision 1.3  2005/04/06 15:26:54  sstrege
**  Requirements update fixes
**
**  Revision 1.1  2005/03/10 23:42:31  sstrege
**  changed name of evslog.h to cfe_evs_log.h
**
**  Revision 1.1  2005/02/17 23:31:43  sstrege
**  Adding evslog.h
**
**  
**
**/

#ifndef _cfe_evs_log_
#define _cfe_evs_log_

/********************* Include Files  ************************/

#include "cfe_evs_msg.h"         /* EVS public definitions */

/* ==============   Section I: Macro and Constant Type Definitions   =========== */

/* ==============   Section II: Internal Structures ============ */    

/* ==============   Section III: Function Prototypes =========== */

void    EVS_AddLog ( CFE_EVS_Packet_t *EVS_PktPtr );
void    EVS_ClearLog ( void );
boolean CFE_EVS_WriteLogFileCmd (CFE_SB_MsgPayloadPtr_t Payload);
boolean CFE_EVS_SetLoggingModeCmd (CFE_SB_MsgPayloadPtr_t Payload);                          

#endif  /* _cfe_evs_log_ */
