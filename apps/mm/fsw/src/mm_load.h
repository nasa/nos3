/*************************************************************************
** File:
**   $Id: mm_load.h 1.4 2015/03/02 14:27:03EST sstrege Exp  $
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
**   Specification for the CFS Memory Manager routines that process
**   memory load and fill ground commands
**
** References:
**   Flight Software Branch C Coding Standard Version 1.2
**   CFS MM Heritage Analysis Document
**   CFS MM CDR Package
**
** Notes:
**
**   $Log: mm_load.h  $
**   Revision 1.4 2015/03/02 14:27:03EST sstrege 
**   Added copyright information
**   Revision 1.3 2008/05/19 15:23:17EDT dahardison 
**   Version after completion of unit testing
** 
**************************************************************************/
#ifndef _mm_load_
#define _mm_load_

/*************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"

/*************************************************************************
** Exported Functions
*************************************************************************/
/************************************************************************/
/** \brief Process memory poke command
**  
**  \par Description
**       Processes the memory poke command that will load a memory
**       location with data specified in the command message.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #MM_POKE_CC
**
*************************************************************************/
void MM_PokeCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Process load memory with interrupts disabled command
**  
**  \par Description
**       Processes the load memory with interrupts disabled command
**       that will load up to #MM_MAX_UNINTERRUPTABLE_DATA bytes into
**       ram with interrupts disabled during the actual load
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #MM_LOAD_MEM_WID_CC
**
*************************************************************************/
void MM_LoadMemWIDCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Process memory load from file command
**  
**  \par Description
**       Processes the memory load from file command that will read a 
**       file and store the data in the command specified address range 
**       of memory.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #MM_LOAD_MEM_FROM_FILE_CC
**
*************************************************************************/
void MM_LoadMemFromFileCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Process memory fill command
**  
**  \par Description
**       Processes the memory fill command that will load an address
**       range of memory with the command specified fill pattern 
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #MM_FILL_MEM_CC
**
*************************************************************************/
void MM_FillMemCmd(CFE_SB_MsgPtr_t MessagePtr);

#endif /* _mm_load_ */

/************************/
/*  End of File Comment */
/************************/
