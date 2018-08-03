/*************************************************************************
** File:
**   $Id: md_utils.c 1.7 2015/03/01 17:17:17EST sstrege Exp  $
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
**   Utility functions used for processing CFS Memory Dwell commands
**
**   $Log: md_utils.c  $
**   Revision 1.7 2015/03/01 17:17:17EST sstrege 
**   Added copyright information
**   Revision 1.6 2012/01/09 18:14:23EST aschoeni 
**   Fixed buffer overflow issue
**   Revision 1.5 2009/06/18 10:11:53EDT rmcgraw 
**   DCR8291:1 Changed #defines from OS_MEM_ to CFE_PSP_MEM_
**   Revision 1.4 2009/06/12 14:19:05EDT rmcgraw 
**   DCR82191:1 Changed OS_Mem function calls to CFE_PSP_Mem
**   Revision 1.3 2009/01/12 14:33:35EST nschweis 
**   Removed debug statements from source code.  CPID 4688:1.
**   Revision 1.2 2008/07/02 13:53:39EDT nsschweiss 
**   CFS MD Post Code Review Version
**   Date: 08/05/09
**   CPID: 1653:2
** 
*************************************************************************/

/*************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "md_utils.h"
#include "md_app.h"
#include <string.h>
extern MD_AppData_t MD_AppData;


/******************************************************************************/

boolean MD_TableIsInMask(int16 TableId, uint16 TableMask)
{
   uint16  LocalMask = TableMask;
   boolean Status;
   
   /* Shift TableId - 1 times */
   if (TableId - 1)
   {
      LocalMask = LocalMask >> (TableId -1);
   }
   
   /* If result is odd, */
   /* then table is in mask. */
   if ( (LocalMask & (uint16) 1) == (uint16) 1)
      Status = TRUE;
   else
      Status = FALSE;
    
   return Status;
   
} /* End MD_TableIsInMask */

/******************************************************************************/

void MD_UpdateDwellControlInfo (uint16 TableIndex)
{
    /* Initialize local data */
    uint16 EntryIndex               = 0;
    uint16 NumDwellAddresses        = 0;
    uint16 NumDwellDataBytes        = 0;
    uint32 NumDwellDelayCounts      = 0;
    MD_DwellPacketControl_t *TblPtr = &MD_AppData.MD_DwellTables[TableIndex]; 
    
    /* 
    ** Sum Address Count, Data Size, and Rate from Dwell Entries 
    */
    while ((EntryIndex <  MD_DWELL_TABLE_SIZE) &&
            (TblPtr->Entry[EntryIndex].Length != 0))
    {
        NumDwellAddresses++;
        NumDwellDataBytes   += TblPtr->Entry[EntryIndex].Length;
        NumDwellDelayCounts += TblPtr->Entry[EntryIndex].Delay;
        EntryIndex++;
    }
    
    /* Copy totals to dwell control structure. */
    TblPtr->AddrCount = NumDwellAddresses;
    TblPtr->DataSize  = NumDwellDataBytes;
    TblPtr->Rate      = NumDwellDelayCounts;
    
    return;
} /* MD_UpdateDwellControlInfo */

/******************************************************************************/

/*
**  Data Validation Functions
*/
/******************************************************************************/
boolean MD_ValidEntryId            ( uint16 EntryId )
{
    boolean IsValid;
    
    if ((EntryId >= 1) && ( EntryId <= MD_DWELL_TABLE_SIZE ))  
    {
        /* validate  value (1..MD_DWELL_TABLE_SIZE ) */
        IsValid = TRUE;
    }
    else
    {
        IsValid = FALSE;
    }
    return IsValid;
}


/******************************************************************************/

boolean MD_ValidAddrRange( uint32 Addr, uint32 Size )
{
    boolean IsValid;

    if ( CFE_PSP_MemValidateRange (Addr,Size, CFE_PSP_MEM_ANY) == OS_SUCCESS ) 
    {
        IsValid = TRUE;
    }
   else                                         
    { 
        IsValid = FALSE;
    }  
    
    return IsValid;
}
/******************************************************************************/
boolean MD_ValidTableId( uint16 TableId)
{
    boolean IsValid;
    
    if ((TableId >= 1) && (TableId <= MD_NUM_DWELL_TABLES)) 
    {
       IsValid=TRUE;
    }
    else
    {
       IsValid=FALSE;
    }
      
    return IsValid;
}

/******************************************************************************/

boolean MD_ValidFieldLength( uint16 FieldLength)
{

    boolean IsValid;

    if ( (FieldLength == 0)  || 
         (FieldLength == 1)  ||
         (FieldLength == 2)  || 
         (FieldLength == 4) )
    {
        IsValid= TRUE;
    }
    else
    {
        IsValid= FALSE;
    }
    
    return IsValid;
}

/******************************************************************************/

/************************/
/*  End of File Comment */
/************************/
