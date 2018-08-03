/*************************************************************************
** File:
**   $Id: mm_dump.h 1.4 2015/03/02 14:26:51EST sstrege Exp  $
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
**   Specification for the CFS Memory Manager memory dump ground commands.
**
** References:
**   Flight Software Branch C Coding Standard Version 1.2
**   CFS MM Heritage Analysis Document
**   CFS MM CDR Package
**
** Notes:
**
**   $Log: mm_dump.h  $
**   Revision 1.4 2015/03/02 14:26:51EST sstrege 
**   Added copyright information
**   Revision 1.3 2008/05/19 15:23:03EDT dahardison 
**   Version after completion of unit testing
** 
*************************************************************************/
#ifndef _mm_dump_
#define _mm_dump_

/*************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"

/************************************************************************
** Macro Definitions
*************************************************************************/
/*
** This macro defines the maximum number of bytes that can be dumped
** in an event message string based upon the setting of the 
** CFE_EVS_MAX_MESSAGE_LENGTH configuration parameter.
**
** The event message format is:
**    Message head "Memory Dump: "             13 characters
**    Message body "0xFF "                      5 characters per dump byte
**    Message tail "from address: 0xFFFFFFFF"  25 characters including NUL
*/
/**
** \name Maximum dump bytes in an event string */ 
/** \{ */
#define MM_MAX_DUMP_INEVENT_BYTES  ((CFE_EVS_MAX_MESSAGE_LENGTH - (13 + 25)) / 5)
/** \} */

/*
** This macro defines the size of the scratch buffer used to build
** the dump in event message string. Set it to the size of the
** largest piece shown above including room for a NUL terminator. 
*/
/**
** \name Dump in an event scratch string size */ 
/** \{ */
#define MM_DUMPINEVENT_TEMP_CHARS    25
/** \} */

/*************************************************************************
** Exported Functions
*************************************************************************/
/************************************************************************/
/** \brief Process memory peek command
**  
**  \par Description
**       Processes the memory peek command that will read a memory
**       location and report the data in an event message.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #MM_PEEK_CC
**
*************************************************************************/
void MM_PeekCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Process memory dump to file command
**  
**  \par Description
**       Processes the memory dump to file command that will read a 
**       address range of memory and store the data in a command
**       specified file.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #MM_DUMP_MEM_TO_FILE_CC
**
*************************************************************************/
void MM_DumpMemToFileCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Process memory dump in event command
**  
**  \par Description
**       Processes the memory dump in event command that will read  
**       up to #MM_MAX_DUMP_INEVENT_BYTES from memory and report
**       the data in an event message.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #MM_DUMP_IN_EVENT_CC, #MM_MAX_DUMP_INEVENT_BYTES
**
*************************************************************************/
void MM_DumpInEventCmd(CFE_SB_MsgPtr_t MessagePtr);

#endif /* _mm_dump_ */

/************************/
/*  End of File Comment */
/************************/
