/*
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement.
**
** File   : ostimer.c
** Author : Alan Cudmore
** Purpose: This file contains the OSAL Timer API for vxWorks6
*/

/****************************************************************************************
                                    INCLUDE FILES
****************************************************************************************/

#include "common_types.h"
#include "osapi.h"
#include "utf_types.h"
#include "osapi-os-timer.h"
#include "utf_ostimer.h"
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <sys/signal.h>
#include <sys/errno.h>
#include <pthread.h>

/****************************************************************************************
                                EXTERNAL FUNCTION PROTOTYPES
****************************************************************************************/

uint32 OS_FindCreator(void);

/****************************************************************************************
                                INTERNAL FUNCTION PROTOTYPES
****************************************************************************************/

/****************************************************************************************
                                     DEFINES
****************************************************************************************/

/*
** Since the API is storing the timer values in a 32 bit integer as Microseconds, 
** there is a limit to the number of seconds that can be represented.
*/
#define MAX_SECS_IN_USEC 4293

#define UNINITIALIZED 0

/****************************************************************************************
                                    LOCAL TYPEDEFS 
****************************************************************************************/

typedef struct 
{
   uint32              free;
   char                name[OS_MAX_API_NAME];
   uint32              creator;
   uint32              start_time;
   uint32              interval_time;
   uint32              accuracy;
   OS_TimerCallback_t  callback_ptr;
   uint32              host_timerid;

} OS_timer_record_t;

/****************************************************************************************
                                   GLOBAL DATA
****************************************************************************************/

OS_timer_record_t OS_timer_table[OS_MAX_TIMERS];
uint32           os_clock_accuracy;

/*
** The Semaphore for protecting the above table
SEM_ID OS_timer_table_sem;
*/

int32 os_timer_return_value[NUM_OF_OSTIMER_PROCS]=
 {
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_OSTIMER_Set_Api_Return_Code() --                                   */
/*  Set a return code for an api to a specified value.                     */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_OSTIMER_Set_Api_Return_Code(int32 ApiIndex, int32 ReturnCode)
{
   /* Check that ApiIndex is in the valid range */
   if ( (ApiIndex >= NUM_OF_OSTIMER_PROCS ) || (ApiIndex < 0 ) )  
   {
      UTF_put_text("UTF Error: Invalid ApiIndex %d passed to UTF_OSTIMER_Set_Api_Return_Code\n",
      ApiIndex);
      return -1;
   }
   /* If index is valid, assign return value and report action */
   os_timer_return_value[ApiIndex] = ReturnCode;

   if (ReturnCode == UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
   	  UTF_put_text("UTF Info: Return code for OSTIMER Api #%d returned to default\n",
   	               ApiIndex);
   }
   else
   {
   	  UTF_put_text("UTF Info: Return value for OSTIMER Api #%d set to %lx\n", 
   	                ApiIndex, ReturnCode);
   }
   return OS_FS_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_OSTIMER_Use_Default_Api_Return_Code() --                          */
/* Use to return to default return code for an api.                        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_OSTIMER_Use_Default_Api_Return_Code(int32 ApiIndex)
{
	int32 Success;
	
   /* Check that ApiIndex is in the valid range */
   if ( (ApiIndex >= NUM_OF_OSTIMER_PROCS ) || (ApiIndex < 0 ) )  
   {
      UTF_put_text("UTF Error: Invalid ApiIndex %d passed to UTF_OSTIMER_Use_Default_Api_Return_Code\n",
      ApiIndex);
      return -1;
   }

   /* If index is valid, assign return value and report action */
   Success = UTF_OSTIMER_Set_Api_Return_Code(ApiIndex, UTF_CFE_USE_DEFAULT_RETURN_CODE);
   
   return Success;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_OSTIMER_Set_Api_Return_Code() --                             */
/*  Script interface function for UTF_CFE_ES_Set_Api_Return_Code.          */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void UTF_SCRIPT_OSTIMER_Set_Api_Return_Code(int argc,char *argv[])
{
    int32 Index;
    int32 Code;
	/* Check for correct number of arguments */
	if (argc != 3)
	{
	   UTF_error("UTF Error: Read %d args w/script cmd UTF_SCRIPT_OSTIMER_Set_Api_Return_Code. Expected 2.\n",
	   argc -1 );
	   UTF_exit();
	}
    /* Extract argument values */
    Index = UTF_arg2dbl(argv[1]);
    Code  = UTF_arg2uint(argv[2]);
    /* Call function to set API return code */
	UTF_OSTIMER_Set_Api_Return_Code(Index, Code);
	return;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_OSTIMER_Use_Default_Api_Return_Code() --                     */
/*  Script interface function for UTF_CFE_ES_Set_Api_Return_Code.          */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void UTF_SCRIPT_OSTIMER_Use_Default_Api_Return_Code(int argc,char *argv[])
{
    int32 Index;

	/* Check for correct number of arguments */
	if (argc != 2)
	{
	   UTF_error("UTF Error: Read %d args w/script cmd UTF_SCRIPT_OSTIMER_Set_Api_Return_Code. Expected 1.\n",
	   argc -1 );
	   UTF_exit();
	}
 
    /* Extract argument values */
    Index = UTF_arg2dbl(argv[1]);
 
    /* Call function to set API return code */
	UTF_OSTIMER_Set_Api_Return_Code(Index, UTF_CFE_USE_DEFAULT_RETURN_CODE);
	return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_OSTIMER_Set_ClockAccuracy() --                                      */
/*  Function to set the global clock accuracy variable.                    */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void UTF_OSTIMER_Set_ClockAccuracy(uint32 clockAccuracy)
{
	os_clock_accuracy = clockAccuracy;
}

/****************************************************************************************
                                INITIALIZATION FUNCTION
****************************************************************************************/
int32 OS_TimerAPIInit ( void )
{
/*   int    status; */
   int    i;
   
   /*
   ** Mark all timers as available
   */
   for ( i = 0; i < OS_MAX_TIMERS; i++ )
   {
      OS_timer_table[i].free      = TRUE;
      OS_timer_table[i].creator   = UNINITIALIZED;
      strcpy(OS_timer_table[i].name,"");
   }

   return OS_SUCCESS;
}

/****************************************************************************************
                                INTERNAL FUNCTIONS
****************************************************************************************/
/*
** Timer Signal Handler.
** The purpose of this function is to convert the host os timer number to the 
** OSAL timer number and pass it to the User defined signal handler.
** The Host (vxworks ) timer_id gets passed to the timer callback, so if we
** want the OSAL timer ID to be passed to the user function, this wrapper
** function must be used. 
*/
void OS_TimerSignalHandler(int host_timer_id)
{
   int     i;

   for (i = 0; i < OS_MAX_TIMERS; i++ )   
   {
      if  (( OS_timer_table[i].free == FALSE ) && ( host_timer_id == OS_timer_table[i].host_timerid))
      {
         (OS_timer_table[i].callback_ptr)(i);
         break;
      }
   } 
}

 
/******************************************************************************
 **  Function:  OS_UsecToTimespec
 **
 **  Purpose:  Convert Microseconds to a POSIX timespec structure.
 **
void OS_UsecToTimespec(uint32 usecs, struct timespec *time_spec)
{
   if ( usecs < 1000000 )
   {
      time_spec->tv_nsec = (usecs * 1000);
      time_spec->tv_sec = 0;
   }
   else
   {
      time_spec->tv_sec = usecs / 1000000;
      time_spec->tv_nsec = (usecs % 1000000) * 1000;
   }
	
}
*******************************************************************************/

/******************************************************************************
 **  Function:  OS_TimespecToUsec
 **
 **  Purpose:  Convert a POSIX timespec structure to microseconds
 **
void OS_TimespecToUsec(struct timespec time_spec, uint32 *usecs)
{
   if ( (*usecs) < 1000000 )
   {
      *usecs = time_spec.tv_nsec / 1000;
   }
   else
   {
*/
      /*
      ** Need to account for maximum number of seconds that will fit in the 
      ** 32 bit usec integer
      */
/*
      if ( time_spec.tv_sec > MAX_SECS_IN_USEC )
      {
         time_spec.tv_sec = MAX_SECS_IN_USEC;
      }
      *usecs = (time_spec.tv_sec * 1000000 ) + (time_spec.tv_nsec / 1000 );
   }
}
*******************************************************************************/

/*******************************************************************************
                                   Timer API
*******************************************************************************/

/******************************************************************************
**  Function:  OS_TimerCreate
**
**  Purpose:  Create a new OSAL Timer
**
**  Arguments:
**
**  Return:
*/
int32 OS_TimerCreate(uint32 *timer_id, const char *timer_name, uint32 *clock_accuracy, OS_TimerCallback_t  callback_ptr)
{
   uint32   possible_tid;
   int32    i;

   	/* Handle Preset Return Code */
   	if (os_timer_return_value[OS_TIMER_CREATE_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   	{
		return os_timer_return_value[OS_TIMER_CREATE_PROC];
   	}

   if ( timer_id == NULL || timer_name == NULL)
   {
        return OS_INVALID_POINTER;
   }

   /* 
   ** we don't want to allow names too long
   ** if truncated, two names might be the same 
   */
   if (strlen(timer_name) > OS_MAX_API_NAME)
   {
      return OS_ERR_NAME_TOO_LONG;
   }
       
   for(possible_tid = 0; possible_tid < OS_MAX_TIMERS; possible_tid++)
   {
      if (OS_timer_table[possible_tid].free == TRUE)
         break;
   }

   if( possible_tid >= OS_MAX_TIMERS || OS_timer_table[possible_tid].free != TRUE)
   {
        return OS_ERR_NO_FREE_IDS;
   }

   /* 
   ** Check to see if the name is already taken 
   */
   for (i = 0; i < OS_MAX_TIMERS; i++)
   {
       if ((OS_timer_table[i].free == FALSE) &&
            strcmp ((char*) timer_name, OS_timer_table[i].name) == 0)
       {
            return OS_ERR_NAME_TAKEN;
       }
   }

   /*
   ** Verify callback parameter
   */
   if (callback_ptr == NULL ) 
   {
      return OS_TIMER_ERR_INVALID_ARGS;
   }    

   /* 
   ** Set the possible timer Id to not free so that
   ** no other task can try to use it 
   */
   OS_timer_table[possible_tid].free = FALSE;
   OS_timer_table[possible_tid].creator = 5;
   strcpy(OS_timer_table[possible_tid].name, timer_name);
   OS_timer_table[possible_tid].start_time = 0;
   OS_timer_table[possible_tid].interval_time = 0;
    
   OS_timer_table[possible_tid].callback_ptr = callback_ptr;

   /*
   ** Return the clock accuracy to the user
   */
   *clock_accuracy = os_clock_accuracy;
   
   /*
   ** Return timer ID 
   */
   *timer_id = possible_tid;

   return OS_SUCCESS;
}

/******************************************************************************
**  Function:  OS_TimerSet
**
**  Purpose:  
**
**  Arguments:
**    (none)
**
**  Return:
**    (none)
*/
int32 OS_TimerSet(uint32 timer_id, uint32 start_time, uint32 interval_time)
{  
   	/* Handle Preset Return Code */
   	if (os_timer_return_value[OS_TIMER_SET_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   	{
		return os_timer_return_value[OS_TIMER_SET_PROC];
   	}
   	
   /* 
   ** Check to see if the timer_id given is valid 
   */
   if (timer_id >= OS_MAX_TIMERS || OS_timer_table[timer_id].free == TRUE)
   {
      return OS_ERR_INVALID_ID;
   }

   /*
   ** Save the start and interval times 
   */
   OS_timer_table[timer_id].start_time = start_time;
   OS_timer_table[timer_id].interval_time = interval_time;

   return OS_SUCCESS;
}

/******************************************************************************
**  Function:  OS_TimerDelete
**
**  Purpose: 
**
**  Arguments:
**    (none)
**
**  Return:
**    (none)
*/
int32 OS_TimerDelete(uint32 timer_id)
{
   	/* Handle Preset Return Code */
   	if (os_timer_return_value[OS_TIMER_DELETE_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   	{
		return os_timer_return_value[OS_TIMER_DELETE_PROC];
   	}
   	
   /* 
   ** Check to see if the timer_id given is valid 
   */
   if (timer_id >= OS_MAX_TIMERS || OS_timer_table[timer_id].free == TRUE)
   {
      return OS_ERR_INVALID_ID;
   }

   /*
   ** Delete the timer 
   */
   OS_timer_table[timer_id].free = TRUE;

   return OS_SUCCESS;
}

/***********************************************************************************
**
**    Name: OS_TimerGetIdByName
**
**    Purpose: This function tries to find a Timer Id given the name 
**             The id is returned through timer_id
**
**    Returns: OS_INVALID_POINTER if timer_id or timer_name are NULL pointers
**             OS_ERR_NAME_TOO_LONG if the name given is to long to have been stored
**             OS_ERR_NAME_NOT_FOUND if the name was not found in the table
**             OS_SUCCESS if success
**             
*/
int32 OS_TimerGetIdByName (uint32 *timer_id, const char *timer_name)
{
    uint32 i;

   	/* Handle Preset Return Code */
   	if (os_timer_return_value[OS_TIMER_GETIDBYNAME_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   	{
		return os_timer_return_value[OS_TIMER_GETIDBYNAME_PROC];
   	}
   	
    if (timer_id == NULL || timer_name == NULL)
    {
        return OS_INVALID_POINTER;
    }

    /* 
    ** a name too long wouldn't have been allowed in the first place
    ** so we definitely won't find a name too long
    */
    if (strlen(timer_name) > OS_MAX_API_NAME)
    {
        return OS_ERR_NAME_TOO_LONG;
    }

    for (i = 0; i < OS_MAX_TIMERS; i++)
    {
        if (OS_timer_table[i].free != TRUE &&
                (strcmp (OS_timer_table[i].name , (char*) timer_name) == 0))
        {
            *timer_id = i;
            return OS_SUCCESS;
        }
    }
   
    /* 
    ** The name was not found in the table,
    **  or it was, and the sem_id isn't valid anymore 
    */
    return OS_ERR_NAME_NOT_FOUND;
    
}/* end OS_TimerGetIdByName */

/***************************************************************************************
**    Name: OS_TimerGetInfo
**
**    Purpose: This function will pass back a pointer to structure that contains 
**             all of the relevant info( name and creator) about the specified timer.
**             
**    Returns: OS_ERR_INVALID_ID if the id passed in is not a valid timer 
**             OS_INVALID_POINTER if the timer_prop pointer is null
**             OS_SUCCESS if success
*/
int32 OS_TimerGetInfo (uint32 timer_id, OS_timer_prop_t *timer_prop)  
{
   	/* Handle Preset Return Code */
   	if (os_timer_return_value[OS_TIMER_GETINFO_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   	{
		return os_timer_return_value[OS_TIMER_GETINFO_PROC];
   	}

    /* 
    ** Check to see that the id given is valid 
    */
    if (timer_id >= OS_MAX_TIMERS || OS_timer_table[timer_id].free == TRUE)
    {
       return OS_ERR_INVALID_ID;
    }

    if (timer_prop == NULL)
    {
        return OS_INVALID_POINTER;
    }

    /* 
    ** put the info into the stucture 
    */

    timer_prop ->creator       = OS_timer_table[timer_id].creator;
    strcpy(timer_prop-> name, OS_timer_table[timer_id].name);
    timer_prop ->start_time    = OS_timer_table[timer_id].start_time;
    timer_prop ->interval_time = OS_timer_table[timer_id].interval_time;
    timer_prop ->accuracy      = OS_timer_table[timer_id].accuracy;

    return OS_SUCCESS;
    
} /* end OS_TimerGetInfo */

