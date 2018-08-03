/* FILE:  timer.h -- a countdown timer.
 *
 *  Copyright © 2007-2014 United States Government as represented by the 
 *  Administrator of the National Aeronautics and Space Administration. 
 *  All Other Rights Reserved.  
 *
 *  This software was created at NASA's Goddard Space Flight Center.
 *  This software is governed by the NASA Open Source Agreement and may be 
 *  used, distributed and modified only pursuant to the terms of that 
 *  agreement.
 *  
 * SUMMARY:  Allows multiple countdown timers (caller passes a 'timer'
 *   data structure in/out of each routine).
 * ORIGINAL PROGRAMMER:  Tim Ray 301-286-0581
 * CHANGES:
 *   2007_07_31 Tim Ray
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 */

/* List of routines:
 *   Start the countdown timer --------------> timer__start
 *   Cancel the timer -----------------------> timer__cancel
 *   Has the timer expired? -----------------> timer__expired
 *   Current reading of timer ---------------> timer__read
 *   Pause the timer ------------------------> timer__pause
 *   Resume the timer -----------------------> timer__resume
 *   Jam this value into the timer ----------> timer__jam   not yet implemented
 */


#ifndef H_TIMER
#define H_TIMER 1

#include <time.h>

typedef enum { TIMER_OFF, TIMER_RUNNING, TIMER_PAUSED } TIMER_MODE;

typedef struct
{
  u_int_2                  desired_countdown;
  TIMER_MODE               mode;
  time_t                   start_time;
  time_t                   pause_time;
} TIMER;


/*--------------------------*/
/* Details for each routine */
/*--------------------------*/

void timer__cancel (TIMER *timer);

boolean timer__expired (TIMER *timer);

boolean timer__is_timer_paused (TIMER *timer);
/* WHAT IT DOES:  Returns YES if the given timer's mode is "TIMER_PAUSED" */

boolean timer__is_timer_running (TIMER *timer);
/* WHAT IT DOES:  Returns YES if the given timer's mode is "TIMER_RUNNING" */

void timer__jam (TIMER *timer, int n);    /* NOT YET IMPLEMENTED */
/* WHAT IT DOES:  Sets the timer's value (i.e. how many more seconds to
 *   countdown) to the given value.
 * NOTE:  Does not change the timer's state -- i.e. if the timer is paused,
 *   it remains paused.
 */

void timer__pause (TIMER *timer);

int timer__read (TIMER *timer);

void timer__restart (TIMER *timer);
/* WHAT IT DOES:  Re-starts the timer at it's original countdown value */

void timer__resume (TIMER *timer);

TIMER timer__start (int n);
/* WHAT IT DOES:  Starts the countdown timer at n seconds. */

#endif
