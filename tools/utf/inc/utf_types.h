/*
** File: utf_types.h
**  $Id: utf_types.h 1.3 2010/10/25 14:09:28GMT-05:00 jmdagost Exp  $
**
**      Copyright (c) 2004-2006, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
** Purpose: This file contains the Unit Test Framework function prototypes for utf_sim.c.
**
** References:
**
** Assumptions and Notes:
**
** 2005/07/12 nschweiss Modifying UTF_add_input_queue arguments for use with cFE.
**
** $Date: 2010/10/25 14:09:28GMT-05:00 $
** $Revision: 1.3 $
** $Log: utf_types.h  $
** Revision 1.3 2010/10/25 14:09:28GMT-05:00 jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.2 2010/10/04 15:05:04EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.1 2008/04/17 08:09:33EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/inc/project.pj
** Revision 1.3 2006/06/22 13:53:33EDT wfmoleski 
** Most of the changes to these files was to add the copyright comments. There were other changes needed
** to bring the UTF up to cFE 3.3
** Revision 1.2 2006/03/24 14:25:58EST sslegel 
** Removed C++ comment
** Revision 1.1 2006/03/21 16:44:26EST sslegel
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/inc/project.pj
** Revision 1.1 2004/12/21 15:00:57EST sslegel
** Initial revision
** Member added to project /home/mksdata/MKS-SDO-REPOSITORY/fsw/tools/utf/project.pj
** Revision 1.2  2004/05/07 19:27:56  sslegel
** Fix revision history tag
**
*/

#ifndef _utf_types_
#define _utf_types_

#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <stdarg.h>
#include    <ctype.h>
#include    "utf_custom.h"
#include    "cfe_sb.h"

/* valid data types used for UTF_get_data and UTF_put_data calls */
#define     UTF_AS_BYTE                     1
#define     UTF_AS_WORD                     2
#define     UTF_AS_DWORD                    4

/*
**   Exported Functions
*/

/******************************************************************************
**  Function: UTF_set_output_filename()
**
**  Purpose:
**    Defaults to "sbsim.out". Set to other name if desired.
*/
void UTF_set_output_filename(char *filename);

/******************************************************************************
**  Function: UTF_add_sim_address()
**
**  Purpose:
**    Adds a memory range to the simulated address table.
*/
void UTF_add_sim_address(unsigned long start, unsigned long size, char *name);

/******************************************************************************
**  Function: UTF_sim_address_lookup()
**
**  Purpose:
**    Return a simulated address corresponding to 'address'. Simulation
**    addresses are added by calling UTF_add_sim_address.
**
**    The simulated address may be either RAM or memory mapped IO.
**
**    Returns NULL if 'address' .. 'address' + 'size' is not in the table.
*/
void *UTF_sim_address_lookup (unsigned long int address, unsigned short int size);

/******************************************************************************
**  Function: UTF_sim_name_lookup()
**
**  Purpose:
**    This routine returns a descriptive name for the specified address.
*/
char *UTF_sim_name_lookup(unsigned long int address);

/******************************************************************************
**  Function: UTF_read_sim_address()
**
**  Purpose:
**    Read 'num_bytes' from simulated 'address' into '*data_ptr'.
**
**    Returns TRUE if 'address' .. 'address' + 'num_bytes' is in the
**    simulation memory table.
*/
char UTF_read_sim_address(unsigned long int address, unsigned short int num_bytes, void *data_ptr);

/******************************************************************************
**  Function: UTF_write_sim_address()
**
**  Purpose:
**    Write 'num_bytes' from '*data_ptr' into simulated 'address'.
**
**    Returns TRUE if 'address' .. 'address' + 'num_bytes' is in the
**    simulation memory table.
*/
char UTF_write_sim_address(unsigned long int address, unsigned short int num_bytes, void *data_ptr);

typedef char (*utf_write_sim_hook_type)(unsigned long int address, unsigned short int num_bytes, void *data_ptr);
typedef char (*utf_read_sim_hook_type)(unsigned long int address, unsigned short int num_bytes, void *data_ptr);

/******************************************************************************
**  Function: UTF_set_read_sim_hook()
**
**  Purpose:
**    Sets hooks into the simulated memory read write routines.  This hook gets called before
**    any other processing is done.  Returning TRUE indicates that the hook has handled the
**    simulated memory access and no other processing should be done.  Otherwise the memory access
**    will be simulated using a ram buffer
*/
void UTF_set_read_sim_hook(utf_read_sim_hook_type hook);

/******************************************************************************
**  Function: UTF_set_write_sim_hook()
**
**  Purpose:
**    Sets hooks into the simulated memory read write routines.  This hook gets called before
**    any other processing is done.  Returning TRUE indicates that the hook has handled the
**    simulated memory access and no other processing should be done.  Otherwise the memory access
**    will be simulated using a ram buffer
*/
void UTF_set_write_sim_hook(utf_write_sim_hook_type hook);

/******************************************************************************
**  Function: UTF_get_buffer()
**
**  Purpose:
**    Gets a free buffer, returns NULL if none available
*/
void *UTF_get_buffer(void);

/******************************************************************************
**  Function: UTF_release_buffer()
**
**  Purpose:
**    Releases a buffer allocated by UTF_get_buffer.  Returns FALSE if the buffer
**    address is not a member of the buffer pool
*/
char UTF_release_buffer(void *buffer);

/******************************************************************************
**  Function: UTF_valid_buffer()
**
**  Purpose:
**    Checks to see if the buffer address is a member of the buffer pool without
**    releasing the buffer
*/
char UTF_valid_buffer(void *buffer);

typedef struct
{
    void    *data;
    int     length;
} utf_queue_data_type;

typedef struct
{
   utf_queue_data_type  buffers[UTF_QUEUE_SIZE]; /* queue of buffers. */
   int                  head;                    /* next buffer to read */
   int                  tail;                    /* next buffer to write */
   /* If head = tail, queue is empty. Queue always has one unused
      slot, to allow us to distinquish full from empty. */
} utf_queue_type;

/******************************************************************************
**  Function: UTF_add_input_queue()
**
**  Purpose:
**    Add a input queue to the file_descriptor_table. Calls to UTF_get_data using the
**    specified PipeId will return data from the queue.
*/
void UTF_add_input_queue(unsigned long PipeId);

/******************************************************************************
**  Function: UTF_lookup_queue()
**
**  Purpose:
**    Return a queue struct if 'source_id' is defined to be read from a
**    queue; NULL otherwise.
*/
utf_queue_type *UTF_lookup_queue (CFE_SB_PipeId_t PipeId);

/******************************************************************************
**  Function: UTF_queue_add()
**
**  Purpose:
**    Add data from '*data' to queue for 'PipeId'.
*/
void UTF_queue_add (CFE_SB_PipeId_t PipeId, const void *data, size_t length);

/******************************************************************************
**  Function: UTF_queue_get()
**
**  Purpose:
**    Get data from queue for 'source_id'
*/
void UTF_queue_get (CFE_SB_PipeId_t PipeId, void *data);

/******************************************************************************
**  Function: UTF_queue_empty()
**
**  Purpose:
**    Non-zero if queue for 'source_id' is empty.
*/
/*
 * Freemon Johnson change to add type to PipeId 9/1/05
 */
/*char UTF_queue_empty (PipeId);*/
char UTF_queue_empty (CFE_SB_PipeId_t PipeId);
/*
 * PipeId change ENDS
 */

/******************************************************************************
**  Function: UTF_add_input_file()
**
**  Purpose:
**    Add a input file to the file_descriptor_table.  Calls to UTF_get_data using the
**    specified source_id will return data from the associated file
*/
void UTF_add_input_file(unsigned long source_id, char *filename);

/******************************************************************************
**  Function: UTF_get_input_fd()
**
**  Purpose:
**    Return current input file; may be stdin
*/
FILE *UTF_get_input_fd(unsigned long source_id);

/******************************************************************************
**  Function: UTF_get_output_fd()
**
**  Purpose:
**    Return current output file; may be stdout
*/
FILE *UTF_get_output_fd(void);

/******************************************************************************
**  Function: UTF_put_data()
**
**  Purpose:
**    Formatted output to 'output_fd'.
*/
void UTF_put_data(void *, unsigned short int, unsigned short int);

/******************************************************************************
**  Function: UTF_put_text()
**
**  Purpose:
**    Formatted output to 'output_fd'.
*/
void UTF_put_text(char *, ...); /* just like printf */

/******************************************************************************
**  Function: UTF_get_data()
**
**  Purpose:
**    Read data from the input source specified by source_id
*/
char UTF_get_data(unsigned long       source_id,
                  void               *buffer,
                  unsigned short int  length,
                  unsigned short int  data_type);

/******************************************************************************
**  Function: UTF_error()
**
**  Purpose:
**    Write a message to stderr, abort.
*/
void UTF_error(char *spec, ...); /* just like printf */

/******************************************************************************
**  Function: UTF_exit()
**
**  Purpose:
**    Close all files, exit with normal status.
*/
void UTF_exit (void);

typedef void (*utf_special_command_type)(int argc, char *argv[]);

/******************************************************************************
**  Function: UTF_add_special_command()
**
**  Purpose:
**    Add a special command to the lookup table.
*/
void UTF_add_special_command(char *name, utf_special_command_type function);

/******************************************************************************
**  Function: UTF_special_command_lookup()
**
**  Purpose:
**    Lookup a named special command
*/
utf_special_command_type UTF_special_command_lookup(char *name);

/******************************************************************************
**  Function: UTF_arg2uint()
**
**  Purpose:
**    Convert a string argument to a unsigned long.
*/
unsigned long UTF_arg2uint(char *val);

/******************************************************************************
**  Function: UTF_arg2int()
**
**  Purpose:
**    Convert a string argument to a signed long.
*/
long UTF_arg2int(char *val);

/******************************************************************************
**  Function: UTF_arg2dbl()
**
**  Purpose:
**    Convert a string argument to a double floating point number.
*/
double UTF_arg2dbl(char *val);

#endif
