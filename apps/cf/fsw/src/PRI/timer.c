/* FILE:  timer.c  --  a countdown timer.
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
 * SPECS:  see timer.h
 * ORIGINAL PROGRAMMER:  Tim Ray x0581
 * CHANGES:
 *   2007_07_31 Tim Ray
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 */

#include "cfdp.h"
#include "cfdp_private.h"

#define  NO  0
#define  YES  1



/*=r=************************************************************************/
boolean timer__is_timer_paused (TIMER *timer)
   {
   /*------------------------------------------------------------*/
     if (timer->mode == TIMER_PAUSED)
       return (YES);
     else
       return (NO);
   }



/*=r=************************************************************************/
boolean timer__is_timer_running (TIMER *timer)
   {
   /*------------------------------------------------------------*/
     if (timer->mode == TIMER_RUNNING)
       return (YES);
     else
       return (NO);
   }



/*=r=*************************************************************************/
TIMER timer__start (int n)
   {
     TIMER        timer;
/*------------------------------------------------------------*/
   timer.desired_countdown = n;
   timer.start_time = time (NULL);
   timer.mode = TIMER_RUNNING;
   return (timer);
   }



/*=r=************************************************************************/
void timer__restart (TIMER *timer)
   {
   /*------------------------------------------------------------*/
     *timer = timer__start (timer->desired_countdown);
   }



/*=r=*************************************************************************/
void timer__cancel (TIMER *timer)
   {
/*------------------------------------------------------------*/
     timer->mode = TIMER_OFF;
   }



/*=r=*************************************************************************/
boolean timer__expired (TIMER *timer)
   {
   time_t            current_time;
   boolean           has_timer_expired;
   int               how_long_has_timer_been_running;
/*------------------------------------------------------------*/
   has_timer_expired = NO;
   if (timer->mode == TIMER_RUNNING)
      {
      current_time = time (NULL);
      
      /* This is weird, but sometimes the system time has gone backward
       * and the current-time has become *earlier* than the start-time!
       * If that happens, assume the timer has not expired.
       */
      if (current_time < timer->start_time)
        /* This is the weird case -- time went backwards! */
        has_timer_expired = NO;
      else
        /* This is the normal calculation */
        {
          how_long_has_timer_been_running = current_time - timer->start_time;
          if (how_long_has_timer_been_running >= timer->desired_countdown)
            has_timer_expired = YES;
        }
      }
   return (has_timer_expired);
   }



/*=r=*************************************************************************/
int timer__read (TIMER *timer)
   {
   time_t            current_time;
   int               how_long_has_timer_been_running;
   int               time_left;
/*------------------------------------------------------------*/
   if (timer->mode == TIMER_OFF)
     return (0);
   else if (timer->mode == TIMER_PAUSED)
     how_long_has_timer_been_running = timer->pause_time - timer->start_time;
       
   else 
      {
        current_time = time (NULL);
        how_long_has_timer_been_running = current_time - timer->start_time;
      }

   if (how_long_has_timer_been_running > timer->desired_countdown)
     time_left = 0;
   else
     time_left = timer->desired_countdown - how_long_has_timer_been_running;

   return (time_left);
   }



/*=r=************************************************************************/
void timer__pause (TIMER *timer)
   {
   /*------------------------------------------------------------*/
     if (timer->mode == TIMER_RUNNING)
       {
         timer->mode = TIMER_PAUSED;
         timer->pause_time = time (NULL);
       }
   }



/*=r=************************************************************************/
void timer__resume (TIMER *timer)
   {
     time_t            current_time;
     int               how_long_did_timer_run;
   /*------------------------------------------------------------*/
     if (timer->mode == TIMER_PAUSED)
       {
         timer->mode = TIMER_RUNNING;
         current_time = time (NULL);
         /* Determine how long the timer ran before being paused */
         how_long_did_timer_run = timer->pause_time - timer->start_time;
         /* Reset the effective start-time to match that interval */
         timer->start_time = current_time - how_long_did_timer_run;
       }
   }
