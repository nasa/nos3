/*************************************************************************
** File:
**   $Id: hs_monitors.h 1.2 2015/11/12 14:25:13EST wmoleski Exp  $
**
**   Copyright © 2007-2014 United States Government as represented by the 
**   Administrator of the National Aeronautics and Space Administration. 
**   All Other Rights Reserved.  
**
**   This software was created at NASA's Goddard Space Flight Center.
**   This software is governed by the NASA Open Source Agreement and may be 
**   used, distributed and modified only pursuant to the terms of that 
**   agreement.
**
** Purpose:
**   Specification for the CFS Health and Safety (HS) routines that
**   handle application and event monitoring
**
** Notes:
**
**   $Log: hs_monitors.h  $
**   Revision 1.2 2015/11/12 14:25:13EST wmoleski 
**   Checking in changes found with 2010 vs 2009 MKS files for the cFS HS Application
**   Revision 1.4 2015/03/03 12:16:13EST sstrege 
**   Added copyright information
**   Revision 1.3 2010/09/29 18:27:51EDT aschoeni 
**   Added Utilization Monitoring
**   Revision 1.2 2009/05/04 17:44:30EDT aschoeni 
**   Updated based on actions from Code Walkthrough
**   Revision 1.1 2009/05/01 13:57:44EDT aschoeni 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hs/fsw/src/project.pj
**
**************************************************************************/
#ifndef _hs_monitors_h_
#define _hs_monitors_h_

/*************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"

/*************************************************************************
** Exported Functions
*************************************************************************/
/************************************************************************/
/** \brief Check execution status of each app in AppMon table
**
**  \par Description
**       Cycles through the Application Monitor Table checking the current
**       execution count for each monitored application. If the count fails
**       to increment for the table specified duration, the table specified
**       action is taken.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
void HS_MonitorApplications(void);

/************************************************************************/
/** \brief Search the EventMon table for matches to the incoming event
**
**  \par Description
**       Searches the Event Monitor Table for matches to the incoming
**       event message. If a match is found, the table specified action is
**       taken.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message
**
*************************************************************************/
void HS_MonitorEvent(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Monitor the utilization tracker counter
**
**  \par Description
**       Monitors the utilization tracker counter incremented by the Idle
**       Task, converting it into an estimated CPU Utilization for the
**       previous cycle. If the utilization is over a certain theshold
**       for a certain amount of time, an event is output.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
void HS_MonitorUtilization(void);

/************************************************************************/
/** \brief Validate application monitor table
**
**  \par Description
**       This function is called by table services when a validation of
**       the application monitor table is required
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   *TableData     Pointer to the table data to validate
**
**  \returns
**  \retcode #CFE_SUCCESS            \retdesc \copydoc CFE_SUCCESS            \endcode
**  \retcode #HS_AMTVAL_ERR_ACT      \retdesc \copydoc HS_AMTVAL_ERR_ACT      \endcode
**  \retcode #HS_AMTVAL_ERR_NUL      \retdesc \copydoc HS_AMTVAL_ERR_NUL      \endcode
**  \endreturns
**
**  \sa #HS_ValidateEMTable, #HS_ValidateXCTable, #HS_ValidateMATable
**
*************************************************************************/
int32 HS_ValidateAMTable(void *TableData);

/************************************************************************/
/** \brief Validate event monitor table
**
**  \par Description
**       This function is called by table services when a validation of
**       the event monitor table is required
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   *TableData     Pointer to the table data to validate
**
**  \returns
**  \retcode #CFE_SUCCESS            \retdesc \copydoc CFE_SUCCESS            \endcode
**  \retcode #HS_EMTVAL_ERR_ACT      \retdesc \copydoc HS_EMTVAL_ERR_ACT      \endcode
**  \retcode #HS_EMTVAL_ERR_NUL      \retdesc \copydoc HS_EMTVAL_ERR_NUL      \endcode
**  \endreturns
**
**  \sa #HS_ValidateAMTable, #HS_ValidateXCTable, #HS_ValidateMATable
**
*************************************************************************/
int32 HS_ValidateEMTable(void *TableData);

/************************************************************************/
/** \brief Validate execution counter table
**
**  \par Description
**       This function is called by table services when a validation of
**       the execution counter table is required
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   *TableData     Pointer to the table data to validate
**
**  \returns
**  \retcode #CFE_SUCCESS            \retdesc \copydoc CFE_SUCCESS            \endcode
**  \retcode #HS_XCTVAL_ERR_TYPE     \retdesc \copydoc HS_XCTVAL_ERR_TYPE     \endcode
**  \retcode #HS_XCTVAL_ERR_NUL      \retdesc \copydoc HS_XCTVAL_ERR_NUL      \endcode
**  \endreturns
**
**  \sa #HS_ValidateAMTable, #HS_ValidateEMTable, #HS_ValidateMATable
**
*************************************************************************/
int32 HS_ValidateXCTable(void *TableData);

/************************************************************************/
/** \brief Validate message actions table
**
**  \par Description
**       This function is called by table services when a validation of
**       the message actions table is required
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   *TableData     Pointer to the table data to validate
**
**  \returns
**  \retcode #CFE_SUCCESS            \retdesc \copydoc CFE_SUCCESS            \endcode
**  \retcode #HS_MATVAL_ERR_ID       \retdesc \copydoc HS_MATVAL_ERR_ID       \endcode
**  \retcode #HS_MATVAL_ERR_LEN      \retdesc \copydoc HS_MATVAL_ERR_LEN      \endcode
**  \retcode #HS_MATVAL_ERR_ENA      \retdesc \copydoc HS_MATVAL_ERR_ENA      \endcode
**  \endreturns
**
**  \sa #HS_ValidateAMTable, #HS_ValidateEMTable, #HS_ValidateXCTable
**
*************************************************************************/
int32 HS_ValidateMATable(void *TableData);

/************************************************************************/
/** \brief Update and store CDS data
**
**  \par Description
**       This function is called to update and then store the data in the
**       critical data store.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   ResetsPerformed     Number of HS caused processor resets
**  \param [in]   MaxResets           Max number of resets allowed
**
*************************************************************************/
void HS_SetCDSData(uint16 ResetsPerformed, uint16 MaxResets);

#endif /* _hs_monitors_h_ */

/************************/
/*  End of File Comment */
/************************/
