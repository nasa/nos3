/*
** File: utf_osloader.h
**  $Id: utf_osloader.h 1.4 2010/11/24 17:11:00GMT-05:00 jmdagost Exp  $
**
**      Copyright (c) 2004-2006, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
** Purpose: This module defines the UTF symbol table structure used in the
**		    OS_SymbolLookup and OS_SymbolTableDump functions in utf_osloader.c
**
** References:
**
** Assumptions and Notes:
**
** $Date: 2010/11/24 17:11:00GMT-05:00 $
** $Revision: 1.4 $
** $Log: utf_osloader.h  $
** Revision 1.4 2010/11/24 17:11:00GMT-05:00 jmdagost 
** Added prototype for setting OS symbol table dump to file return status.
** Revision 1.3 2010/10/25 15:09:23EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.2 2010/10/04 15:05:02EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.1 2008/08/21 15:34:08EDT wfmoleski 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/inc/project.pj
*/

#ifndef _utf_osloader_
#define _utf_osloader_

/*
**   Include section
*/
#include "osconfig.h"

#define UTF_MAX_SYMBOLS		128

typedef struct
{
   char 	symbolName[OS_MAX_API_NAME];
   uint32   symbolAddr;
   boolean  RecordUsed;

} UTF_SymbolTable_t;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/*  UTF_InitSymbolTable()                                                  */
/*  Initialize the UTF static Symbol Table structure.                      */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void UTF_InitSymbolTable(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/*  UTF_SetSymbolTableEntry(UTF_SymbolTable_t)                             */
/*  Set the supplied entry in the Symbol Table if the arguments are valid  */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_SetSymbolTableEntry (UTF_SymbolTable_t entry);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/*  UTF_SetOSSymbolTableDumpStatus(int32)                                  */
/*  Set the return status for the for UTF OS_SymbolTableDump function      */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void UTF_SetOSSymbolTableDumpStatus (int32 status);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/*  UTF_DumpSymbolTable()                                                  */
/*  Print the Symbol Table to the standard output device.                  */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void UTF_DumpSymbolTable (void);

#endif
