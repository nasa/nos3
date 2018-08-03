/*
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement.
**
** File   : osloader.c
** Author : Alan Cudmore
** Purpose: This file contains the module loader and symbol lookup functions for the OSAL.
*/

/****************************************************************************************
                                    INCLUDE FILES
****************************************************************************************/

#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"

#include "common_types.h"
#include "osapi.h"
#include "utf_types.h"
#include "utf_osloader.h"

static UTF_SymbolTable_t symbolTbl[UTF_MAX_SYMBOLS];
static int32 symbolTableDumpStatus = OS_SUCCESS;

/****************************************************************************************
                                    UTF Symbol table API
****************************************************************************************/
/*--------------------------------------------------------------------------------------
    Name: UTF_InitSymbolTable
    
    Purpose: Initializes the symbol table 

---------------------------------------------------------------------------------------*/
void UTF_InitSymbolTable (void)
{
int i;

	for (i=0; i<UTF_MAX_SYMBOLS; i++)
	{
		symbolTbl[i].symbolName[0] = '\0';
		symbolTbl[i].symbolAddr = 0;
		symbolTbl[i].RecordUsed = FALSE;
	}
}

/*--------------------------------------------------------------------------------------
    Name: UTF_SetSymbolTableEntry
    
    Purpose: Set the next available entry to the contents of the argument 

    Parameters: UTF_SymbolTable_t - the entry contents to copy to the Symbol Table

    Returns: OS_ERROR if the symbol could not be added to the Symbol Table
             OS_SUCCESS if the symbol was copied to the symbol table 
             OS_INVALID_POINTER if the symbol name is null
             OS_ERR_NAME_TOO_LONG if the supplied symbol name contains more characters
                                  than allowed
             
             The address of the symbol will be stored in the pointer that is passed in.
---------------------------------------------------------------------------------------*/
int32 UTF_SetSymbolTableEntry (UTF_SymbolTable_t entry)
{
int32 returnStatus=OS_SUCCESS;
boolean openSlotFound=FALSE;
int i;

	/* Check argument validity */
	if (strlen(entry.symbolName) == 0)
	{
    	UTF_put_text("UTF Info: Null string passed to UTF_SetSymbolTableEntry\n");
 		return OS_INVALID_POINTER;
	}

	if (strlen(entry.symbolName) >= OS_MAX_API_NAME)
	{
    	UTF_put_text("UTF Info: Symbol Name too long\n");
		return (OS_ERR_NAME_TOO_LONG);
	}
	
	/* Copy the entry into the next available slot */
	for (i=0; i<UTF_MAX_SYMBOLS; i++)
	{
  		if (symbolTbl[i].RecordUsed == FALSE)
  		{
  			openSlotFound = TRUE;
  			strcpy(symbolTbl[i].symbolName,entry.symbolName);
  			symbolTbl[i].symbolAddr = entry.symbolAddr;
  			symbolTbl[i].RecordUsed = TRUE;
  			break;
  		}
	}

	/* If a slot was not found, the Symbol Table is full */
	if (openSlotFound == FALSE)
		returnStatus = OS_ERROR;
  
  return(returnStatus);
}/* end UTF_SetSymbolTableEntry */

/*--------------------------------------------------------------------------------------
    Name: UTF_DumpSymbolTable
    
    Purpose: Print the contents of the Symbol Table to STDOUT

---------------------------------------------------------------------------------------*/
void UTF_DumpSymbolTable (void)
{
int i;

  UTF_put_text("\n   Symbol Name          Address\n");
  
  /* Dump all the used slots */
  for (i=0; i<UTF_MAX_SYMBOLS; i++)
  {
	if (symbolTbl[i].RecordUsed == TRUE)
	{
  		UTF_put_text("%d '%s' - %08x\n",i+1,symbolTbl[i].symbolName,symbolTbl[i].symbolAddr);
  	}
  } 	
} /* end UTF_DumpSymbolTable */

/*--------------------------------------------------------------------------------------
    Name: UTF_SetOSSymbolTableDumpStatus
    
    Purpose: Set the return status for OS_SymbolTableDump()

---------------------------------------------------------------------------------------*/
void UTF_SetOSSymbolTableDumpStatus (int32 status)
{

  symbolTableDumpStatus = status;
  
  return;

} /* end UTF_SetOSSymbolTableDumpStatus */

/****************************************************************************************
                                    Symbol table API
****************************************************************************************/
/*--------------------------------------------------------------------------------------
    Name: OS_SymbolLookup
    
    Purpose: Find the Address of a Symbol 

    Parameters: 

    Returns: OS_ERROR if the symbol could not be found
             OS_SUCCESS if the symbol is found 
             OS_INVALID_POINTER if the pointer passed in is invalid
             
             The address of the symbol will be stored in the pointer that is passed in.
---------------------------------------------------------------------------------------*/
int32 OS_SymbolLookup( uint32 *SymbolAddress, char *SymbolName )
{
int32 returnStatus=OS_SUCCESS;
boolean symbolFound=FALSE;
int i;
 
	/* Check the arguments for validity */
	if (SymbolAddress != NULL && SymbolName != NULL)
	{
		/* Search the table for the Symbol */
		for (i=0; i<UTF_MAX_SYMBOLS; i++)
		{
			if (strcmp(SymbolName, symbolTbl[i].symbolName) == 0)
            {
            	*SymbolAddress = symbolTbl[i].symbolAddr;
            	symbolFound = TRUE;
				OS_printf("OSAL: Address passed back = %x\n",*SymbolAddress);
            	break;
            }
		}
        
        if (symbolFound == FALSE)
        {
			OS_printf("OSAL: Error, cannot locate symbol '%s'\n",SymbolName);
			returnStatus = OS_ERROR;
        }
	}
	else
		returnStatus = OS_INVALID_POINTER;

   return(returnStatus);
}/* end OS_SymbolLookup */

/*--------------------------------------------------------------------------------------
    Name: OS_SymbolTableDump
    
    Purpose: Dumps the system symbol table to a file

    Parameters: 

    Returns: OS_ERROR if the symbol table could not be read or dumped
             OS_INVALID_FILE  if the file could not be opened or written
             OS_SUCCESS if the symbol is found 
---------------------------------------------------------------------------------------*/
int32 OS_SymbolTableDump ( char *filename, uint32 SizeLimit )
{
	(void) filename;	/* refer to function argument to avoid compiler warning */
	(void) SizeLimit;	/* refer to function argument to avoid compiler warning */

    if (symbolTableDumpStatus == OS_SUCCESS)
    {
	    UTF_DumpSymbolTable();
    }

	return(symbolTableDumpStatus);
   
}/* end OS_SymbolTableDump */

