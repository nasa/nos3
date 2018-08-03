/*************************************************************************
** File:
**   $Id: lc_custom.h 1.2 2015/03/04 16:09:52EST sstrege Exp  $
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
**   Specification for the CFS Limit Checker (LC) mission specific
**   custom function template
**
** Notes:
**
**   $Log: lc_custom.h  $
**   Revision 1.2 2015/03/04 16:09:52EST sstrege 
**   Added copyright information
**   Revision 1.1 2012/07/31 16:53:38EDT nschweis 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lcx/fsw/src/project.pj
**   Revision 1.2 2008/12/03 13:59:40EST dahardis 
**   Corrections from peer code review
**   Revision 1.1 2008/10/29 14:19:12EDT dahardison 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lc/fsw/src/project.pj
** 
**************************************************************************/
#ifndef _lc_custom_
#define _lc_custom_

/*************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"

/*************************************************************************
** Exported Functions
*************************************************************************/

/************************************************************************/
/** \brief Execute RTS
**  
**  \par Description
**       Support function for actionpoint processing that is called
**       to send an RTS request when an actionpoint evaluation
**       determines it has failed
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   RTSId        ID of the RTS to request
**
*************************************************************************/
void LC_ExecuteRTS(uint16 RTSId);

/************************************************************************/
/** \brief Mission specific custom function
**  
**  \par Description
**       This is the mission specific custom function entry point.
**       It gets called whenever the OperatorID in a watchpoint
**       definition table entry is set to #LC_OPER_CUSTOM and 
**       must return one of the defined watchpoint evaluation
**       result types
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in] WatchIndex         The watchpoint number (zero based
**                                 watchpoint definition table index) for
**                                 the watchpoint definition that caused
**                                 the call
**
**  \param [in] ProcessedWPData    The watchpoint data extracted from
**                                 the message that it was contained
**                                 in. This is the data after any
**                                 sizing, bit-masking, and endianess
**                                 fixing that LC might have done
**                                 according to the watchpoint definition
**
**  \param [in] MessagePtr         A #CFE_SB_MsgPtr_t pointer that
**                                 references the software bus message that
**                                 contained the watchpoint data. If the
**                                 custom function needs the raw watchpoint
**                                 data, it can use this pointer and the
**                                 watchpoint definition to extract it.
**
**  \param [in] WDTCustomFuncArg   This is the custom function argument
**                                 for this watchpoint from the watchpoint
**                                 definition table. It can be used for
**                                 whatever purpose the mission developers
**                                 want. LC doesn't use it.
**
**  \returns
**  \retcode #LC_WATCH_TRUE   \retdesc \copydoc LC_WATCH_TRUE  \endcode
**  \retcode #LC_WATCH_FALSE  \retdesc \copydoc LC_WATCH_FALSE \endcode
**  \retcode #LC_WATCH_ERROR  \retdesc \copydoc LC_WATCH_ERROR \endcode
**
**  \sa #LC_WDTEntry_t
**
*************************************************************************/
uint8 LC_CustomFunction(uint16          WatchIndex,
                        uint32          ProcessedWPData,
                        CFE_SB_MsgPtr_t MessagePtr,
                        uint32          WDTCustomFuncArg);
 
#endif /* _lc_custom_ */

/************************/
/*  End of File Comment */
/************************/
