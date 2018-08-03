/*
** File: utf_cfe_sb.h
**  $Id: utf_cfe_sb.h 1.3 2010/10/25 14:09:25GMT-05:00 jmdagost Exp  $
**
**      Copyright (c) 2004-2006, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
** Purpose: This file contains the Unit Test Framework function prototypes for utf_cfe_sb.c.
**
** References:
**
** Assumptions and Notes:
**
** $Date: 2010/10/25 14:09:25GMT-05:00 $
** $Revision: 1.3 $
** $Log: utf_cfe_sb.h  $
** Revision 1.3 2010/10/25 14:09:25GMT-05:00 jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.2 2010/10/04 15:04:23EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.1 2008/04/17 08:09:30EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/inc/project.pj
** Revision 1.6 2007/08/27 09:56:40EDT wfmoleski 
** Modified UTF to match the implemetation of SB_GetLastSenderID.
** Revision 1.5 2006/09/05 10:13:38EDT wfmoleski 
** Added a special return code UTF_CFE_SB_RCVMSG_EXIT_RETURN_CODE that is processed by the UTF's
** version of CFE_SB_RcvMsg. If this return code is set and an enpty pipe is encountered, the test process will exit.
** Revision 1.4 2006/06/21 15:37:50EDT wfmoleski 
** Added copyright prologs to utf_sb files
** Revision 1.3 2006/06/20 10:52:59EDT wfmoleski 
** Added the implementation of the CFE_DeletePipe function. Also, the capability to define a hook function
** for DeletePipe is implemented.
** Revision 1.2 2006/03/24 14:24:57EST sslegel 
** removed unused return code defines
** Revision 1.1 2006/03/21 16:44:25EST sslegel
** Initial revision
** 2005/07/25 nschweiss Added UTF_put_packet();
** 2005/07/13 nschweiss Adapted for cFE.
** Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/inc/project.pj
** Revision 1.1 2004/12/21 15:00:55EST sslegel
** Initial revision
** Member added to project /home/mksdata/MKS-SDO-REPOSITORY/fsw/tools/utf/project.pj
** Revision 1.3  2004/12/03 16:24:19  sslegel
** Added new function UTF_set_sb_receive_return_status to change the default return status from a software bus receive call.
** Revision 1.2  2004/05/07 19:27:56  sslegel
** Fix revision history tag
**
*/

#ifndef _utf_cfe_sb_
#define _utf_cfe_sb_

/*
**   Include section
*/
#include <stdio.h>
#include "common_types.h"
#include "ccsds.h"

#define UTF_SOURCE_SEND     0
#define UTF_SOURCE_RECEIVE  1

/* Function Hook Stuff */
#define CFE_SB_SUBSCRIBE_HOOK            0
#define CFE_SB_SUBSCRIBEEX_HOOK          1
#define CFE_SB_CREATEPIPE_HOOK           2
#define CFE_SB_DELETEPIPE_HOOK           3

/* Return Code Stuff */
#define CFE_SB_CREATEPIPE_PROC           0
#define CFE_SB_SUBSCRIBEEX_PROC          1
#define CFE_SB_SUBSCRIBE_PROC            2
#define CFE_SB_UNSUBSCRIBE_PROC          3
#define CFE_SB_SENDMSG_PROC              4
#define CFE_SB_RCVMSG_PROC               5
#define CFE_SB_DELETEPIPE_PROC           6
#define CFE_SB_GETLASTSENDERID_PROC      7
#define NUM_OF_CFE_SB_API_PROCS          8

#define UTF_CFE_USE_DEFAULT_RETURN_CODE -500
/* Special return code that indicates to the UTF code 
 * to call UTF_exit when no data is read over the pipe */
#define UTF_CFE_SB_RCVMSG_EXIT_RETURN_CODE -501

/*
**   Exported Functions
*/

/******************************************************************************
**  Function: UTF_set_header_only_packet_output_all()
**
**  Purpose:
**    If 'header_only' is non-zero, print only the packet primary headers
**    to the output file. Otherwise, print the raw packet, then the
**    headers and data.
*/
void UTF_set_header_only_packet_output_all (int header_only);

/******************************************************************************
**  Function: UTF_set_header_only_packet_output()
**
**  Purpose:
**    If 'header_only' is non-zero, print only the packet primary header for sid
**    to the output file. Otherwise, print the raw packet, then the
**    headers and data.
*/
void UTF_set_header_only_packet_output (unsigned short sid, int header_only);

/******************************************************************************
**  Function: UTF_disable_packet_output()
**
**  Purpose:
**    Disable all output for 'packet_id'
*/
void UTF_disable_packet_output (unsigned short int packet_id);

/******************************************************************************
**  Function: UTF_put_packet_header()
**
**  Purpose:
**    Print the CCSDS header in 'packet' in a nice way.
*/
void UTF_put_packet_header (uint8 source, CCSDS_PriHdr_t *packet);

typedef void (*utf_packet_handler)(uint8 source, void *packet);

/******************************************************************************
**  Function: UTF_set_packet_handler()
**
**  Purpose:
**    Set 'handler' as the print handler for stream 'sid'. Default is to
**    print hex words.
*/
void UTF_set_packet_handler (uint16 sid, utf_packet_handler handler);

/******************************************************************************
**  Function:  UTF_set_default_packet_handler()
**
**  Purpose:
**    Specify a customized displaying function for packets that do not have
**    a specific displaying function.
*/
void UTF_set_default_packet_handler (utf_packet_handler handler);

void UTF_put_packet (uint8 source, CCSDS_PriHdr_t *packet);

/******************************************************************************
**  Function: UTF_SB_set_function_hook()
**
**  Purpose:
**    Install a user defined hook function for a SB function call.
*/
void UTF_SB_set_function_hook(int Index, void *FunPtr);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_SB_Set_Api_Return_Code() --                                     */
/*  Set a return code for an api to a specified value.                     */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 UTF_CFE_SB_Set_Api_Return_Code(int32 ApiIndex, int32 ReturnCode);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_SB_Use_Default_Api_Return_Code() --                             */
/* Use to return to default return code for an api.                        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 UTF_CFE_SB_Use_Default_Api_Return_Code(int32 ApiIndex);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_SB_Set_Api_Return_Code() --                                  */
/*  Script interface function for UTF_CFE_SB_Set_Api_Return_Code.          */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void UTF_SCRIPT_SB_Set_Api_Return_Code(int argc,char *argv[]);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_SB_Use_Default_Api_Return_Code() --                          */
/*  Script interface function for UTF_CFE_SB_Set_Api_Return_Code.          */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void UTF_SCRIPT_SB_Use_Default_Api_Return_Code(int argc,char *argv[]);

#endif
