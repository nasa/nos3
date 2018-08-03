/* FILE: nak.c -- Module to keep track of what needs to be Nak-ed.
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
 * SPECS:  nak.h
 * ORIGINAL PROGRAMMER:  Tim Ray 301-286-0581
 * NOTE:  Many of the routines use a pointer argument called 'nak_ptr'.
 *   The use of that argument allows this module to support multiple 
 *   transactions (each maintaining an independent list of Naks).
 * CHANGES:
 *   2007_07_31 Tim Ray (TR)
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 *   2007_11_09 TR
 *     - Better portability.  C++ uses 'new' as a keyword, so any use of
 *       'new' as a variable name was changed to 'neww'.
 *   2007_12_06 TR
 *     - Enhancement.  Message-class filtering can now be changed on-the-fly.
 *   2008_01_02 TR
 *     - Made assertions safer (avoid crashing if program continues).
 *   2008_03_03 TR
 *     - Careful.  In 'nak__list_as_string' allow 64 bytes of cushion
 *       instead of 32.  
 */

#include "cfdp.h"
#include "cfdp_private.h"

#define NO 0
#define YES 1




/*=r=************************************************************************/
static void m__add_node_to_end_of_list (NAK *nak_ptr, NODE *neww)
     /* WHAT IT DOES:  Adds the given node to the end of the list */
   {
     NODE            *p;
   /*------------------------------------------------------------*/

     if (nak_ptr->head == NULL)
       /* List is empty; this new node will be the head (i.e. first) */
       nak_ptr->head = neww;
     else
       {
         /* Move to the last node on the list... */
         for (p=nak_ptr->head; p->next!=NULL; p=p->next)
           ;
         /* ... and attach the new node to the end. */
         p->next = neww;
       }
   }



/*=r=************************************************************************/
static void m__new_node (NAK *nak_ptr, u_int_4 begin, u_int_4 end, NODE **neww)
/* WHAT IT DOES:  Attempts to create a new node.  If successful, the node
 *   is loaded with the given begin & end values, and a pointer
 *   to that node is returned (via 'neww').  If unsuccessful, 'neww' is null.
 * NOTE:  'nak_ptr' is an input parameter that is only relevant when dynamic 
 *   allocation is disabled (in that case, 'creation' is different).
 */
   {
     NODE           *node_ptr;
   /*------------------------------------------------------------*/

     /* If configured to do so, output a debug message */
     if (cfdp_is_message_class_enabled (CFDP_MSG_DEBUG_NAK))
       d_msg__ ("Gap added (%lu-%lu).\n", begin, end);

     /* Initialize the node-pointer to a value indicating failure */
     *neww = NULL;

     /* Attempt to allocate memory for a new node */
     *neww = nak_mem__malloc (nak_ptr);
     if (*neww == NULL)
       /* Oops.  Out of memory. */
       return;

     /* Load the new node with the specified 'begin' and 'end' values */
     node_ptr = *neww;
     node_ptr->begin = begin;
     node_ptr->end = end;
     node_ptr->next = NULL;
   }



/*=r=************************************************************************/
static void m__previous (NAK *nak_ptr, NODE *current, NODE **previous)
/* WHAT IT DOES:  Given a Nak-list ('nak') and a node ('current'), 
 *   it attempts to locate the node previous to 'current'.  If successful,
 *   '*previous' contains a pointer to that node; otherwise, '*previous' is
 *   null.
 */
   {
     NODE          *p;
   /*------------------------------------------------------------*/

     /* Initialize '*previous' to indicate failure */
     *previous = NULL;

     if (current == nak_ptr->head)
       /* There is no 'previous' node for the list-head */
       return;

     /* Walk through the list, looking for a node which points to the 
      * given node.
      */
     for (p=nak_ptr->head; p!=NULL; p=p->next)
       if (p->next == current)
         /* We found it. */
         break;

     /* Validate that the given node was on the list */
     ASSERT__ (p != NULL);

     /* Return the result.  If the walk completed without finding the
      * node (assertion fired) the result will be NULL, which
      * is the best answer we can give.
      */
     *previous = p;
   }



/*=r=************************************************************************/
static void m__incorporate_gap (NAK *nak_ptr, u_int_4 begin, u_int_4 end)
     /* WHAT IT DOES:  To avoid 'losing' the knowledge of this gap forever,
      *   extend the end-point of the last gap to include this one.
      *   On the bad side, this results in retransmission of data that
      *   was already received.  On the good side, the transaction is able
      *   to complete successfully, because all data gaps are remembered.
      */ 
{
  GAP             *p;
  /*------------------------------------------------------------*/

  /* Before doing anything, validate the input pointer */
  ASSERT__ (nak_ptr->head != NULL);
  if (nak_ptr->head == NULL)
    /* Avoid crashing if engine user continues after assertion fires */
    return;

  /* Move to the last node (gap) on the given list */
  for (p=nak_ptr->head; p->next!=NULL; p=p->next)
    ;
  p->end = end;
  if (end > nak_ptr->end_of_scope)
    /* Extend the scope of this Nak */
    nak_ptr->end_of_scope = end;
}



/*=r=************************************************************************/
static void m__add_gap_to_list (NAK *nak_ptr, u_int_4 begin, u_int_4 end)
/* WHAT IT DOES:  Either adds a new node to the end of the list (containing
 *   the specified gap), or adds the specified gap to the existing node
 *   at the end of the list.  
 * INPUTS:  'begin' and 'end' specify the gap.  'nak_ptr' provides the
 *   current linked-list of gaps.
 */
{
  NODE            *new_node_ptr;
  /*------------------------------------------------------------*/

  /* Attempt to create a new node containing the given range */
  m__new_node (nak_ptr, begin, end, &new_node_ptr);

  /* If successful, add the new node to the list.  Otherwise, incorporate
   * this new gap without adding a new node (i.e. make the best of a bad
   * situation).
   */
  if (new_node_ptr != NULL)
    m__add_node_to_end_of_list (nak_ptr, new_node_ptr);
  else
    /* Oops.  Out of memory. */
    m__incorporate_gap (nak_ptr, begin, end);
}



/*=r=************************************************************************/
static void m__remove_node (NAK *nak_ptr, NODE *obsolete)
/* WHAT IT DOES:  Removes the given node from the given nak-list. */
   {
     NODE         *node_before_obsolete;
   /*------------------------------------------------------------*/

     /* (The nak-list should never be empty at this point) */
     ASSERT__ (nak_ptr->head != NULL);
     if (nak_ptr->head == NULL)
       /* Avoid crashing if engine user continues after assertion fires */
       return;

     if (obsolete == nak_ptr->head)
       /* Remove node from front of list */
       {
         nak_ptr->head = obsolete->next;
         nak_mem__free (nak_ptr, obsolete);
       }

     else
       /* Remove node from interior/end of list */
       {
         /* Find the node that is previous to the obsolete node, 
          * and have that node point around the obsolete node (so that all
          * nodes are still linked together).
          * Then the obsolete node can be deleted safely.
          */
         m__previous (nak_ptr, obsolete, &node_before_obsolete);
         ASSERT__ (node_before_obsolete != NULL);
         if (node_before_obsolete == NULL)
           /* Avoid crashing if engine user continues after assertion fires */
           return;
         node_before_obsolete->next = obsolete->next;
         nak_mem__free (nak_ptr, obsolete);
       }
   }



/*=r=************************************************************************/
static void m__remove_range_from_node (NAK *nak_ptr, NODE *p, 
                                       u_int_4 begin, u_int_4 end)
/* WHAT IT DOES:  It removes the given range ('begin' through 'end') from 
 *   the given node (presumed to be within the given nak-list).
 * NOTE:  Any range is acceptable (i.e. if the range does not overlap the
 *   node's range, then no action is taken).
 * NOTE:  If it is necessary to delete the node, then the given nak-list
 *   will be modified.
 */
   {
     NODE                *new_node_ptr;
   /*------------------------------------------------------------*/

     /* If configured to do so, output a debug message */
     if (cfdp_is_message_class_enabled (CFDP_MSG_DEBUG_NAK))
       d_msg__ ("Removing range (%lu-%lu) from node (%lu-%lu).\n",
                begin, end, p->begin, p->end);

     /* Example of case 1:  node contains 100-200, begin=100, end=200 */
     if ((begin <= p->begin) && (end >= p->end))
       /* The range overlaps the entire node; remove node from list */
       m__remove_node (nak_ptr, p);

     /* Example of case 2:  node contains 100-200, begin=300, end=400 */
     else if ((end <= p->begin) || (begin >= p->end))
       /* The range does not overlap the node at all; no action needed */
       ;

     /* Example of case 3:  node contains 100-200, begin=50, end=150 */
     else if ((begin <= p->begin) && (end > p->begin))
       /* The range overlaps a front portion of the node; shorten the node */
       p->begin = end;

     /* Example of case 4:  node contains 100-200, begin=150, end=250 */
     else if ((begin < p->end) && (end >=p->end))
       /* The range overlaps a rear portion of the node; shorten the node */
       p->end = begin;

     /* Example of case 5:  node contains 100-200, begin=125, end=175.
      * Single gap of 100-200 is replaced with two gaps: 100-125 and 175-200.
      */
     else if ((begin > p->begin) && (end < p->end))
       /* The range overlaps an interior portion of the node, but does not
        * overlap the beginning or end.  Break the single node into 2 nodes.
        */
       {
         /* Create a new node and put the remaining "rear" portion into it */
         m__new_node (nak_ptr, end, p->end, &new_node_ptr);
         if (new_node_ptr == NULL)
           /* Oops.  Out of memory.  Leave nak as-is. 
            * There is some loss of efficiency (due to unnecessary retrans
            * by the Sender), but the transaction will be able to complete
            * successfully (because no data gap has been 'forgotten').
            */
           return;
         /* Modify original node to leave only the remaining "front" portion */
         p->end = begin;
         /* Adjust 'next' pointers so as to insert the new node after the
          * current node.
          */
         new_node_ptr->next = p->next;
         p->next = new_node_ptr;
       }

     else
       /* This code should never be reached 
        * (unless the other "if" cases above don't cover all possibilities).
        * If this code is reached, report the bug.
        * If the assertion routine allows the program to continue,
        * abort the routine.
        */
       {
         ASSERT__BUG_DETECTED;
         return;
       }
   }



/*=r=************************************************************************/
static void m__remove_range_from_list (NAK *nak_ptr, u_int_4 begin, 
                                       u_int_4 end)
     /* WHAT IT DOES:  Removes the specified range from the given list
      *   of gaps.
      */
{
  NODE                *p;
  NODE                *whos_next;
  /*------------------------------------------------------------*/

  /* Walk through the given list, removing the specified range from
   * each node on the list.
   */
  for (p=nak_ptr->head; p!=NULL; p=whos_next)
    /* Note that the use of "whos_next" avoids a core dump in the
     * event that the current node gets deleted inside 
     * 'm__remove_range_from_node'.  (p->next is blown away!)
     */
    {
      whos_next = p->next;
      m__remove_range_from_node (nak_ptr, p, begin, end);
    }
}



/*=r=************************************************************************/
void nak__copy_nak (NAK *dest_ptr, NAK *src_ptr)
{
#if IS_DYNAMIC_ALLOCATION_ENABLED==0
  int i;
#endif
  /*------------------------------------------------------------*/

  dest_ptr->start_of_scope = src_ptr->start_of_scope;
  dest_ptr->end_of_scope = src_ptr->end_of_scope;
  dest_ptr->is_metadata_missing = src_ptr->is_metadata_missing;

#if IS_DYNAMIC_ALLOCATION_ENABLED==0

  /* In the static allocation version, an array is made to look like a
   * linked list.  Therefore, all the pointers (inside each node on the list!)
   * will have to be adjusted (so that they point to memory locations 
   * within the destination array rather than within the source array).  
   */

  /* Step one:  copy the arrays */
  for (i=0; i<MAX_GAPS_PER_TRANSACTION; i++)
    {
      dest_ptr->in_use[i] = src_ptr->in_use[i];
      dest_ptr->gap[i] = src_ptr->gap[i];
    }

  /* Step two:  adjust the pointers */
  if (src_ptr->head == NULL)
    dest_ptr->head = NULL;
  else
    dest_ptr->head = src_ptr->head - &src_ptr->gap[0] + &dest_ptr->gap[0];
  for (i=0; i<MAX_GAPS_PER_TRANSACTION; i++)
    {
      if (src_ptr->gap[i].next == NULL)
        dest_ptr->gap[i].next = NULL;
      else
        dest_ptr->gap[i].next = 
          src_ptr->gap[i].next - &src_ptr->gap[0] + &dest_ptr->gap[0];
    }

#else
  /* In the dynamic allocation version, the linked list doesn't move;
   * we just have to make sure we point to the head of the list.
   */
  dest_ptr->head = src_ptr->head;

#endif
}



/*=r=************************************************************************/
void nak__init (NAK *nak_ptr, int sender_or_receiver)
   {
   /*------------------------------------------------------------*/

     /* Initialize the scope to be "empty" */
     nak_ptr->start_of_scope = 0;
     nak_ptr->end_of_scope = 0;
     
     /* Initialize the gap-list to be empty */
     nak_mem__init_heap (nak_ptr);
     nak_ptr->head = NULL;

     /* If the caller is a Receiver, add one node to the list to indicate
      * that Metadata has not been received (note that CFDP represents
      * Metadata as data range 0-0).
      */
     if (sender_or_receiver == I_AM_A_RECEIVER)
       nak_ptr->is_metadata_missing = YES;
     else
       nak_ptr->is_metadata_missing = NO;
   }



/*=r=************************************************************************/
void nak__set_end_of_scope (NAK *nak_ptr, u_int_4 end)
{
  /*------------------------------------------------------------*/

  /* Validate input */
  if (end < nak_ptr->end_of_scope)
    w_msg__ ("cfdp_engine: Nak module accepted request to set 'scope' "
             "smaller (%lu->%lu)\n",
             nak_ptr->end_of_scope, end);

  /* If the current scope has been extended, then a gap has been introduced
   * (between the current end-of-scope and the new end-of-scope); add this
   * new gap to the list.
   */
  if (end > nak_ptr->end_of_scope)
     m__add_gap_to_list (nak_ptr, nak_ptr->end_of_scope, end);

  nak_ptr->end_of_scope = end;
}



/*=r=************************************************************************/
void nak__data_sent (NAK *nak_ptr, u_int_4 begin, u_int_4 end)
{
  /*------------------------------------------------------------*/
  
  /* Remove this data-range from the gap-list */
  m__remove_range_from_list (nak_ptr, begin, end);

  /* If appropriate, extend the current scope */
  if (begin == nak_ptr->end_of_scope)
    nak_ptr->end_of_scope = end;
}



/*=r=************************************************************************/
void nak__data_received (NAK *nak_ptr, u_int_4 begin, u_int_4 end)
     /* ALGORITHM:  The given data-range (begin-end) must fit one of the four 
      *   cases below.  Determine which case applies, and perform the 
      *   corresponding action(s).
      * ASSUMPTION:  The scope always starts at zero.
      */
{
  /*------------------------------------------------------------*/

  /* Validate the given inputs */
  if ((begin < 0) || (end < 0) || (begin > end))
    {
      e_msg__ ("cfdp_engine: Nak module ignored receipt of Filedata "
               "with invalid range (%lu-%lu)\n",
               begin, end);
      return;
    }
  else
    {
      /* If configured to do so, output a debug message */
      if (cfdp_is_message_class_enabled (CFDP_MSG_DEBUG_NAK))
        d_msg__ ("Valid_filedata_received: %lu-%lu.\n", begin, end);
    }      
  
  /*--------------------------------------------------------------
   * Case 1 -- the given range is fully within the current scope.
   * Action: the scope doesn't change; just remove the given range
   * from the current gap-list.
   * Example:  scope=0-1000, range=100-200.  
   *--------------------------------------------------------------*/

  if (end <= nak_ptr->end_of_scope)
    m__remove_range_from_list (nak_ptr, begin, end);

  /*------------------------------------------------------------------------
   * Case 2 -- the given range is both inside and outside the current scope.
   * Action:  The scope has to be extended, and the portion of the given
   * range that is within the current scope has to be removed from the
   * current gap-list.
   * Example:  scope=0-1000, range=900-1100.  New scope is 0-1100,
   * and the range 900-1000 is removed from gap-list.
   *------------------------------------------------------------------------*/

  else if ((begin < nak_ptr->end_of_scope) && (end > nak_ptr->end_of_scope))
    {
      m__remove_range_from_list (nak_ptr, begin, nak_ptr->end_of_scope);
      nak_ptr->end_of_scope = end;
    }

  /*-----------------------------------------------------------------
   * Case 3 -- the given range begins at the current scope boundary
   * (i.e. there is no gap between this new data and previously-received
   * data).  
   * Action:  Just extend the current scope.
   * Example:  scope=0-1000, range=1000-1100.  New scope is 0-1100.
   *-----------------------------------------------------------------*/

  else if (begin == nak_ptr->end_of_scope)
    nak_ptr->end_of_scope = end;

  /*----------------------------------------------------------------------
   * Case 4 -- the given range is completely outside the current scope;
   * i.e. there is a gap between the end of any previously-received
   * data and the start of this new data.  
   * Action:  Add the new gap to the list, and extend the current scope.
   * Example:  scope=0-1000, range=1100-1200.  New scope is 0-1200, and
   * a gap from 1000-1100 is added to the gap-list.
   *----------------------------------------------------------------------*/

  else if (begin > nak_ptr->end_of_scope)
    {
      m__add_gap_to_list (nak_ptr, nak_ptr->end_of_scope, begin);
      nak_ptr->end_of_scope = end;
    }
  
  else 
    /* If we got here, then there is a bug in the above code */
    ASSERT__BUG_DETECTED;
}



/*=r=************************************************************************/
u_int_4 nak__how_many_filedata_gaps (NAK *nak_ptr)
   {
     u_int_4          how_many_gaps;
     NODE            *p;
   /*------------------------------------------------------------*/

     /* Each node on the given list contains one gap, so simply count
      * how many nodes are on the list.
      */
     how_many_gaps = 0;
     for (p=nak_ptr->head; p!=NULL; p=p->next)
       how_many_gaps ++;

     return (how_many_gaps);
   }



/*=r=************************************************************************/
boolean nak__is_list_empty (NAK *nak_ptr)
   {
   /*------------------------------------------------------------*/

     if ((nak_ptr->is_metadata_missing) || (nak_ptr->head != NULL))
       return (NO);
     return (YES);
   }



/*=r=************************************************************************/
char *nak__list_as_string (NAK *nak_ptr)
   {
#define LONGEST_STRING 256
#define LONGEST_SUBSTRING 32
     boolean         out_of_room = NO;
     NODE           *p;
     static char     string [LONGEST_STRING];
     char            substring [LONGEST_SUBSTRING];
   /*------------------------------------------------------------*/

     /* Include the scope of the Nak */
     sprintf (string, "(%lu-%lu)", nak_ptr->start_of_scope, 
              nak_ptr->end_of_scope);

     /* Include whether or not Metadata is missing */
     if (nak_ptr->is_metadata_missing)
       APPEND (string, " Metadata");

     /* Include as many File-data gaps as will fit in the string */
     if (nak_ptr->head != NULL)
       for (p=nak_ptr->head; p!=NULL; p=p->next)
         {
           if (!out_of_room)
             {
               if (strlen(string) + LONGEST_SUBSTRING + 64 >= LONGEST_STRING)
                 /* Careful.  We are in danger of writing past the end of the
                  * allotted memory for 'string' (i.e. we are out of room).
                  * We'll have to skip some gaps.
                  * Hopefully, the user understands that "..." indicates
                  * that there are more gaps.
                  */
                 {
                   out_of_room = YES;
                   APPEND (string, " ... ");
                 }
               else
                 /* The normal case; add this gap to the string */
                 {
                   sprintf (substring, " %lu-%lu", p->begin, p->end);
                   APPEND (string, substring);
                 }
             }
           else
             /* We already ran out of room; ignore all but the last gap */
             {
               if (p->next == NULL)
                 /* This is the last gap */
                 {
                   sprintf (substring, " %lu-%lu", p->begin, p->end);
                   APPEND (string, substring);
                 }
             }
         }

     /* Show how many gaps there are */
     sprintf (substring, " (%lu total gaps)\n", 
              nak__how_many_filedata_gaps (nak_ptr));
     APPEND (string, substring);
     return (string);
   }
#undef LONGEST_STRING
#undef LONGEST_SUBSTRING



/*=r=************************************************************************/
void nak__gaps_as_string (NAK *nak_ptr, char *string,
                                      int max_string_length)
   {
#define LONGEST_SUBSTRING 32
     boolean         first_gap = YES;
     int             length;
     NODE           *p;
     char            substring [LONGEST_SUBSTRING];
   /*------------------------------------------------------------*/

     /* Initialize the string and its length */
     memset (string, 0, max_string_length);
     length = 0;

     /* Append one file-gap at a time to the string until either all the
      * file-gaps are included or we reach the end of the string.
      */
     for (p=nak_ptr->head; p!=NULL; p=p->next)
       {
         /* Generate a substring that represents the current file-gap */
         if (first_gap)
           /* Don't put a space at the front */
           {
           sprintf (substring, "%lu-%lu", p->begin, p->end);
           first_gap = NO;
           }
         else
           sprintf (substring, " %lu-%lu", p->begin, p->end);

         if (length + strlen (substring) > max_string_length)
           /* We're out of room, so stop. */
           break;
         utils__strncat (string, substring, max_string_length);
         length += strlen (substring);
       }
   }
#undef LONGEST_SUBSTRING



/*=r=************************************************************************/
void nak__metadata_sent_or_received (NAK *nak_ptr)
{
  /*------------------------------------------------------------*/
  nak_ptr->is_metadata_missing = NO;
}



/*=r=*************************************************************************/
u_int_4 nak__how_many_bytes_missing (NAK *nak_ptr)
{
  u_int_4                answer;
  NODE                  *ptr;
   /*------------------------------------------------------------*/

  /* Initialize the answer */
  answer = 0;

  /* If the Nak-list is empty, then no bytes are missing */
  if (nak_ptr->head == NULL)
    return (answer);

  /* Otherwise, add the bytes missing in each gap to get the total */
  ptr = nak_ptr->head;
  while (ptr != NULL)
    {
      if (ptr->end == ptr->begin)
        /* This indicates a missing Metadata PDU; don't count it */
        ;
      else
        answer += ptr->end - ptr->begin + 1;
      ptr = ptr->next;
    }

  return (answer);
}



/*=r=*************************************************************************/
u_int_4 nak__how_many_bytes_received (NAK *nak_ptr)
{
  u_int_4               answer;
  u_int_4               total_without_gaps;
   /*------------------------------------------------------------*/

  /* Example of the algorithm:
   *    assume current scope is from 0-5000  (if no gaps, then 5000 bytes in)
   *    assume nak-list contains 2000-2999   (1000 bytes missing)
   *    Then bytes_received = 5000 - 1000 = 4000
   */

  total_without_gaps = nak_ptr->end_of_scope - nak_ptr->start_of_scope;
  answer = total_without_gaps - nak__how_many_bytes_missing (nak_ptr);
  return (answer);
}



