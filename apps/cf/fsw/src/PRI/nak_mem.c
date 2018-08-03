/* FILE: nak_mem.c -- a module that manages memory within a NAK structure.
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
 * SPECS:  see 'nak_mem.h'
 * ORIGINAL PROGRAMMER:  Tim Ray 301-286-0581
 * CHANGES:
 *   2007_07_31 Tim Ray (TR)
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 *   2007_12_06 TR
 *     - Enhancement.  Message-class filtering can now be changed on-the-fly.
 */

/*#include <malloc.h>*/
#include "cfdp.h"
#include "cfdp_private.h"

#define NO 0
#define YES 1




/*=r=************************************************************************/
void nak_mem__init_heap (NAK *nak)
{
  /*------------------------------------------------------------*/

#if IS_DYNAMIC_ALLOCATION_ENABLED==0
  int i;
  /* For static allocation, mark that each 'slot' in the array is unused */
  for (i=0; i<MAX_GAPS_PER_TRANSACTION; i++)
    nak->in_use[i] = NO;

#elif IS_DYNAMIC_ALLOCATION_ENABLED==1
  ;   /* No action necessary */

#else
  #error Enable or disable dynamic memory allocation in 'cfdp_config.h'.
#endif
}



/*=r=************************************************************************/
void nak_mem__free (NAK *nak, NODE *ptr)
{
  /*------------------------------------------------------------*/

#if IS_DYNAMIC_ALLOCATION_ENABLED==0
  int  i;
  /* Search the static array for a slot that matches the given node */
  /* On 2006_08_02, I noticed a bug due to the algorithm potentially
   * matching an array slot that is not currently in use.
   * So, a line of code was added.    Tim Ray
   */
  for (i=0; i<MAX_GAPS_PER_TRANSACTION; i++)
    if ((ptr->begin == nak->gap[i].begin) &&
        (ptr->end == nak->gap[i].end) &&
        (nak->in_use[i]))    /* this line added on 2006_08_02 */
      /* Found the matching slot; 'free' it */
      {
        if (cfdp_is_message_class_enabled (CFDP_MSG_DEBUG_MEMORY_USE))
          d_msg__ ("nak_mem:  Freeing slot #%u.\n", i);
        nak->in_use[i] = NO;
        return;
      }
  /* No matching slot */
  e_msg__ ("cfdp_engine: <BUG> unable to 'free' nak memory.\n");

#elif IS_DYNAMIC_ALLOCATION_ENABLED==1
  free (ptr);

#else
  #error Enable or disable dynamic memory allocation in 'cfdp_config.h'.
#endif
}



/*=r=************************************************************************/
NODE *nak_mem__malloc (NAK *nak)
{
  /*------------------------------------------------------------*/

#if IS_DYNAMIC_ALLOCATION_ENABLED==0
  int                 i;
  /* Search the static array for a slot that is currently unused */
  for (i=0; i<MAX_GAPS_PER_TRANSACTION; i++)
    if (!nak->in_use[i])
      /* Found a free slot; use it */
      {
        if (cfdp_is_message_class_enabled (CFDP_MSG_DEBUG_MEMORY_USE))
          d_msg__ ("nak_mem:  Allocating slot #%u.\n", i);
        nak->in_use[i] = YES;
        return (&(nak->gap[i]));
      }
  /* All slots are in use */
  return (NULL);

#elif IS_DYNAMIC_ALLOCATION_ENABLED==1
  return (malloc (sizeof(NODE)));

#else
  #error Enable or disable dynamic memory allocation in 'cfdp_config.h'.
#endif
}
