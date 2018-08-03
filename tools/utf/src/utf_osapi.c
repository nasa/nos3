/*
** File: utf_osapi.c
**  $Id: utf_osapi.c 1.10 2012/02/28 10:32:59GMT-05:00 wmoleski Exp  $
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
** Purpose: This module defines the UTF versions of functions for the OS APIs abstraction layer
**          implementation for POSIX.  The real functions are contained in the file osapi.c.
**
** References:
**
** Assumptions and Notes:
**
** $Date: 2012/02/28 10:32:59GMT-05:00 $
** $Revision: 1.10 $
** $Log: utf_osapi.c  $
** Revision 1.10 2012/02/28 10:32:59GMT-05:00 wmoleski 
** Added function hooks and Return Code handling and updated the examples to test these changes.
** Revision 1.9 2012/01/13 12:52:01EST acudmore 
** Changed license text to reflect open source
** Revision 1.8 2010/11/24 17:13:30EST jmdagost 
** Updated argument list for OS_InitAttachHandler()
** Revision 1.7 2010/10/25 15:06:46EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.6 2010/10/04 14:57:48EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.5 2010/09/20 13:42:59EDT wmoleski 
** The UTF source prototypes for OS_API_Init, OS_FS_Init, OS_TaskCreate and OS_TimerAPIInit were
** modified to match the OSAL 3.1 changes.
** Revision 1.4 2009/10/13 14:44:13EDT wmoleski 
** Added the UTF_version() function to print out the current UTF version being used.
** Revision 1.3 2008/08/21 15:32:37EDT wfmoleski 
** Added the capability of defining a function hook for the OS_BinSemGive function.
** Revision 1.2 2008/08/20 07:45:44EDT wfmoleski 
** The OS_API_Init() function calls a new FS api (OS_FS_Init()) function. This call was added to the UTF
** implementation.
** Revision 1.1 2008/04/17 08:09:39EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/src/project.pj
** Revision 1.9 2008/02/26 13:30:10EST wfmoleski 
** Added the OS_TaskDelete function hook capability to utf_osapi and the example test 14. I also changed
** the OS_FPUExFPUMask function in the UTF to OS_FPUExSetMask.
** Revision 1.8 2007/05/25 11:12:55EDT wfmoleski 
** Modifications to add new API functions and minor differences from cFE 4.1.0
** Revision 1.7 2006/06/22 14:01:13EDT wfmoleski 
** All of these changes added the copyright comments. There was nothing else that required any UTF
** changes to bring it up to cFE 3.3
** Revision 1.6 2006/06/14 16:12:13EDT nsschweiss 
** Incorporated Steve Slegel's fix to microsecond rounding problem in UTF_double_to_hwtime().
** Revision 1.5 2006/06/12 15:50:39EDT nsschweiss 
** Removed static keyword from UTF_double_to_hwtime return type.
** This will make the function signature consistent with the header file.
** Revision 1.4 2006/04/10 15:40:51EDT sslegel 
** Updated a comment
** Revision 1.3 2006/03/29 14:01:21EST nsschweiss 
** Modified so that system could be built under Mac_OS: included <time.h>.
** Revision 1.2 2006/03/24 14:27:16EST sslegel 
** Added queueget and queueput hooks
** Moved some functions to utf_bsp_support.c
** 2006/03/01 nschweiss Modified OS_MemCpy function signature to match header file exactly.
** 2005/09/15 nschweiss Remove '#define ix86' statement.
** 09/14/05 nschweiss Commented out some unneeded elements which were causing compiler warnings.
** 06/23/05 Copied from osapi.c in cFE Build 1. Functions have been gutted to provide baseline for cFE UTF.  N. Schweiss
*/

/*
** Include section
*/
#include "common_types.h"
#include "osapi.h"
#include "cfe_error.h"
#include "utf_types.h"
#include "utf_osapi.h"
#include "utf_version.h"
#include <time.h>

/*
** Global data for the API
*/
/* Macro definition */
#define round(x) ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))

/* Function Hook Stuff */
typedef struct
{
    int32 (*OS_TaskDelay)(uint32);
    int32 (*OS_BinSemTake)(uint32);
    int32 (*OS_BinSemTimedWait)(uint32, uint32);
    int32 (*OS_MutSemTake)(uint32);
    int32 (*OS_GetLocalTime)(OS_time_t *);
    int32 (*OS_QueueGet)(uint32, void *, uint32, uint32 *, int32);
    int32 (*OS_QueuePut)(uint32, void *, uint32, uint32);
    int32 (*OS_TaskDelete)(uint32);
    int32 (*OS_BinSemGive) (uint32);
    int32 (*OS_BinSemCreate) (uint32, const char *, uint32, uint32);
} UTF_OSAPI_HookTable_t;

UTF_OSAPI_HookTable_t UTF_OSAPI_HookTable = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

/* Return Code Stuff */
int32 cfe_osapi_return_value[NUM_OF_CFE_OSAPI_PROCS]=
{
    UTF_CFE_USE_DEFAULT_RETURN_CODE,
    UTF_CFE_USE_DEFAULT_RETURN_CODE,
};

/* Simulated Time Stuff */
static double                utf_simulated_time;
static OS_time_t             utf_simulated_hwtime;



/******************************************************************************
**  Function: UTF_OSAPI_set_function_hook()
**
**  Purpose:
**    Install a user defined hook function for a OSAPI function call.
*/
void UTF_OSAPI_set_function_hook(int Index, void *FunPtr)
{
    if      (Index == OS_TASKDELAY_HOOK)       { UTF_OSAPI_HookTable.OS_TaskDelay = FunPtr; }
    else if (Index == OS_BINSEMTAKE_HOOK)      { UTF_OSAPI_HookTable.OS_BinSemTake = FunPtr; }
    else if (Index == OS_BINSEMTIMEDWAIT_HOOK) { UTF_OSAPI_HookTable.OS_BinSemTimedWait = FunPtr; }
    else if (Index == OS_MUTSEMTAKE_HOOK)      { UTF_OSAPI_HookTable.OS_MutSemTake = FunPtr; }
    else if (Index == OS_GETLOCALTIME_HOOK)    { UTF_OSAPI_HookTable.OS_GetLocalTime = FunPtr; }
    else if (Index == OS_QUEUEGET_HOOK)        { UTF_OSAPI_HookTable.OS_QueueGet = FunPtr; }
    else if (Index == OS_QUEUEPUT_HOOK)        { UTF_OSAPI_HookTable.OS_QueuePut = FunPtr; }
    else if (Index == OS_TASKDELETE_HOOK)      { UTF_OSAPI_HookTable.OS_TaskDelete = FunPtr; }
    else if (Index == OS_BINSEMGIVE_HOOK)      { UTF_OSAPI_HookTable.OS_BinSemGive = FunPtr; }
    else if (Index == OS_BINSEMCREATE_HOOK)    { UTF_OSAPI_HookTable.OS_BinSemCreate = FunPtr; }
    else                                       { UTF_error("Invalid OSAPI Hook Index In Set Hook Call %d", Index); }
}

/******************************************************************************
**  Function: UTF_double_to_hwtime()
**
**  Purpose:
**    Convert a floating-point time value to hardware time format.
**    For this simulation, hardware time is identical to work time.
*/
OS_time_t UTF_double_to_hwtime(double time)
{
    OS_time_t   hwtime;

    hwtime.seconds = (uint32)(time);
    hwtime.microsecs  = round(1000000.0 * (time - (double)hwtime.seconds));

    return hwtime;
}

/******************************************************************************
**  Function: UTF_init_sim_time()
**
**  Purpose:
**    Initialize global variables for simulated TIME hardware abstraction layer.
**    This function must be called before using any TIME functions.
*/
void UTF_init_sim_time(double time)
{
   UTF_set_sim_time(time);
}

/******************************************************************************
**  Function: UTF_get_sim_time()
**
**  Purpose:
**    Return the current simulated time for the TIME library functions.
*/
double UTF_get_sim_time(void)
{
   return utf_simulated_time;
}

/******************************************************************************
**  Function: UTF_get_sim_hwtime()
**
**  Purpose:
**    Return the current simulated hardware time for the TIME library functions.
*/
OS_time_t UTF_get_sim_hwtime(void)
{
   return utf_simulated_hwtime;
}

/******************************************************************************
**  Function: UTF_set_sim_time()
**
**  Purpose:
**    Set the simulated time for the TIME library functions.  The simulated time is 
**    set as a floating point number based on seconds "." fraction. of a second.  So 
**    setting a time of 1.0625 = 1 second and 62500 microseconds, or 1 second and 1 
**    subsecond (16 subseconds per second so 1/16 = .0625).  
*/
void UTF_set_sim_time(double time)
{
   utf_simulated_time = time;
   utf_simulated_hwtime = UTF_double_to_hwtime(time);
}

/******************************************************************************
**  Function: UTF_version()
**
**  Print out the UTF version being used.
*/
void UTF_version(void)
{
  UTF_put_text("UTF Version: %d.%d.%d.%c\n",
		UTF_MAJOR_VERSION,
		UTF_MINOR_VERSION,
		UTF_SUBMINOR_VERSION,
		UTF_REVISION);
}

int32 UTF_CFE_OSAPI_Set_Api_Return_Code(int32 ApiIndex, uint32 ReturnCode)
{
    /* Check that ApiIndex is in the valid range */
    if ( (ApiIndex >= NUM_OF_CFE_OSAPI_PROCS ) || (ApiIndex < 0 ) )
    {
        UTF_put_text("UTF Error: Invalid ApiIndex %d passed to UTF_CFE_SB_Set_Api_Return_Code\n", ApiIndex);
        return -1;
    }
    /* If index is valid, assign return value and report action */
    cfe_osapi_return_value[ApiIndex] = ReturnCode;

    if (ReturnCode == UTF_CFE_USE_DEFAULT_RETURN_CODE)
    {
        UTF_put_text("UTF Info: Return code for CFE SB Api #%d returned to default\n", ApiIndex);
    }
    else
    {
        UTF_put_text("UTF Info: Return value for CFE SB Api #%d set to %lx\n", ApiIndex, ReturnCode);
    }
    return CFE_SUCCESS;
}

int32 UTF_CFE_OSAPI_Use_Default_Api_Return_Code(int32 ApiIndex)
{
    int32 Success;

    /* Check that ApiIndex is in the valid range */
    if ( (ApiIndex >= NUM_OF_CFE_OSAPI_PROCS ) || (ApiIndex < 0 ) )
    {
        UTF_put_text("UTF Error: Invalid ApiIndex %d passed to UTF_CFE_SB_Use_Default_Api_Return_Code\n", ApiIndex);
        return -1;
    }

    /* If index is valid, assign return value and report action */
    Success = UTF_CFE_OSAPI_Set_Api_Return_Code(ApiIndex, UTF_CFE_USE_DEFAULT_RETURN_CODE);

    return Success;
}

void UTF_SCRIPT_OSAPI_Set_Api_Return_Code(int argc,char *argv[])
{
    int32 Index;
    int32 Code;
    /* Check for correct number of arguments */
    if (argc != 3)
    {
        UTF_error("UTF Error: Read %d args w/script cmd UTF_SCRIPT_SB_Set_Api_Return_Code. Expected 2.\n", argc -1 );
        UTF_exit();
    }
    /* Extract argument values */
    Index = UTF_arg2dbl(argv[1]);
    Code  = UTF_arg2uint(argv[2]);
    /* Call function to set API return code */
    UTF_CFE_OSAPI_Set_Api_Return_Code(Index, Code);
    return;
}

void UTF_SCRIPT_OSAPI_Use_Default_Api_Return_Code(int argc,char *argv[])
{
    int32 Index;

    /* Check for correct number of arguments */
    if (argc != 2)
    {
        UTF_error("UTF Error: Read %d args w/script cmd UTF_SCRIPT_SB_Set_Api_Return_Code. Expected 1.\n", argc-1);
        UTF_exit();
    }

    /* Extract argument values */
    Index = UTF_arg2dbl(argv[1]);

    /* Call function to set API return code */
    UTF_CFE_OSAPI_Set_Api_Return_Code(Index, UTF_CFE_USE_DEFAULT_RETURN_CODE);
    return;
}

/*---------------------------------------------------------------------------------------
   Name: OS_API_Init

   Purpose: Initialize the tables that the OS API uses to keep track of information
            about objects

   returns: nothing
---------------------------------------------------------------------------------------*/
int32 OS_API_Init(void)
{
	return OS_FS_Init();
}

/**********************************************************************************
                                    TASK API
**********************************************************************************/

/*---------------------------------------------------------------------------------------
   Name: OS_TaskCreate

   Purpose: Creates a task and starts running it.

   returns: OS_INVALID_POINTER if any of the necessary pointers are NULL
            OS_ERR_NAME_TOO_LONG if the name of the task is too long to be copied
            OS_ERR_INVALID_PRIORITY if the priority is bad
            OS_ERR_NO_FREE_IDS if there can be no more tasks created
            OS_ERR_NAME_TAKEN if the name specified is already used by a task
            OS_ERROR if the operating system calls fail
            OS_SUCCESS if success

    NOTES: task_id is passed back to the user as the ID. stack_pointer is usually null.
           Flags are unused at this point.
---------------------------------------------------------------------------------------*/
int32 OS_TaskCreate (uint32 *task_id, const char *task_name, osal_task_entry function_pointer,
                      const uint32 *stack_pointer, uint32 stack_size, uint32 priority,
                      uint32 flags)
{
    (void) task_id;          /* refer to function argument to avoid compiler warning */
    (void) task_name;        /* refer to function argument to avoid compiler warning */
    (void) function_pointer; /* refer to function argument to avoid compiler warning */
    (void) stack_pointer;    /* refer to function argument to avoid compiler warning */
    (void) stack_size;       /* refer to function argument to avoid compiler warning */
    (void) priority;         /* refer to function argument to avoid compiler warning */
    (void) flags;            /* refer to function argument to avoid compiler warning */

    return(OS_SUCCESS);
}

/*--------------------------------------------------------------------------------------
     Name: OS_TaskDelete

    Purpose: Deletes the specified Task and removes it from the OS_task_table.

    returns: OS_ERR_INVALID_ID if the ID given to it is invalid
             OS_ERROR if the OS delete call fails
             OS_SUCCESS if success
---------------------------------------------------------------------------------------*/
int32 OS_TaskDelete (uint32 task_id)
{
    if (UTF_OSAPI_HookTable.OS_TaskDelete)
        return(UTF_OSAPI_HookTable.OS_TaskDelete(task_id));

    return(OS_SUCCESS);
}

/*--------------------------------------------------------------------------------------
     Name: OS_TaskInstallDeleteHandler

    Purpose: Installs a handler for when the task is deleted.

    returns: status
---------------------------------------------------------------------------------------*/
int32 OS_TaskInstallDeleteHandler(void *function_pointer)
{
	if (function_pointer == NULL)
		return(OS_INVALID_POINTER);
		
	UTF_OSAPI_set_function_hook(OS_TASKDELETE_HOOK, function_pointer);
    return(OS_SUCCESS);
}

/*--------------------------------------------------------------------------------------
     Name: OS_TaskExit

    Purpose: Exits the calling task and removes it from the OS_task_table.

    returns: Nothing 
---------------------------------------------------------------------------------------*/

void OS_TaskExit()
{
	UTF_exit();
}

/*---------------------------------------------------------------------------------------
   Name: OS_TaskDelay

   Purpose: Delay a task for specified amount of milliseconds

   returns: OS_ERROR if sleep fails or millisecond = 0
            OS_SUCCESS if success
---------------------------------------------------------------------------------------*/
int32 OS_TaskDelay(uint32 millisecond)
{
    (void) millisecond;          /* refer to function argument to avoid compiler warning */

    if (UTF_OSAPI_HookTable.OS_TaskDelay)
        return(UTF_OSAPI_HookTable.OS_TaskDelay(millisecond));

    return(OS_SUCCESS);
}

/*---------------------------------------------------------------------------------------
   Name: OS_TaskSetPriority

   Purpose: Sets the given task to a new priority

    returns: OS_ERR_INVALID_ID if the ID passed to it is invalid
             OS_ERR_INVALID_PRIORITY if the priority is greater than the max
             allowed
             OS_ERROR if the OS call to change the priority fails
             OS_SUCCESS if success
---------------------------------------------------------------------------------------*/
int32 OS_TaskSetPriority (uint32 task_id, uint32 new_priority)
{
    (void) task_id;          /* refer to function argument to avoid compiler warning */
    (void) new_priority;     /* refer to function argument to avoid compiler warning */

    return (OS_SUCCESS);
}

/*---------------------------------------------------------------------------------------
   Name: OS_TaskRegister

   Purpose: Registers the calling task id with the task by adding the var to the tcb
            It searches the OS_task_table to find the task_id corresponding to the tcb_id

   Returns: OS_ERR_INVALID_ID if there the specified ID could not be found
            OS_ERROR if the OS call fails
            OS_SUCCESS if success
---------------------------------------------------------------------------------------*/
static uint32 utf_task_registered = FALSE;
int32 OS_TaskRegister (void)
{
    utf_task_registered = TRUE;
    return (OS_SUCCESS);
}

/*---------------------------------------------------------------------------------------
   Name: OS_TaskGetId

   Purpose: This function returns the #defined task id of the calling task

   Notes: The OS_task_key is initialized by the task switch if AND ONLY IF the
          OS_task_key has been registered via OS_TaskRegister(..).  If this is not
          called prior to this call, the value will be old and wrong.
---------------------------------------------------------------------------------------*/
uint32 OS_TaskGetId (void)
{
    if (utf_task_registered == FALSE) {
        UTF_put_text("Task not registered before first call to OS_TaskGetId");
    }
    return (uint32)(7);
}

/*--------------------------------------------------------------------------------------
    Name: OS_TaskGetIdByName

    Purpose: This function tries to find a task Id given the name of a task

    Returns: OS_INVALID_POINTER if the pointers passed in are NULL
             OS_ERR_NAME_TOO_LONG if th ename to found is too long to begin with
             OS_ERR_NAME_NOT_FOUND if the name wasn't found in the table
             OS_SUCCESS if SUCCESS
---------------------------------------------------------------------------------------*/
int32 OS_TaskGetIdByName (uint32 *task_id, const char *task_name)
{
    (void) task_id;          /* refer to function argument to avoid compiler warning */
    (void) task_name;        /* refer to function argument to avoid compiler warning */

    if (utf_task_registered == FALSE) {
        UTF_put_text("Task not registered before first call to OS_TaskGetIdByName");
    }
    return (uint32)(7);
}

/*---------------------------------------------------------------------------------------
    Name: OS_TaskGetInfo

    Purpose: This function will pass back a pointer to structure that contains
             all of the relevant info (creator, stack size, priority, name) about the
             specified task.

    Returns: OS_ERR_INVALID_ID if the ID passed to it is invalid
             OS_INVALID_POINTER if the task_prop pointer is NULL
             OS_SUCCESS if it copied all of the relevant info over
---------------------------------------------------------------------------------------*/
int32 OS_TaskGetInfo (uint32 task_id, OS_task_prop_t *task_prop)
{
    (void) task_id;          /* refer to function argument to avoid compiler warning */

    task_prop = NULL; /* currently not implemented */
    return(OS_INVALID_POINTER);
}

/****************************************************************************************
                              MESSAGE QUEUE API
*****************************************************************************************/

/*---------------------------------------------------------------------------------------
   Name: OS_QueueCreate

   Purpose: Create a message queue which can be refered to by name or ID

   Returns: OS_INVALID_POINTER if a pointer passed in is NULL
            OS_ERR_NAME_TOO_LONG if the name passed in is too long
            OS_ERR_NO_FREE_IDS if there are already the max queues created
            OS_ERR_NAME_TAKEN if the name is already being used on another queue
            OS_ERROR if the OS create call fails
            OS_SUCCESS if success

   Notes: the flags parameter is unused.
---------------------------------------------------------------------------------------*/
int32 OS_QueueCreate (uint32 *queue_id, const char *queue_name, uint32 queue_depth,
                       uint32 data_size, uint32 flags)
{
    (void) queue_id;      /* refer to function argument to avoid compiler warning */
    (void) queue_name;    /* refer to function argument to avoid compiler warning */
    (void) queue_depth;   /* refer to function argument to avoid compiler warning */
    (void) data_size;     /* refer to function argument to avoid compiler warning */
    (void) flags;         /* refer to function argument to avoid compiler warning */

    return (OS_SUCCESS);
}

/*--------------------------------------------------------------------------------------
    Name: OS_QueueDelete

    Purpose: Deletes the specified message queue.

    Returns: OS_ERR_INVALID_ID if the id passed in does not exist
             OS_ERROR if the OS call to delete the queue fails
             OS_SUCCESS if success

    Notes: If There are messages on the queue, they will be lost and any subsequent
           calls to QueueGet or QueuePut to this queue will result in errors
---------------------------------------------------------------------------------------*/
int32 OS_QueueDelete (uint32 queue_id)
{
    (void) queue_id;      /* refer to function argument to avoid compiler warning */

    return (OS_SUCCESS);
}

/*---------------------------------------------------------------------------------------
   Name: OS_QueueGet

   Purpose: Receive a message on a message queue.  Will pend or timeout on the receive.
   Returns: OS_ERR_INVALID_ID if the given ID does not exist
            OS_ERR_INVALID_POINTER if a pointer passed in is NULL
            OS_QUEUE_EMPTY if the Queue has no messages on it to be recieved
            OS_QUEUE_TIMEOUT if the timeout was OS_PEND and the time expired
            OS_QUEUE_INVALID_SIZE if the size copied from the queue was not correct
            OS_SUCCESS if success
---------------------------------------------------------------------------------------*/
int32 OS_QueueGet (uint32 queue_id, void *data, uint32 size, uint32 *size_copied, int32 timeout)
{

    if (UTF_OSAPI_HookTable.OS_QueueGet)
        return(UTF_OSAPI_HookTable.OS_QueueGet(queue_id, data, size, size_copied, timeout));

    if(data == NULL || size_copied == NULL )
    {
        UTF_put_text("QueueGet Parameter failure\n");
        return (OS_ERROR);
    }
    return (OS_SUCCESS);
}

/*---------------------------------------------------------------------------------------
   Name: OS_QueuePut

   Purpose: Put a message on a message queue.

   Returns: OS_ERR_INVALID_ID if the queue id passed in is not a valid queue
            OS_INVALID_POINTER if the data pointer is NULL
            OS_QUEUE_FULL if the queue cannot accept another message
            OS_ERROR if the OS call returns an error
            OS_SUCCESS if SUCCESS

   Notes: The flags parameter is not used.  The message put is always configured to
            immediately return an error if the receiving message queue is full.
---------------------------------------------------------------------------------------*/
int32 OS_QueuePut (uint32 queue_id, void *data, uint32 size, uint32 flags)
{

    if (UTF_OSAPI_HookTable.OS_QueuePut)
        return(UTF_OSAPI_HookTable.OS_QueuePut(queue_id, data, size, flags));

    if(data == NULL )
    {
        UTF_put_text("QueuePut Parameter failure\n");
        return (OS_ERROR);
    }
    return (OS_SUCCESS);
}

/*--------------------------------------------------------------------------------------
    Name: OS_QueueGetIdByName

    Purpose: This function tries to find a queue Id given the name of the queue. The
             id of the queue is passed back in queue_id

    Returns: OS_INVALID_POINTER if the name or id pointers are NULL
             OS_ERR_NAME_TOO_LONG the name passed in is too long
             OS_ERR_NAME_NOT_FOUND the name was not found in the table
             OS_SUCCESS if success
---------------------------------------------------------------------------------------*/
int32 OS_QueueGetIdByName (uint32 *queue_id, const char *queue_name)
{
    (void) queue_name;         /* refer to function argument to avoid compiler warning */

    if(queue_id == NULL)
    {
        UTF_put_text("QueueGetIdByName Parameter failure\n");
        return (OS_ERROR);
    }
    else
    {
        return (OS_SUCCESS);
    }
}

/******************************************************************************************
                                  SEMAPHORE API
******************************************************************************************/

/*---------------------------------------------------------------------------------------
   Name: OS_BinSemCreate

   Purpose: Creates a binary semaphore with initial value specified by
            sem_initial_value and name specified by sem_name. sem_id will be
            returned to the caller

   Returns: OS_INVALID_POINTER if sen name or sem_id are NULL
            OS_ERR_NAME_TOO_LONG if the name given is too long
            OS_ERR_NO_FREE_IDS if all of the semaphore ids are taken
            OS_ERR_NAME_TAKEN if this is already the name of a binary semaphore
            OS_SEM_FAILURE if the OS call failed
            OS_SUCCESS if success

   Notes: options is an unused parameter
---------------------------------------------------------------------------------------*/
int32 OS_BinSemCreate (uint32 *sem_id, const char *sem_name, uint32 sem_initial_value, uint32 options)
{
    (void) sem_name;           /* refer to function argument to avoid compiler warning */
    (void) sem_initial_value;  /* refer to function argument to avoid compiler warning */
    (void) options;            /* refer to function argument to avoid compiler warning */

    if (UTF_OSAPI_HookTable.OS_BinSemCreate)
        return(UTF_OSAPI_HookTable.OS_BinSemCreate(sem_id,sem_name,sem_initial_value,options));

    /* Handle Preset Return Code */
    if (cfe_osapi_return_value[CFE_OSAPI_BINSEMCREATE_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
    {
        return cfe_osapi_return_value[CFE_OSAPI_BINSEMCREATE_PROC];
    }

    if (*sem_id > OS_MAX_BIN_SEMAPHORES)
    {
        UTF_put_text("BinSemCreate Parameter failure\n");
        return (OS_ERROR);
    }
    return (OS_SUCCESS);
}

/*--------------------------------------------------------------------------------------
     Name: OS_BinSemDelete

    Purpose: Deletes the specified Binary Semaphore.

    Returns: OS_ERR_INVALID_ID if the id passed in is not a valid binary semaphore
             OS_ERR_SEM_NOT_FULL if the semahore is taken and cannot be deleted
             OS_SEM_FAILURE the OS call failed
             OS_SUCCESS if success

    Notes: Since we can't delete a semaphore which is currently locked by some task
           (as it may ber crucial to completing the task), the semaphore must be full to
           allow deletion.
---------------------------------------------------------------------------------------*/
int32 OS_BinSemDelete (uint32 sem_id)
{
    if (sem_id > OS_MAX_BIN_SEMAPHORES)
    {
        UTF_put_text("BinSemDelete Parameter failure\n");
        return (OS_ERROR);
    }
    return (OS_SUCCESS);
}

/*---------------------------------------------------------------------------------------
    Name: OS_BinSemGive

    Purpose: The function  unlocks the semaphore referenced by sem_id by performing
             a semaphore unlock operation on that semaphore.If the semaphore value
             resulting from this operation is positive, then no threads were blocked
             waiting for the semaphore to become unlocked; the semaphore value is
             simply incremented for this semaphore.

    Returns: OS_SEM_FAILURE the semaphore was not previously  initialized or is not
             in the array of semaphores defined by the system
             OS_ERR_INVALID_ID if the id passed in is not a binary semaphore
             OS_SUCCESS if success
---------------------------------------------------------------------------------------*/
int32 OS_BinSemGive (uint32 sem_id)
{
	/* Check to see if a functin hook is defined */
    if (UTF_OSAPI_HookTable.OS_BinSemGive)
        return(UTF_OSAPI_HookTable.OS_BinSemGive(sem_id));

    if (sem_id >= OS_MAX_BIN_SEMAPHORES)
    {
        UTF_put_text("BinSemGive Parameter failure\n");
        return (OS_SEM_FAILURE);
    }
    return (OS_SUCCESS);
}

/*---------------------------------------------------------------------------------------
    Name:    OS_BinSemTake

    Purpose: The locks the semaphore referenced by sem_id by performing a
             semaphore lock operation on that semaphore.If the semaphore value
             is currently zero, then the calling thread shall not return from
             the call until it either locks the semaphore or the call is
             interrupted by a signal.

    Return:  OS_SEM_FAILURE : the semaphore was not previously initialized
             or is not in the array of semaphores defined by the system
             OS_ERR_INVALID_ID the Id passed in is not a valid binar semaphore
             OS_SEM_FAILURE if the OS call failed
             OS_SUCCESS if success
----------------------------------------------------------------------------------------*/
int32 OS_BinSemTake (uint32 sem_id)
{
    if (UTF_OSAPI_HookTable.OS_BinSemTake)
        return(UTF_OSAPI_HookTable.OS_BinSemTake(sem_id));

    if (sem_id >= OS_MAX_BIN_SEMAPHORES)
    {
        UTF_put_text("BinSemTake Parameter failure\n");
        return (OS_SEM_FAILURE);
    }
    return (OS_SUCCESS);
}

/*---------------------------------------------------------------------------------------
    Name: OS_BinSemTimedWait

    Purpose: The function locks the semaphore referenced by sem_id . However,
             if the semaphore cannot be locked without waiting for another process
             or thread to unlock the semaphore , this wait shall be terminated when
             the specified timeout ,msecs, expires.

    Returns: OS_SEM_TIMEOUT if semaphore was not relinquished in time
             OS_SUCCESS if success
             OS_SEM_FAILURE the semaphore was not previously initialized or is not
             in the array of semaphores defined by the system
             OS_ERR_INVALID_ID if the ID passed in is not a valid semaphore ID

NOTE: sem_timedwait is not implemented in OS X POSIX. The code that would be required is
left #if 0 'd out for when sem_timewait is implemented. For now, however, a rough
apporoximation by sem_trywait is used instead.
----------------------------------------------------------------------------------------*/
int32 OS_BinSemTimedWait (uint32 sem_id, uint32 msecs)
{
    if (UTF_OSAPI_HookTable.OS_BinSemTimedWait)
        return(UTF_OSAPI_HookTable.OS_BinSemTimedWait(sem_id, msecs));

    /* Handle Preset Return Code */
    if (cfe_osapi_return_value[CFE_OSAPI_BINSEMTIMEDWAIT_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
    {
        return cfe_osapi_return_value[CFE_OSAPI_BINSEMTIMEDWAIT_PROC];
    }

    if (sem_id >= OS_MAX_BIN_SEMAPHORES)
    {
        UTF_put_text("BinSemTimedWait Parameter failure\n");
        return (OS_SEM_FAILURE);
    }
    return (OS_SUCCESS);
}

/*--------------------------------------------------------------------------------------
    Name: OS_BinSemGetIdByName

    Purpose: This function tries to find a binary sem Id given the name of a bin_sem
             The id is returned through sem_id

    Returns: OS_INVALID_POINTER if semid or sem_name are NULL pointers
             OS_ERR_NAME_TOO_LONG if the name given is to long to have been stored
             OS_ERR_NAME_NOT_FOUND if the name was not found in the table
             OS_SUCCESS if success
---------------------------------------------------------------------------------------*/
int32 OS_BinSemGetIdByName (uint32 *sem_id, const char *sem_name)
{
    (void) sem_name;           /* refer to function argument to avoid compiler warning */

    sem_id = NULL;  /* currently not implemented */
    return(OS_INVALID_POINTER);
}

/*---------------------------------------------------------------------------------------
    Name: OS_BinSemGetInfo

    Purpose: This function will pass back a pointer to structure that contains
             all of the relevant info( name and creator) about the specified binary
             semaphore.

    Returns: OS_ERR_INVALID_ID if the id passed in is not a valid semaphore
             OS_INVALID_POINTER if the bin_prop pointer is null
             OS_SUCCESS if success
---------------------------------------------------------------------------------------*/
int32 OS_BinSemGetInfo (uint32 sem_id, OS_bin_sem_prop_t *bin_prop)
{
    (void) sem_id;          /* refer to function argument to avoid compiler warning */

    bin_prop = NULL;  /* currently not implemented */
    return(OS_INVALID_POINTER);
}

/*---------------------------------------------------------------------------------------
   Name: OS_CountSemCreate

   Purpose: Creates a counting semaphore with initial value specified by
            sem_initial_value and name specified by sem_name. sem_id will be
            returned to the caller

   Returns: OS_INVALID_POINTER if sen name or sem_id are NULL
            OS_ERR_NAME_TOO_LONG if the name given is too long
            OS_ERR_NO_FREE_IDS if all of the semaphore ids are taken
            OS_ERR_NAME_TAKEN if this is already the name of a counting semaphore
            OS_SEM_FAILURE if the OS call failed
            OS_SUCCESS if success


   Notes: options is an unused parameter
---------------------------------------------------------------------------------------*/
int32 OS_CountSemCreate (uint32 *sem_id, const char *sem_name, uint32 sem_initial_value,
                        uint32 options)
{
    if (sem_id == NULL || sem_name == NULL)
        return OS_INVALID_POINTER;

    /* we don't want to allow names too long*/
    /* if truncated, two names might be the same */

    if (strlen(sem_name) >= OS_MAX_API_NAME)
            return OS_ERR_NAME_TOO_LONG;

    if (*sem_id > OS_MAX_COUNT_SEMAPHORES)
    {
        UTF_put_text("BinSemCreate Parameter failure\n");
        return (OS_ERROR);
    }
    return (OS_SUCCESS);
}

/*--------------------------------------------------------------------------------------
     Name: OS_CountSemDelete

    Purpose: Deletes the specified Counting Semaphore.

    Returns: OS_ERR_INVALID_ID if the id passed in is not a valid counting semaphore
             OS_ERR_SEM_NOT_FULL if the semahore is taken and cannot be deleted
             OS_SEM_FAILURE the OS call failed
             OS_SUCCESS if success

    Notes: Since we can't delete a semaphore which is currently locked by some task
           (as it may ber crucial to completing the task), the semaphore must be full to
           allow deletion.
---------------------------------------------------------------------------------------*/
int32 OS_CountSemDelete (uint32 sem_id)
{
	if (sem_id > OS_MAX_COUNT_SEMAPHORES)
    {
        UTF_put_text("BinSemDelete Parameter failure\n");
        return (OS_ERROR);
    }
    return (OS_SUCCESS);
}

/*---------------------------------------------------------------------------------------
    Name: OS_CountSemGive

    Purpose: The function  unlocks the semaphore referenced by sem_id by performing
             a semaphore unlock operation on that semaphore.If the semaphore value 
             resulting from this operation is positive, then no threads were blocked             waiting for the semaphore to become unlocked; the semaphore value is
             simply incremented for this semaphore.

    
    Returns: OS_SEM_FAILURE the semaphore was not previously  initialized or is not
             in the array of semaphores defined by the system
             OS_ERR_INVALID_ID if the id passed in is not a counting semaphore
             OS_SUCCESS if success

---------------------------------------------------------------------------------------*/
int32 OS_CountSemGive (uint32 sem_id)
{
	if (sem_id > OS_MAX_COUNT_SEMAPHORES)
    {
        UTF_put_text("CountSemGive Parameter failure\n");
		return OS_ERR_INVALID_ID;
    }
    return (OS_SUCCESS);
}

/*---------------------------------------------------------------------------------------
    Name:    OS_CountSemTake

    Purpose: The locks the semaphore referenced by sem_id by performing a
             semaphore lock operation on that semaphore.If the semaphore value
             is currently zero, then the calling thread shall not return from
             the call until it either locks the semaphore or the call is
             interrupted by a signal.

    Return:  OS_SEM_FAILURE : the semaphore was not previously initialized
             or is not in the array of semaphores defined by the system
             OS_ERR_INVALID_ID the Id passed in is not a valid countar semaphore
             OS_SEM_FAILURE if the OS call failed
             OS_SUCCESS if success

----------------------------------------------------------------------------------------*/
int32 OS_CountSemTake (uint32 sem_id)
{
    /* Check Parameters */
    if(sem_id >= OS_MAX_COUNT_SEMAPHORES)
        return OS_ERR_INVALID_ID;

    return OS_SUCCESS;

}/* end OS_CountSemTake */


/*---------------------------------------------------------------------------------------
    Name: OS_CountSemTimedWait

    Purpose: The function locks the semaphore referenced by sem_id . However,
             if the semaphore cannot be locked without waiting for another process
             or thread to unlock the semaphore , this wait shall be terminated when
             the specified timeout ,msecs, expires.

    Returns: OS_SEM_TIMEOUT if semaphore was not relinquished in time
             OS_SUCCESS if success
             OS_SEM_FAILURE the semaphore was not previously initialized or is not
             in the array of semaphores defined by the system
             OS_ERR_INVALID_ID if the ID passed in is not a valid semaphore ID
----------------------------------------------------------------------------------------*/
int32 OS_CountSemTimedWait (uint32 sem_id, uint32 msecs)
{
   if (sem_id >= OS_MAX_COUNT_SEMAPHORES)
        return OS_ERR_INVALID_ID;

    return OS_SUCCESS;

}/* end OS_CountSemTimedWait */

/*--------------------------------------------------------------------------------------
    Name: OS_CountSemGetIdByName

    Purpose: This function tries to find a counting sem Id given the name of a count_sem
             The id is returned through sem_id

    Returns: OS_INVALID_POINTER is semid or sem_name are NULL pointers
             OS_ERR_NAME_TOO_LONG if the name given is to long to have been stored
             OS_ERR_NAME_NOT_FOUND if the name was not found in the table
             OS_SUCCESS if success

---------------------------------------------------------------------------------------*/
int32 OS_CountSemGetIdByName (uint32 *sem_id, const char *sem_name)
{
    if (sem_id == NULL || sem_name == NULL)
        return OS_INVALID_POINTER;

    /* a name too long wouldn't have been allowed in the first place
     * so we definitely won't find a name too long*/

    if (strlen(sem_name) >= OS_MAX_API_NAME)
		return OS_ERR_NAME_TOO_LONG;

    return OS_SUCCESS;

}/* end OS_CountSemGetIdByName */

/*---------------------------------------------------------------------------------------
    Name: OS_CountSemGetInfo

    Purpose: This function will pass back a pointer to structure that contains
             all of the relevant info( name and creator) about the specified counting
             semaphore.

    Returns: OS_ERR_INVALID_ID if the id passed in is not a valid semaphore
             OS_INVALID_POINTER if the count_prop pointer is null
             OS_SUCCESS if success
---------------------------------------------------------------------------------------*/
int32 OS_CountSemGetInfo (uint32 sem_id, OS_count_sem_prop_t *count_prop)
{
    /* Check to see that the id given is valid */
    if (sem_id >= OS_MAX_COUNT_SEMAPHORES)
        return OS_ERR_INVALID_ID;

    if (count_prop == NULL)
        return OS_INVALID_POINTER;

    return OS_SUCCESS;

} /* end OS_CountSemGetInfo */

/****************************************************************************************
                                  MUTEX API
****************************************************************************************/

/*---------------------------------------------------------------------------------------
    Name: OS_MutSemCreate

    Purpose: Creates a mutex semaphore initially full.

    Returns: OS_INVALID_POINTER if sem_id or sem_name are NULL
             OS_ERR_NAME_TOO_LONG if the sem_name is too long to be stored
             OS_ERR_NO_FREE_IDS if there are no more free mutex Ids
             OS_ERR_NAME_TAKEN if there is already a mutex with the same name
             OS_SEM_FAILURE if the OS call failed
             OS_SUCCESS if success

    Notes: the options parameter is not used in this implementation
---------------------------------------------------------------------------------------*/
int32 OS_MutSemCreate (uint32 *sem_id, const char *sem_name, uint32 options)
{
    (void) sem_name;        /* refer to function argument to avoid compiler warning */
    (void) options;         /* refer to function argument to avoid compiler warning */

    if (*sem_id > OS_MAX_MUTEXES)
    {
        UTF_put_text("MutSemCreate Parameter failure\n");
        return (OS_ERROR);
    }
    return (OS_SUCCESS);
}

/*--------------------------------------------------------------------------------------
     Name: OS_MutSemDelete

    Purpose: Deletes the specified Mutex Semaphore.

    Returns: OS_ERR_INVALID_ID if the id passed in is not a valid mutex
             OS_ERR_SEM_NOT_FULL if the mutex is empty
             OS_SEM_FAILURE if the OS call failed
             OS_SUCCESS if success

    Notes: The mutex must be full to take it, so we have to check for fullness
---------------------------------------------------------------------------------------*/
int32 OS_MutSemDelete (uint32 sem_id)
{
    if (sem_id > OS_MAX_MUTEXES)
    {
        UTF_put_text("MutSemDelete Parameter failure\n");
        return (OS_ERROR);
    }
    return (OS_SUCCESS);
 }

/*---------------------------------------------------------------------------------------
    Name: OS_MutSemGive

    Purpose: The function releases the mutex object referenced by sem_id.The
             manner in which a mutex is released is dependent upon the mutex's type
             attribute.  If there are threads blocked on the mutex object referenced by
             mutex when this function is called, resulting in the mutex becoming
             available, the scheduling policy shall determine which thread shall
             acquire the mutex.

    Returns: OS_SUCCESS if success
             OS_SEM_FAILURE if the semaphore was not previously  initialized
             OS_ERR_INVALID_ID if the id passed in is not a valid mutex
---------------------------------------------------------------------------------------*/
int32 OS_MutSemGive (uint32 sem_id)
{
    if (sem_id > OS_MAX_MUTEXES)
    {
        UTF_put_text("MutSemGive Parameter failure\n");
        return (OS_ERROR);
    }
    return (OS_SUCCESS);
}

/*---------------------------------------------------------------------------------------
    Name: OS_MutSemTake

    Purpose: The mutex object referenced by sem_id shall be locked by calling this
             function. If the mutex is already locked, the calling thread shall
             block until the mutex becomes available. This operation shall return
             with the mutex object referenced by mutex in the locked state with the
             calling thread as its owner.

    Returns: OS_SUCCESS if success
             OS_SEM_FAILURE if the semaphore was not previously initialized or is
             not in the array of semaphores defined by the system
             OS_ERR_INVALID_ID the id passed in is not a valid mutex
---------------------------------------------------------------------------------------*/
int32 OS_MutSemTake (uint32 sem_id)
{
    if (UTF_OSAPI_HookTable.OS_MutSemTake)
        return(UTF_OSAPI_HookTable.OS_MutSemTake(sem_id));

    if (sem_id > OS_MAX_MUTEXES)
    {
        UTF_put_text("MutSemTake Parameter failure\n");
        return (OS_ERROR);
    }
    return (OS_SUCCESS);
}

/*--------------------------------------------------------------------------------------
    Name: OS_MutSemGetIdByName

    Purpose: This function tries to find a mutex sem Id given the name of a bin_sem
             The id is returned through sem_id

    Returns: OS_INVALID_POINTER is semid or sem_name are NULL pointers
             OS_ERR_NAME_TOO_LONG if the name given is to long to have been stored
             OS_ERR_NAME_NOT_FOUND if the name was not found in the table
             OS_SUCCESS if success
---------------------------------------------------------------------------------------*/
int32 OS_MutSemGetIdByName (uint32 *sem_id, const char *sem_name)
{
    (void) sem_name;           /* refer to function argument to avoid compiler warning */

    sem_id = NULL;  /* currently not implemented */
    return(OS_INVALID_POINTER);
}

/*---------------------------------------------------------------------------------------
    Name: OS_MutSemGetInfo

    Purpose: This function will pass back a pointer to structure that contains
             all of the relevant info( name and creator) about the specified mutex
             semaphore.

    Returns: OS_ERR_INVALID_ID if the id passed in is not a valid semaphore
             OS_INVALID_POINTER if the mut_prop pointer is null
             OS_SUCCESS if success
---------------------------------------------------------------------------------------*/
int32 OS_MutSemGetInfo (uint32 sem_id, OS_mut_sem_prop_t *mut_prop)
{
    (void) sem_id;          /* refer to function argument to avoid compiler warning */

    mut_prop = NULL;  /* currently not implemented */
    return(OS_INVALID_POINTER);
}

/****************************************************************************************
                                  Time/Tick Related API
****************************************************************************************/

/*---------------------------------------------------------------------------------------
** Name: OS_Milli2Ticks
**
** Purpose:
** This function accepts a time interval in milli_seconds, as an input and
** returns the tick equivalent  for this time period. The tick value is
**  rounded up.
**
** Parameters:
**      milli_seconds : the time interval ,in milli second , to be translated
**                      to ticks
**
** Return Values: the number of ticks rounded up.
---------------------------------------------------------------------------------------*/
int32 OS_Milli2Ticks(uint32 milli_seconds)
{
    uint32 num_of_ticks,tick_duration_usec ;

    tick_duration_usec = OS_Tick2Micros() ;

    num_of_ticks =
        ( (milli_seconds * 1000) + tick_duration_usec -1 ) / tick_duration_usec ;

    return(num_of_ticks) ;
}

/*---------------------------------------------------------------------------------------
** Name: OS_Tick2Micros
**
** Purpose:
** This function returns the duration of a system tick in micro seconds.
**
** Return Values: duration of a system tick in micro seconds
---------------------------------------------------------------------------------------*/
int32 OS_Tick2Micros (void)
{
    return((1/(CLOCKS_PER_SEC))*1000); 
}

/*---------------------------------------------------------------------------------------
 * Name: OS_GetLocalTime
 *
 * Purpose: This functions get the local time of the machine its on
 * ------------------------------------------------------------------------------------*/
int32 OS_GetLocalTime(OS_time_t *time_struct)
{
    if (UTF_OSAPI_HookTable.OS_GetLocalTime)
        return(UTF_OSAPI_HookTable.OS_GetLocalTime(time_struct));

    time_struct->seconds = utf_simulated_hwtime.seconds;
    time_struct->microsecs = utf_simulated_hwtime.microsecs;

    return (OS_SUCCESS);
}

/****************************************************************************************
                                  Exception API
****************************************************************************************/

int32 OS_ExcAttachHandler(uint32 ExceptionNumber, void (*ExceptionHandler)(uint32, uint32 *,uint32), int32 Parameter)
{
    return(CFE_OSAPI_NOT_IMPLEMENTED);
}

int32 OS_ExcEnable(int32 ExceptionNumber)
{
    return(CFE_OSAPI_NOT_IMPLEMENTED);
}

int32 OS_ExcDisable(int32 ExceptionNumber)
{
    return(CFE_OSAPI_NOT_IMPLEMENTED);
}

/****************************************************************************************
                                  Floating Point Unit API
****************************************************************************************/

int32 OS_FPUExcAttachHandler(uint32 ExceptionNumber, void * ExceptionHandler, int32 Parameter)
{
    return(CFE_OSAPI_NOT_IMPLEMENTED);
}

int32 OS_FPUExcEnable(int32 ExceptionNumber)
{
    return(CFE_OSAPI_NOT_IMPLEMENTED);
}

int32 OS_FPUExcDisable(int32 ExceptionNumber)
{
    return(CFE_OSAPI_NOT_IMPLEMENTED);
}

int32 OS_FPUExcSetMask(uint32 Mask)
{
    return(CFE_OSAPI_NOT_IMPLEMENTED);
}

int32 OS_FPUExcGetMask(uint32 *Mask)
{
    return(CFE_OSAPI_NOT_IMPLEMENTED);
}

/****************************************************************************************
                                  Interrupt API
****************************************************************************************/

int32 OS_IntAttachHandler(uint32 InterruptNumber, osal_task_entry InterruptHandler, int32 Parameter)
{
    if (InterruptHandler == NULL) {
        return(OS_INVALID_POINTER);
    }
    else {
        return(OS_SUCCESS);
    }
}

int32 OS_IntEnable(int32 Level)
{
    return(OS_SUCCESS);
}

int32 OS_IntDisable(int32 Level)
{
    return(OS_SUCCESS);
}

int32 OS_IntUnlock (int32 IntLevel)
{
    return(OS_SUCCESS);
}

int32 OS_IntLock (void)
{
    return(OS_SUCCESS);
}

int32 OS_IntSetMask(uint32 Mask)
{
    return(CFE_OSAPI_NOT_IMPLEMENTED);
}

int32 OS_IntGetMask(uint32 *Mask)
{
    return(CFE_OSAPI_NOT_IMPLEMENTED);
}

int32 OS_IntAck(int32 InterruptNumber)
{
    return(CFE_OSAPI_NOT_IMPLEMENTED);
}

/****************************************************************************************
                                  Debug API
****************************************************************************************/

/*---------------------------------------------------------------------------------------
 *  Name: OS_GetErrorName()
 *  purpose: A handy function to copy the name of the error to a buffer.
---------------------------------------------------------------------------------------*/
int32 OS_GetErrorName(int32 error_num, os_err_name_t * err_name)
{
    os_err_name_t local_name;
    uint32 return_code;

    return_code = OS_SUCCESS;

    switch (error_num)
    {
        case OS_SUCCESS:
            strcpy(local_name,"OS_SUCCESS"); break;
        case OS_ERROR:
            strcpy(local_name,"OS_ERROR"); break;
        case OS_INVALID_POINTER:
            strcpy(local_name,"OS_INVALID_POINTER"); break;
        case OS_ERROR_ADDRESS_MISALIGNED:
            strcpy(local_name,"OS_ADDRESS_MISALIGNED"); break;
        case OS_ERROR_TIMEOUT:
            strcpy(local_name,"OS_ERROR_TIMEOUT"); break;
        case OS_INVALID_INT_NUM:
            strcpy(local_name,"OS_INVALID_INT_NUM"); break;
        case OS_SEM_FAILURE:
            strcpy(local_name,"OS_SEM_FAILURE"); break;
        case OS_SEM_TIMEOUT:
            strcpy(local_name,"OS_SEM_TIMEOUT"); break;
        case OS_QUEUE_EMPTY:
            strcpy(local_name,"OS_QUEUE_EMPTY"); break;
        case OS_QUEUE_FULL:
            strcpy(local_name,"OS_QUEUE_FULL"); break;
        case OS_QUEUE_TIMEOUT:
            strcpy(local_name,"OS_QUEUE_TIMEOUT"); break;
        case OS_QUEUE_INVALID_SIZE:
            strcpy(local_name,"OS_QUEUE_INVALID_SIZE"); break;
        case OS_QUEUE_ID_ERROR:
            strcpy(local_name,"OS_QUEUE_ID_ERROR"); break;
        case OS_ERR_NAME_TOO_LONG:
            strcpy(local_name,"OS_ERR_NAME_TOO_LONG"); break;
        case OS_ERR_NO_FREE_IDS:
            strcpy(local_name,"OS_ERR_NO_FREE_IDS"); break;
        case OS_ERR_NAME_TAKEN:
            strcpy(local_name,"OS_ERR_NAME_TAKEN"); break;
        case OS_ERR_INVALID_ID:
            strcpy(local_name,"OS_ERR_INVALID_ID"); break;
        case OS_ERR_NAME_NOT_FOUND:
            strcpy(local_name,"OS_ERR_NAME_NOT_FOUND"); break;
        case OS_ERR_SEM_NOT_FULL:
            strcpy(local_name,"OS_ERR_SEM_NOT_FULL"); break;
        case OS_ERR_INVALID_PRIORITY:
            strcpy(local_name,"OS_ERR_INVALID_PRIORITY"); break;

        default: strcpy(local_name,"ERROR_UNKNOWN");
                 return_code = OS_ERROR;
    }
    strcpy((char*) err_name, local_name);
    return return_code;
}

/* ---------------------------------------------------------------------------
 * Name: OS_printf
 *
 * Purpose: This function abstracts out the printf type statements. This is
 *          useful for using OS- specific thats that will allow non-polled
 *          print statements for the real time systems.
 *
 * Note:    This function uses a
 *          circular buffer to keep the pointer to the string for a while
 *          this is (probably) necessary for VxWorks because its logMsg
 *          function only passes pointers, not copies of the strings. The
 *          circular buffer will keep the point around until logMsg sends the
 *          data
 *
 ---------------------------------------------------------------------------*/
void OS_printf( const char *String, ...)
{
    va_list     ptr;
    char msg_buffer [OS_BUFFER_SIZE];

    va_start(ptr,String);
    vsprintf(&msg_buffer[0], String, ptr);
    va_end(ptr);

    msg_buffer[OS_BUFFER_SIZE-1] = '\0';
    UTF_put_text("%s", &msg_buffer[0]);
}
