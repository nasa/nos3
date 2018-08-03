/*
** File: utf_cfe_psp_memory.c
** $Id: utf_cfe_psp_memory.c 1.5 2012/01/13 12:51:57GMT-05:00 acudmore Exp  $
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
** Purpose: This module defines the UTF versions of the cFE platform support 
**          package memory functions.
**          This file contains glue routines between the cFE and the Platform
**          Support Package (PSP).
**
** References:
**
** Assumptions and Notes:
**
** $Date: 2012/01/13 12:51:57GMT-05:00 $
** $Revision: 1.5 $
** $Log: utf_cfe_psp_memory.c  $
** Revision 1.5 2012/01/13 12:51:57GMT-05:00 acudmore 
** Changed license text to reflect open source
** Revision 1.4 2010/11/29 08:45:17EST jmdagost 
** Enhanced support for CFE_PSP_EepromWriteEnable and CFE_PSP_EepromWriteDisable
** Revision 1.3 2010/10/25 15:06:30EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.2 2010/10/04 14:57:14EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.1 2009/10/20 13:25:34EDT wmoleski 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/src/project.pj
*/

/*
**  Include Files
*/
#include "common_types.h"
#include "osapi.h"
#include "cfe_es.h"

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
/* UTF CDS byte array declaration */
uint8 cdsArea[CFE_ES_CDS_SIZE];
uint8 *CFE_PSP_CDSPtr = &cdsArea[0];

int32 cfe_psp_return_value[NUM_OF_CFE_PSP_PROCS]=
 {
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_PSP_Set_Api_Return_Code() --                                   */
/*  Set a return code for an api to a specified value.                     */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_PSP_Set_Api_Return_Code(int32 ApiIndex, uint32 ReturnCode)
{
   /* Check that ApiIndex is in the valid range */
   if ( (ApiIndex >= NUM_OF_CFE_PSP_PROCS ) || (ApiIndex < 0 ) )  
   {
      UTF_put_text("UTF Error: Invalid ApiIndex %d passed to UTF_PSP_Set_Api_Return_Code\n",
      ApiIndex);
      return -1;
   }
   /* If index is valid, assign return value and report action */
   cfe_psp_return_value[ApiIndex] = ReturnCode;

   if (ReturnCode == UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
   	  UTF_put_text("UTF Info: Return code for cFE PSP Api #%d returned to default\n",
   	               ApiIndex);
   }
   else
   {
   	  UTF_put_text("UTF Info: Return value for cFE PSP Api #%d set to %lx\n", 
   	                ApiIndex, ReturnCode);
   }
   return CFE_PSP_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_PSP_Use_Default_Api_Return_Code() --                              */
/* Use to return to default return code for an api.                        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_PSP_Use_Default_Api_Return_Code(int32 ApiIndex)
{
	int32 Success;
	
   /* Check that ApiIndex is in the valid range */
   if ( (ApiIndex >= NUM_OF_CFE_PSP_PROCS ) || (ApiIndex < 0 ) )  
   {
      UTF_put_text("UTF Error: Invalid ApiIndex %d passed to UTF_PSP_Use_Default_Api_Return_Code\n",
      ApiIndex);
      return -1;
   }

   /* If index is valid, assign return value and report action */
   Success = UTF_PSP_Set_Api_Return_Code(ApiIndex, UTF_CFE_USE_DEFAULT_RETURN_CODE);
   
   return Success;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_PSP_Set_Api_Return_Code() --                           */
/*  Script interface function for UTF_PSP_Set_Api_Return_Code.   */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void UTF_SCRIPT_PSP_Set_Api_Return_Code(int argc,char *argv[])
{
    int32 Index;
    int32 Code;
	/* Check for correct number of arguments */
	if (argc != 3)
	{
	   UTF_error("UTF Error: Read %d args w/script cmd UTF_SCRIPT_PSP_Set_Api_Return_Code. Expected 2.\n",
	   argc -1 );
	   UTF_exit();
	}
    /* Extract argument values */
    Index = UTF_arg2dbl(argv[1]);
    Code  = UTF_arg2uint(argv[2]);
    /* Call function to set API return code */
	UTF_PSP_Set_Api_Return_Code(Index, Code);
	return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_PSP_Use_Default_Api_Return_Code() --                   */
/*  Script interface function for UTF_CFE_PSP_Set_Api_Return_Code.   */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void UTF_SCRIPT_PSP_Use_Default_Api_Return_Code(int argc,char *argv[])
{
    int32 Index;

	/* Check for correct number of arguments */
	if (argc != 2)
	{
	   UTF_error("UTF Error: Read %d args w/script cmd UTF_SCRIPT_PSP_Set_Api_Return_Code. Expected 1.\n",
	   argc -1 );
	   UTF_exit();
	}
 
    /* Extract argument values */
    Index = UTF_arg2dbl(argv[1]);
 
    /* Call function to set API return code */
	UTF_PSP_Set_Api_Return_Code(Index, UTF_CFE_USE_DEFAULT_RETURN_CODE);
	return;
}

/******************************************************************************
**  Function: CFE_PSP_GetCDSSize
**
**  Purpose: 
**    This function fetches the size of the OS Critical Data Store area.
**
**  Arguments:
**    (none)
**
**  Return:
**    (none)
*/

int32 CFE_PSP_GetCDSSize(uint32 *SizeOfCDS)
{
   int32 return_code;
   
   if ( SizeOfCDS == NULL )
   {
       return_code = CFE_PSP_ERROR;
   }
   else
   {
       *SizeOfCDS = CFE_ES_CDS_SIZE;
       return_code = CFE_PSP_SUCCESS;
   }
   
   return(return_code);
}

/******************************************************************************
**  Function: CFE_PSP_WriteToCDS
**
**  Purpose:
**    This function writes to the CDS Block.
**
**  Arguments:
**    (none)
**
**  Return:
**    (none)
*/

int32 CFE_PSP_WriteToCDS(void *PtrToDataToWrite, uint32 CDSOffset, uint32 NumBytes)
{
   uint8 *CopyPtr;
   int32  return_code;
         
   if ( PtrToDataToWrite == NULL )
   {
       return_code = CFE_PSP_ERROR;
   }
   else
   {
       if ( (CDSOffset < CFE_ES_CDS_SIZE ) && ( (CDSOffset + NumBytes) <= CFE_ES_CDS_SIZE ))
       {
          CopyPtr = &(CFE_PSP_CDSPtr[CDSOffset]);
		  /* Need to add a write to the CDS file */
          memcpy(CopyPtr, (char *)PtrToDataToWrite,NumBytes);
          
          return_code = CFE_PSP_SUCCESS;
       }
       else
       {
          return_code = CFE_PSP_ERROR;
       }
       
   } /* end if PtrToDataToWrite == NULL */
   
   return(return_code);
}

/******************************************************************************
**  Function: CFE_PSP_ReadFromCDS
**
**  Purpose:
**   This function reads from the CDS Block
**
**  Arguments:
**    (none)
**
**  Return:
**    (none)
*/

int32 CFE_PSP_ReadFromCDS(void *PtrToDataToRead, uint32 CDSOffset, uint32 NumBytes)
{
   uint8 *CopyPtr;
   int32  return_code;
      
   if ( PtrToDataToRead == NULL )
   {
       return_code = CFE_PSP_ERROR;
   }
   else
   {
       if ( (CDSOffset < CFE_ES_CDS_SIZE ) && ( (CDSOffset + NumBytes) <= CFE_ES_CDS_SIZE ))
       {
          CopyPtr = &(CFE_PSP_CDSPtr[CDSOffset]);
		  /* Need to add a read from the CDS file */
          memcpy((char *)PtrToDataToRead,CopyPtr, NumBytes);
          
          return_code = CFE_PSP_SUCCESS;
       }
       else
       {
          return_code = CFE_PSP_ERROR;
       }
       
   } /* end if PtrToDataToWrite == NULL */
   
   return(return_code);   
}

/*******************************************************************************
**  Function: CFE_PSP_GetKernelTextSegmentInfo
**
**  Purpose:
**    This function returns the start and end address of the kernel text segment.
**     It may not be implemented on all architectures.
**
**  Arguments:
**    (none)
**
**  Return:
**    (none)
*/
int32 CFE_PSP_GetKernelTextSegmentInfo(void *PtrToKernelSegment, uint32 *SizeOfKernelSegment)
{
  int32 return_code;
  uint32 Address;

  /* Handle Preset Return Code */
  if (cfe_psp_return_value[CFE_PSP_GETKERNELTEXTSEGMENTINFO_PROC] != UTF_CFE_USE_DEFAULT_RETURN_CODE)
  {
    return cfe_psp_return_value[CFE_PSP_GETKERNELTEXTSEGMENTINFO_PROC];
  }

  if ( SizeOfKernelSegment == NULL )
  {
    return_code = CFE_PSP_ERROR;
  }
  else
  {
    Address = (uint32) (&CFE_ES_RegisterApp);
    memcpy(PtrToKernelSegment,&Address,sizeof(PtrToKernelSegment));
    *SizeOfKernelSegment = (uint32) ((uint32) &CCSDS_ComputeCheckSum
                                     - (uint32) &CFE_ES_RegisterApp);
    return_code = CFE_PSP_SUCCESS;
  }

  return(return_code);
}

/******************************************************************************
**  Function: CFE_PSP_GetCFETextSegmentInfo
**
**  Purpose:
**    This function returns the start and end address of the CFE text segment.
**     It may not be implemented on all architectures.
**
**  Arguments:
**    (none)
**
**  Return:
**    (none)
*/
int32 CFE_PSP_GetCFETextSegmentInfo(void *PtrToCFESegment, uint32 *SizeOfCFESegment)
{
  int32 return_code;
  uint32 Address;

  /* Handle Preset Return Code */
  if (cfe_psp_return_value[CFE_PSP_GETCFETEXTSEGMENTINFO_PROC] != UTF_CFE_USE_DEFAULT_RETURN_CODE)
  {
    return cfe_psp_return_value[CFE_PSP_GETCFETEXTSEGMENTINFO_PROC];
  }

  if ( SizeOfCFESegment == NULL )
  {
    return_code = CFE_PSP_ERROR;
  }
  else
  {
    Address = (uint32) (&CFE_ES_RegisterApp);
    memcpy(PtrToCFESegment,&Address,sizeof(PtrToCFESegment));
    *SizeOfCFESegment = (uint32) ((uint32) &CCSDS_ComputeCheckSum
                                     - (uint32) &CFE_ES_RegisterApp);
    return_code = CFE_PSP_SUCCESS;
  }

  return(return_code);
}
