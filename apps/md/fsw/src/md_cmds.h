/*************************************************************************
** File:
**   $Id: md_cmds.h 1.3 2015/03/01 17:17:37EST sstrege Exp  $
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
**   Specification for the CFS Memory Dwell ground commands.
**
**
** Notes:
**
**   $Log: md_cmds.h  $
**   Revision 1.3 2015/03/01 17:17:37EST sstrege 
**   Added copyright information
**   Revision 1.2 2009/04/18 15:08:15EDT dkobe 
**   Corrected comment for function parameter
**   Revision 1.1 2008/07/02 13:47:14EDT nsschweiss 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/md/fsw/src/project.pj
** 
*************************************************************************/
/*
** Ensure that header is included only once...
*/
#ifndef _md_cmds_h_
#define _md_cmds_h_

/*************************************************************************
** Includes
*************************************************************************/

/* md_msg needs to be included for MD_SymAddr_t definition */ 
#include "md_msg.h"

/*****************************************************************************/
/**
** \brief Process Memory Dwell Start Command
**
** \par Description
**          Extract command arguments, take appropriate actions,
**          issue event, and increment the command counter or 
**          error counter as appropriate.
** 
** \par Assumptions, External Events, and Notes:
**          Correct message length has been verified.
**
** \param[in] MessagePtr a pointer to the message received from the command pipe
**                                      
** \retval None
******************************************************************************/
void MD_ProcessStartCmd(CFE_SB_MsgPtr_t MessagePtr);


/*****************************************************************************/
/**
** \brief Stop dwell table.
**
** \par Description
**          Stop specified table.
** 
** \par Assumptions, External Events, and Notes:
**          Correct message length has been verified.
**
** \param[in] TableId identifier.  (1.. MD_NUM_DWELL_TABLES)
**                                      
** \retval None
******************************************************************************/
void MD_StopTable(int16 TableId);


/*****************************************************************************/
/**
** \brief Process Memory Dwell Stop Command
**
** \par Description
**          Extract command arguments, take appropriate actions,
**          issue event, and increment the command counter or 
**          error counter as appropriate.
** 
** \par Assumptions, External Events, and Notes:
**          Correct message length has been verified.
**
** \param[in] MessagePtr a pointer to the message received from the command pipe
**                                      
** \retval None
******************************************************************************/
void MD_ProcessStopCmd(CFE_SB_MsgPtr_t MessagePtr);


/*****************************************************************************/
/**
** \brief Process Memory Dwell Jam Command
**
** \par Description
**          Extract command arguments, take appropriate actions,
**          issue event, and increment the command counter or 
**          error counter as appropriate.
** 
** \par Assumptions, External Events, and Notes:
**          Correct message length has been verified.
**
** \param[in] MessagePtr a pointer to the message received from the command pipe
**                                      
** \retval None
******************************************************************************/
void MD_ProcessJamCmd(CFE_SB_MsgPtr_t MessagePtr);

/*****************************************************************************/
/**
** \brief Process Set Signature Command
**
** \par Description
**          Extract command arguments, take appropriate actions,
**          issue event, and increment the command counter or 
**          error counter as appropriate.
** 
** \par Assumptions, External Events, and Notes:
**          Correct message length has been verified.
**
** \param[in] MessagePtr a pointer to the message received from the command pipe
**                                      
** \retval None
******************************************************************************/
void MD_ProcessSignatureCmd(CFE_SB_MsgPtr_t MessagePtr);


#endif /* _md_cmds_ */
/************************/
/*  End of File Comment */
/************************/
