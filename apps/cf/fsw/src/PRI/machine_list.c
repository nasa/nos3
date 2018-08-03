/* FILE: machine_list.c
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
 * SPECS: machine_list.h
 * ORIGINAL PROGRAMMER:  Tim Ray 301-286-0581
 * NOTES:
 *   1) No sorting is done when a machine is created, and a linear search
 *      is used when retreiving.  This can be optimized later if needed.
 * CHANGES:
 *   2007_07_31 Tim Ray
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 */

/*#include <malloc.h>*/
#include "cfdp.h"
#include "cfdp_private.h"

#define INITIALIZE_ONCE { if (!m_initialized) m__initialize(); }

#define NO 0
#define YES 1

/* These variables are only known with this source file, but are shared
 * by all the routines within this source file.
 */
static u_int_4            m_current_walk_index = 0;
static boolean            m_initialized = NO;
static boolean            m_is_this_slot_in_use [MAX_CONCURRENT_TRANSACTIONS];
static MACHINE            m_machine [MAX_CONCURRENT_TRANSACTIONS];





/*=r=*************************************************************************/
static void m__initialize (void)
     /* WHAT IT DOES:  Initializes the module-level variables. */
{
  u_int_4             i;
  /*------------------------------------------------------------*/

  /* Initially, all machine-slots are free */
  for (i=0; i<MAX_CONCURRENT_TRANSACTIONS; i++)
    m_is_this_slot_in_use[i] = NO;

  m_initialized = YES;
}



/*=r=************************************************************************/
MACHINE *machine_list__allocate_slot (void)
{
  MACHINE            *answer;
  u_int_4             i;
  /*------------------------------------------------------------*/
  
  INITIALIZE_ONCE;

  /* Linear search until we find an unused slot in the machine-array 
   * (or until all slots are checked). 
   */
  i = 0;
  while (m_is_this_slot_in_use[i] && (i < MAX_CONCURRENT_TRANSACTIONS))
    i ++;

  if (i < MAX_CONCURRENT_TRANSACTIONS)
    /* Success.  We found a free slot. */
    {
      m_is_this_slot_in_use[i] = YES;
      answer = &(m_machine[i]);
    }
  else
    /* Failure.  No free slots */
    answer = NULL;

  return (answer);
}



/*=r=************************************************************************/
boolean machine_list__deallocate_slot (MACHINE *machine_to_delete)
{
  u_int_4             i;
  MACHINE            *machine_ptr;
  boolean             were_we_successful;
  /*------------------------------------------------------------*/

  INITIALIZE_ONCE;

  /* Check one slot at a time until we find one whose address matches the
   * address of the given machine (or until all slots are checked).
   */
  were_we_successful = NO;
  i = 0;
  while (!were_we_successful && (i < MAX_CONCURRENT_TRANSACTIONS))
    {
      machine_ptr = &(m_machine[i]);
      if (machine_ptr == machine_to_delete)
        /* Found it */
        were_we_successful = YES;
      else
        i ++;
    }

  if (were_we_successful)
    m_is_this_slot_in_use[i] = NO;

  return (were_we_successful);
}



/*=r=*************************************************************************/
void machine_list__open_a_walk (void)
{
  /*------------------------------------------------------------*/

  INITIALIZE_ONCE;

  m_current_walk_index = 0;
}



/*=r=************************************************************************/
MACHINE *machine_list__get_next_machine (void)
{
  static MACHINE       *answer;
  /*------------------------------------------------------------*/

  INITIALIZE_ONCE;

  /* Pick up where we left off, and check one slot at a time until
   * we find one that is being used (or until all slots are checked).
   */
  while (!m_is_this_slot_in_use[m_current_walk_index] &&
         (m_current_walk_index < MAX_CONCURRENT_TRANSACTIONS))
    m_current_walk_index ++;

  if (m_current_walk_index < MAX_CONCURRENT_TRANSACTIONS)
    /* We found the next in-use slot */
    {
      answer = &(m_machine[m_current_walk_index]);
      /* Next time, pick up from the slot after this one */
      m_current_walk_index ++;
    }
  else
    /* We've reached the end of this walk */
    answer = NULL;

  return (answer);
}



/*=r=************************************************************************/
MACHINE *machine_list__get_this_trans (TRANSACTION t)
{
  boolean                   found;
  u_int_4                   i;
  static MACHINE           *answer;
  /*------------------------------------------------------------*/

  INITIALIZE_ONCE;

  /* Check one slot at a time until we find a machine assigned to the
   * given transaction (or until all slots are checked).
   */
  i = 0;
  found = NO;
  while (!found && (i < MAX_CONCURRENT_TRANSACTIONS))
    {
      if (m_is_this_slot_in_use[i] &&
          cfdp_are_these_trans_equal (t, m_machine[i].hdr.trans))
        /* Success.  Machine in slot 'i' is assigned to the transaction */
        found = YES;
      else
        i ++;
    }

  if (found)
    answer = &(m_machine[i]);
  else
    answer = NULL;

  return (answer);
}



/*=r=************************************************************************/
u_int_4 machine_list__how_many_machines (void)
{
  u_int_4          answer;
  u_int_4          i;
  /*------------------------------------------------------------*/
  
  INITIALIZE_ONCE;

  /* Simply count how many slots are in use */
  answer = 0;
  for (i=0; i<MAX_CONCURRENT_TRANSACTIONS; i++)
    if (m_is_this_slot_in_use[i])
      answer ++;

  return (answer);
}



/*=r=************************************************************************/
void machine_list__display_list (void)
     /* This routine uses multiple calls to the 'd_msg__' routine to
      * output one message (not good).  However, it is only called
      * when memory usage is being debugged.
      */
{
  u_int_4          i;
  /*------------------------------------------------------------*/
  INITIALIZE_ONCE;
  
  d_msg__ ("   List:  ");

  if (machine_list__how_many_machines() == 0)
    d_msg__ ("(empty)\n");

  else
    {
      /* Display the transaction assigned to each machine on our list */
      for (i=0; i<MAX_CONCURRENT_TRANSACTIONS; i++)
        if (m_is_this_slot_in_use[i])
          d_msg__ ("%s (%lu)  ",
                   cfdp_trans_as_string (m_machine[i].hdr.trans), i);
                   
      d_msg__ (".\n");
    }
}
