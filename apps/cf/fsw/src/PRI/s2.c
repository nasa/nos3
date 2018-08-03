/* FILE: s2.c  CFDP Class 2 Sender.
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
 * SPECS:  s2.h
 * ORIGINAL PROGRAMMER:  Tim Ray 301-286-0581
 * CHANGES:
 *   2007_07_31 Tim Ray (TR)
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 *   2007_09_07 TR
 *     - Bug fix.  If we have sent an EOF-Cancel and are waiting for an
 *       Ack-EOF-Cancel from our partner, do *not* send any more Filedata.
 *   2007_11_01 TR
 *     - Bug fix.  In the previous version, I developed some elegant logic
 *       to ensure that outgoing File Directives get priority over 
 *       outgoing Filedata.  It was beautiful for a single transaction,
 *       but didn't work across multiple concurrent transactions.
 *       SO, the old event 'throttle_outgoing_pdus' was broken into 
 *       separate events for File-Directives and Filedata.
 *   2007_11_09 TR
 *     - Better portability.  C++ uses 'public' as a keyword, so any use of
 *       'public' as a variable name was changed to 'publik'.
 *   2007_11_19 TR
 *     - Bug fix.  If the first pdu received from our partner is a
 *       'Finished-Cancel', then perform a local 'cancel'.  
 *   2007_12_06 TR
 *     - Enhancement.  Message-class filtering can now be changed on-the-fly.
 *   2007_12_07 TR
 *     - Added error messages to help explain why filestore-rejection occurs.
 *   2008_01_24 TR
 *     - Used a public status field rather than a private status field
 *       (to indicate a transaction that exists solely to re-send an Ack-Fin).
 */

#include "cfdp.h"
#include "cfdp_private.h"

#define  NO  0
#define  YES  1





/*=r=*************************************************************************/
static void m__ask_partner_to_cancel (MACHINE *m)
     /* WHAT IT DOES:  Builds and queues an outgoing EOF-Cancel PDU. */
{
  TRANS_STATUS      *mp = &(m->publik);   /* useful shorthand */
  /*------------------------------------------------------------*/

  /* Build and queue an outgoing EOF-Cancel PDU (to let our partner know
   * that we are cancelling).
   */
  m->eof = aaa__build_eof (mp->condition_code, 0, mp->md.file_size);
  m->is_outgoing_eof_buffered = YES;
  mp->attempts = 0;   /* We haven't sent any EOF-Cancel PDUs */
}



/*=r=*************************************************************************/
static void m__move_to_state_s2 (MACHINE *m)
{
  /*------------------------------------------------------------*/

  /* Move to state S2 (Cancelling) */
  m->publik.state = S2;
  if (cfdp_is_message_class_enabled (CFDP_MSG_STATE_CHANGE))
    d_msg__ ("<S2>  (State S2) 'Transaction Cancelled'\n");
}



/*=r=*************************************************************************/
static boolean m__fault_handler (MACHINE *m, CONDITION_CODE fault)
/* WHAT IT DOES:  Responds to the given fault in accordance with the
 *   engine user's setting of "response to fault" (in the MIB).
 * RETURN STATUS:  If '1', the response to the given fault was 'ignore', 
 *   and the calling state table routine may continue with whatever it
 *   was doing.  If '0', the response was suspend, cancel, or abandon
 *   (and the calling state table routine should return immediately).
 */
{
  boolean          ok_for_caller_to_continue;
  /*------------------------------------------------------------*/

  /* Save the fault-type */
  m->publik.condition_code = fault;
  /* 'Indication' callback allows engine user to respond to events */
  indication__ (IND_FAULT, &(m->publik)); 

  /* Unless the user has configured to 'ignore' this fault, the calling
   * routine should not continue with what it was doing.
   */
  ok_for_caller_to_continue = NO;

  if (mib__response (fault) == RESPONSE_CANCEL) 
    { 
      m->have_we_initiated_a_cancel = YES; 
      aaa__cancel_locally (m); 
      m__ask_partner_to_cancel (m); 
      m__move_to_state_s2 (m); 
    } 

  else if (mib__response (fault) == RESPONSE_SUSPEND) 
    aaa__notice_of_suspension (m);

  else if (mib__response (fault) == RESPONSE_IGNORE) 
    ok_for_caller_to_continue = YES;

  else if (mib__response (fault) == RESPONSE_ABANDON) 
    aaa__abandon_this_transaction (m); 

  else 
    e_msg__ ("cfdp_engine: problem with the Fault mechanism in <S2>.\n");
  
  return (ok_for_caller_to_continue);
}



/*=r=*************************************************************************/
static void m__release_an_ack_pdu (MACHINE *m)
     /* WHAT IT DOES:  Outputs an Ack PDU, 
      *   and performs the actions required after the PDU is released.
      */
{
  /*------------------------------------------------------------*/

  /*----------------*/
  /* Output the PDU */
  /*----------------*/

  aaa__send_ack (m->hdr, m->ack);
  m->is_outgoing_ack_buffered = NO;

  /*----------------------------------------------------*/
  /* Perform actions required after the PDU is released */
  /*----------------------------------------------------*/

  /* We have just sent an Ack-Finished PDU to our partner.  Normally, this
   * is the last PDU of the transaction (and we can shut down).
   * However, if we have initiated a Cancel, we can't shut down until our
   * partner Acks our cancel.
   */

  if (m->have_we_initiated_a_cancel)
    /* Keep running until our Cancel is ack'd. */
    ;
  else
    /* We're done, wrap up and shut down. */
    aaa__transaction_has_finished (m);
}



/*=r=*************************************************************************/
static void m__release_a_metadata_pdu (MACHINE *m)
     /* WHAT IT DOES:  Outputs a Metadata PDU, 
      *   and performs the actions required after the PDU is released.
      */
{
  TRANS_STATUS      *mp = &(m->publik);   /* useful shorthand */
  /*------------------------------------------------------------*/

  /*----------------*/
  /* Output the PDU */
  /*----------------*/

  aaa__send_metadata (m->hdr, m->publik.md);
  m->is_outgoing_md_buffered = NO;

  /*----------------------------------------------------*/
  /* Perform actions required after the PDU is released */
  /*----------------------------------------------------*/

  /* Update our bookkeeping (of what data still needs to be sent) */
  nak__metadata_sent_or_received (&(m->nak));
  
  /* 'Indication' callback allows engine user to respond to events */
  indication__ (IND_METADATA_SENT, &(m->publik));
  
  if (mp->attempts == 0)
    /* First transmission of Metadata */
    {
      
      if (mp->md.file_transfer && !m->is_external_xfer_open)
        /* This is the typical case -- we have been asked to send a file,
         * and the engine user is letting us build all the PDUs.
         */
        {
          if (!aaa__open_source_file (m))
            {
              e_msg__ ("cfdp_engine: unable to open source file '%s'.\n",
                       m->publik.md.source_file_name);
              if (!m__fault_handler (m, FILESTORE_REJECTION))
                return;
            }
          if (!aaa__is_file_structure_valid (m))
            {
              if (!m__fault_handler (m, INVALID_FILE_STRUCTURE))
                return;
            }
          
          /* Queue the entire file for metered released */
          nak__set_end_of_scope (&(m->nak), mp->md.file_size);
        }
      
      else if (mp->md.file_transfer && m->is_external_xfer_open)
        /* Special case:  Although a file is to be transferred, we are
         * not supposed to send Filedata PDUs (unless retransmissions
         * are necessary).  The engine user will send the initial set
         * of Filedata PDUs.
         * So, just chill until the engine user finishes sending the file.
         */
        ;
      
      else
        /* No file transfer, so initiate transfer of EOF */
        {
          m->eof = aaa__build_eof (mp->condition_code, 0, 0);
          m->is_outgoing_eof_buffered = YES;
        }
      
    }
}



/*=r=*************************************************************************/
static void m__release_an_eof_pdu (MACHINE *m)
     /* WHAT IT DOES:  Outputs an EOF PDU, 
      *   and performs the actions required after the PDU is released.
      */
{
  TRANS_STATUS      *mp = &(m->publik);   /* useful shorthand */
  /*------------------------------------------------------------*/

  /*----------------*/
  /* Output the PDU */
  /*----------------*/

  aaa__send_eof (m->hdr, m->eof);
  m->is_outgoing_eof_buffered = NO;

  /*----------------------------------------------------*/
  /* Perform actions required after the PDU is released */
  /*----------------------------------------------------*/

  /* Bookkeeping */
  mp->attempts ++;
  if (m->eof.condition_code == NO_ERROR)
    mp->phase = 2;
  else
    mp->phase = 4;

  /* Give our partner ack-timeout seconds to ack this pdu */
  m->ack_timer = timer__start (mib__ack_timeout(NULL));

  /* Issue the EOF-Sent Indication once */
  if (!m->has_eof_been_sent)
    {
      m->has_eof_been_sent = YES;
      /* If configured to do so, let the engine user know EOF was sent */
      if (mib__issue_eof_sent())
        indication__ (IND_EOF_SENT, &(m->publik));
      /* At this point, our partner is supposed to begin sending PDUs back,
       * so start the Inactivity-timer.
       */
      m->inactivity_timer = timer__start (mib__inactivity_timeout(NULL));
    }
}



/*=r=*************************************************************************/
static void m__release_a_file_data_pdu (MACHINE *m, FD *fd)
     /* WHAT IT DOES:  Outputs a Filedata PDU */
{
  TRANS_STATUS      *mp = &(m->publik);   /* useful shorthand */
  /*------------------------------------------------------------*/

  /*----------------*/
  /* Output the PDU */
  /*----------------*/
  aaa__send_one_file_data_pdu (m, fd);

  /*----------------------------------------------------*/
  /* Perform actions required after the PDU is released */
  /*----------------------------------------------------*/

  /* Remove this data range from our list of data gaps */
  nak__data_sent (&(m->nak), fd->offset, fd->offset+fd->buffer_length);
  
  if (nak__how_many_filedata_gaps (&(m->nak)) == 0)
    /* All data has been (re)transmitted */
    {
      /* Note:  the file is closed each time all data-output
       * completes, and re-opened each time a Nak is received.
       */
      fclose_callback (m->fp);
      m->is_there_an_open_file = NO;
      
      /* If we haven't already sent an EOF PDU, queue one up now */
      if (!m->has_eof_been_sent)
        {
          m->eof = aaa__build_eof (mp->condition_code, 
                                   mp->file_checksum_as_calculated,
                                   mp->md.file_size);
          m->is_outgoing_eof_buffered = YES;
        }
    }
}



/*=r=*************************************************************************/
static void m__ack_fin_from_previous_machine (MACHINE *m, 
                                              PDU_AS_STRUCT *pdu_ptr)
     /* Special case:
      * It's possible that a Class 2 Sender's final outgoing PDU 
      * (an Ack-Finished PDU) is not delivered to the Receiver.  In that 
      * case, the Sender will shut down, but the Receiver will not.  The 
      * Receiver will resend the Finished PDU.  That will cause
      * another Class 2 Sender state machine to be created.
      * This routine (re)sends the Ack-Finished PDU so the Receiver
      * can complete.
      */
{
  TRANS_STATUS      *mp = &(m->publik);   /* useful shorthand */
  /*------------------------------------------------------------*/

  /* Bookkeeping */
  mp->is_this_trans_solely_for_ack_fin = YES;
  mp->phase = 4;

  /* Save this Finished PDU */
  m->fin = pdu_ptr->data_field.fin;

  mp->condition_code = m->fin.condition_code;
  if (mp->condition_code != NO_ERROR)
    /* Our partner is cancelling the transaction */
    aaa__cancel_locally (m);

  /* Reuse the incoming PDU-header (with 'direction' field reversed )*/
  m->hdr = pdu_ptr->hdr;
  m->hdr.direction = TOWARD_RECEIVER;
  mp->trans = m->hdr.trans;
  mp->partner_id = m->hdr.dest_id;

  /* 'Indication' callback allows engine user to respond to events */
  indication__ (IND_TRANSACTION, &(m->publik));
  indication__ (IND_MACHINE_ALLOCATED, &(m->publik));
  
  /* Build and queue an Ack of the Finished PDU */
  m->ack = aaa__build_ack_fin (m);
  m->is_outgoing_ack_buffered = YES;

  /* (After the Ack-Finished PDU is released, we will shut down) */
}








/*=r=************************************************************************/
void s2__state_table (MACHINE *m, int event, 
                      PDU_AS_STRUCT *pdu_ptr, REQUEST *req_ptr)
     /* NOTE:  Conceptually, this state table has 2 states:
      *    S1 = Nominal 
      *    S2 = Transaction Cancelled
      * So, the state is always S1 unless the transaction is cancelled.
      * Transactions can be cancelled in 3 ways:
      *    1) the local User can issue a Cancel Request
      *    2) our CFDP partner can send a Finished-Cancel PDU.
      *    3) A fault may occur, and the response to that fault may be
      *       to cancel.
      */
{
  static FD          fd;
  TRANS_STATUS      *mp = &(m->publik);   /* useful shorthand */
  /*------------------------------------------------------------*/

  /* Possibly display some debug info */
  if (cfdp_is_message_class_enabled (CFDP_MSG_STATE_ALL))
    aaa__display_state_and_event ("S2", m->publik.state, event);


  /*---------------------------------------------------------------------*/
  /* Here is the response to each event.  Generally, the response is the */
  /* same for both states.  If not, the logic within a single event will */
  /* be broken into two parts.                                           */
  /*---------------------------------------------------------------------*/


  if (event == THROTTLE_OUTGOING_FILE_DIR_PDU)
    /* Release, at most, one outgoing File Directive PDU */
    {
      if (mp->frozen)
        /* We can't release any PDUs while frozen */
        ;

      else if (mp->suspended)
        /* The only PDU that can released while suspended is an Ack */
        {
          if (m->is_outgoing_ack_buffered)
            /* An Ack PDU is ready.  Do we have the "green light"? */
            if (pdu_output__ready (FILE_DIR_PDU, mp->trans, m->hdr.dest_id))
              /* Light is green.  Send it */
              m__release_an_ack_pdu (m);
        }

      else if (m->is_outgoing_ack_buffered)
        /* We're not frozen or suspended, so we can release any PDU.
         * An Ack is ready.  Do we have the "green light"?
         */
        {
          if (pdu_output__ready (FILE_DIR_PDU, mp->trans, m->hdr.dest_id))
            /* Light is green.  Send it */
            m__release_an_ack_pdu (m);
        }

      else if (m->is_outgoing_md_buffered)
        /* We're not frozen or suspended, so we can release any PDU.
         * A Metadata is ready.  Do we have the "green light"?
         */
        {
          if (pdu_output__ready (FILE_DIR_PDU, mp->trans, m->hdr.dest_id))
            /* Light is green. Send it. */
            m__release_a_metadata_pdu (m);
        }

      else if (m->is_outgoing_eof_buffered)
        /* We're not frozen or suspended, so we can release any PDU.
         * An EOF is ready.  Do we have the "green light"?
         */
        {
          if (pdu_output__ready (FILE_DIR_PDU, mp->trans, m->hdr.dest_id))
            /* Light is green. Send it. */
            m__release_an_eof_pdu (m);
        }

    }


              
  else if (event == THROTTLE_OUTGOING_FILE_DATA_PDU)
    /* Release, at most, one outgoing Filedata PDU. */
    {
      if (mp->frozen || mp->suspended)
        /* We can't release Filedata PDUs while frozen or suspended */
        ;

      else if (nak__how_many_filedata_gaps (&(m->nak)) > 0)
        /* There is File Data ready.  Do we have the "green light?" */
        {
          /* Careful:  If we have sent an EOF-Cancel and are awaiting 
           * an Ack-EOF-Cancel, don't send any more Filedata!
           */
          if (!mp->cancelled)
            {
              if (pdu_output__ready (FILE_DATA_PDU, mp->trans, m->hdr.dest_id))
                /* Light is green. Send it. */
                m__release_a_file_data_pdu (m, &fd);
            }
        }
    }


              
  else if (event == RECEIVED_PUT_REQUEST)
    /* A Put Request was received   NOMINAL PATH STARTS HERE... */
    {
      m->has_a_put_request_been_received = YES;

      /* Generate a Metadata pdu */
      aaa__build_metadata_from_put_req (*req_ptr, &m->hdr, &mp->md);
      
      /* Make some info available to the engine user */
      mp->trans = m->hdr.trans;
      mp->partner_id = req_ptr->info.put.dest_id;

      /* 'Indication' callback allows engine user to respond to events */
      indication__ (IND_TRANSACTION, &(m->publik));
      indication__ (IND_MACHINE_ALLOCATED, &(m->publik));
      
      /* Queue the Metadata PDU for metered release */
      m->is_outgoing_md_buffered = YES;
    }



  else if (event == RECEIVED_SUSPEND_REQUEST)
    /* A Suspend Request was received; suspend timers */
    aaa__notice_of_suspension (m);



  else if (event == RECEIVED_RESUME_REQUEST)
    /* A Resume Request was received; possibly resume timers */
    {
      mp->suspended = NO;
      /* 'Indication' callback allows engine user to respond to events */
      indication__ (IND_RESUMED, &(m->publik));
      /* Timers remain paused if either 'suspended' or 'frozen' */
      if (!mp->frozen)
        aaa__resume_timers (m);
    }



  else if (event == RECEIVED_CANCEL_REQUEST)
    /* Received a Cancel Request (from our User, not our CFDP partner) */
    {
      /* If the transaction isn't already being cancelled, begin doing so */
      if (mp->state == S1)
        {
          mp->condition_code = CANCEL_REQUEST_RECEIVED;
          m->have_we_initiated_a_cancel = YES;
          aaa__cancel_locally (m);
          m__ask_partner_to_cancel (m);
          m__move_to_state_s2 (m);
        }
    }



  else if (event == RECEIVED_ABANDON_REQUEST)
    /* Abandon this transaction */
    aaa__abandon_this_transaction (m);



  else if (event == RECEIVED_REPORT_REQUEST)
    /* A Report Request was received; issue a Report */
    indication__ (IND_REPORT, &(m->publik));



  else if (event == RECEIVED_FREEZE_REQUEST)
    /* A "Freeze" was received, so suspend all timers */
    {
      mp->frozen = YES;
      aaa__suspend_timers (m);
    }



  else if (event == RECEIVED_THAW_REQUEST)
    /* A "Thaw" was received; possibly resume all timers */
    {
      mp->frozen = NO;
      /* Timers remain paused if either 'suspended' or 'frozen' */
      if (!mp->suspended)
        aaa__resume_timers (m);
    }



  else if (event == RECEIVED_ACK_EOF_NO_ERROR_PDU)
    /* An Ack-EOF-No-error PDU was received.  Nominally, this lets us know
     * that the Receiver got our EOF, and they will Nak any missing data.
     */
    {
      if (mp->state == S1)
        {
          if (mp->attempts == 0)
            /* Protocol violation */
            w_msg__ ("cfdp_engine: protocol violation: "
                     "ack of non-existent EOF-no-error.\n");
          else
            {
              /* We are now in Phase 3 (the handoff is complete) */
              mp->phase = 3;
              timer__cancel (&m->ack_timer);
              /* If an EOF is queued, don't send it */
              m->is_outgoing_eof_buffered = NO;
            }
        }

      else if (mp->state == S2)
        /* The transaction is being cancelled, so ignore this input */
        ;
    }



  else if (event == RECEIVED_ACK_EOF_CANCEL_PDU)
    /* An Ack-EOF-Cancel PDU was received.  When we initiate a Cancel,
     * this is the expected response from our partner, and we can shut down.
     */
    {
      if (mp->state == S1)
        /* Protocol violation */
        w_msg__ ("cfdp_engine: protocol violation: "
                 "ack of non-existent EOF-cancel.\n");

      else if (mp->state == S2)
        /* This Ack is in response to our EOF-Cancel; so shut down */
        {
          m->ack = pdu_ptr->data_field.ack;
          aaa__transaction_has_finished (m);
        }
    }



  else if (event == RECEIVED_NAK_PDU)
    /* A Nak PDU was received.  Our partner has told us what data is missing.
     * Nominally, we queue up the missing data for retransmission.
     * If the transaction has been cancelled, we don't bother.
     */
    {
      mp->how_many_naks ++;

      if (mp->state == S1)
        {
          /* Note:  The file is closed each time all data-output completes,
           * and re-opened each time a Nak is received.
           */
          if (mp->md.file_transfer && !m->is_there_an_open_file)
            {
              if (!aaa__open_source_file (m))
                {
                  e_msg__ ("cfdp_engine: unable to open source file '%s'.\n",
                           m->publik.md.source_file_name);
                  if (!m__fault_handler (m, FILESTORE_REJECTION))
                    return;
                }
            }

          /* Initiate the requested Filedata retransmission by giving a 
           * copy of this Nak to our bookkeeper (the 'nak' module).
           * (the requested retransmissions will be metered out one PDU at
           * a time by the 'throttle_outgoing_pdus' event)
           */
          nak__copy_nak (&m->nak, &pdu_ptr->data_field.nak);

          /* Queue Metadata retransmission (if requested in the Nak) */
          if (m->nak.is_metadata_missing)
            m->is_outgoing_md_buffered = YES;
        }
    }



  else if (event == RECEIVED_FINISHED_NO_ERROR_PDU)
    /* Received a Finished (no error) PDU.  Nominally, this tells us that
     * our partner has received all data; we Ack their Finished and shut
     * down.
     */
    {

      /* If the final Ack-Finished that we sent from a previous (now dead)
       * state machine was dropped, our partner would re-send the 
       * Finished PDU, and we would have the special case that is
       * handled here.
       */
      if (!m->has_a_put_request_been_received)
        {
          m__ack_fin_from_previous_machine (m, pdu_ptr);
          return;
        }

      /* Check for protocol violation; if so, don't respond */
      if (!m->has_eof_been_sent)
        w_msg__ ("cfdp_engine: protocol violation: "
                 "received Finished-no-error before sending EOF.\n");

      /* If no protocol violation, then respond */
      else
        {
          /* Save this Finished PDU */
          m->fin = pdu_ptr->data_field.fin;
          if (m->fin.delivery_code != 0)
            w_msg__ ("cfdp_engine: Finished-No-error from partner "
                     "shouldn't say delivery was incomplete.\n");
          
          /* Acknowledge the Finished PDU with an Ack-Finished PDU */
          m->ack = aaa__build_ack_fin (m);
          m->is_outgoing_ack_buffered = YES;

          if (mp->state == S1)
            {
              /* Move to Phase 4 (Shut Down) */
              mp->phase = 4;
              
              /* If Metadata and/or EOF are buffered, don't send them */
              m->is_outgoing_md_buffered = NO;
              m->is_outgoing_eof_buffered = NO;
            }

          else
            /* Ignore this pdu in state S2 (we are already cancelling) */
            ;
        }
    }



  else if (event == RECEIVED_FINISHED_CANCEL_PDU)
    /* Received a Finished-Cancel Pdu.  This is our partner's way of 
     * initiating a cancellation of the transaction.  We always Ack
     * this pdu.  If we haven't initiated a Cancel from our side, then
     * we shut down after sending the Ack.  If we have, we keep running
     * until our own EOF-Cancel is acked.
     */
    {
      /* If the final Ack-Finished that we sent from a previous (now dead)
       * state machine was dropped, our partner would re-send the 
       * Finished PDU, and we would have the special case that is
       * handled here.
       */
      if (!m->has_a_put_request_been_received)
        {
          m__ack_fin_from_previous_machine (m, pdu_ptr);
          return;
        }

      /* Note: a Finished-Cancel PDU can be received at any time.
       * Therefore, there is no check for a protocol violation here.
       */

      /* Save a copy of the Finished PDU */
      m->fin = pdu_ptr->data_field.fin;

      /* If we haven't already cancelled locally, accept partner's request */
      if (mp->state == S1)
        {
          aaa__cancel_locally (m);
          mp->condition_code = m->fin.condition_code;
          mp->phase = 4;
          m__move_to_state_s2 (m);
        }

      /* Regardless, we must acknowledge receipt of the Finished PDU */
      m->ack = aaa__build_ack_fin (m);
      m->is_outgoing_ack_buffered = YES;
    }



  else if (event == ACK_TIMER_EXPIRED)
    /* Ack-timer expired.  Resend the EOF Pdu (if permitted). */
    {
      /* 'Indication' callback allows engine user to respond to events */
      indication__ (IND_ACK_TIMER_EXPIRED, &(m->publik));

      if (mp->state == S1)
        /* This is the nominal situation */
        {
          /* If we've already sent the EOF-no-error PDU as many times as 
           * allowed, then declare a Fault.
           */
          if (mp->attempts >= mib__ack_limit (NULL))
            /* Let the Fault mechanism decide what to do */
            if (!m__fault_handler (m, POSITIVE_ACK_LIMIT_REACHED))
              return;

          /* Try again -- Re-queue the EOF for metered release */
          m->is_outgoing_eof_buffered = YES;
        }

      else
        /* Transaction is being cancelled */
        {
          /* If we've already sent the EOF-Cancel PDU as many times as
           * allowed, the only choice is to give up (abandon the transaction).
           */
          if (mp->attempts >= mib__ack_limit (NULL))
            /* "Positive Ack Limit" has been reached; give up */
            aaa__abandon_this_transaction (m);
          else
            /* Try again -- Re-queue the EOF for metered release */
            m->is_outgoing_eof_buffered = YES;
        }
    }



  else if (event == INACTIVITY_TIMER_EXPIRED)
    /* We are waiting for a response from our partner, and there is none. */
    {
      /* 'Indication' callback allows engine user to respond to events */
      indication__ (IND_INACTIVITY_TIMER_EXPIRED, &(m->publik));

      if (mp->state == S1)
        {
          /* Restart the Inactivity-timer */
          m->inactivity_timer = timer__start (mib__inactivity_timeout(NULL));
          /* Let the Fault mechanism decide what else to do */
          if (!m__fault_handler (m, INACTIVITY_DETECTED))
            return;
        }
      else
        /* We are already cancelling; just give up completely */
        aaa__abandon_this_transaction (m);
    }



  else if (event == EXTERNAL_FILE_TRANSFER_COMPLETED)
    /* This event was added in May 2007 to support high-speed (external)
     * file transfer by the engine user.  When this event fires, we know
     * that the engine user has completed sending the first round of
     * Filedata PDUs.  Therefore, we can send the EOF PDU and take over
     * responsibility for handling acks and naks.
     */
    {
      /* Queue an outgoing EOF-No-error PDU */
      m->eof = aaa__build_eof (mp->condition_code, 
                               mp->file_checksum_as_calculated,
                               mp->md.file_size);
      m->is_outgoing_eof_buffered = YES;

      /* Clear the flags related to external xfer */
      m->is_external_xfer_open = NO;
      m->should_external_xfer_be_closed = NO;
    }



  else 
    /* The above events are the only ones that should occur.  If some
     * other event occurs, ignore it (but warn the user).
     */
    w_msg__ ("cfdp_engine: ignored event '%s' in state %s "
             "for trans '%s' <S1>.\n",
             event__event_as_string(event), mp->state, 
             cfdp_trans_as_string (mp->trans));
}
