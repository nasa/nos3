/*************************************************************************
** File:
**   $Id: lc_action.h 1.2 2015/03/04 16:09:56EST sstrege Exp  $
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
**   handle actionpoint processing
**
** Notes:
**
**   $Log: lc_action.h  $
**   Revision 1.2 2015/03/04 16:09:56EST sstrege 
**   Added copyright information
**   Revision 1.1 2012/07/31 16:53:36EDT nschweis 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lcx/fsw/src/project.pj
**   Revision 1.2 2011/02/07 17:58:22EST lwalling 
**   Modify sample AP commands to target groups of AP's
**   Revision 1.1 2008/10/29 14:18:48EDT dahardison 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lc/fsw/src/project.pj
** 
**************************************************************************/
#ifndef _lc_action_
#define _lc_action_

/*************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"

/************************************************************************
** Macro Definitions
*************************************************************************/
/**
** \name LC Actionpoint Event Trailer */ 
/** \{ */
#define LC_AP_EVENT_TAIL_STR       ": AP = %d, FailCount = %d, RTS = %d"

#define LC_AP_EVENT_TAIL_LEN       36   /**< \brief Length of string  
                                                    including NUL. 
                                                    Needed by LC_verify.h */
/** \} */

/*************************************************************************
** Exported Functions
*************************************************************************/
/************************************************************************/
/** \brief Sample actionpoints
**  
**  \par Description
**       Support function for #LC_SampleAPReq that will sample the
**       selected actionpoints.  The start and end arguments define
**       which actionpoint(s) the command will sample.  If both the
**       start and end arguments are set to #LC_ALL_ACTIONPOINTS,
**       the command will be interpreted as a request to sample all
**       actionpoints (heritage).  Otherwise, the start index must
**       be less than or equal to the end index, and both must be
**       within the bounds of the actionpoint table.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   StartIndex   The first actionpoint to sample
**                             (zero based actionpoint table index)
**
**  \param [in]   EndIndex     The last actionpoint to sample
**                             (zero based actionpoint table index)
**
**  \sa #LC_SampleAPReq
**
*************************************************************************/
void LC_SampleAPs(uint16 StartIndex, uint16 EndIndex);

/************************************************************************/
/** \brief Validate actionpoint definition table (ADT)
**  
**  \par Description
**       This function is called by table services when a validation of 
**       the actionpoint definition table is required
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   *TableData     Pointer to the table data to validate
**  
**  \returns
**  \retcode #CFE_SUCCESS            \retdesc \copydoc CFE_SUCCESS            \endcode
**  \retcode #LC_ADTVAL_ERR_DEFSTATE \retdesc \copydoc LC_ADTVAL_ERR_DEFSTATE \endcode
**  \retcode #LC_ADTVAL_ERR_RTSID    \retdesc \copydoc LC_ADTVAL_ERR_RTSID    \endcode
**  \retcode #LC_ADTVAL_ERR_FAILCNT  \retdesc \copydoc LC_ADTVAL_ERR_FAILCNT  \endcode
**  \retcode #LC_ADTVAL_ERR_EVTTYPE  \retdesc \copydoc LC_ADTVAL_ERR_EVTTYPE  \endcode
**  \retcode #LC_ADTVAL_ERR_RPN      \retdesc \copydoc LC_ADTVAL_ERR_RPN      \endcode
**  \endreturns
**
**  \sa #LC_ValidateWDT
**
*************************************************************************/
int32 LC_ValidateADT(void *TableData);
 
#endif /* _lc_action_ */

/************************/
/*  End of File Comment */
/************************/
