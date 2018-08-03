/*************************************************************************
** File:
**   $Id: lc_watch.h 1.2 2015/03/04 16:09:51EST sstrege Exp  $
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
**   Specification for the CFS Limit Checker (LC) routines that
**   handle watchpoint processing
**
** Notes:
**
**   $Log: lc_watch.h  $
**   Revision 1.2 2015/03/04 16:09:51EST sstrege 
**   Added copyright information
**   Revision 1.1 2012/07/31 16:53:40EDT nschweis 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lcx/fsw/src/project.pj
**   Revision 1.2 2011/06/08 15:59:08EDT lwalling 
**   Add prototype for LC_CreateHashTable()
**   Revision 1.1 2008/10/29 14:19:54EDT dahardison 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lc/fsw/src/project.pj
** 
**************************************************************************/
#ifndef _lc_watch_
#define _lc_watch_

/*************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"

/*************************************************************************
** Exported Functions
*************************************************************************/
/************************************************************************/
/** \brief Check message for watchpoints
**  
**  \par Description
**       Processes a single software bus command pipe message that
**       doesn't match any LC predefined command or message ids,
**       which indicates it's probably a watchpoint message.
**       It will search the watchpoint definition table for matches
**       to this MessageID and handle them as needed.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessageID    A #CFE_SB_MsgId_t that holds the
**                             message ID 
**
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #LC_ProcessWP
**
*************************************************************************/
void LC_CheckMsgForWPs(CFE_SB_MsgId_t  MessageID, 
                       CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Validate watchpoint definition table (WDT)
**  
**  \par Description
**       This function is called by table services when a validation of 
**       the watchpoint definition table is required
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   *TableData     Pointer to the table data to validate
**  
**  \returns
**  \retcode #CFE_SUCCESS            \retdesc \copydoc CFE_SUCCESS            \endcode
**  \retcode #LC_WDTVAL_ERR_DATATYPE \retdesc \copydoc LC_WDTVAL_ERR_DATATYPE \endcode
**  \retcode #LC_WDTVAL_ERR_OPER     \retdesc \copydoc LC_WDTVAL_ERR_OPER     \endcode
**  \retcode #LC_WDTVAL_ERR_MID      \retdesc \copydoc LC_WDTVAL_ERR_MID      \endcode
**  \retcode #LC_WDTVAL_ERR_FPNAN    \retdesc \copydoc LC_WDTVAL_ERR_FPNAN    \endcode
**  \retcode #LC_WDTVAL_ERR_FPINF    \retdesc \copydoc LC_WDTVAL_ERR_FPINF    \endcode
**  \endreturns
**
**  \sa #LC_ValidateADT
**
*************************************************************************/
int32 LC_ValidateWDT(void *TableData);

/************************************************************************/
/** \brief Create watchpoint hash table
**  
**  \par Description
**       Creates a hash table to optimize the process of getting direct
**       access to all the watchpoint table entries that reference a
**       particular MessageID without having to search the entire table.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \sa #LC_GetHashTableIndex, #LC_AddWatchpoint
**
*************************************************************************/
void LC_CreateHashTable(void);
 
#endif /* _lc_watch_ */

/************************/
/*  End of File Comment */
/************************/
