 /*************************************************************************
 ** File:
 **   $Id: sc_atsrq.h 1.7 2015/03/02 12:58:51EST sstrege Exp  $
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
 **     This file contains header for the functions to handle all of the ATS
 **     executive requests and internal reuqests to control
 **     the ATP and ATSs.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sc_atsrq.h  $
 **   Revision 1.7 2015/03/02 12:58:51EST sstrege 
 **   Added copyright information
 **   Revision 1.6 2010/09/28 10:34:35EDT lwalling 
 **   Update list of included header files
 **   Revision 1.5 2010/04/21 15:39:30EDT lwalling 
 **   Moved prototype for SC_BeginAts to header file
 **   Revision 1.4 2010/04/19 10:48:25EDT lwalling 
 **   Added entry for Append ATS command handler
 **   Revision 1.3 2009/01/26 14:44:43EST nyanchik 
 **   Check in of Unit test
 **   Revision 1.2 2009/01/05 08:26:50EST nyanchik 
 **   Check in after code review changes
  *************************************************************************/

#ifndef _sc_atsreq_
#define _sc_atsreq_

#include "cfe.h"

/************************************************************************/
/** \brief  Starts an ATS
 **  
 **  \par Description
 **         This function starts an ATS by finding the first ATS command.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]          AtsId             The Ats to begin
 **
 **  
 **  \param [in]         TimeOffset         Where to start in the ATS
 ** 
 **  \retstmt Returns TRUE   if the ATS was started \endcode
 **  \retstmt Returns FALSE  if the ATS was NOT started \endcode
 **
 *************************************************************************/
boolean SC_BeginAts (uint16 AtsId, uint16 TimeOffset);

/************************************************************************/
/** \brief  Start an ATS Command
 **  
 **  \par Description
 **         This function starts an ATS on the ATP. This routine does
 **         not actually execute any commands, it simply sets up all
 **         of the data structures to indicate that the specified ATS
 **         is now running. This function also does all of the parameter
 **         checking.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         CmdPacket      a #CFE_SB_MsgPtr_t pointer that 
 **                                     references a software bus message 
 **
 **  \sa #SC_START_ATS_CC
 **
 *************************************************************************/
void SC_StartAtsCmd (CFE_SB_MsgPtr_t CmdPacket);

/************************************************************************/
/** \brief Stop the executing ATS Command
 **  
 **  \par Description
 **            This routine stops an ATS from executing on the ATP.
 **            This routine will execute even if an ATS is not currently
 **            executing in the buffer.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         CmdPacket      a #CFE_SB_MsgPtr_t pointer that 
 **                                     references a software bus message 
 **
 **  \sa #SC_STOP_ATS_CC
 **
 *************************************************************************/
void SC_StopAtsCmd (CFE_SB_MsgPtr_t CmdPacket);

/************************************************************************/
/** \brief Stops an ATS & clears out data
 **  
 **  \par Description
 **         This is a generic routine that is used to clear out the
 **            ATP information to stop an ATS.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **
 *************************************************************************/
void SC_KillAts (void);

/************************************************************************/
/** \brief  Switch the ATS Command
 **  
 **  \par Description
 **         This function initiates an ATS switch. An ATS switch cannot be
 **         immediatly started when the command is received because of the
 **         risk of sending out duplicate commands in the new buffer.
 **         (if buffer A has executed 3 of the 5 commands for second N, and
 **         the switch command is recvd at second N, the switch would
 **         happen in the same second, causing buffer B to execute all
 **         5 commands in second N , assuming that the buffers had
 **         an overlap of duplicate commands.)
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         CmdPacket      a #CFE_SB_MsgPtr_t pointer that 
 **                                     references a software bus message 
 **
 **  \sa #SC_SWITCH_ATS_CC
 **
 *************************************************************************/
void SC_GroundSwitchCmd (CFE_SB_MsgPtr_t CmdPacket);

/************************************************************************/
/** \brief
 **  
 **  \par Description
 **         This routine is called when the ATS IN-LINE request SWITCH
 **         ATS is encountered. This routine stops the current ATS from
 **         executing and starts the 'other' one. It is assumed that there
 **         is an ATS running because this command is only valid as an
 **         IN-LINE ATS request. 
 **       
 **       
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **
 **  \returns
 **  \retstmt Returns TRUE   if the switch was successful \endcode
 **  \retstmt Returns FALSE  if the switch was NOT successful \endcode 
 **  \endreturns
 **
 *************************************************************************/
boolean SC_InlineSwitch (void);

/************************************************************************/
/** \brief Switches ATS's at a safe time
 **  
 **  \par Description
 **       This function does the ATS switch when it is determined that
 **       the switch is "safe". When the switch request was made, the
 **       switch pend flag was set. After every scheduling of the SCP,
 **       the switch pend flag is checked. If the switch pend flag is
 **       set, this routine is called. This routine checks to see that
 **       the current time is one second past the time to start the
 **       new ATS. If it is the correct time, then the switch is performed.
 **       All of this has the effect of creating a syncronized switch of
 **       the ATS buffers, assuring that no duplicate commands are sent.
 **       
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **
 *************************************************************************/
void SC_ServiceSwitchPend (void);

/************************************************************************/
/** \brief Jump time in an ATS Command
 **  
 **  \par Description
 **         This command is used to jump to a specified time in the 
 **         currently running ATS. The jump command will effectively 
 **         restart the ATS at the time given in the command. Because 
 **         there is no restriction on the time given in the command, 
 **         the ATP may try to restart the ATS at any time before or 
 **         after the current time. In the case of the time tag being
 **         before the current time, ( a backwards jump ) the ATP will 
 **         simply skip the commands that have been executed ( or failed 
 **         execution ) and end up at the same location as before. In the 
 **         case of the jump time being after the current time, the ATP 
 **         will skip all commands with time tags less than the jump time 
 **         and start executing the ATS at the time equal to the jump 
 **         time. If there are no commands with time tags equal to the 
 **         jump time, the ATP will set up the ATS to wait for the first 
 **         command after the jump time. When a command is skipped while 
 **         doing the jump, the command's status is marked as SKIPPED unless
 **         it has already been marked as EXECUTED, FAILED_DISTRIBUTION, 
 **         or FAILED_CHECKSUM. 
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         CmdPacket      a #CFE_SB_MsgPtr_t pointer that 
 **                                     references a software bus message 
 **
 **  \sa #SC_JUMP_ATS_CC
 **
 *************************************************************************/
void SC_JumpAtsCmd (CFE_SB_MsgPtr_t CmdPacket);


/************************************************************************/
/** \brief Lets an ATS continue if a command failed the checksum
 **  
 **  \par Description
 **         This routine sets whether or not to let an ATS continue when
 **         one of the commands in the ATS fails a checksum validation
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         CmdPacket      a #CFE_SB_MsgPtr_t pointer that 
 **                                     references a software bus message 
 **
 **  \sa #SC_CONTINUE_ATS_ON_FAILURE_CC
 ** 
 **
 *************************************************************************/
void SC_ContinueAtsOnFailureCmd(CFE_SB_MsgPtr_t CmdPacket);


/************************************************************************/
/** \brief  Append to an ATS Command
 **  
 **  \par Description
 **         This function adds the contents of the Append ATS table to
 **         the selected ATS.  The ATS is then re-sorted for command
 **         execution order.  This command may target an ATS that is
 **         currently active (executing).  This command will not change
 **         the ATS execution state.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         CmdPacket      a #CFE_SB_MsgPtr_t pointer that 
 **                                     references a software bus message 
 **
 **  \sa #SC_APPEND_ATS_CC
 **
 *************************************************************************/
void SC_AppendAtsCmd (CFE_SB_MsgPtr_t CmdPacket);


#endif /* _sc_atsreq_ */

/************************/
/*  End of File Comment */
/************************/
