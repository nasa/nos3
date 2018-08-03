 /*************************************************************************
 ** File:
 **   $Id: sc_rtsrq.h 1.1.1.6.1.1 2015/03/02 13:05:10EST sstrege Exp  $
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
 **     This file contains the headers to handle all of the RTS
 **     executive requests and internal reuqests to control
 **     the RTP and RTSs.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sc_rtsrq.h  $
 **   Revision 1.1.1.6.1.1 2015/03/02 13:05:10EST sstrege 
 **   Added copyright information
 **   Revision 1.1.1.6 2011/09/23 14:27:21EDT lwalling 
 **   Made group commands conditional on configuration definition
 **   Revision 1.1.1.5 2011/03/14 10:53:44EDT lwalling 
 **   Add new prototypes -- SC_StartRtsGrpCmd(), SC_StopRtsGrpCmd(), SC_DisableGrpCmd(), SC_EnableGrpCmd().
 **   Revision 1.1.1.4 2010/09/28 10:33:09EDT lwalling 
 **   Update list of included header files
 **   Revision 1.1.1.3 2010/05/18 15:30:38EDT lwalling 
 **   Change AtsId/RtsId to AtsIndex/RtsIndex or AtsNumber/RtsNumber
 **   Revision 1.1.1.2 2009/01/05 07:36:43EST nyanchik 
 **   Initial revision
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sc/fsw/src/project.pj
 *************************************************************************/

#ifndef _sc_rtsreq_
#define _sc_rtsreq_

#include "cfe.h"

/************************************************************************/
/** \brief Start an RTS Command
 **  
 **  \par Description
 **             This routine starts the execution of an RTS.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         CmdPacket      a #CFE_SB_MsgPtr_t pointer that 
 **                                     references a software bus message 
 **
 **  \sa #SC_START_RTS_CC
 **
 *************************************************************************/
void SC_StartRtsCmd (CFE_SB_MsgPtr_t CmdPacket);

#if (SC_ENABLE_GROUP_COMMANDS == TRUE)
/************************************************************************/
/** \brief Start a group of RTS Command
 **  
 **  \par Description
 **             This routine starts the execution of a group of RTS.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         CmdPacket      a #CFE_SB_MsgPtr_t pointer that 
 **                                     references a software bus message 
 **
 **  \sa #SC_START_RTSGRP_CC
 **
 *************************************************************************/
void SC_StartRtsGrpCmd (CFE_SB_MsgPtr_t CmdPacket);
#endif

/************************************************************************/
/** \brief  Stop an RTS from executing Command
 **  
 **  \par Description
 **             This routine stops the execution of an RTS.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         CmdPacket      a #CFE_SB_MsgPtr_t pointer that 
 **                                     references a software bus message 
 **
 **  \sa #SC_STOP_RTS_CC
 **
 ************************************************************************/
void SC_StopRtsCmd (CFE_SB_MsgPtr_t CmdPacket);

#if (SC_ENABLE_GROUP_COMMANDS == TRUE)
/************************************************************************/
/** \brief  Stop a group of RTS from executing Command
 **  
 **  \par Description
 **             This routine stops the execution of a group of RTS.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         CmdPacket      a #CFE_SB_MsgPtr_t pointer that 
 **                                     references a software bus message 
 **
 **  \sa #SC_STOP_RTS_CC
 **
 ************************************************************************/
void SC_StopRtsGrpCmd (CFE_SB_MsgPtr_t CmdPacket);
#endif

/************************************************************************/
/** \brief Disable an RTS Command
 **  
 **  \par Description
 **             This routine disables an enabled RTS.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         CmdPacket      a #CFE_SB_MsgPtr_t pointer that 
 **                                     references a software bus message 
 **
 **  \sa #SC_DISABLE_RTS_CC
 **
 *************************************************************************/
void SC_DisableRtsCmd (CFE_SB_MsgPtr_t CmdPacket);

#if (SC_ENABLE_GROUP_COMMANDS == TRUE)
/************************************************************************/
/** \brief Disable a group of RTS Command
 **  
 **  \par Description
 **             This routine disables a group of enabled RTS.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         CmdPacket      a #CFE_SB_MsgPtr_t pointer that 
 **                                     references a software bus message 
 **
 **  \sa #SC_DISABLE_RTS_CC
 **
 *************************************************************************/
void SC_DisableRtsGrpCmd (CFE_SB_MsgPtr_t CmdPacket);
#endif

/************************************************************************/
/** \brief Enable an RTS Command
 **  
 **  \par Description
 **             This routine enables a disabled RTS.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         CmdPacket      a #CFE_SB_MsgPtr_t pointer that 
 **                                     references a software bus message 
 **
 **  \sa #SC_ENABLE_RTS_CC
 **
 *************************************************************************/
void SC_EnableRtsCmd (CFE_SB_MsgPtr_t CmdPacket);

#if (SC_ENABLE_GROUP_COMMANDS == TRUE)
/************************************************************************/
/** \brief Enable a group of RTS Command
 **  
 **  \par Description
 **             This routine enables a group of disabled RTS.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         CmdPacket      a #CFE_SB_MsgPtr_t pointer that 
 **                                     references a software bus message 
 **
 **  \sa #SC_ENABLE_RTSGRP_CC
 **
 *************************************************************************/
void SC_EnableRtsGrpCmd (CFE_SB_MsgPtr_t CmdPacket);
#endif

/************************************************************************/
/** \brief Stops an RTS & clears out data
 **  
 **  \par Description
 **      This is a generic routine to stop an RTS
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         RtsIndex       RTS index to kill (base zero) 
 **
 *************************************************************************/
void SC_KillRts (uint16 RtsIndex);

/************************************************************************/
/** \brief Automatically starts an RTS
 **  
 **  \par Description
 **        This function sends a command back to the SC app to
 **        start the RTS designated as the auto-start RTS (usually 1)
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         RtsNumber      RTS number to start (base one) 
 **
 *************************************************************************/
void SC_AutoStartRts (uint16 RtsNumber);

#endif /* _sc_rtsreq_ */

/************************/
/*  End of File Comment */
/************************/
