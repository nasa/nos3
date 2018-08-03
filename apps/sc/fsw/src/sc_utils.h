 /*************************************************************************
 ** File:
 **   $Id: sc_utils.h 1.5 2015/03/02 12:59:01EST sstrege Exp  $
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
 **   This file contains the utilty functions for Stored Command
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sc_utils.h  $
 **   Revision 1.5 2015/03/02 12:59:01EST sstrege 
 **   Added copyright information
 **   Revision 1.4 2010/09/28 10:32:30EDT lwalling 
 **   Update list of included header files
 **   Revision 1.3 2010/03/26 18:03:50EDT lwalling 
 **   Remove pad from ATS and RTS structures, change 32 bit ATS time to two 16 bit values
 **   Revision 1.2 2009/01/05 08:27:00EST nyanchik 
 **   Check in after code review changes
 *************************************************************************/
#ifndef _sc_utils_
#define _sc_utils_

#include "cfe.h"
#include "sc_app.h"

/************************************************************************/
/** \brief Gets the current time from CFE
 **  
 **  \par Description
 **       Queries the CFE TIME services and retieves the Current time
 **       
 **       
 **  \par Assumptions, External Events, and Notes:
 **        This routine stores the time in #SC_AppData
 **
 **
 *************************************************************************/
void SC_GetCurrentTime (void);

SC_AbsTimeTag_t SC_GetAtsEntryTime (SC_AtsEntryHeader_t *Entry);


/************************************************************************/
/** \brief Computes an absolute time from relative time
 **  
 **  \par Description
 **       This function computes an absolute time from 'now' and the 
 **       relative time passed into the function
 **       
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]        RelTime         The relative time to compute from
 **
 **  \returns
 **  \retstmt Returns the computed absolute time   \endcode
 **  \endreturns
 **
 *************************************************************************/
SC_AbsTimeTag_t SC_ComputeAbsTime (uint16 RelTime);

/************************************************************************/
/** \brief Compares absolute time
 **  
 **  \par Description
 **       
 **       This function compares two absolutes time. 
 **       
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]    AbsTime1            The first time to compare
 **
 **  \param [in]    AbsTime2            The second time to compare
 **
 **  \returns
 **  \retstmt Returns TRUE  if AbsTime1 is greater than AbsTime2 \endcode
 **  \retstmt Returns FALSE  if AbsTime1 is less than AbsTime2 \endcode
 **  \endreturns
 **
 *************************************************************************/

boolean SC_CompareAbsTime (SC_AbsTimeTag_t AbsTime1, SC_AbsTimeTag_t AbsTime2);

/************************************************************************/
/** \brief Verify command message length
 **  
 **  \par Description
 **       This routine will check if the actual length of a software bus
 **       command message matches the expected length and send an
 **       error event message if a mismatch occurs
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **       
 **  \param [in]   msg              A #CFE_SB_MsgPtr_t pointer that
 **                                 references the software bus message 
 **
 **  \param [in]   ExpectedLength   The expected length of the message
 **                                 based upon the command code
 **
 **  \returns
 **  \retstmt Returns TRUE if the length is as expected      \endcode
 **  \retstmt Returns FALSE if the length is not as expected \endcode
 **  \endreturns
 **
 **  \sa #SC_LEN_ERR_EID
 **
 *************************************************************************/
boolean SC_VerifyCmdLength(CFE_SB_MsgPtr_t msg, 
                           uint16          ExpectedLength);

#endif /*_sc_utils_*/

/************************/
/*  End of File Comment */
/************************/
