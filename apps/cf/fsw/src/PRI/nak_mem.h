/* FILE: nak_mem.h -- specs for a module that manages memory within a NAK
 *   structure.  These routines free the caller from concerns about the
 *   type of memory allocation being used (static or dynamic).
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
 * SUMMARY:  This module can manage the memory for an unlimited number of
 *   different naks.  It allows the client to use the 'nak' structure as
 *   if memory is dynamically allocated (regardless of the internal storage
 *   mechanism).
 * USAGE:  For each nak, call 'nak_mem__init_heap' once before using any of
 *   the other routines.  Then call the 'malloc' and 'free' routines as
 *   you would for a typical usage of dynamic allocation.  
 * CHANGES:
 *   2007_07_31 Tim Ray
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 */


#ifndef H_NAK_MEM
#define H_NAK_MEM 1

#include "cfdp.h"


typedef GAP NODE;

void nak_mem__init_heap (NAK *nak);
     /* WHAT IT DOES:  If dynamic memory allocation is disabled, the static
      *   'heap' is initialized to be entirely 'free'.  
      *   (When dynamic allocation is enabled, this routine does nothing)
      * NOTE:  This routine must be called once before using the next two.
      */

void nak_mem__free (NAK *nak, NODE *ptr);
     /* WHAT IT DOES:  If dynamic memory allocation is enabled, the given
      *   NAK-pointer is ignored and the memory pointed to by the NODE-pointer
      *   is freed.  If dynamic allocation is disabled, the specified NAK
      *   is updated to 'free' the specified NODE (within the NAK's static
      *   'free-list').
      */

NODE *nak_mem__malloc (NAK *nak);
     /* WHAT IT DOES:  Attempts to 'allocate' memory to hold a node.
      *   If successful, returns a pointer to the node; otherwise, NULL.
      *   If dynamic memory allocation is enabled, the given NAK-pointer
      *   is ignored, and a simple 'malloc' is performed.
      *   If dynamic memory allocation is disabled, the specified NAK is
      *   updated to 'malloc' an available NODE (within the NAK's static
      *   'free-list').
      */

#endif
