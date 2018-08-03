/*
** File: utf_custom.h
**  $Id: utf_custom.h 1.4 2010/10/25 14:09:28GMT-05:00 jmdagost Exp  $
**
**      Copyright (c) 2004-2006, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
** Purpose: This file contains the Unit Test Framework configuration settings.
**
** References:
**
** Assumptions and Notes:
**
** $Date: 2010/10/25 14:09:28GMT-05:00 $
** $Revision: 1.4 $
** $Log: utf_custom.h  $
** Revision 1.4 2010/10/25 14:09:28GMT-05:00 jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.3 2010/10/04 15:04:55EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.2 2008/07/18 09:45:12EDT wfmoleski 
** The UTF special command limit was doubled from 10 to 20 in order to accomodate the CFS CS
** application's unit testing.
** Revision 1.1 2008/04/17 08:09:31EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/inc/project.pj
** Revision 1.4 2006/06/22 13:53:32EDT wfmoleski 
** Most of the changes to these files was to add the copyright comments. There were other changes needed
** to bring the UTF up to cFE 3.3
** Revision 1.3 2006/06/12 17:02:08EDT nsschweiss 
** commented out std output.
** Revision 1.2 2006/05/26 15:12:06EDT nsschweiss 
** Change setting to use standard output.
** Revision 1.1 2006/03/21 16:44:25EST sslegel 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/inc/project.pj
** 2005/07/13 nschweiss Increase size of buffer 3100 -> 5000.
** Revision 1.1 2004/12/21 15:00:52EST sslegel
** Initial revision
** Member added to project /home/mksdata/MKS-SDO-REPOSITORY/fsw/tools/utf/project.pj
** Revision 1.4  2004/11/04 20:32:25  sslegel
** Increased the max number of simulated addresses
**
** Revision 1.3  2004/05/07 19:27:56  sslegel
** Fix revision history tag
**
*/

#ifndef _utf_custom_
#define _utf_custom_

#define      UTF_SIM_MEM_LOOKUP_TABLE_SIZE      64      /* max number of sim memory address ranges */
#define      UTF_FD_TABLE_SIZE                  10      /* max number of input files */
#define      UTF_QUEUE_COUNT                    5       /* number of queues */
#define      UTF_QUEUE_SIZE                     20      /* queue depth */
#define      UTF_NUMBER_OF_BUFFERS              10      /* max number of packet buffers */
#define      UTF_BUFFER_SIZE                    5000    /* buffer size used for storing packets, must be large enough to hold max size packet */
#define      UTF_SPECIAL_COMMAND_TABLE_SIZE     20      /* max number of special commands */
#define      UTF_SPECIAL_COMMAND_MAX_ARGS       20      /* max number of special command arguments */

/*#define      UTF_USE_STDOUT */                        /* send all output to stdout instead of a file */
/*#define      UTF_USE_STDIN */                         /* get all input from stdin instead of a file */
#define      UTF_INTERACTIVE                            /* beep and wait for a key to be pressed when there is an error */

#endif

