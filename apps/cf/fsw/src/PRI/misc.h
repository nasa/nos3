/* FILE:  misc.h -- specs for miscellaneous CFDP-related stuff
 *   partner.
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
 * CHANGES:
 *   2007_07_31 Tim Ray
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 */


#ifndef H_MISC
#define H_MISC 1

#include "cfdp.h"



/*-----------------------------*/
/* Transaction-sequence-number */
/*-----------------------------*/

u_int_4 misc__new_trans_seq_num (void);
/* WHAT IT DOES:  This routine should be called each time our CFDP entity
 *   initiates a transaction (i.e. when we are the Sender).  
 *   It returns the transaction-sequence-number that should be used.
 *   (This routine ensures that the sequence-number increments with each
 *   new transaction)
 */

void misc__set_trans_seq_num (u_int_4 value);
/* WHAT IT DOES:  Allows the client to override the normal mechanism for
 *   choosing the sequence-number to be assigned to the next transaction.
 *   The given value will be returned to the next caller of the routine
 *   'misc__new_trans_seq_num' (and incremented normally thereafter).
 */



/*-------------------------*/
/* Freeze-related routines */
/*-------------------------*/

void misc__freeze_all_partners (void);

void misc__thaw_all_partners (void);

boolean misc__are_any_partners_frozen (void);
/* WHAT IT DOES:  Returns '1' if any partner is frozen; otherwise, '0'. */



/*--------------------*/
/* Summary statistics */
/*--------------------*/

typedef struct 
{
  u_int_4    successful_sender_count;
  u_int_4    successful_receiver_count;
  u_int_4    unsuccessful_sender_count;
  u_int_4    unsuccessful_receiver_count;
} SUMMARY_STATISTICS;


SUMMARY_STATISTICS misc__get_summary_statistics (void);
/* WHAT IT DOES:  Returns a summary of state machine statistics. */

void misc__reset_summary_statistics (void);
/* WHAT IT DOES:  Resets all the statistical totals to zero. */

void misc__update_summary_statistics (MACHINE *m);
/* WHAT IT DOES:  Assumes that the given state machine has just completed,
 *   and adds this state machine's outcome to the summary statistics.
 */



/*---------------------*/
/* Last Condition-Code */
/*---------------------*/

/* Context:  These routines support automated testing of the engine */

CONDITION_CODE misc__get_last_condition_code (void);
/* WHAT IT DOES:  Answers the question "what was the Completion-Code of 
 *   the most recently completed transaction?".  (answer via return status)
 */

void misc__set_last_condition_code (CONDITION_CODE new_value);
/* WHAT IT DOES:  Sets the value of 'last_condition_code' (presumably,
 *   called at the end of each transaction).
 */



/*-------------------------------------*/
/* Was the last transaction abandoned? */
/*-------------------------------------*/

/* Context:  These routines support automated testing of the engine. */
/* Note:  There is no Condition-Code to indicate that a transaction was
 *   abandoned.  That's why 'abandon?' is handled separately.
 */

boolean misc__get_last_trans_abandoned (void);
/* WHAT IT DOES:  Answers the question "was the last transaction abandoned?" */

void misc__set_last_trans_abandoned (boolean yes_or_no);
/* WHAT IT DOES:  Sets the value of 'was_the_last_transaction_abandoned'
 *   (presumably, called at the end of each transaction).
 */

#endif
