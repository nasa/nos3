/* FILE: r2.c   CFDP Class 2 Receiver.
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
 * SPECS: r2.h
 * ORIGINAL PROGRAMMER:  Tim Ray 301-286-0581
 * CHANGES:
 *   2007_07_31 Tim Ray (TR)
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 *   2007_11_01 TR
 *     - Renamed the 'throttle' event (part of a bug fix).
 *   2007_11_05 TR
 *     - Moved the 'retain temp file' logic to aaa.c.
 *   2007_11_09 TR
 *     - Better portability.  C++ uses 'public' as a keyword, so any use of
 *       'public' as a variable name was changed to 'publik'.
 *   2007_12_06 TR
 *     - Enhancement.  Message-class filtering can now be changed on-the-fly.
 *   2007_12_07 TR
 *     - Added error messages to help explain why filestore-rejection occurs.
 *   2008_01_02 TR
 *     - Made assertions safer (avoid crashing if program continues).
 *     - Protocol Optimization:  Don't wait for the Nak-timer to expire
 *       before checking to see if retransmission is complete.
 *   2008_01_08 TR
 *     - Bug fix (see protocol optimization from last week).  Make sure we
 *       don't try to close the temp file more than once!
 *   2008_01_24 TR
 *     - Bug-fix: don't attempt to remove a non-existent file.  This module
 *       now clears the variable 'is_there_a_temp_file' if the temp-file is
 *       successfully renamed to its final destination.
 *   2008_03_03 TR
 *     - Bug-fix.  In 'm__release_an_ack_pdu', if an ack-eof-no-error pdu
 *       is being released, we should enter phase 3 if the transaction
 *       has *not* been cancelled.  (rather than if it *has* been)
 */

#include "cfdp.h"
#include "cfdp_private.h"

#define PRINTF printf
#define NO 0
#define YES 1






/*=r=*************************************************************************/
static void m__ask_partner_to_cancel (MACHINE *m)
     /* WHAT IT DOES:  Builds and queues an outgoing Finished-Cancel PDU. */
{
  TRANS_STATUS      *mp = &(m->publik);   /* useful shorthand */
  /*------------------------------------------------------------*/

  /* Build and queue an outgoing Finished-Cancel PDU (to let our partner know
   * that we are cancelling).
   */
  m->fin = aaa__build_fin (mp->condition_code, mp->delivery_code);
  m->is_outgoing_fin_buffered = YES;
  mp->attempts = 0;   /* We haven't sent any Finished-Cancel PDUs */
}



/*=r=*************************************************************************/
static void m__move_to_state_s2 (MACHINE *m)
{
  /*------------------------------------------------------------*/

  /* Move to state S2 (Cancelling) */
  m->publik.state = S2;
  if (cfdp_is_message_class_enabled (CFDP_MSG_STATE_CHANGE))
    d_msg__ ("<R2>  (State S2) 'Transaction Cancelled'\n");
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
    e_msg__ ("cfdp_engine: problem with the Fault mechanism in <R2>.\n");
  
  return (ok_for_caller_to_continue);
}



/*=r=*************************************************************************/
static void m__all_data_has_been_received (MACHINE *m)
     /* WHAT IT DOES:  Performs the actions required after all data has
      *   been delivered.  Typically, validates the delivered file, moves
      *   it to the destination specified in the Metadata PDU, and
      *   builds/queues an outgoing Finished-No-error PDU.
      */
{
  TRANS_STATUS      *mp = &(m->publik);   /* useful shorthand */
  /*------------------------------------------------------------*/

  /* IMPORTANT:  The program will crash if the actions in this routine are
   * carried out more than once, so prevent that from happening.
   */
  if (mp->delivery_code == DATA_COMPLETE)
    /* Return immediately! */
    {
      w_msg__ ("cfdp_engine: 'all_data_has_been_received' called 2+ times.\n");
      return;
    }

  /* We've received everything, so delivery is complete... */
  mp->delivery_code = DATA_COMPLETE;

  /* ... and there is no need to send a Nak back to the sender. */
  m->is_outgoing_nak_buffered = NO;
  timer__cancel (&m->nak_timer);
  
  /* If a file has been transferred, do something with it */
  if (mp->md.file_transfer)
    {
      /* If the temporary file is still open, then close it */
      if (m->is_there_an_open_file)
        {
          ASSERT__ (m->fp != NULL);
          if (m->fp == NULL)
            /* This should never happen, but if it does, avoid crashing */
            ;
          else
            fclose_callback (m->fp);
          m->is_there_an_open_file = NO;
        }
      
      /* Validate the received file (via the checksum) */
      if (!aaa__is_file_checksum_valid (m))
        {
          if (!m__fault_handler (m, FILE_CHECKSUM_FAILURE))
            return;
        }
      
      /* Move the temporary file to the real destination */
      if (!rename_callback (mp->temp_file_name, mp->md.dest_file_name))
        /* Success */
        m->is_there_a_temp_file = NO;
      else
        /* Oops. Our attempt to move the file failed. */
        {
          e_msg__ ("cfdp_engine: unable to rename '%s' to '%s'.\n",
                   mp->temp_file_name, mp->md.dest_file_name);
          e_msg__ ("cfdp_engine: are they on separate filesystems?\n");
          if (!m__fault_handler (m, FILESTORE_REJECTION))
            return;
        }
    }
  
  /* <NOT_SUPPORTED> Filestore Requests (executed here) */
  
  /* Build and queue a Finished Pdu */
  m->fin = aaa__build_fin (NO_ERROR, mp->delivery_code);
  m->is_outgoing_fin_buffered = YES;
  mp->attempts = 0;
}



/*=r=*************************************************************************/
static void m__release_an_ack_pdu (MACHINE *m)
     /* WHAT IT DOES:  Outputs an Ack PDU, 
      *   and performs the actions required after the PDU is released.
      */
{
  TRANS_STATUS        *mp = &(m->publik);   /* useful shorthand */
  /*------------------------------------------------------------*/

  /*----------------*/
  /* Output the PDU */
  /*----------------*/

  aaa__send_ack (m->hdr, m->ack);
  m->is_outgoing_ack_buffered = NO;

  /*----------------------------------------------------*/
  /* Perform actions required after the PDU is released */
  /*----------------------------------------------------*/

  if (m->ack.condition_code == NO_ERROR)
    /* An Ack-EOF-no-error PDU was just sent to our CFDP partner.
     * The transaction remains in-progress, and enters Phase 3
     * (Fill Gaps) unless we have initiated a Cancel.
     */
    {
      if (!m->have_we_initiated_a_cancel)
        mp->phase = 3;
    }

  else
    /* An Ack-EOF-Cancel PDU was just sent to our CFDP partner.
     * The transaction was cancelled by our partner and we acknowledged
     * their cancellation.  Typically, we can shut down now.
     * However, if we have also initiated a Cancel, we can't shut down until
     * our partner Acks our cancel.
     */
    {
      if (m->have_we_initiated_a_cancel)
        /* Keep running until our own Cancel is ack'd. */
        ;
      else
        /* We're done, wrap up and shut down. */
        aaa__transaction_has_finished (m);
    }
}



/*=r=*************************************************************************/
static void m__release_a_fin_pdu (MACHINE *m)
     /* WHAT IT DOES:  Outputs a Finished PDU, 
      *   and performs the actions required after the PDU is released.
      */
{
  TRANS_STATUS        *mp = &(m->publik);   /* useful shorthand */
  /*------------------------------------------------------------*/

  /*----------------*/
  /* Output the PDU */
  /*----------------*/

  aaa__send_finished (m->hdr, m->fin);
  m->is_outgoing_fin_buffered = NO;

  /*----------------------------------------------------*/
  /* Perform actions required after the PDU is released */
  /*----------------------------------------------------*/

  /* We are now in Phase 4 (Shut down) */
  mp->phase = 4;
  
  /* Set up to retransmit this PDU if our partner doesn't Ack it */
  mp->attempts ++;
  m->ack_timer = timer__start (mib__ack_timeout(NULL));
}



/*=r=*************************************************************************/
static void m__release_a_nak_pdu (MACHINE *m)
     /* WHAT IT DOES:  Outputs a Nak PDU, 
      *   and performs the actions required after the PDU is released.
      */
{
  TRANS_STATUS        *mp = &(m->publik);   /* useful shorthand */
  /*------------------------------------------------------------*/

  /*----------------*/
  /* Output the PDU */
  /*----------------*/

  aaa__send_nak (m->hdr, m->nak);
  m->is_outgoing_nak_buffered = NO;

  /* If the gap-list will not fit in one Nak PDU, we will need to
   * spread the gaps across multiple Nak PDUs...
   * <NOT_SUPPORTED>  A gap-list that spans multiple Nak PDUs
   */

  /*----------------------------------------------------*/
  /* Perform actions required after the PDU is released */
  /*----------------------------------------------------*/

  /* Keep track of how many Naks we send */
  mp->how_many_naks ++;

  /* If we are nominal, give our partner nak-timeout seconds to respond */
  if (mp->state == S1)
    {
      mp->attempts ++;
      m->nak_timer = timer__start (mib__nak_timeout(NULL));
    }

  /* If the transaction is being cancelled, don't bother */
  else
    ;
}








/*=r=************************************************************************/
void r2__state_table (MACHINE *m, int event, 
                      PDU_AS_STRUCT *pdu_ptr, REQUEST *req_ptr)
     /* NOTE:  Conceptually, this state table has 2 states:
      *    S1 = Nominal 
      *    S2 = Transaction Cancelled
      * So, the state is always S1 unless the transaction is cancelled.
      * Transactions can be cancelled in 3 ways:
      *    1) the local User can issue a Cancel Request
      *    2) our CFDP partner can send an EOF-Cancel PDU.
      *    3) A fault may occur, and the response to that fault may be
      *       to cancel.
      */
{
  TRANS_STATUS        *mp = &(m->publik);   /* useful shorthand */
  /*------------------------------------------------------------*/

  /* Possibly display some debug info */
  if (cfdp_is_message_class_enabled (CFDP_MSG_STATE_ALL))
    aaa__display_state_and_event ("R2", m->publik.state, event);


  /*-----------------------*/
  /* Respond to each Event */
  /*-----------------------*/

  

  if (event == THROTTLE_OUTGOING_FILE_DIR_PDU)
    /* Release, at most, one outgoing PDU. */
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

      else if (m->is_outgoing_nak_buffered)
        /* We're not frozen or suspended, so we can release any PDU.
         * A Nak is ready.  Do we have the "green light"?
         */
        {
          if (pdu_output__ready (FILE_DIR_PDU, mp->trans, m->hdr.dest_id))
            /* Light is green.  Send it */
            m__release_a_nak_pdu (m);
        }

      else if (m->is_outgoing_fin_buffered)
        /* We're not frozen or suspended, so we can release any PDU.
         * A Finished is ready.  Do we have the "green light"?
         */
        {
          if (pdu_output__ready (FILE_DIR_PDU, mp->trans, m->hdr.dest_id))
            /* Light is green.  Send it */
            m__release_a_fin_pdu (m);
        }
    }



  else if (event == RECEIVED_SUSPEND_REQUEST)
    /* A Suspend Request was received; suspend timers */
    aaa__notice_of_suspension (m);
  


  else if (event == RECEIVED_RESUME_REQUEST)
    /* A Resume Request was received; possibly resume timers */
    {
      /* 'Indication' callback allows engine user to respond to events */
      indication__ (IND_RESUMED, &(m->publik));
      mp->suspended = NO;
      /* Timers remain paused if either 'suspended' or 'frozen' */
      if (!mp->frozen)
        aaa__resume_timers (m);
    }



  else if (event == RECEIVED_CANCEL_REQUEST)
    /* Received a Cancel Request (from our User, not from our partner) */
    {
      /* If the transaction isn't already being cancelled, begin doing so */
      if (mp->state == S1)
        {
          mp->condition_code = CANCEL_REQUEST_RECEIVED;
          m->have_we_initiated_a_cancel = YES;
          /* Cancel both locally... */
          aaa__cancel_locally (m);
          /* ... and remotely (i.e. have our partner cancel too) */
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
    /* A "Freeze" was received, so suspend all timers. */
    {
      mp->frozen = YES;
      aaa__suspend_timers (m);
    }



  else if (event == RECEIVED_THAW_REQUEST)
    /* A "Thaw" was received; possibly resume all timers. */
    {
      mp->frozen = NO;
      /* Timers remain paused if either 'suspended' or 'frozen' */
      if (!mp->suspended)
        aaa__resume_timers (m);
    }



  else if (event == RECEIVED_METADATA_PDU)
    /* Received a Metadata PDU.  This is normally the first PDU we receive
     * but not always.  And its possible that we might receive more than one
     * Metadata pdu; We only respond to the first Metadata we receive.
     */
    {
      /* Ignore PDUs received while cancelling */
      if (mp->state == S2)
        return;

      /* CFDP messages that we send back to our partner will reuse the
       * header that we receive in the first message from them (except 
       * that the "direction" field will be reversed).
       * Note: The first message we receive may be Metadata, Filedata, or EOF.
       */

      /* Our outgoing PDU header will be the same as the first incoming
       * PDU header (but with the 'direction' field reversed).
       */
      if (!m->has_a_pdu_been_received)
        {
          aaa__reuse_senders_first_hdr (pdu_ptr->hdr, m);
          /* 'Indication' callback allows engine user to respond to events */
          indication__ (IND_MACHINE_ALLOCATED, &(m->publik));
          m->has_a_pdu_been_received = YES;
        }

      /* Note:  Metadata may arrive more than once (it shouldn't, but it
       * can).  Respond to the first Metadata and ignore all the others.
       */
      if (!mp->has_md_been_received)
        /* This is the first Metadata PDU for this transaction */
        {
          mp->has_md_been_received = YES;
          mp->md = pdu_ptr->data_field.md;
          /* 'Indication' callback allows engine user to respond to events */
          indication__ (IND_METADATA_RECV, &(m->publik));
  
          /* If this transaction will transfer a file, prepare */
          if (mp->md.file_transfer)
            {
              /* Open a temporary file (unless a file was already opened
               * in response to a Filedata or EOF PDU).
               * Note:  Normally, Metadata is the first PDU received,
               * but that is not guaranteed.
               */
              if (!m->is_there_an_open_file)
                {
                  if (aaa__open_temp_file (m))
                    m->is_there_an_open_file = YES;
                  else
                    {
                      e_msg__ ("cfdp_engine: unable to open a temp file "
                               "(MD).\n");
                      if (!m__fault_handler (m, FILESTORE_REJECTION))
                        return;
                    }
                }
            }

          /* Update the Nak-list to indicate that Metadata has been received */
          nak__metadata_sent_or_received (&(m->nak));

          /* <NOT_SUPPORTED> Metadata TLVs (processed here) */

          /* Protocol Optimization:  Don't wait for the Nak-timer to expire
           * before checking to see if retransmission is complete.
           */
          if (m->has_eof_been_received)
            {
              if (nak__is_list_empty (&m->nak))
                /* All data has been received; begin nominal Finish */
                m__all_data_has_been_received (m);
            }
        }
    }
  


  else if (event == RECEIVED_FILEDATA_PDU)
    /* Received a Filedata Pdu.  Nominally, we store it in a file, and 
     * update our bookkeeping.  But if the transaction is being cancelled,
     * don't bother.
     */
    {
      /* Ignore PDUs received while cancelling */
      if (mp->state == S2)
        return;
      
      /* Our outgoing PDU header will be the same as the first incoming
       * PDU header (but with the 'direction' field reversed).
       */
      if (!m->has_a_pdu_been_received)
        {
          aaa__reuse_senders_first_hdr (pdu_ptr->hdr, m);
          /* 'Indication' callback allows engine user to respond to events */
          indication__ (IND_MACHINE_ALLOCATED, &(m->publik));
          m->has_a_pdu_been_received = YES;
        }

      /* If we haven't already opened a file to receive the data, do so now */
      if (!m->is_there_an_open_file)
        {
          if (aaa__open_temp_file (m))
            m->is_there_an_open_file = YES;
          else
            {
              e_msg__ ("cfdp_engine: unable to open a temp file (FD).\n");
              if (!m__fault_handler (m, FILESTORE_REJECTION))
                return;
            }
        }
      
      /* Save the File-data in the temporary file */
      mp->fd_offset = pdu_ptr->data_field.fd.offset;
      mp->fd_length = pdu_ptr->data_field.fd.buffer_length;
      if (mib__issue_file_segment_recv())
        /* 'Indication' callback allows engine user to respond to events */
        indication__ (IND_FILE_SEGMENT_RECV, &(m->publik));
      aaa__store_file_data (m->fp, &pdu_ptr->data_field.fd);

      /* Update the received file size */
      if (mp->fd_offset + mp->fd_length > mp->received_file_size)
        /* This FD has made the file bigger than before */
        mp->received_file_size = mp->fd_offset + mp->fd_length;

      /* If this Filedata segment exposes a gap, add gap to nak-list.
       * (the 'nak' module takes care of all this bookkeeping stuff)
       */
      nak__data_received (&m->nak, mp->fd_offset, 
                          mp->fd_offset + mp->fd_length);

      /* If this Filedata PDU arrived *after* the EOF PDU (typically, not
       * the case), check for a "file size error".
       */
      if (m->has_eof_been_received)
        /* A "file_size_error" means that the received file-data cannot be
         * stored without extending the file's length beyond the length
         * specified in the EOF PDU.
         */
        if (!aaa__is_file_size_valid (m))
          if (!m__fault_handler (m, FILE_SIZE_ERROR))
            return;

      /* Protocol Optimization:  Don't wait for the Nak-timer to expire
       * before checking to see if retransmission is complete.
       */
      if (m->has_eof_been_received)
        {
          if (nak__is_list_empty (&m->nak))
            /* All data has been received; begin nominal Finish */
            m__all_data_has_been_received (m);
        }
    }
  


  else if (event == RECEIVED_EOF_NO_ERROR_PDU)
    /* Received an EOF-no-error Pdu.  This tells us that the Sender has
     * sent all data once.  We always Ack this pdu.  Nominally, we
     * take stock and respond with either a Nak or Finished PDU.  However,
     * if the transaction is being cancelled, don't bother.
     */
    {

      /*-------------------------------*/
      /* Always acknowledge an EOF PDU */
      /*-------------------------------*/

      /* Save a copy of the EOF */
      m->eof = pdu_ptr->data_field.eof;

      /* Our outgoing PDU header will be the same as the first incoming
       * PDU header (but with the 'direction' field reversed).
       */
      aaa__reuse_senders_first_hdr (pdu_ptr->hdr, m);

      /* Build and queue an Ack of the EOF (every EOF must be acked) */
      m->ack = aaa__build_ack_eof (m);
      m->is_outgoing_ack_buffered = YES;

      /*---------------------------------------*/
      /* Ignore PDUs received while cancelling */
      /*---------------------------------------*/
      if (mp->state == S2)
        return;
      
      /*--------------------------------------------------*/
      /* The rest of this logic only executes in state S1 */
      /*--------------------------------------------------*/

      /* We are now in Phase 2 (Handoff) */
      mp->phase = 2;

      /* Issue the Machine-Allocated Indication once per transaction */
      if (!m->has_a_pdu_been_received)
        {
          /* 'Indication' callback allows engine user to respond to events */
          indication__ (IND_MACHINE_ALLOCATED, &(m->publik));
          m->has_a_pdu_been_received = YES;
        }

      /* If configured to do so, issue an Indication to the user */
      if (mib__issue_eof_recv())
        indication__ (IND_EOF_RECV, &(m->publik));

      /* If this is the *first* EOF received for this transaction, then
       * additional actions must be taken; otherwise, it can be ignored.
       * Why would a second EOF be sent if we received the first one?
       * Because our Ack of the first EOF was dropped.
       */
      if (!m->has_eof_been_received)
        {
          m->has_eof_been_received = YES;

          /* Update the Nak-list (because this EOF may expose a file gap
           * beyond the highest Filedata offset received -- for example,
           * we received Filedata for offset 0 through 900, and the EOF
           * tells us the file is 1000 bytes long, so we are missing
           * filedata 900-1000).
           * (Again, the 'nak' module takes care of this bookkeeping stuff)
           */
          nak__set_end_of_scope (&m->nak, m->eof.file_size);
          
          /* Verify that the file-size given in the EOF is valid.  For 
           * example, if we received Filedata for offset 0 through 1100,
           * and the EOF tells us the file is 1000 bytes long, the file size
           * is invalid.
           */
          if (!aaa__is_file_size_valid (m))
            if (!m__fault_handler (m, FILE_SIZE_ERROR))
              return;

          if (nak__is_list_empty (&m->nak))
            /* All data has been received; begin nominal Finish */
            m__all_data_has_been_received (m);
          else
            /* Get the missing data (queue up our first Nak) */
            m->is_outgoing_nak_buffered = YES;
        }
    }
  


  else if (event == RECEIVED_EOF_CANCEL_PDU)
    /* Received an EOF-Cancel Pdu.  We always Ack this pdu.  If we have
     * initiated a Cancel, we have to wait for an Ack of our own cancel;
     * otherwise, we shut down after sending the Ack.
     */
    {
      /* Save a copy of the EOF PDU */
      m->eof = pdu_ptr->data_field.eof;

      /* Our outgoing PDU header will be the same as the first incoming
       * PDU header (but with the 'direction' field reversed).
       */
      if (!m->has_a_pdu_been_received)
        {
          aaa__reuse_senders_first_hdr (pdu_ptr->hdr, m);
          /* 'Indication' callback allows engine user to respond to events */
          indication__ (IND_MACHINE_ALLOCATED, &(m->publik));
          m->has_a_pdu_been_received = YES;
        }

      /* If we haven't already cancelled locally, accept partner's request */
      if (mp->state == S1)
        {
          aaa__cancel_locally (m);
          mp->condition_code = m->eof.condition_code;
          mp->phase = 4;
          m__move_to_state_s2 (m);
        }

      /* Regardless, we must acknowledge receipt of the EOF PDU */
      m->ack = aaa__build_ack_eof (m);
      m->is_outgoing_ack_buffered = YES;
    }



  else if (event == RECEIVED_ACK_FIN_NO_ERROR_PDU)
    /* Received Ack of Finished-no-error Pdu.  Nominally, we are done and
     * can shut down immediately.  However, if we have initiated a cancel,
     * we have to wait for an Ack of our Finished-Cancel PDU.
     */
    {
      if (mp->state == S1)
        /* This is the nominal situation */
        {
          if (mp->delivery_code == DATA_COMPLETE)
            /* We're done; we can shut down now */
            aaa__transaction_has_finished (m);
          else
            /* Why is our partner sending an Ack-Finished when we haven't
             * sent a Finished?
             */
            w_msg__ ("cfdp_engine: protocol violation: "
                     "ack of non-existent Finished-no-error.\n");
        }

      else
        /* Transaction is already being cancelled, so ignore this pdu */
        ;
    }



  else if (event == RECEIVED_ACK_FIN_CANCEL_PDU)
    /* Received Ack of Finished-Cancel Pdu.  If we have initiated a cancel,
     * this completes the transaction, and we can shut down immediately.
     * Otherwise, our partner has violated the protocol.
     */
    {
      if (mp->state == S1)
        /* Protocol violation - we didn't send a Finished-Cancel,
         * but our partner has  acknowledged receipt.
         */
        w_msg__ ("cfdp_engine: protocol violation: "
                 "ack of non-existent Finish-Cancel.\n");

      else
        /* Transaction is being cancelled */
        {
          /* This is the nominal path when a Cancel is initiated from our
           * side.  We sent a Finished-Cancel, and partner ack'd it, so
           * we can shut down this state machine.
           */
          aaa__transaction_has_finished (m);
        }                
    }



  else if (event == ACK_TIMER_EXPIRED)
    /* Ack-timer expired.  Resend the Fin Pdu (if permitted). */
    {
      /* 'Indication' callback allows engine user to respond to events */
      indication__ (IND_ACK_TIMER_EXPIRED, &(m->publik));

      if (mp->state == S1)
        /* This is the nominal situation */
        {
          /* If we've already sent the Fin PDU as many times as allowed,
           * then declare a Fault.
           */
          if (mp->attempts >= mib__ack_limit (NULL))
            /* Let the Fault mechanism decide what to do */
            if (!m__fault_handler (m, POSITIVE_ACK_LIMIT_REACHED))
              return;

          /* Try again -- Re-queue the Fin for metered release */
          m->is_outgoing_fin_buffered = YES;
        }

      else
        /* Transaction is being cancelled */
        {
          /* If we've already sent the Fin-Cancel PDU as many times as
           * allowed, the only choice is to give up (abandon the transaction).
           */
          if (mp->attempts >= mib__ack_limit (NULL))
            /* "Positive Ack Limit" has been reached; give up */
            aaa__abandon_this_transaction (m);
          else
            /* Try again -- Re-queue the Fin for metered release */
            m->is_outgoing_fin_buffered = YES;
        }
    }



  else if (event == NAK_TIMER_EXPIRED)
    /* Nak-timer expired.  If all data has been received, we move on to
     * phase 4 (send a Finished-No-Error PDU).  If not, send another
     * Nak PDU (if permitted).
     */
    {
      /* 'Indication' callback allows engine user to respond to events */
      indication__ (IND_NAK_TIMER_EXPIRED, &(m->publik));

      /* We should always be in state S1 when the Nak-timer expires, but
       * if we are not, then ignore it.
       */
      if (mp->state == S2)
        return;

      /* Every time the Nak-timer expires we check to see whether we 
       * have received all the requested retransmissions.
       */
      if (nak__is_list_empty (&m->nak))
        /* All data has been received; begin nominal Finish */
        m__all_data_has_been_received (m);
      else
        /* Still some data missing, so send another Nak (if permitted) */
        {
          /* If we've already sent the Nak PDU as many times as allowed,
           * then declare a Fault.
           */
          if (mp->attempts >= mib__nak_limit (NULL))
            /* Let the Fault mechanism decide what to do */
            if (!m__fault_handler (m, NAK_LIMIT_REACHED))
              return;

          /* Queue another outgoing Nak */
          m->is_outgoing_nak_buffered = YES;
        }
    }
  


  else if (event == INACTIVITY_TIMER_EXPIRED)
    /* Inactivity-timer expired */
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
        /* Transaction is being cancelled; give up */
        aaa__abandon_this_transaction (m);
    }



  else 
    /* Ignore any other events */
    w_msg__ ("cfdp_engine: ignored event '%s' in state %s "
             "for trans '%s' <R2>.\n",
             event__event_as_string(event), mp->state, 
             cfdp_trans_as_string (mp->trans));
}
