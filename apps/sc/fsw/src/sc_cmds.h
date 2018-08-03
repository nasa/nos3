 /*************************************************************************
 ** File:
 **   $Id: sc_cmds.h 1.6 2015/03/02 12:59:03EST sstrege Exp  $
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
 **     This file contains functions to handle processing an RTS or ATS
 **     command as well as the generic ground commands
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sc_cmds.h  $ 
 **   Revision 1.6 2015/03/02 12:59:03EST sstrege  
 **   Added copyright information 
 **   Revision 1.5 2011/09/26 13:44:38EDT lwalling  
 **   Change function name from SC_SendHkStatus() to SC_SendHkPacket() 
 **   Revision 1.4 2010/09/28 10:43:26EDT lwalling  
 **   Update list of included header files, remove prototype for SC_GetTableAddresses() 
 **   Revision 1.3 2009/01/26 14:44:44EST nyanchik  
 **   Check in of Unit test 
 **   Revision 1.2 2009/01/05 08:26:51EST nyanchik  
 **   Check in after code review changes 
 *************************************************************************/
#ifndef _sc_cmds_
#define _sc_cmds_

#include "cfe.h"

/************************************************************************/
/** \brief Routes commands to be processed
 **  
 **  \par Description
 **           This routine determines the source of a request to
 **           the Stored Command processor and routes it to one of the lower
 **           level request processing routines
 **
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         CmdPacket      a #CFE_SB_MsgPtr_t pointer that 
 **                                     references a software bus message
 **
 *************************************************************************/
void SC_ProcessRequest (CFE_SB_MsgPtr_t CmdPacket);

/************************************************************************/
/** \brief Sends out an Event message
 **  
 **  \par Description
 **       Command for testing aliveness of SC
 **       
 **       
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         CmdPacket      a #CFE_SB_MsgPtr_t pointer that 
 **                                     references a software bus message 
 **
 **  \sa #SC_NOOP_CC
 **
 *************************************************************************/
void SC_NoOpCmd (CFE_SB_MsgPtr_t CmdPacket);

/************************************************************************/
/** \brief Reset Counters Command
 **  
 **  \par Description
 **       Clears the command counters and error counters for SC
 **       
 **       
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         CmdPacket      a #CFE_SB_MsgPtr_t pointer that 
 **                                     references a software bus message 
 **
 **  \sa #SC_RESET_COUNTERS_CC
 **
 *************************************************************************/
void SC_ResetCountersCmd (CFE_SB_MsgPtr_t CmdPacket);

/************************************************************************/
/** \brief Send Hk Packet to the ground 
 **  
 **  \par Description
 **       This routine collects the housekeeping status information,
 **       formats the packet and sends the packet over the software bus
 **       to health and safety.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None

 *************************************************************************/
void SC_SendHkPacket (void);

/************************************************************************/
/** \brief Process an ATS Command
 **  
 **  \par Description
 **       SC_ProcessAtpCmd takes ONE command from the current
 **       ATS buffer and executes it. It then figures out when the
 **       next command needs to execute and it returns. If for some
 **       reason the next ATS command cannot be found, then the
 **       ATS is stopped. If the command happens to be a Switch ATS command
 **       the command is executed locally instead of sending it out on the
 **       Software Bus.
 **             
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 *************************************************************************/
void SC_ProcessAtpCmd (void);

/************************************************************************/
/** \brief Process an RTS Command
 **  
 **  \par Description
 **        This routine processes ONE command from ONE active Relative
 **        Time Sequence. The command that has to be executed is already
 **        set up in the RTPs control block, It simply has to fetch the
 **        command, execute it, and get the next command. 
 **             
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 *************************************************************************/
void SC_ProcessRtpCommand (void);

#endif /*_sc_cmds_*/

/************************/
/*  End of File Comment */
/************************/
