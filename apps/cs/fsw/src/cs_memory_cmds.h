/************************************************************************
 ** File:
 **   $Id: cs_memory_cmds.h 1.3 2017/02/16 15:33:15EST mdeschu Exp  $
 **
 **   Copyright (c) 2007-2014 United States Government as represented by the 
 **   Administrator of the National Aeronautics and Space Administration. 
 **   All Other Rights Reserved.  
 **
 **   This software was created at NASA's Goddard Space Flight Center.
 **   This software is governed by the NASA Open Source Agreement and may be 
 **   used, distributed and modified only pursuant to the terms of that 
 **   agreement.
 **
 ** Purpose: 
 **   Specification for the CFS Memory cmds
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **   CFS CS Heritage Analysis Document
 **   CFS CS CDR Package
 **
 *************************************************************************/
#ifndef _cs_memory_cmds_
#define _cs_memory_cmds_

/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/
#include "cfe.h"

/************************************************************************/
/** \brief Process a disable background checking for the Memory
 **        table command
 **  
 **  \par Description
 **       Disables background checking for the Memory table 
 **       
 **  \par Assumptions, External Events, and Notes:
 **       In order for background checking of individual areas
 **       to checksum (OS code segment, cFE core, Eeprom, Memory,
 **       Apps, and Tables) to occur, the table must be enabled
 **       and overall checksumming must be enabled.
 **       
 **  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
 **                             references the software bus message 
 **
 **  \sa #CS_DISABLE_MEMORY_CC
 **
 *************************************************************************/
void CS_DisableMemoryCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Process an enable background checking for the Memory 
 **        table command 
 **  
 **  \par Description
 **       Allows the Memory table to be background checksummed.
 **
 **  \par Assumptions, External Events, and Notes:
 **       In order for background checking of individual areas
 **       to checksum (OS code segment, cFE core, Eeprom, Memory,
 **       Apps, and Tables) to occur, the table must be enabled
 **       and overall checksumming must be enabled.
 **       
 **  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
 **                             references the software bus message 
 **
 **  \sa #CS_ENABLE_MEMORY_CC
 **
 *************************************************************************/
void CS_EnableMemoryCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Proccess a report baseline of a Memory Entry command 
 **  
 **  \par Description
 **        Reports the baseline checksum of the specified Memory table
 **        entry if it has already been computed
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **       
 **  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
 **                             references the software bus message 
 **
 **  \sa #CS_REPORT_BASELINE_MEMORY_CC
 **
 *************************************************************************/
void CS_ReportBaselineEntryIDMemoryCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Process a disable background checking for an Memory 
 **        entry command 
 **  
 **  \par Description
 **       Disables the specified Memory entry to be background checksummed.
 **
 **  \par Assumptions, External Events, and Notes:
 **       In order for background checking of individual entries
 **       to checksum to occur, the entry must be enabled, 
 **       the table must be enabled, and overall checksumming must be enabled.
 **       This command updates both the results table and the definition table.
 **       If the entry exists in the results table but not in the definition
 **       table, the command is still successful, but the definition table
 **       is not updated.  If the entry does not exist in the results table, 
 **       neither table is updated.
 **       
 **  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
 **                             references the software bus message 
 **
 **  \sa #CS_DISABLE_ENTRY_MEMORY_CC
 **
 *************************************************************************/
void CS_DisableEntryIDMemoryCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Process a recopmute baseline of a Memory table entry command
 **  
 **  \par Description
 **        Recomputes the checksum of a Memory table entry and use that 
 **        value as the new baseline for that entry.
 **        
 **  \par Assumptions, External Events, and Notes:
 **       None
 **       
 **  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
 **                             references the software bus message 
 **
 **  \sa #CS_RECOMPUTE_BASELINE_MEMORY_CC
 **
 *************************************************************************/
void CS_RecomputeBaselineMemoryCmd (CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Process an enable background checking for an Memory 
 **        entry command 
 **  
 **  \par Description
 **       Allows the specified Memory entry to be background checksummed.
 **
 **  \par Assumptions, External Events, and Notes:
 **       In order for background checking of individual entries
 **       to checksum to occur, the entry must be enabled, 
 **       the table must be enabled and, overall checksumming must be enabled.
 **       This command updates both the results table and the definition table.
 **       If the entry exists in the results table but not in the definition
 **       table, the command is still successful, but the definition table
 **       is not updated.  If the entry does not exist in the results table, 
 **       neither table is updated.
 **       
 **  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
 **                             references the software bus message 
 **
 **  \sa #CS_ENABLE_ENTRY_MEMORY_CC
 **
 *************************************************************************/
void CS_EnableEntryIDMemoryCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Process a get Memory Entry by Address command 
 **  
 **  \par Description
 **       Send the entry ID of the specified address in an event message
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **       
 **  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
 **                             references the software bus message 
 **
 **  \sa #CS_GET_ENTRY_ID_MEMORY_CC
 **
 *************************************************************************/
void CS_GetEntryIDMemoryCmd(CFE_SB_MsgPtr_t MessagePtr);

#endif /* cs_memory_cmds */

/************************/
/*  End of File Comment */
/************************/
