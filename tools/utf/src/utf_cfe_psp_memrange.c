/*
** File: utf_cfe_psp_memrange.c
** $Id: utf_cfe_psp_memrange.c 1.5 2012/02/24 13:59:40GMT-05:00 wmoleski Exp  $
**
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement.
**  
**
** Purpose: This module defines the UTF versions of the cFE platform support package
**          memory range functions.
**
** References:
**
** Assumptions and Notes:
**
** $Date: 2012/02/24 13:59:40GMT-05:00 $
** $Revision: 1.5 $
** $Log: utf_cfe_psp_memrange.c  $
** Revision 1.5 2012/02/24 13:59:40GMT-05:00 wmoleski 
** The CFE_PSP_MemValidateRange function was modified to match the actual implementation in cFE 6.3.1.0
** Revision 1.4 2012/01/13 12:51:58EST acudmore 
** Changed license text to reflect open source
** Revision 1.3 2010/10/25 15:06:43EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.2 2010/10/04 14:57:16EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.1 2009/10/20 13:25:37EDT wmoleski 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/src/project.pj
*/

/*
**  Include Files
*/
#include "common_types.h"
#include "osapi.h"

/*
** Types and prototypes for this module
*/
#include "utf_types.h"
#include "utf_cfe_psp.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/*
** global memory
*/
CFE_PSP_MemTable_t CFE_PSP_MemoryTable[CFE_PSP_MEM_TABLE_SIZE] =
{
{ CFE_PSP_MEM_INVALID, 0, 0, 0, CFE_PSP_MEM_ATTR_READWRITE },
{ CFE_PSP_MEM_INVALID, 0, 0, 0, CFE_PSP_MEM_ATTR_READWRITE },
{ CFE_PSP_MEM_INVALID, 0, 0, 0, CFE_PSP_MEM_ATTR_READWRITE },
{ CFE_PSP_MEM_INVALID, 0, 0, 0, CFE_PSP_MEM_ATTR_READWRITE },
{ CFE_PSP_MEM_INVALID, 0, 0, 0, CFE_PSP_MEM_ATTR_READWRITE },
{ CFE_PSP_MEM_INVALID, 0, 0, 0, CFE_PSP_MEM_ATTR_READWRITE },
{ CFE_PSP_MEM_INVALID, 0, 0, 0, CFE_PSP_MEM_ATTR_READWRITE },
{ CFE_PSP_MEM_INVALID, 0, 0, 0, CFE_PSP_MEM_ATTR_READWRITE },
{ CFE_PSP_MEM_INVALID, 0, 0, 0, CFE_PSP_MEM_ATTR_READWRITE },
{ CFE_PSP_MEM_INVALID, 0, 0, 0, CFE_PSP_MEM_ATTR_READWRITE },
};

/* External references */
extern int32 cfe_psp_return_value[NUM_OF_CFE_PSP_PROCS];

/*
** Name: CFE_PSP_MemValidateRange
**
** Purpose:
**    Validate the memory range and type using the global CFE_PSP_MemoryTable
**
** Assumptions and Notes:
**
** Parameters:
**    Address -- A 32 bit starting address of the memory range
**    Size    -- A 32 bit size of the memory range (Address+Size = End Address)
**    MemoryType -- The memory type to validate: CFE_PSP_MEM_RAM, CFE_PSP_MRM_EEPROM, or CFE_PSP_MEM_ANY
**
** Global Inputs: None
**
** Global Outputs: None
**
** Return Values:
**   CFE_PSP_SUCCESS -- Memory range and type information is valid and can be used.
**   CFE_PSP_INVALID_MEM_ADDR -- Starting address is not valid
**   CFE_PSP_INVALID_MEM_TYPE -- Memory type associated with the range does not match the passed in type.
**   CFE_PSP_INVALID_MEM_RANGE -- The Memory range associated with the address is not large enough to contain
**                            Address + Size.
*/
int32 CFE_PSP_MemValidateRange(uint32 Address, uint32 Size, uint32 MemoryType)
{
   uint32 RangeIsValid = FALSE;
   uint32 StartAddressToTest = Address;
   uint32 EndAddressToTest = Address + Size - 1;
   uint32 StartAddressInTable;
   uint32 EndAddressInTable;   uint32 TypeInTable;
   int32  ReturnCode = CFE_PSP_INVALID_MEM_ADDR;   uint32 i;

   /* Handle Preset Return Code */
   if (cfe_psp_return_value[CFE_PSP_MEMVALIDATERANGE_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_psp_return_value[CFE_PSP_MEMVALIDATERANGE_PROC];
   }

   if ( MemoryType != CFE_PSP_MEM_ANY && MemoryType != CFE_PSP_MEM_RAM && MemoryType != CFE_PSP_MEM_EEPROM )
   {
      return(CFE_PSP_INVALID_MEM_TYPE);
   }

   if ( EndAddressToTest < StartAddressToTest )
   {
      return(CFE_PSP_INVALID_MEM_RANGE);
   }

   for ( i = 0; i < CFE_PSP_MEM_TABLE_SIZE; i++ )
   {
      /*
      ** Only look at valid memory table entries
      */
      if ( CFE_PSP_MemoryTable[i].MemoryType != CFE_PSP_MEM_INVALID )
      {
         StartAddressInTable = CFE_PSP_MemoryTable[i].StartAddr;
         EndAddressInTable   = CFE_PSP_MemoryTable[i].StartAddr + CFE_PSP_MemoryTable[i].Size - 1;
         TypeInTable         = CFE_PSP_MemoryTable[i].MemoryType;

         /*
         ** Step 1: Get the Address to Fit within the range
         */
         if (( StartAddressToTest >= StartAddressInTable ) && ( StartAddressToTest <= EndAddressInTable ))
         {
            /*
            ** Step 2: Does the End Address Fit within the Range?
            **         should not have to test the lower address,
            **         since the StartAddressToTest is already in the range.
            **         Can it be fooled by overflowing the 32 bit int?
            */
            if ( EndAddressToTest <= EndAddressInTable )
            {
               /*
               ** Step 3: Is the type OK?
               */
               if ( MemoryType == CFE_PSP_MEM_ANY )
               {
                  RangeIsValid = TRUE;
                  ReturnCode = CFE_PSP_SUCCESS;
                  break;  /* Make sure this breaks out of the loop! */
               }
               else if ( MemoryType == CFE_PSP_MEM_RAM && TypeInTable == CFE_PSP_MEM_RAM )
               {
                  RangeIsValid = TRUE;
                  ReturnCode = CFE_PSP_SUCCESS;
                  break;
               }
               else if ( MemoryType == CFE_PSP_MEM_EEPROM && TypeInTable == CFE_PSP_MEM_EEPROM )
               {
                  RangeIsValid = TRUE;
                  ReturnCode = CFE_PSP_SUCCESS;
                  break;
               }
               else
               {
                  ReturnCode = CFE_PSP_INVALID_MEM_TYPE;
               }
            }
            else
            {
               ReturnCode = CFE_PSP_INVALID_MEM_RANGE;
            }
         }
         else
         {
            ReturnCode = CFE_PSP_INVALID_MEM_ADDR;
         }
      } /* End if MemoryType != CFE_PSP_MEM_INVALID */
   } /* End for */
  return(ReturnCode);
}

/*
** Name: CFE_PSP_MemRanges
**
** Purpose:
**   Return the number of memory ranges in the CFE_PSP_MemoryTable
**
** Assumptions and Notes:
**
** Parameters:
**    None
**
** Global Inputs: None
**
** Global Outputs: None
**
** Return Values:
**   Positive integer number of entries in the memory range table
*/
uint32 CFE_PSP_MemRanges(void)
{
    return(CFE_PSP_MEM_TABLE_SIZE);
}

/*
** Name: CFE_PSP_MemRangeSet
**
** Purpose:
**   This function populates one of the records in the CFE_PSP_MemoryTable.
**
** Assumptions and Notes:
**    Becasue the table is fixed size, the entries are set by using the integer index.
**    No validation is done with the address or size.
**
** Parameters:\
**    RangeNum --   A 32 bit integer ( starting with 0 ) specifying the MemoryTable entry.
**    MemoryType -- The memory type to validate: CFE_PSP_MEM_RAM, CFE_PSP_MEM_EEPROM, or CFE_PSP_MEM_ANY
**    Address --    A 32 bit starting address of the memory range
**    Size    --    A 32 bit size of the memory range (Address + Size = End Address )
**    WordSize --   The minimum addressable size of the range:
**                     ( CFE_PSP_MEM_SIZE_BYTE, CFE_PSP_MEM_SIZE_WORD, CFE_PSP_MEM_SIZE_DWORD )
**    Attributes -- The attributes of the Memory Range:
**                     (CFE_PSP_MEM_ATTR_WRITE, CFE_PSP_MEM_ATTR_READ, CFE_PSP_MEM_ATTR_READWRITE)
**
** Global Inputs: Reads CFE_PSP_MemoryTable
**
** Global Outputs: Changes CFE_PSP_MemoryTable
**
** Return Values:
**   CFE_PSP_SUCCESS -- Memory range set successfuly.
**   CFE_PSP_INVALID_MEM_RANGE -- The index into the table is invalid
**   CFE_PSP_INVALID_MEM_ADDR -- Starting address is not valid
**   CFE_PSP_INVALID_MEM_TYPE -- Memory type associated with the range does not match the passed in type.
**   CFE_PSP_INVALID_MEM_SIZE -- The Memory range associated with the address is not large enough to contain
**                            Address + Size.
**   CFE_PSP_INVALID_MEM_WORDSIZE -- The WordSIze parameter is not one of the predefined types.
**   CFE_PSP_INVALID_MEM_ATTR -- The Attributes parameter is not one of the predefined types.
*/
int32 CFE_PSP_MemRangeSet(uint32 RangeNum, uint32 MemoryType, uint32 StartAddr,
                     uint32 Size,  uint32 WordSize,   uint32 Attributes)
{
    /* Handle Preset Return Code */
    if (cfe_psp_return_value[CFE_PSP_MEMRANGESET_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
    {
        return cfe_psp_return_value[CFE_PSP_MEMRANGESET_PROC];
    }

    if ( RangeNum >= CFE_PSP_MEM_TABLE_SIZE )
    {
        return(CFE_PSP_INVALID_MEM_RANGE);
    }
    if ((MemoryType != CFE_PSP_MEM_RAM) && (MemoryType != CFE_PSP_MEM_EEPROM))
    {
        return(CFE_PSP_INVALID_MEM_TYPE);
    }
    if ((WordSize != CFE_PSP_MEM_SIZE_BYTE) && 
	(WordSize != CFE_PSP_MEM_SIZE_WORD) && 
	(WordSize != CFE_PSP_MEM_SIZE_DWORD))
    {
        return(CFE_PSP_INVALID_MEM_WORDSIZE);
    }

    if ((Attributes != CFE_PSP_MEM_ATTR_READ) && 
	(Attributes != CFE_PSP_MEM_ATTR_WRITE) &&
	(Attributes != CFE_PSP_MEM_ATTR_READWRITE))
    {
        return(CFE_PSP_INVALID_MEM_ATTR);
    }

    /*
    ** Parameters check out, add the range
    */
    CFE_PSP_MemoryTable[RangeNum].MemoryType = MemoryType;
    CFE_PSP_MemoryTable[RangeNum].StartAddr = StartAddr;
    CFE_PSP_MemoryTable[RangeNum].Size = Size;
    CFE_PSP_MemoryTable[RangeNum].WordSize = WordSize;
    CFE_PSP_MemoryTable[RangeNum].Attributes = Attributes;

    return(CFE_PSP_SUCCESS);
}

/*
** Name: CFE_PSP_MemRangeGet
**
** Purpose:
**              This function retrieves one of the records in the CFE_PSP_MemoryTable.
**
** Assumptions and Notes:
**    Becasue the table is fixed size, the entries are accessed by using the integer index.
**
** Parameters:
**    RangeNum --   A 32 bit integer ( starting with 0 ) specifying the MemoryTable entry.
**    *MemoryType -- A pointer to the 32 bit integer where the Memory Type is stored.
**                   ( CFE_PSP_MEM_TYPE_RAM, CFE_PSP_MEM_TYPE_EEPROM, CFE_PSP_MEM_TYPE_INVALID )
**    *Address --    A pointer to the 32 bit integer where the 32 bit starting address of the memory range
**                   is stored.
**    *EndAddr    --    A pointer to the 32 bit integer where the 32 bit size of the memory range
**                   is stored.
**    *WordSize --   A pointer to the 32 bit integer where the the minimum addressable size of the range:
**                     ( CFE_PSP_MEM_SIZE_BYTE, CFE_PSP_MEM_SIZE_WORD, CFE_PSP_MEM_SIZE_DWORD )
**    Attributes -- The attributes of the Memory Range:
**                     (CFE_PSP_MEM_ATTR_WRITE, CFE_PSP_MEM_ATTR_READ, CFE_PSP_MEM_ATTR_READWRITE)
**
** Global Inputs: Reads CFE_PSP_MemoryTable
**
** Global Outputs: Changes CFE_PSP_MemoryTable
**
** Return Values:
**   CFE_PSP_SUCCESS -- Memory range returned successfuly.
**   CFE_PSP_INVALID_POINTER   -- Parameter error
**   CFE_PSP_INVALID_MEM_RANGE -- The index into the table is invalid
*/
int32 CFE_PSP_MemRangeGet(uint32 RangeNum, uint32 *MemoryType,
		 uint32 *StartAddr, uint32 *Size, uint32 *WordSize,
		 uint32 *Attributes)
{
    /* Handle Preset Return Code */
    if (cfe_psp_return_value[CFE_PSP_MEMRANGEGET_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
    {
        return cfe_psp_return_value[CFE_PSP_MEMRANGEGET_PROC];        }
    if ( MemoryType == NULL || StartAddr == NULL || Size == NULL || WordSize == NULL || Attributes == NULL )
    {
        return(CFE_PSP_INVALID_POINTER);
    }

    if ( RangeNum >= CFE_PSP_MEM_TABLE_SIZE )
    {
        return(CFE_PSP_INVALID_MEM_RANGE);
    }

    *MemoryType = CFE_PSP_MemoryTable[RangeNum].MemoryType;
    *StartAddr  = CFE_PSP_MemoryTable[RangeNum].StartAddr;
    *Size       = CFE_PSP_MemoryTable[RangeNum].Size;
    *WordSize   = CFE_PSP_MemoryTable[RangeNum].WordSize;
    *Attributes = CFE_PSP_MemoryTable[RangeNum].Attributes;

    return(CFE_PSP_SUCCESS);
}

int32 CFE_PSP_MemSetAttributes(uint32 Key, uint32 Value)
{
    (void) Key;    /* refer to function argument to avoid compiler warning */
    (void) Value;  /* refer to function argument to avoid compiler warning */

    /* currently there are not keys defined so just return CFE_PSP_ERROR */
    return(CFE_PSP_ERROR);
}

int32 CFE_PSP_MemGetAttributes(uint32 Key, uint32 *Value)
{
    (void) Key;    /* refer to function argument to avoid compiler warning */
    (void) Value;  /* refer to function argument to avoid compiler warning */

    /* currently there are not keys defined so just return CFE_PSP_ERROR */
    return(CFE_PSP_ERROR);
}
