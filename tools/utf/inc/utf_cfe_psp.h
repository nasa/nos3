/*
** File: utf_cfe_bsp.h
**  $Id: utf_cfe_psp.h 1.4 2010/11/29 08:44:49GMT-05:00 jmdagost Exp  $
**
**      Copyright (c) 2004-2006, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
** Purpose: This module defines the UTF specific function prototypes for the functions contained in utf_bsp_support.c
**
** References:
**
** Assumptions and Notes:
**
** $Date: 2010/11/29 08:44:49GMT-05:00 $
** $Revision: 1.4 $
** $Log: utf_cfe_psp.h  $
** Revision 1.4 2010/11/29 08:44:49GMT-05:00 jmdagost 
** Enhanced support for CFE_PSP_EepromWriteEnable and CFE_PSP_EepromWriteDisable
** Revision 1.3 2010/10/25 15:09:27EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.2 2010/10/04 15:04:21EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.1 2009/10/13 15:32:08EDT wmoleski 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/inc/project.pj
** Revision 1.3 2008/08/20 13:40:29EDT wfmoleski 
** The UTF_OSBSP_Set_Api_Return_Code function was named UTF_OSABSP_Set_Api_Return_Code causing
** an undefined symbol when it was used. THis was found while testing the OS_FS changes.
** Revision 1.2 2008/07/28 11:07:21EDT wfmoleski 
** Added code to allow the return code setting capability of the UTF to be used on the 2 new BSP functions.
** Also, testing of this capability was added to test11 of the UTF examples.
** Revision 1.1 2008/04/17 08:09:31EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/inc/project.pj
** Revision 1.2 2006/06/22 14:01:12EDT wfmoleski 
** All of these changes added the copyright comments. There was nothing else that required any UTF
** changes to bring it up to cFE 3.3
** Revision 1.1 2006/04/10 16:30:21EDT sslegel 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/inc/project.pj
**
*/

#ifndef _utf_cfe_psp_
#define _utf_cfe_psp_

/*
**   Include section
*/
#include "common_types.h"

/* Function Hook Stuff */
#define CFE_PSP_EEPROMWRITE8_HOOK    0
#define CFE_PSP_EEPROMWRITE16_HOOK   1
#define CFE_PSP_EEPROMWRITE32_HOOK   2
#define CFE_PSP_EEPROMWRITEENA_HOOK  3
#define CFE_PSP_EEPROMWRITEDIS_HOOK  4

/*********** API Identifiers Used to set return codes************/
#define CFE_PSP_GETKERNELTEXTSEGMENTINFO_PROC 0
#define CFE_PSP_GETCFETEXTSEGMENTINFO_PROC    1
#define CFE_PSP_MEMVALIDATERANGE_PROC         2
#define CFE_PSP_MEMRANGESET_PROC              3
#define CFE_PSP_MEMRANGEGET_PROC              4
#define CFE_PSP_EEPROMWRITEENA_PROC           5
#define CFE_PSP_EEPROMWRITEDIS_PROC           6
#define NUM_OF_CFE_PSP_PROCS                  7

#define UTF_CFE_USE_DEFAULT_RETURN_CODE -500

/******************************************************************************
**  Function: UTF_set_processor_id()
**
**  Purpose:
**    Sets the processor id returned from CFE_PSP_GetProcessorId.
*/
void UTF_set_processor_id(uint32 processor_id);

/******************************************************************************
**  Function: UTF_set_spacecraft_id()
**
**  Purpose:
**    Sets the spacecraft id returned from CFE_PSP_GetSpacecraftId.
*/
void UTF_set_spacecraft_id(uint32 spacecraft_id);

/******************************************************************************
**  Function: UTF_PSP_set_function_hook()
**
**  Purpose:
**    Install a user defined hook function for a OSAPIArch call.
*/
void UTF_PSP_set_function_hook(int Index, void *FunPtr);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_PSP_Set_Api_Return_Code() --                                  */
/*  Set a return code for an api to a specified value.                     */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 UTF_PSP_Set_Api_Return_Code(int32 ApiIndex, uint32 ReturnCode);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_PSP_Use_Default_Api_Return_Code() --                          */
/* Use to return to default return code for an api.                        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_PSP_Use_Default_Api_Return_Code(int32 ApiIndex);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_PSP_Set_Api_Return_Code() --                           */
/*  Script interface function for UTF_PSP_Set_Api_Return_Code.       */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void UTF_SCRIPT_PSP_Set_Api_Return_Code(int argc,char *argv[]);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_PSP_Use_Default_Api_Return_Code() --                   */
/*  Script interface function for UTF_PSP_Use_Default_Api_Return_Code. */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void UTF_SCRIPT_PSP_Use_Default_Api_Return_Code(int argc,char *argv[]);

#endif
