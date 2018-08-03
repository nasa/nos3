/************************************************************************
** File:
**   $Id: hk_msgdefs.h 1.4 2015/03/04 14:58:28EST sstrege Exp  $
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
**  The CFS Housekeeping (HK) Application header file
**
** Notes:
**
** $Log: hk_msgdefs.h  $
** Revision 1.4 2015/03/04 14:58:28EST sstrege 
** Added copyright information
** Revision 1.3 2011/11/30 16:01:06EST jmdagost 
** Updated doxygen comments for Reset Counters command.
** Revision 1.2 2009/12/03 18:10:24EST jmdagost 
** Updated the comments for the no-op and reset counters commands.
** Revision 1.1 2009/12/03 16:45:37EST jmdagost 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hk/fsw/src/project.pj
**
*************************************************************************/
#ifndef _hk_msgdefs_h_
#define _hk_msgdefs_h_


/*************************************************************************
** Includes
**************************************************************************/
#include "cfe.h"


/****************************************
** HK app command packet command codes
****************************************/

/** \hkcmd Housekeeping No-Op
**
**  \par Description
**       This command will increment the command execution counter and send an
**       event containing the version number of the application
**
**  \hkcmdmnemonic \HK_NOOP
**
**  \par Command Structure
**       #CFE_SB_CmdHdr_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \HK_CMDPC - command execution counter will
**         increment
**       - The #HK_NOOP_CMD_EID informational event message will
**         be generated
**
**  \par Error Conditions
**       There are no error conditions for this command. If the Housekeeping
**       app receives the command, the event is sent (although it
**       may be filtered by EVS) and the counter is incremented
**       unconditionally.
**
**  \par Criticality
**       None
**
**  \sa
*/
#define HK_NOOP_CC                          0


/** \hkcmd Housekeeping Reset Counters
**
**  \par Description
**       This command resets the following counters within the HK
**        housekeeping telemetry:
**       - Command Execution Counter (\HK_CMDPC)
**       - Command Error Counter (\HK_CMDEC)
**       - Combined Packets Sent Counter (\HK_CMBPKTSSENT)
**       - Missing Data Counter (\HK_MISSDATACTR)
**
**  \hkcmdmnemonic \HK_RESETCTRS
**
**  \par Command Structure
**       #CFE_SB_CmdHdr_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \HK_CMDPC - command execution counter will be reset
**       - \b \c \HK_CMDEC - command error counter will be reset
**       - \b \c \HK_CMBPKTSSENT - combined packets sent counter will be reset
**       - \b \c \HK_MISSDATACTR - missing data counter will be reset
**       - The #HK_RESET_CNTRS_CMD_EID informational event message will
**         be generated
**
**  \par Error Conditions
**       There are no error conditions for this command. If the Housekeeping
**       App receives the command, the event is sent (although it
**       may be filtered by EVS) and the counter is incremented
**       unconditionally.
**
**  \par Criticality
**       This command is not inherently dangerous.  However, it is
**       possible for ground systems and on-board safing procedures
**       to be designed such that they react to changes in the counter
**       values that are reset by this command.
**
**  \sa
*/
#define HK_RESET_CC                         1

      
#endif /* _hk_msgdefs_h_ */

/************************/
/*  End of File Comment */
/************************/
