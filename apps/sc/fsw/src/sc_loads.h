 /*************************************************************************
 ** File:
 **   $Id: sc_loads.h 1.10 2015/03/02 12:58:24EST sstrege Exp  $
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
 **   This file contains functions to handle validation of TBL tables,
 **   as well as setting up Stored Command's internal data structures for
 **   those tables
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sc_loads.h  $ 
 **   Revision 1.10 2015/03/02 12:58:24EST sstrege  
 **   Added copyright information 
 **   Revision 1.9 2010/09/28 10:44:09EDT lwalling  
 **   Update list of included header files, add ATS Append function prototypes 
 **   Revision 1.8 2010/05/18 15:31:38EDT lwalling  
 **   Change AtsId/RtsId to AtsIndex/RtsIndex or AtsNumber/RtsNumber 
 **   Revision 1.7 2010/05/05 11:23:24EDT lwalling  
 **   Move local function prototypes from header to source file 
 **   Revision 1.6 2010/04/21 15:44:03EDT lwalling  
 **   Changed SC_ProcessAppend arg from ATS ID to ATS index 
 **   Revision 1.5 2010/04/16 15:27:16EDT lwalling  
 **   Changed occasional use of ATS Append to more common Append ATS 
 **   Revision 1.4 2010/04/15 16:54:32EDT lwalling  
 **   Add entries for ATS Append functions 
 **   Revision 1.3 2010/04/05 11:52:33EDT lwalling  
 **   Create function prototypes for validate, update and process Append ATS tables 
 **   Revision 1.2 2009/01/05 08:26:53EST nyanchik  
 **   Check in after code review changes 
 *************************************************************************/
#ifndef _sc_loads_
#define _sc_loads_

#include "cfe.h"

/************************************************************************/
/** \brief Loads an ATS into the data structures in SC
 **  
 **  \par Description
 **         This routine is called when the SC app gets a new ATS table.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]    AtsIndex            ATS table array index (base zero)
 **
 **
 *************************************************************************/
void SC_LoadAts (uint16 AtsIndex);


/************************************************************************/
/** \brief Validation function for an ATS
 **  
 **  \par Description
 **              This routine is called from the cFE Table Services and passed
 **            as a parameter in the cFE Table Registration call.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \returns
 **  \retcode #CFE_SUCCESS         \retdesc \copydoc CFE_SUCCESS   \endcode 
 **  \retcode #SC_ERROR            \retdesc \copydoc SC_ERROR   \endcode
 **  \endreturns
 **
 *************************************************************************/
int32 SC_ValidateAts (void *TableData);


/************************************************************************/
/** \brief Validation function for the Append ATS Table
 **  
 **  \par Description
 **         This routine is called from the cFE Table Services as part of
 **         the table load/validate/commit process.  The function pointer
 **         is passed as a parameter in the cFE Table Registration call.
 **       
 **  \par Assumptions, External Events, and Notes:
 **         None
 **
 **  \returns
 **  \retcode #CFE_SUCCESS         \retdesc \copydoc CFE_SUCCESS   \endcode 
 **  \retcode #SC_ERROR            \retdesc \copydoc SC_ERROR   \endcode
 **  \endreturns
 **
 *************************************************************************/
int32 SC_ValidateAppend (void *TableData);


/************************************************************************/
/** \brief Updates Append ATS Info table per new contents of Append ATS table
 **  
 **  \par Description
 **         This routine is called when the SC app receives notification
 **         from cFE Table Services that the Append ATS table contents
 **         have been updated.
 **       
 **  \par Assumptions, External Events, and Notes:
 **         None
 **
 *************************************************************************/
void SC_UpdateAppend (void);


/************************************************************************/
/** \brief Appends contents of Append ATS table to indicated ATS table
 **  
 **  \par Description
 **         This routine is called from the Append ATS command handler to
 **         append the contents of the Append ATS table to the end of the
 **         indicated ATS table.
 **       
 **  \par Assumptions, External Events, and Notes:
 **         None
 **
 **  \param [in]    AtsIndex            ATS table array index (base zero)
 **
 *************************************************************************/
void SC_ProcessAppend (uint16 AtsIndex);


/************************************************************************/
/** \brief Loads an RTS into the data structures in SC
 **  
 **  \par Description
 **         This routine is called when the SC app gets a new RTS table.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]    RtsIndex            RTS table array index (base zero)
 **
 **
 *************************************************************************/
void SC_LoadRts (uint16 RtsIndex);


/************************************************************************/
/** \brief Validation function for an RTS
 **  
 **  \par Description
 **              This routine is called from the cFE Table Services and passed
 **            as a parameter in the cFE Table Registration call.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \returns
 **  \retcode #CFE_SUCCESS         \retdesc \copydoc CFE_SUCCESS   \endcode
 **  \retcode #SC_ERROR            \retdesc \copydoc SC_ERROR   \endcode
 **  \endreturns
 **
 *************************************************************************/
int32 SC_ValidateRts (void *TableData);


#endif /*_sc_loads_*/

/************************/
/*  End of File Comment */
/************************/
