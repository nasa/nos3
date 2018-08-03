/*************************************************************************
** File:
**   $Id: lc_cmds.h 1.2 2015/03/04 16:09:54EST sstrege Exp  $
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
**   handle command processing
**
** Notes:
**
**   $Log: lc_cmds.h  $
**   Revision 1.2 2015/03/04 16:09:54EST sstrege 
**   Added copyright information
**   Revision 1.1 2012/07/31 16:53:37EDT nschweis 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lcx/fsw/src/project.pj
**   Revision 1.5 2011/06/08 16:00:21EDT lwalling 
**   Remove prototype for LC_SubscribeWP()
**   Revision 1.4 2011/03/01 15:40:49EST lwalling 
**   Move prototypes for LC_SubscribeWP() and LC_UpdateTaskCDS() to lc_cmds.h
**   Revision 1.3 2011/03/01 09:37:46EST lwalling 
**   Modified table management logic and updates to CDS
**   Revision 1.2 2011/02/14 16:50:55EST lwalling 
**   Added prototypes for LC_ResetResultsAP() and LC_ResetResultsWP()
**   Revision 1.1 2008/10/29 14:19:06EDT dahardison 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lc/fsw/src/project.pj
** 
**************************************************************************/
#ifndef _lc_cmds_
#define _lc_cmds_

/*************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"

/*************************************************************************
** Exported Functions
*************************************************************************/
/************************************************************************/
/** \brief Process a command pipe message
**  
**  \par Description
**       Processes a single software bus command pipe message. Checks
**       the message and command IDs and calls the appropriate routine
**       to handle the message.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \returns
**  \retcode #CFE_SUCCESS   \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #LC_HousekeepingReq        \endcode
**  \endreturns
**
**  \sa #CFE_SB_RcvMsg
**
*************************************************************************/
int32 LC_AppPipe(CFE_SB_MsgPtr_t MessagePtr);
 
/************************************************************************/
/** \brief Reset HK counters
**  
**  \par Description
**       Utility function that resets housekeeping counters to zero
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \sa #LC_ResetCmd
**
*************************************************************************/
void LC_ResetCounters(void);

/************************************************************************/
/** \brief Reset AP results
**  
**  \par Description
**       Utility function that resets selected entries in actionpoint results table
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   StartIndex   Start of area to reset (base zero)
**  \param [in]   EndIndex     End of area to reset (base zero)
**  \param [in]   ResetCmd     Reset AP stats command does not reset all fields
**       
**  \sa #LC_ResetAPStatsCmd
**
*************************************************************************/
void LC_ResetResultsAP(uint32 StartIndex, uint32 EndIndex, boolean ResetCmd);

/************************************************************************/
/** \brief Reset WP results
**  
**  \par Description
**       Utility function that resets selected entries in watchpoint results table
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   StartIndex   Start of area to reset (base zero)
**  \param [in]   EndIndex     End of area to reset (base zero)
**  \param [in]   ResetCmd     Reset WP stats command does not reset all fields
**       
**  \sa #LC_ResetWPStatsCmd
**
*************************************************************************/
void LC_ResetResultsWP(uint32 StartIndex, uint32 EndIndex, boolean ResetCmd);

/************************************************************************/
/** \brief Write to Critical Data Store (CDS)
**  
**  \par Description
**       This function updates the CDS areas containing the watchpoint
**       results table, the actionpoint results table and the LC
**       application global data.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #CFE_ES_CopyToCDS  \endcode
**  \endreturns
**
*************************************************************************/
int32 LC_UpdateTaskCDS(void);

#endif /* _lc_cmds_ */

/************************/
/*  End of File Comment */
/************************/
