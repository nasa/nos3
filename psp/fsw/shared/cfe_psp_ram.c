/*
** File   :	cfe_psp_memram.c
**
**
**      Copyright (c) 2004-2011, United States Government as represented by 
**      Administrator for The National Aeronautics and Space Administration. 
**      All Rights Reserved.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement. 
**
** Author :	Ezra Yeheskeli
**
** Purpose:
**		   This file  contains some of the cFE Platform Support Layer.
**         It contains the processor architecture specific calls.
**
**  16-Nov-2003 Ezra Yeheskeli
**          - First Creation.
**
*/

/*
** Include section
*/

#include "cfe_psp.h"


/*
** global memory
*/

/*
 ** Name: CFE_PSP_MemRead8
 **
 ** Purpose:
 **         Read one byte of memory.
 **
 **
 ** Parameters:
 **	MemoryAddress : Address to be read
 **  ByteValue  : The address content will be copied to the location pointed by this argument
 **
 ** Global Inputs: None
 **
 ** Global Outputs: None
 **
 **
 **
 ** Return Values: CFE_PSP_SUCCESS
 */
int32 CFE_PSP_MemRead8( cpuaddr MemoryAddress, uint8 *ByteValue )
{
    
	(*ByteValue) = *((uint8 *)MemoryAddress) ;
    
	return(CFE_PSP_SUCCESS) ;
}

/*
 ** Name: CFE_PSP_MemWrite8
 **
 ** Purpose:
 **         Write one byte of memory.
 **
 **
 ** Parameters:
 **	MemoryAddress : Address to be written to
 **  ByteValue  : The content pointed by this argument will be copied to the address
 **
 ** Global Inputs: None
 **
 ** Global Outputs: None
 **
 **
 **
 ** Return Values:
 **		CFE_PSP_SUCCESS
 */
int32 CFE_PSP_MemWrite8 ( cpuaddr MemoryAddress, uint8 ByteValue )
{
    *((uint8 *)MemoryAddress) = ByteValue;
	return(CFE_PSP_SUCCESS) ;
    
}

/*
 ** Name: CFE_PSP_MemRead16
 **
 ** Purpose:
 **         Read  2 bytes of memory.
 **
 **
 ** Parameters:
 **	MemoryAddress : Address to be read
 **  uint16Value : The address content will be copied to the location pointed by
 **            this argument
 **
 ** Global Inputs: None
 **
 ** Global Outputs: None
 **
 **
 **
 ** Return Values:
 **		CFE_PSP_SUCCESS
 **		CFE_PSP_ERROR_ADD_MISALIGNED The Address is not aligned to 16 bit
 **      addressing scheme.
 */
int32 CFE_PSP_MemRead16( cpuaddr MemoryAddress, uint16 *uint16Value )
{
	/* check 16 bit alignment  , check the 1st lsb */
	if( MemoryAddress & 0x00000001)
	{
		return(CFE_PSP_ERROR_ADDRESS_MISALIGNED) ;
	}
	(*uint16Value) = *((uint16 *)MemoryAddress) ;
   return(CFE_PSP_SUCCESS) ;
    
}
/*
 ** Name: CFE_PSP_MemWrite16
 **
 ** Purpose:
 **         Write 2 byte of memory.
 **
 **
 ** Parameters:
 **	MemoryAddress : Address to be written to
 **  uint16Value : The content pointed by this argument will be copied to the
 **            address
 **
 ** Global Inputs: None
 **
 ** Global Outputs: None
 **
 **
 **
 ** Return Values:
 **      CFE_PSP_SUCCESS
 **		CFE_PSP_ERROR_ADD_MISALIGNED The MemoryAddress is not aligned to 16 bit
 **      addressing scheme.
 */
int32 CFE_PSP_MemWrite16 ( cpuaddr MemoryAddress, uint16 uint16Value )
{
	/* check 16 bit alignment  , check the 1st lsb */
	if( MemoryAddress & 0x00000001)
	{
		return(CFE_PSP_ERROR_ADDRESS_MISALIGNED) ;
	}
   *((uint16 *)MemoryAddress) = uint16Value;
	return(CFE_PSP_SUCCESS) ;
}
/*
 ** Name: CFE_PSP_MemRead32
 **
 ** Purpose:
 **         Read 4 bytes of memory.
 **
 **
 ** Parameters:
 **	MemoryAddress : Address to be read
 **  uint32Value : The address content will be copied to the location pointed by
 **            this argument
 **
 ** Global Inputs: None
 **
 ** Global Outputs: None
 **
 **
 **
 ** Return Values:
 **		CFE_PSP_SUCCESS
 **		CFE_PSP_ERROR_ADD_MISALIGNED The Address is not aligned to 16 bit
 **      addressing scheme.
 */
int32 CFE_PSP_MemRead32( cpuaddr MemoryAddress, uint32 *uint32Value )
{    
	/* check 32 bit alignment  */
	if( MemoryAddress & 0x00000003)
	{
		return(CFE_PSP_ERROR_ADDRESS_MISALIGNED) ;
	}
	(*uint32Value) = *((uint32 *)MemoryAddress) ;
    
	return(CFE_PSP_SUCCESS) ;
}

/*
 ** Name: CFE_PSP_MemWrite32
 **
 ** Purpose:
 **         Write 4 byte of memory.
 **
 **
 ** Parameters:
 **	MemoryAddress : Address to be written to
 **  uint32Value : The content pointed by this argument will be copied to the
 **            address
 **
 ** Global Inputs: None
 **
 ** Global Outputs: None
 **
 **
 **
 ** Return Values:
 **		CFE_PSP_SUCCESS
 **		CFE_PSP_ERROR_ADD_MISALIGNED The Address is not aligned to 16 bit
 **      addressing scheme.
 */
int32 CFE_PSP_MemWrite32 ( cpuaddr MemoryAddress, uint32 uint32Value )
{
    
	/* check 32 bit alignment  */
	if( MemoryAddress & 0x00000003)
	{
		return(CFE_PSP_ERROR_ADDRESS_MISALIGNED) ;
	}

    *((uint32 *)MemoryAddress) = uint32Value;

	return(CFE_PSP_SUCCESS) ;
}



