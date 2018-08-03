/*
** File: utf_sim.c
**  $Id: utf_sim.c 1.4 2012/01/13 12:52:03GMT-05:00 acudmore Exp  $
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
** Purpose: This file contains the Unit Test Framework core simulator functions.
**
** References:
**
** Assumptions and Notes:
**
** $Date: 2012/01/13 12:52:03GMT-05:00 $
** $Revision: 1.4 $
** $Log: utf_sim.c  $
** Revision 1.4 2012/01/13 12:52:03GMT-05:00 acudmore 
** Changed license text to reflect open source
** Revision 1.3 2010/10/25 15:06:31EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.2 2010/10/04 14:59:12EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.1 2008/04/17 08:09:40EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/src/project.pj
** Revision 1.5 2007/04/12 10:59:11EDT wfmoleski 
** Changes for 3159:1
** Revision 1.4 2006/06/22 14:01:13EDT wfmoleski 
** All of these changes added the copyright comments. There was nothing else that required any UTF
** changes to bring it up to cFE 3.3
** Revision 1.3 2006/06/09 16:34:35EDT nsschweiss 
** Added UTF_CFE_Init call.
** Revision 1.2 2006/03/24 14:29:36EST sslegel 
** Removed error message from UTF_sim_address_lookup
** Revision 1.1 2006/03/21 16:44:29EST sslegel
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/src/project.pj
** Revision 1.3 2005/12/01 19:09:10EST NSSCHWEISS
** utf 2.3.0.
** Revision 1.2 2005/10/25 16:27:03EDT lswalling
** Merged with oct14 version of utf
** Revision 1.1 2005/09/26 11:30:14EDT lswalling
** Initial revision
** Member added to project d:/mksdata/MKS-LRO-REPOSITORY/utf/src_utf/project.pj
** 2005/11/01 nschweiss Modified UTF_arg2 functions slightly to make more robust.
** 2005/09/15 nschweiss Remove '#define ix86' statement.
** 2005/08/04 Restored UTF_error to Steve's original version.  It prints a message & quits.
** 2005/07/25 Added typecasts for arguments in functions UTF_lookup_queue and UTF_queue_get
**            to prevent compiler warnings.
** Revision 1.1 2004/12/21 15:00:56EST sslegel
** Initial revision
** Member added to project /home/mksdata/MKS-SDO-REPOSITORY/fsw/tools/utf/project.pj
** Revision 1.3  2004/06/01 19:56:36  sslegel
** Removed feature to automatically print any sim mem access, as soon as I made it automatic people were asking me how to disable it.  So now you have to do it the old way using hooks.
** Revision 1.2  2004/05/07 19:27:56  sslegel
** Fix revision history tag
**
*/

/*
** Include Section
*/
#include "common_types.h"
#include "utf_custom.h"
#include "utf_types.h"
#include "utf_cfe.h"

/*
** Local Data Section
*/

/******************************************************************************
** PARSER STUFF
*/

#define UTF_STRING_TOKEN_SIZE            256

enum token_value {
    END_OF_LINE,
    NUMBER,
    NO_DATA,
    LABEL,
    SPECIAL_COMMAND,
};
static enum token_value token;

static char                              number_token;
static char                              string_token[UTF_STRING_TOKEN_SIZE];

/******************************************************************************
** SPECIAL COMMAND STUFF
*/

typedef struct
{
    char                        *name;
    utf_special_command_type    function;
} utf_special_command_table_type;

static utf_special_command_table_type    special_command_table[UTF_SPECIAL_COMMAND_TABLE_SIZE];
static long                              special_command_count = 0;

/******************************************************************************
** SIM MEMORY STUFF
*/

typedef struct
{
   unsigned long int            start;
   unsigned long int            size;
   char                         *memory;
   char                         *name;
} utf_sim_memory_lookup_table_type;

static utf_sim_memory_lookup_table_type  sim_memory_lookup_table[UTF_SIM_MEM_LOOKUP_TABLE_SIZE];
static long                              sim_address_count = 0;

static utf_write_sim_hook_type           write_sim_hook = NULL;
static utf_read_sim_hook_type            read_sim_hook = NULL;

/******************************************************************************
** FILE IO STUFF
*/

typedef struct
{
    char                         *filename;
    FILE                         *fd;
    unsigned long int            line_number;
} utf_file_context_type;

typedef struct
{
    unsigned long               source_id;
    int                         queue_id;    /* 0 for file, non-zero for queue */
    utf_file_context_type       file_context;
} utf_file_descriptor_table_type;

static utf_file_descriptor_table_type    file_descriptor_table[UTF_FD_TABLE_SIZE];
static long                              file_descriptor_count = 0;

static utf_file_context_type             default_fc_in;
static utf_file_context_type             default_fc_out;

static char                              *output_filename = "sbsim.out";

/******************************************************************************
** BUFFER STUFF
*/

typedef struct
{
    unsigned char               buffer[UTF_BUFFER_SIZE];
    char                        in_use;
} utf_buffer_table_type;

static utf_buffer_table_type             buffer_table[UTF_NUMBER_OF_BUFFERS];

/******************************************************************************
** QUEUE STUFF
*/

static utf_queue_type                    queue_table[UTF_QUEUE_COUNT + 1];
static long                              queue_table_count = 0;

/*
** Local prototypes
*/
static void read_hex_data(utf_file_context_type *fc, void *buffer, unsigned short int max_bytes, unsigned short int data_type);
static unsigned short int get_token(utf_file_context_type *fc);
static void cleanup(void);
static unsigned short int c_to_hex (char char_in);
static void split_line(char *string, int *argc, char *argv[]);

/******************************************************************************
** MEMORY MAPPED IO ROUTINES
**
** A set of routines are provided to simulate hardware registers/buffers
** that reside in fixed memory address spaces.  These routines map a
** hardware address space to a ram address space within an sim_memory_buffer
** array.  This memory can then be read and written to by your application
** code without having to conditionally compile out or add special code
** to your application.
*/

/******************************************************************************
**  Function: UTF_set_read_sim_hook()
**
**  Purpose:
**    This function sets a custom sim mem read hook.
*/
void UTF_set_read_sim_hook(utf_read_sim_hook_type hook)
{
    if (hook != NULL) {

        read_sim_hook = hook;
    }
    else {
        UTF_error("Sim Mem Read Sim Hook Function Pointer is NULL");
    }
}

/******************************************************************************
**  Function: UTF_set_write_sim_hook()
**
**  Purpose:
**    This function sets a custom sim mem write hook.
*/
void UTF_set_write_sim_hook(utf_write_sim_hook_type hook)
{
    if (hook != NULL) {

        write_sim_hook = hook;
    }
    else {
        UTF_error("Sim Mem Write Sim Hook Function Pointer is NULL");
    }
}

/******************************************************************************
**  Function: UTF_add_sim_address()
**
**  Purpose:
**    This function adds a memory segment to the sim mem lookup table.
*/
void UTF_add_sim_address(unsigned long start, unsigned long size, char *name)
{
    if (sim_address_count < UTF_SIM_MEM_LOOKUP_TABLE_SIZE) {

        if (size > 0) {

            sim_memory_lookup_table[sim_address_count].start = start;
            sim_memory_lookup_table[sim_address_count].size = size;
            sim_memory_lookup_table[sim_address_count].name = name;
            sim_memory_lookup_table[sim_address_count].memory = malloc(size);
            if (sim_memory_lookup_table[sim_address_count].memory == NULL) {
                UTF_error("Sim Mem Error Allocating Sim Memory Space, Address 0X%x, Bytes %d", start, size);
            }
            memset (sim_memory_lookup_table[sim_address_count].memory, 0, size);
            sim_address_count++;
        }
        else {
            UTF_error("Sim Mem Memory Segment Size is 0, Address: 0X%x", start);
        }
    }
    else {
        UTF_error("Sim Mem Maximum Number of Memory Addresses Exceeded, Max: %d", UTF_SIM_MEM_LOOKUP_TABLE_SIZE);
    }
}

/******************************************************************************
**  Function: UTF_sim_address_lookup()
**
**  Purpose:
**    This routine translates an hardware register/buffer address to a ram address
**    contained within the sim_memory array.  If the address passed into this routine
**    does not match an entry in the sim_memory_lookup_table then a NULL pointer will
**    be returned.  This routine also checks to be sure that the amount of memory that
**    you intend to access falls within a single memory segment in the
**    sim_memory_lookup_table
*/
void *UTF_sim_address_lookup(unsigned long int address, unsigned short int size)
{
    unsigned short int    i;

    /* search the sim_memory_lookup_table for a matching address */
    for (i=0; i < sim_address_count; i++)
    {	/* Check if starting address is within range */
        if ((address >= sim_memory_lookup_table[i].start) &&
            (address <  sim_memory_lookup_table[i].start + sim_memory_lookup_table[i].size))
        {
            /* check if planned read/write extends beyond this memory segment */
            if ((address - sim_memory_lookup_table[i].start) + size > sim_memory_lookup_table[i].size)
            {
                UTF_put_text("UTF ERROR: Requested simulated memory access extends beyond segment end.\n");
				UTF_put_text("Requested memory start addr / size: 0x%08x / 0x%04X\n",address,size);
				UTF_put_text("Simulated memory start addr / size: 0x%08X / 0x%04X\n",
                   			sim_memory_lookup_table[i].start, sim_memory_lookup_table[i].size);
				return NULL;
            }
			else
				/* Specified range lies within existing simulated memory region */
	            return((void *)(sim_memory_lookup_table[i].memory + (address - sim_memory_lookup_table[i].start)));
        }
    }

    return NULL;
}

/******************************************************************************
**  Function: UTF_sim_name_lookup()
**
**  Purpose:
**    This routine returns a descriptive name for the specified address.
*/
char *UTF_sim_name_lookup(unsigned long int address)
{

    unsigned short int    i;

    /* search the sim_memory_lookup_table for a matching address */
    for (i=0; i < sim_address_count; i++)
    {
        if ((address >= sim_memory_lookup_table[i].start) &&
            (address <  sim_memory_lookup_table[i].start + sim_memory_lookup_table[i].size)) {

            return(sim_memory_lookup_table[i].name);
        }
    }

    return "?";
}

/******************************************************************************
**  Function: UTF_read_sim_address()
**
**  Purpose:
**    This function reads a simulated hardware address and places the result
**    in a buffer pointed to by data_ptr.
*/
char UTF_read_sim_address(unsigned long int address, unsigned short int num_bytes, void *data_ptr)
{

    void *sim_memory_ptr;

    if (read_sim_hook == NULL ||
        read_sim_hook(address, num_bytes, data_ptr) == FALSE)
    {
        sim_memory_ptr = UTF_sim_address_lookup(address, num_bytes);
        if (sim_memory_ptr != NULL)
        {
            memcpy(data_ptr, sim_memory_ptr, num_bytes);
            return TRUE;
        }
        else
        {
            UTF_put_text("Sim Mem Memory Address Not Found 0x%8x\n", address);
            return FALSE;
        }
    }

    return TRUE;

}

/******************************************************************************
**  Function: UTF_write_sim_address()
**
**  Purpose:
**    This function writes the data contained in a buffer pointed to by data_ptr
**    to a simulated hardware address.
*/
char UTF_write_sim_address(unsigned long int address, unsigned short int num_bytes, void *data_ptr)
{

    void *sim_memory_ptr;

    if (write_sim_hook == NULL ||
        write_sim_hook(address, num_bytes, data_ptr) == FALSE)
    {
        sim_memory_ptr = UTF_sim_address_lookup(address, num_bytes);
        if (sim_memory_ptr != NULL)
        {
            memcpy(sim_memory_ptr, data_ptr, num_bytes);
            return TRUE;
        }
        else
        {
            UTF_put_text("Sim Mem Memory Address Not Found 0x%8x\n", address);
            return FALSE;
        }
    }

    return TRUE;
}

/******************************************************************************
** BUFFER MANAGEMENT ROUTINES
**
** A set of routines is provided to allocate and deallocate memory buffers.
** This code is currently used to simulate software bus buffers but it can
** also be used for other things.  The only restriction is that all buffers
** must be the same size, this was done to simplify the code.
*/

/******************************************************************************
**  Function: UTF_get_buffer()
**
**  Purpose:
**    This function allocates a free buffer from the pool.  If no buffers are
**    available then it returns a NULL pointer otherwise it returns a pointer to
**    a buffer.  Buffers can be made available for reuse by calling the
**    release_buffer routine.
*/
void *UTF_get_buffer(void)
{

    unsigned short int  i;

    /* simple loop through buffer table looking for the first free buffer */
    for (i=0; i < UTF_NUMBER_OF_BUFFERS; i++)
    {
        if (!buffer_table[i].in_use)
        {
            buffer_table[i].in_use = TRUE;
            return(&buffer_table[i].buffer);
        }
    }

    /* Error; all buffers in use; caller must handle this. */
    return NULL;
}

/******************************************************************************
**  Function: UTF_release_buffer()
**
**  Purpose:
**    This routine returns a previously allocated buffer back to the free buffer
**    pool.  If the buffer pointer passed into this routine does not match one of
**    the buffers in the pool then FALSE will be returned.  Otherwise if the buffer
**    is successfully released then TRUE will be returned.
*/
char UTF_release_buffer(void *buffer)
{

    unsigned short int  i;

    /* simple loop through buffer table looking for the buffer to be released */
    for (i=0; i < UTF_NUMBER_OF_BUFFERS; i++)
    {
        if ((buffer == &buffer_table[i].buffer) && (buffer_table[i].in_use))
        {
            buffer_table[i].in_use = FALSE;
            return TRUE;
        }
    }

    /* unable to release buffer */
    return FALSE;
}

/******************************************************************************
**  Function: UTF_valid_buffer()
**
**  Purpose:
**    This function checks the buffer pool and returns TRUE is the specified buffer is a
**    member of the buffer pool and is marked as allocated.  This can be used to
**    error check the buffer pointer without actually releasing the buffer.
*/
char UTF_valid_buffer(void *buffer)
{

    unsigned short int  i;

    /* simple loop through buffer table looking for the buffer in the buffer_table */
    for (i=0; i < UTF_NUMBER_OF_BUFFERS; i++)
    {
        if ((buffer == &buffer_table[i].buffer) && (buffer_table[i].in_use))
        {
            return TRUE;
        }
    }

    return FALSE;

}

/******************************************************************************
** QUEUE MANAGEMENT ROUTINES
*/

/******************************************************************************
**  Function: UTF_add_input_queue()
**
**  Purpose:
**    This function maps a source_id to a queue by adding a new entry to the
**    file_descriptor_table.
*/
void UTF_add_input_queue(unsigned long PipeId)
{
    if (file_descriptor_count >= UTF_FD_TABLE_SIZE) { /* 0 based array */
        UTF_error("Maximum Number of Input Sources Exceeded, Max: %d", UTF_FD_TABLE_SIZE);
    }
    else if (queue_table_count > UTF_QUEUE_COUNT) { /* 1 based array */
        UTF_error("Maximum Number of Input Queues Exceeded, Max: %d", UTF_QUEUE_COUNT);
    }
    else {
        file_descriptor_table[file_descriptor_count].source_id = PipeId;
        file_descriptor_table[file_descriptor_count].queue_id = queue_table_count + 1;  /* index into queue_table, must be > 0 */
        file_descriptor_table[file_descriptor_count].file_context.filename = NULL;      /* not used for queues */
        file_descriptor_table[file_descriptor_count].file_context.fd = NULL;            /* not used for queues */
        file_descriptor_table[file_descriptor_count].file_context.line_number = 0;      /* not used for queues */
        file_descriptor_count++;
        queue_table_count++;
    }
}

/******************************************************************************
**  Function: UTF_lookup_queue()
**
**  Purpose:
**    This function searches the file_descriptor_table for the specified
**    source_id.  If found it returns a pointer to a queue structure otherwise
**    it returns a NULL pointer.
*/
utf_queue_type *UTF_lookup_queue (CFE_SB_PipeId_t PipeId)
{

    int i;

    for (i = 0; i < file_descriptor_count; i++)
    {
        if ((file_descriptor_table[i].source_id == PipeId) &&
            (file_descriptor_table[i].queue_id > 0)) {

            return &queue_table[file_descriptor_table[i].queue_id];
        }
    }

    return NULL;
}

/******************************************************************************
**  Function: UTF_queue_add()
**
**  Purpose:
**    This function adds an entry to the queue.
*/
void UTF_queue_add (CFE_SB_PipeId_t PipeId, const void *data, size_t length)
{
   utf_queue_type *queue = UTF_lookup_queue (PipeId);
   int             next_head;

   if (queue == NULL)
      UTF_error ("Queue Not Found For source_id %lu\n", PipeId);

   next_head = (queue->head + 1) % UTF_QUEUE_SIZE;

   if (length > UTF_BUFFER_SIZE)
   {
      UTF_error ("Buffer Request For %d Bytes Exceeds UTF_BUFFER_SIZE.\n", length);
   }

   if (next_head == queue->tail)
   {
      UTF_error ("Queue Full, source_id %lu\n", PipeId);
   }

   queue->buffers[queue->head].data = UTF_get_buffer();
   if (queue->buffers[queue->head].data == NULL) {
      UTF_error ("Queue Buffer Allocation Error, Not Enough Buffers (UTF_NUMBER_OF_BUFFERS)\n");
   }

   memcpy(queue->buffers[queue->head].data, data, length);

   queue->buffers[queue->head].length = length;

   queue->head = next_head;

}

/******************************************************************************
**  Function: UTF_queue_get()
**
**  Purpose:
**    This function removes an entry from the queue.  If the queue is empty this
**    routine will assert an error so it is a good idea to check the queue using the
**    UTF_queue_empty routine first to be sure there is something available.
*/
void UTF_queue_get (CFE_SB_PipeId_t PipeId, void *buffer)
{
   utf_queue_type *queue = UTF_lookup_queue (PipeId);
   int             next_tail;

   if (queue == NULL)
      UTF_error ("Queue Not Found For source_id %lu\n", PipeId);

   next_tail = (queue->tail + 1) % UTF_QUEUE_SIZE;

   memcpy(buffer, queue->buffers[queue->tail].data, queue->buffers[queue->tail].length);
   UTF_release_buffer(queue->buffers[queue->tail].data);

   if (queue->head == queue->tail)
   {
      UTF_error ("Queue Empty, source_id %lu\n", PipeId);
   }

   queue->tail = next_tail;

}

/******************************************************************************
**  Function: UTF_queue_empty()
**
**  Purpose:
**    This function checks to see if the specified queue is empty.  Returns
**    TRUE if empty.
*/
char UTF_queue_empty (CFE_SB_PipeId_t PipeId)
{
   const utf_queue_type *queue     = UTF_lookup_queue (PipeId);

   if (queue == NULL)
      UTF_error ("Queue Not Found For source_id %lu\n", PipeId);

   return queue->head == queue->tail;
}

/******************************************************************************
** SPECIAL COMMAND ROUTINES
*/

/******************************************************************************
**  Function: UTF_add_special_command()
**
**  Purpose:
**    This function adds a special command to the special command lookup table.
*/
void UTF_add_special_command(char *name, utf_special_command_type function)
{

    if (function != NULL) {

        if (special_command_count < UTF_SPECIAL_COMMAND_TABLE_SIZE) {

            special_command_table[special_command_count].name = name;
            special_command_table[special_command_count].function = function;
            special_command_count++;
        }
        else {
            UTF_error("Maximum Number of Special Commands Exceeded, Max: %d", UTF_SPECIAL_COMMAND_TABLE_SIZE);
        }
    }
    else {
        UTF_error("Special Command Function Pointer is NULL, %s", name);
    }
}

/******************************************************************************
**  Function: UTF_special_command_lookup()
**
**  Purpose:
**    This function searches the special command lookup table for the specified
**    special command name.  If the command is not found then a NULL pointer is
**    returned, otherwise a pointer to the special command handler is returned.
*/
utf_special_command_type UTF_special_command_lookup(char *name)
{

    unsigned short int i;

    for (i=0; i < special_command_count; i++) {
        if (strcmp(name, special_command_table[i].name) == 0) {
            return special_command_table[i].function;
        }
    }
    return NULL;
}

/******************************************************************************
**  Function: UTF_process_special_command()
**
**  Purpose:
**    This function executes a special command.
*/
void UTF_process_special_command(utf_file_context_type *fc, char *command_line)
{

    utf_special_command_type    func;
    int                         argc;
    char                        *argv[UTF_SPECIAL_COMMAND_MAX_ARGS];

    split_line(command_line, &argc, argv);
    if (argc > 0) {

        func = UTF_special_command_lookup(argv[0]);  /* argv[0] is the name of the special command */
        if (func) {
            (*func)(argc, argv);
        }
        else {
            UTF_error("File: %s, Line: %d, Special Command Not Found, %s", fc->filename, fc->line_number, argv[0]);
        }
    }
    else {
        UTF_error("File: %s, Line: %d, Special Command Name Is Missing", fc->filename, fc->line_number);
    }
}

/******************************************************************************
**  Function: split_line()
**
**  Purpose:
**    Split a space delimited string into seperate arguments.  Once the maximum
**    number of arguments is reached then the remainder of the string is placed
**    in the last argument.  This also will modify the original string, inserting
**    null terminators in between arguments.
*/
static void split_line(char *string, int *argc, char *argv[])
{
    char  *ptr;
    int   c;

    *argc = 0;
    ptr = string;
    while (*argc < UTF_SPECIAL_COMMAND_MAX_ARGS) {

        while ((c = *ptr) != '\0' && isspace(c)) ptr++;  /* skip blank spaces */
        if (c == '\0')  break;                           /* at the end of the string so quit */
        argv[(*argc)++] = ptr;                           /* save a ptr to the start of the argument */
        while ((c = *ptr) != '\0' && !isspace(c)) ptr++; /* find the next space */
        *ptr = '\0';                                     /* replace the space with a null terminator */
        if (c != '\0') ptr++;                            /* skip to the next character */

    }

}

/******************************************************************************
**  Function: UTF_arg2uint()
**
**  Purpose:
**    This function converts a string to a unsigned long value.
*/
unsigned long UTF_arg2uint(char *val)
{
    unsigned long  ret_val;
    char           *end;

    ret_val = strtoul(val, &end, 0);
    if (*end != 0)
        UTF_put_text("UTF Warning: %s at end of Unsigned Integer Value %s ignored.\n", end, val);

    return ret_val;
}

/******************************************************************************
**  Function: UTF_arg2int()
**
**  Purpose:
**    This function converts a string to a long value.
*/
long UTF_arg2int(char *val)
{
    long  ret_val;
    char  *end;

    ret_val = strtol(val, &end, 0);
    if (*end != 0)
          UTF_put_text("UTF Warning: %s at end of Integer Value %s ignored.\n", end, val);
    return ret_val;
}

/******************************************************************************
**  Function: UTF_arg2dbl()
**
**  Purpose:
**    This function converts a string to a double floating point value.
*/
double UTF_arg2dbl(char *val)
{
    double  ret_val;
    char    *end;

    ret_val = strtod(val, &end);
    if (*end != 0)
         UTF_put_text("UTF Warning: %s at end of Double Float Value %s ignored.\n", end, val);
    return ret_val;
}

/******************************************************************************
** FILE IO ROUTINES
**
** A set of routines is provided to read and write data to and from text files.
** This software allows the application to read simulated data from multiple files.
** The software however only supports one output file.  The input files can contain
** data as well as special characters that tell the simulator to perform a specified
** function.
*/

/******************************************************************************
**  Function: UTF_add_input_file()
**
**  Purpose:
**    This function maps a source_id to a text file by adding a new entry to the
**    file_descriptor_table.
*/
void UTF_add_input_file(unsigned long source_id, char *filename)
{
    if (file_descriptor_count < UTF_FD_TABLE_SIZE) {
        file_descriptor_table[file_descriptor_count].source_id = source_id;
        file_descriptor_table[file_descriptor_count].queue_id = 0;          /* not used for files */
        file_descriptor_table[file_descriptor_count].file_context.filename = filename;
        file_descriptor_table[file_descriptor_count].file_context.fd = NULL;
        file_descriptor_table[file_descriptor_count].file_context.line_number = 1;
        file_descriptor_count++;
    }
    else {
        UTF_error("Maximum Number of Input Sources Exceeded, Max: %d", UTF_FD_TABLE_SIZE);
    }
}

/******************************************************************************
**  Function: UTF_set_output_filename()
**
**  Purpose:
**    This function sets the name of the output file.
*/
void UTF_set_output_filename(char *filename)
{
	/* Check if a current file is open */
    if (default_fc_out.fd != NULL) {
    	fclose(default_fc_out.fd);
    	default_fc_out.fd = NULL;
    }

	/* Check if the filename is not null */
	if (filename != "") {
	  output_filename = filename;
      default_fc_out.filename = output_filename;
      default_fc_out.fd = fopen(output_filename, "w");
      if (default_fc_out.fd == NULL) {
        UTF_error("Error Opening Output File %s", output_filename);
      }
      default_fc_out.line_number = 0;
	}
}

/******************************************************************************
**  Function: UTF_get_input_fc()
**
**  Purpose:
**    This routine returns a input file context structure for the specified source_id.
**    if the source_id is specified in the file_descriptor_table then the corresponding
**    file context will be returned, otherwise if the source_id is not found in the
**    file_descriptor_table then the default "sbsim.in" file context will be used.  Files
**    are opened the first time this routine is called for each source_id.
*/
utf_file_context_type *UTF_get_input_fc(unsigned long source_id)
{

    unsigned short int  i;

    /* search the file descriptor table for a matching source_id.  if found
       then either open the file if not already open or return the already open
       file context.  If no match is found in the descriptor table then use
       the default file context for the file "sbsim.in" */
    for (i=0; i < file_descriptor_count; i++)
    {
        if (source_id == file_descriptor_table[i].source_id)
        {
            if (file_descriptor_table[i].file_context.fd == NULL)
            {
                file_descriptor_table[i].file_context.fd = fopen(file_descriptor_table[i].file_context.filename, "r");
                if (file_descriptor_table[i].file_context.fd == NULL) {

                    UTF_error("Error Opening Input File %s", file_descriptor_table[i].file_context.filename);
                }
            }
            return(&file_descriptor_table[i].file_context);
        }
    }

    /* no matching source_id in the descriptor table so use the default */

#ifdef UTF_USE_STDIN

    if (default_fc_in.fd == NULL) {

        default_fc_in.filename = "stdin";
        default_fc_in.fd = stdin;
        default_fc_in.line_number = 1;
    }
    return &default_fc_in;

#else

    if (default_fc_in.fd == NULL) {

        default_fc_in.filename = "sbsim.in";
        default_fc_in.fd = fopen("sbsim.in", "r");
        if (default_fc_in.fd == NULL) {
            UTF_error("Error Opening Input File sbsim.in");
        }
        default_fc_in.line_number = 1;
    }
    return &default_fc_in;

#endif

}

/******************************************************************************
**  Function: UTF_get_input_fd()
**
**  Purpose:
**    This routine returns a input file handle for the specified source_id.
*/
FILE *UTF_get_input_fd(unsigned long source_id)
{
    return((UTF_get_input_fc(source_id))->fd);
}

/******************************************************************************
**  Function: UTF_get_output_fc()
**
**  Purpose:
**    Return a output file context. The output file is opened the first time this
**    routine is called.
*/
utf_file_context_type *UTF_get_output_fc (void)
{

#ifdef UTF_USE_STDOUT

    if (default_fc_out.fd == NULL) {

        default_fc_out.filename = "stdout";
        default_fc_out.fd = stdout;
        default_fc_out.line_number = 0;
    }
    return &default_fc_out;

#else

    if (default_fc_out.fd == NULL) {

        default_fc_out.filename = output_filename;
        default_fc_out.fd = fopen(output_filename, "w");
        if (default_fc_out.fd == NULL) {
            UTF_error("Error Opening Output File %s", output_filename);
        }
        default_fc_out.line_number = 0;
    }
    return &default_fc_out;

#endif

}

/******************************************************************************
**  Function: UTF_get_output_fd()
**
**  Purpose:
**    This routine returns a output file handle for the specified source_id.
*/
FILE *UTF_get_output_fd (void)
{
    return((UTF_get_output_fc())->fd);
}

/******************************************************************************
**  Function: UTF_put_text()
**
**  Purpose:
**    This routine writes a text string to the output file.  If you want a
**    carriage return then it must be added to the string to be output.
*/
void UTF_put_text(char *spec, ...)
{

    va_list  args;
    FILE     *fd;

    fd = UTF_get_output_fd();

    va_start(args, spec);
    vfprintf(fd, spec, args);
    va_end(args);

}

/******************************************************************************
**  Function: UTF_put_data()
**
**  Purpose:
**    This routine writes a data buffer to the output file.  It will write length
**    bytes of data as hex digits.  The data_type determines how the data should
**    be interpreted.  This is done to make the printed output easier to read
*/
void UTF_put_data(void *buffer, unsigned short int length, unsigned short int data_type)
{
    FILE               *fd;
    unsigned short int  i;
    unsigned long int  *dword_ptr = (unsigned long int *)buffer;
    unsigned short int *word_ptr  = (unsigned short int *)buffer;
    unsigned char      *byte_ptr  = (unsigned char *)buffer;

    fd = UTF_get_output_fd();

    switch(data_type)
    {
        case UTF_AS_BYTE:

            for (i=0; i < length; i++)
            {
                fprintf(fd, "%02x ", *byte_ptr++);
            }
            fprintf(fd, "\n");
            break;

        case UTF_AS_WORD:

            for (i=0; i < length; i+=2)
            {
                fprintf(fd, "%04x ", *word_ptr++);
            }
            fprintf(fd, "\n");
            break;

        case UTF_AS_DWORD:

            for (i=0; i < length; i+=4)
            {
                fprintf(fd, "%08lx ", *dword_ptr++);
            }
            fprintf(fd, "\n");
            break;

        default:
            UTF_error("Invalid data_type For Write, %d", data_type);
            break;
    }
}

/******************************************************************************
**  Function: UTF_get_data()
**
**  Purpose:
**    This routine reads data from a input file identified by the source_id.
**    The data read will be placed in the destination buffer using the specified
**    void pointer.  This routine calls get_token to parse the input file and return
**    a token which defines the action to be taken.  The simulation will be stopped
**    when the end_of_file is reached.  Data will be read until the length number of
**    bytes has been read or the end of the line is reached.  The data_type determines
**    how the data should be interpreted.  This is done to make the input text file
**    easier to edit and read
** 10/11/05 Returns TRUE if  a command message is received.
** Returns FALSE if an empty pipe indicator is
** received.  If a label, comment, or 'special command' aka script
** command is received, and processing continues until message or empty pipe.
** (No changes today -- just more description.)
*/
char UTF_get_data(unsigned long       source_id,
                  void               *buffer,
                  unsigned short int  length,
                  unsigned short int  data_type)
{

    utf_file_context_type    *fc;

    /* check to see if this source_id is mapped to a queue */
    if (UTF_lookup_queue((unsigned char) source_id) != NULL) {

        if (UTF_queue_empty(source_id)) {
            return(FALSE);
        }
        else {
            UTF_queue_get((unsigned char) source_id, buffer);
            return(TRUE);
        }
    }
    else { /* file input */

        fc = UTF_get_input_fc(source_id);

        while(1) {

            /* skip blank lines */
            do {
                token = get_token(fc);
            } while (token == END_OF_LINE);

            switch (token) {

                case NO_DATA:
                    return(FALSE);
                    break;

                case NUMBER:
                    read_hex_data(fc, buffer, length, data_type);
                    return(TRUE);
                    break;

                case LABEL:
                    UTF_put_text("%s\n", string_token);
                    break;

                case SPECIAL_COMMAND:
                    UTF_process_special_command(fc, string_token);
                    break;

                default:
                    /* should never get here */
                    return(FALSE);
                    break;
            }
        }
    }
}

/******************************************************************************
**  Function: read_hex_data()
**
**  Purpose:
**    This routine reads hex digits from the file specified by fc and places the
**    data in the buffer specified by the void buffer pointer.  The software will
**    read data until it hits the end of the line or it has read max_bytes of data.
**    Note that his code assumes that there will be only one data item (packet) per line.
*/
static void read_hex_data(utf_file_context_type *fc, void *buffer, unsigned short int max_bytes, unsigned short int data_type)
{

    unsigned short int  i;
    unsigned short int  bytes_read = 0;
    unsigned long int   *dword_ptr = (unsigned long int *)buffer;
    unsigned short int  *word_ptr  = (unsigned short int *)buffer;
    unsigned char       *byte_ptr  = (unsigned char *)buffer;

    switch(data_type) {

        /* assumes that 1 NUMBER token has already been read prior to calling this routine */

        case UTF_AS_BYTE:

            while ((token != END_OF_LINE) && (bytes_read < max_bytes)) {

                *byte_ptr = 0;

                for (i=0; i < 2; i++) {
                    if (token == END_OF_LINE) UTF_error("File: %s, Line: %d, Not Enough Characters For AS_BYTE Read", fc->filename, fc->line_number-1);
                    if (token != NUMBER)      UTF_error("File: %s, Line: %d, Invalid Hex Number Read", fc->filename, fc->line_number);
                    *byte_ptr = (*byte_ptr << 4) + c_to_hex(number_token);
                    token = get_token(fc);
                }

                bytes_read += 1;
                byte_ptr++;
            }

            if ((token != END_OF_LINE) && (bytes_read >= max_bytes))
                UTF_error("File: %s, Line: %d, Input Data Exceeds Max Size of %d", fc->filename, fc->line_number, max_bytes);

            break;

        case UTF_AS_WORD:

            if ((max_bytes % 2) != 0) UTF_error("File: %s, Line: %d, Invalid Max_Bytes Parameter For AS_WORD Read, %d", fc->filename, fc->line_number, max_bytes);

            while ((token != END_OF_LINE) && (bytes_read < max_bytes)) {

                *word_ptr = 0;

                for (i=0; i < 4; i++) {
                    if (token == END_OF_LINE) UTF_error("File: %s, Line: %d, Not Enough Characters For AS_WORD Read", fc->filename, fc->line_number-1);
                    if (token != NUMBER)      UTF_error("File: %s, Line: %d, Invalid Hex Number Read", fc->filename, fc->line_number);
                    *word_ptr = (*word_ptr << 4) + c_to_hex(number_token);
                    token = get_token(fc);
                }

                bytes_read += 2;
                word_ptr++;
            }

            if ((token != END_OF_LINE) && (bytes_read >= max_bytes))
                UTF_error("File: %s, Line: %d, Input Data Exceeds Max Size of %d", fc->filename, fc->line_number, max_bytes);

            break;

        case UTF_AS_DWORD:

            if ((max_bytes % 4) != 0) UTF_error("File: %s, Line: %d, Invalid Max_Bytes Parameter For AS_DWORD Read, %d", fc->filename, fc->line_number, max_bytes);

            while ((token != END_OF_LINE) && (bytes_read < max_bytes)) {

                *dword_ptr = 0;

                for (i=0; i < 8; i++) {
                    if (token == END_OF_LINE) UTF_error("File: %s, Line: %d, Not Enough Characters For AS_DWORD Read", fc->filename, fc->line_number-1);
                    if (token != NUMBER)      UTF_error("File: %s, Line: %d, Invalid Hex Number Read", fc->filename, fc->line_number);
                    *dword_ptr = (*dword_ptr << 4) + c_to_hex(number_token);
                    token = get_token(fc);
                }

                bytes_read += 4;
                dword_ptr++;
            }

            if ((token != END_OF_LINE) && (bytes_read >= max_bytes))
                UTF_error("File: %s, Line: %d, Input Data Exceeds Max Size of %d", fc->filename, fc->line_number, max_bytes);

            break;

        default:
            UTF_error("File: %s, Line: %d, Invalid DATA_TYPE For Read, %d", fc->filename, fc->line_number, data_type);
            break;
    }
}

/******************************************************************************
**  Function: get_token()
**
**  Purpose:
**    This routine parses a input text file looking for tokens.  To sipmlify things
**    all tokens are single characters. The following tokens are recognized:
**    \n = END_OF_LINE
**    !  = NO_DATA
**    *  = LABEL
**    @  = SPECIAL_COMMAND
**    ;  = comment which extends to the end of the line
**    hex digit = interpreted as a data set which extends to the end of the line
**    all spaces and unrecognized characters are skipped
*/
static unsigned short int get_token(utf_file_context_type *fc)
{

    char                ch;
    unsigned short int  i;

    while (TRUE) {

        /* skip blank spaces */
        do {
            if ((ch = fgetc(fc->fd)) == EOF) { cleanup(); }
        } while (isspace(ch) && ch != '\n');

        switch (ch) {

            case '\n':                  /* end of line */
                fc->line_number++;
                return(END_OF_LINE);
                break;

            case '!':                   /* no more packets on this pipe */
                return(NO_DATA);
                break;

            case '*':                   /* label */
                for (i=0; i < UTF_STRING_TOKEN_SIZE; i++) {
                    if ((ch = fgetc(fc->fd)) == EOF)  { cleanup(); }
                    else if (ch == '\n' || ch == ';') { ungetc(ch, fc->fd); string_token[i] = '\0'; return(LABEL); }
                    else                              { string_token[i] = ch; }
                }
                UTF_error("File: %s, Line: %d, Label Exceeds %d Characters, %s", fc->filename, fc->line_number, UTF_STRING_TOKEN_SIZE, string_token);
                break;

            case '@':                   /* special command */
                for (i=0; i < UTF_STRING_TOKEN_SIZE; i++) {
                    if ((ch = fgetc(fc->fd)) == EOF)  { cleanup(); }
                    else if (ch == '\n' || ch == ';') { ungetc(ch, fc->fd); string_token[i] = '\0'; return(SPECIAL_COMMAND); }
                    else                              { string_token[i] = ch; }
                }
                UTF_error("File: %s, Line: %d, Special Command Exceeds %d Characters, %s", fc->filename, fc->line_number, UTF_STRING_TOKEN_SIZE, string_token);
                break;

            case ';':                   /* comment extends to the end of the line */
                do {
                    if ((ch = fgetc(fc->fd)) == EOF) cleanup();
                } while (ch != '\n');
                ungetc(ch, fc->fd);
                break;

            case '0':                   /* hex digit */
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
                number_token = ch;
                return(NUMBER);
                break;

            default:                    /* skip unrecognized chars */
                break;
        }
    }
}

/******************************************************************************
**  Function: cleanup()
**
**  Purpose:
**    This routine closes all open files and stops the simulation.
*/
static void cleanup(void)
{

    unsigned short int  i;

    for (i=0; i < file_descriptor_count; i++) {

        if (file_descriptor_table[i].file_context.fd != NULL) {
            fclose(file_descriptor_table[i].file_context.fd);
        }
    }

    if (default_fc_in.fd != NULL) fclose(default_fc_in.fd);
    if (default_fc_out.fd != NULL) fclose(default_fc_out.fd);

    exit(0);

}

/******************************************************************************
**  Function: c_to_hex()
**
**  Purpose:
**    This routine converts text hex characters into there corresponding
**    numerical values.
*/
static unsigned short int c_to_hex (char char_in)
{
    if (char_in >= '0' && char_in <= '9')
        return (char_in - '0');
    else if (char_in >= 'A' && char_in <= 'F')
        return (char_in - 'A' + 0x0A);
    else if (char_in >= 'a' && char_in <= 'f')
        return (char_in - 'a' + 0x0A);
    else
        return(0);
}

/******************************************************************************
**  Function: UTF_error()
**
**  Purpose:
**    This function prints a error message to stderr and quits.
*/
void UTF_error(char *spec, ...)
{

    va_list  args;

    va_start(args, spec);
    vfprintf(stderr, spec, args);
    fprintf(stderr, "\n");
    fflush(stderr);
    va_end(args);

#ifdef UTF_INTERACTIVE

    /* beeps and waits for the user to press a key before exiting */
    fprintf(stderr, "\a");
    getc(stdin);
    cleanup();

#else

    abort();

#endif

}

/******************************************************************************
**  Function: UTF_exit()
**
**  Purpose:
**      Exit without printing a message.
*/
void UTF_exit (void)
{
    cleanup();
}

/******************************************************************************
**  Function: UTF_CFE_Init
**  Purpose:
**    Perform all standard initializations for the UTF. 
*/
int32 UTF_CFE_Init(void)
{
	/* Call CFE_ES_RegisterApp before CFE_TBL_EarlyInit to prevent warning */
	/* messages.  CFE_ES_RegisterApp can be called again later in application */
	/* and causes no problem in the UTF environment.                          */
	CFE_ES_RegisterApp();
	
	/* Initialize data structure which will be used by Table Services.    */
	CFE_TBL_EarlyInit ();
	
	/* Initialize data structure which will be used by Executive Services.    */
	UTF_ES_InitAppRecords();
	
	return CFE_SUCCESS;
	
}

