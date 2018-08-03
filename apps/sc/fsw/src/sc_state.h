 /*************************************************************************
 ** File:
 **   $Id: sc_state.h 1.4 2015/03/02 12:59:05EST sstrege Exp  $
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
 **   This file contains functions to handle getting the next time of
 **   commands for the ATP and RTP  as well as updating the time for
 **   Stored Command.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sc_state.h  $
 **   Revision 1.4 2015/03/02 12:59:05EST sstrege 
 **   Added copyright information
 **   Revision 1.3 2010/09/28 10:33:10EDT lwalling 
 **   Update list of included header files
 **   Revision 1.2 2009/01/05 08:26:58EST nyanchik 
 **   Check in after code review changes
 *************************************************************************/

#ifndef _sc_state_
#define _sc_state_

#include "cfe.h"

/************************************************************************/
/** \brief Gets the next time for an RTS command to run
 **  
 **  \par Description
 **         This function searches the RTS info table to find
 **         the next RTS that needs to run based on the time that the
 **         rts needs to run and it's priority.       
 **       
 **       
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 *************************************************************************/
void SC_GetNextRtsTime (void);

/************************************************************************/
/** \brief Decides whether the ATS or RTS runs next
 **  
 **  \par Description
 **         This function compares the next command times for the RTS
 **         and the ATS and decides which one to schedule next.
 **       
 **       
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 *************************************************************************/
void SC_UpdateNextTime (void);

/************************************************************************/
/** \brief Gets the next RTS command to run
 **  
 **  \par Description
 **         This routine is called when #SC_ProcessRtpCommand
 **         executes an RTS command and needs to get the next command in
 **         the buffer. This routine will get the next RTS command from the
 **         currently executing RTS on the active RTP. If this routine
 **         finds a fatal error with fetching the next RTS command or cannot
 **         find a next RTS command, then the sequence and RTP is stopped.
 **       
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 *************************************************************************/
void SC_GetNextRtsCommand (void);

/************************************************************************/
/** \brief Gets the next ATS command to run
 **  
 **  \par Description
 **         This routine gets the next ATS command from the currently
 **         executing ATS buffer. If there is no next ATS command then
 **         this routine will stop the currently running ATS.
 **       
 **       
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 *************************************************************************/
void SC_GetNextAtsCommand (void);



#endif /* _sc_state_ */

/************************/
/*  End of File Comment */
/************************/
