/* FILE: machine_list.h -- specs for module that maintains a list of CFDP
 *   state machines.
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
 * ORIGINAL PROGRAMMER:  Tim Ray 301-286-0581
 * SUMMARY:
 *     This module contains memory for up to MAX_CONCURRENT_TRANSACTIONS
 *   state machines (each state machine requires one MACHINE structure).
 *   Clients allocate and deallocate state machines.  This module maintains
 *   a list of the state machines, and allows clients to walk through
 *   the list (visiting one node at a time) and/or retrieve the state
 *   machine assigned to a particular transaction.
 *     Again, the actual memory for the machines is contained within this 
 *   module, and clients are given pointers to machine slots.
 * CHANGES:
 *   2007_07_31 Tim Ray
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 */

#ifndef H_MACHINE_LIST
#define H_MACHINE_LIST

#include "cfdp.h"

MACHINE *machine_list__allocate_slot (void);
/* WHAT IT DOES:  If there is an unused slot, it reserves the slot and
 *  returns a pointer to the slot.  Otherwise, returns NULL.
 * WARNING:  The MACHINE structure is NOT initialized!
 */

void machine_list__open_a_walk (void);
MACHINE *machine_list__get_next_machine (void);
/* WHAT THESE DO:  Allow clients to loop through all the machines.  Example:
 *   machine_list__open_a_walk ();
 *   while ((m=machine_list__get_next_machine()) != NULL)
       {
 *       "do something with each machine"
 *     }
 */

MACHINE *machine_list__get_this_trans (TRANSACTION t);
/* WHAT IT DOES:  Given a Transaction (Source-ID + Trans-ID), this routine
 *   tries to find the machine assigned to that transaction.  If successful,
 *   it returns a pointer to that machine; otherwise it returns a Null pointer.
 */

boolean machine_list__deallocate_slot (MACHINE *m);
/* WHAT IT DOES:  Attempts to find an in-use slot whose address matches
 *   the address of the given MACHINE structure.  If successful, frees that
 *   slot and returns '1'.  Otherwise, returns '0'.
 */

u_int_4 machine_list__how_many_machines (void);
/* WHAT IT DOES:  Tells the caller how many machines are currently on the
 *   list.
 */

void machine_list__display_list (void);
/* WHAT IT DOES:  Displays the current machine list. */

#endif
