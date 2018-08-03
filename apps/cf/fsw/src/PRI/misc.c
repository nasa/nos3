/* FILE:  misc.c -- miscellaneous CFDP-related routines.
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
 * SPECS:  see 'misc.h'
 * ORIGINAL PROGRAMMER:  Tim Ray 301-286-0581
 * CHANGES:
 *   2007_07_31 Tim Ray (TR)
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 *   2007_11_09 TR
 *     - Better portability.  C++ uses 'public' as a keyword, so any use of
 *       'public' as a variable name was changed to 'publik'.
 *   2008_01_24 TR
 *     - Used a public status field rather than a private status field
 *       (to indicate a transaction that exists solely to re-send an Ack-Fin).
 *
 *     - Bug fix.  In 'misc__update_summary_statistics', be sure to count
 *       all abandoned transactions as unsuccessful (even if their 
 *       condition_code is 'no_error').
 */

#include "cfdp.h"
#include "cfdp_private.h"

static boolean                 m_are_partners_frozen = NO;
static SUMMARY_STATISTICS      m_statistics = { 0, 0, 0, 0 };

static CONDITION_CODE  m_last_condition_code = NO_ERROR;
static u_int_4         m_trans_seq_num = 1;
static boolean         m_was_last_trans_abandoned = NO;




/*-----------------------------*/
/* Transaction-sequence-number */
/*-----------------------------*/



/*=r=*************************************************************************/
u_int_4 misc__new_trans_seq_num (void)
{
  u_int_4       answer;
  /*------------------------------------------------------------*/

  answer = m_trans_seq_num;
  m_trans_seq_num ++;
  return (answer);
}



/*=r=*************************************************************************/
void misc__set_trans_seq_num (u_int_4 value)
{
  /*------------------------------------------------------------*/

  m_trans_seq_num = value;
}



/*-------------------------*/
/* Freeze-related routines */
/*-------------------------*/



/*=r=*************************************************************************/
void misc__freeze_all_partners (void)
{
   /*------------------------------------------------------------*/

  m_are_partners_frozen = YES;
}



/*=r=*************************************************************************/
void misc__thaw_all_partners (void)
{
   /*------------------------------------------------------------*/

  m_are_partners_frozen = NO;
}



/*=r=*************************************************************************/
boolean misc__are_any_partners_frozen (void)
{
   /*------------------------------------------------------------*/

  return (m_are_partners_frozen);
}



/*--------------------*/
/* Summary statistics */
/*--------------------*/



/*=r=*************************************************************************/
SUMMARY_STATISTICS misc__get_summary_statistics (void)
{
  /*------------------------------------------------------------*/
  return (m_statistics);
}



/*=r=*************************************************************************/
void misc__reset_summary_statistics (void)
{
   /*------------------------------------------------------------*/

  m_statistics.successful_sender_count = 0;
  m_statistics.successful_receiver_count = 0;
  m_statistics.unsuccessful_sender_count = 0;
  m_statistics.unsuccessful_receiver_count = 0;
}



/*=r=*************************************************************************/
void misc__update_summary_statistics (MACHINE *m)
     /* WHAT IT DOES:  Given the status of a state machine that has 
      *   finished, this routine updates the summary statistics.
      */
{
  /*------------------------------------------------------------*/

  if ((m->publik.role == S_1) || (m->publik.role == S_2))
    {
      /* If the transaction existed solely to ack a (retransmitted) 
       * Finished PDU, then ignore it.
       */
      if (m->publik.is_this_trans_solely_for_ack_fin)
        ;
      /* Otherwise, increment the appropriate Sender count */
      else
        {
          /* A successful transaction is neither cancelled nor abandoned */
          if ((m->publik.condition_code == NO_ERROR) &&
              (!m->publik.abandoned))
            m_statistics.successful_sender_count ++;
          else
            m_statistics.unsuccessful_sender_count ++;
        }
    }
  
  else
    /* Increment the appropriate Receiver count */
    {
      /* A successful transaction is neither cancelled nor abandoned */
      if ((m->publik.condition_code == NO_ERROR) &&
          (!m->publik.abandoned))
        m_statistics.successful_receiver_count ++;
      else
        m_statistics.unsuccessful_receiver_count ++;
    }
}


/*---------------------*/
/* Last Condition-Code */
/*---------------------*/


/*=r=*************************************************************************/
void misc__set_last_condition_code (CONDITION_CODE new_value)
{
  /*------------------------------------------------------------*/

  m_last_condition_code = new_value;
}



/*=r=************************************************************************/
CONDITION_CODE misc__get_last_condition_code (void)
{
  /*------------------------------------------------------------*/

  return (m_last_condition_code);
}



/*-------------------------------------*/
/* Was the last transaction abandoned? */
/*-------------------------------------*/



/*=r=*************************************************************************/
boolean misc__get_last_trans_abandoned (void)
{
  /*------------------------------------------------------------*/

  return (m_was_last_trans_abandoned);
}



/*=r=*************************************************************************/
void misc__set_last_trans_abandoned (boolean yes_or_no)
{
  /*------------------------------------------------------------*/

  m_was_last_trans_abandoned = yes_or_no;
}
