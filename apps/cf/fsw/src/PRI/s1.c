/* FILE: s1.c  CFDP Class 1 Sender.
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
 * SPECS:  s1.h
 * ORIGINAL PROGRAMMER:  Tim Ray 301-286-0581
 * CHANGES:
 *   2007_07_31 Tim Ray (TR)
 *     - Official release.  (see pre_release_changes.log for earlier changes)
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
 *   2007_12_06 TR
 *     - Enhancement.  Message-class filtering can now be changed on-the-fly.
 *   2007_12_07 TR
 *     - Added error messages to help explain why filestore-rejection occurs.
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
  m->eof = aaa__build_eof (mp->condition_code, 0, 0);
  m->is_outgoing_eof_buffered = YES;
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
    } 

  else if (mib__response (fault) == RESPONSE_SUSPEND) 
    aaa__notice_of_suspension (m);

  else if (mib__response (fault) == RESPONSE_IGNORE) 
    ok_for_caller_to_continue = YES;

  else if (mib__response (fault) == RESPONSE_ABANDON) 
    aaa__abandon_this_transaction (m); 

  else 
    e_msg__ ("cfdp_engine: problem with the Fault mechanism in <S1>.\n");
  
  return (ok_for_caller_to_continue);
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

  /* 'Indication' callback allows engine user to respond to events */
  indication__ (IND_METADATA_SENT, &(m->publik));

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
      
      /* Queue all the Filedata for metered released */
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
      /* Build and queue an EOF Pdu */
      m->eof = aaa__build_eof (mp->condition_code, 0, 0);
      m->is_outgoing_eof_buffered = YES;
    }
}
  
  

/*=r=*************************************************************************/
static void m__release_an_eof_pdu (MACHINE *m)
     /* WHAT IT DOES:  Outputs an EOF PDU, 
      *   and performs the actions required after the PDU is released.
      */
{
  /*------------------------------------------------------------*/

  /*----------------*/
  /* Output the PDU */
  /*----------------*/

  aaa__send_eof (m->hdr, m->eof);
  m->is_outgoing_eof_buffered = NO;

  /*----------------------------------------------------*/
  /* Perform actions required after the PDU is released */
  /*----------------------------------------------------*/

  /* If configured to do so, let the user know what has happened */
  if (mib__issue_eof_sent())
    indication__ (IND_EOF_SENT, &(m->publik));
  
  /* For a Class 1 transaction, there is no feedback, so we are done */
  aaa__transaction_has_finished (m);
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

  /* Update the list of data to be sent (bookkeeping) */
  nak__data_sent (&(m->nak), fd->offset, fd->offset + fd->buffer_length);
  
  /* If the entire file has been sent, build and queue an EOF PDU */
  if (nak__how_many_filedata_gaps (&(m->nak)) == 0)
    {
      m->eof = aaa__build_eof (mp->condition_code,
                               mp->file_checksum_as_calculated,
                               mp->md.file_size);
      m->is_outgoing_eof_buffered = YES;
    }
}








/*=r=************************************************************************/
void s1__state_table (MACHINE *m, int event,
                      PDU_AS_STRUCT *pdu_ptr, REQUEST *req_ptr)
     /* NOTE:  Why is there no mention of 'state' in this routine?
      *   Although the state tables originally used two states for the
      *   Class 1 Sender (as published in a CCSDS Green Book for CFDP),
      *   the logic has since been rearranged so that only one state
      *   is necessary.
      */
{
  static FD          fd;
  TRANS_STATUS      *mp = &(m->publik);   /* useful shorthand */
  /*------------------------------------------------------------*/
  
  /* Possibly display some debug info */
  if (cfdp_is_message_class_enabled (CFDP_MSG_STATE_ALL))
    aaa__display_state_and_event ("S1", m->publik.state, event);


  /*-----------------------*/
  /* Respond to each Event */
  /*-----------------------*/



  if (event == THROTTLE_OUTGOING_FILE_DIR_PDU)
    /* Release, at most, one outgoing File Directive PDU */
    {
      if (mp->frozen || mp->suspended)
        /* We can't release any PDUs while frozen or suspended */
        ;

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
          if (pdu_output__ready (FILE_DATA_PDU, mp->trans, m->hdr.dest_id))
            /* Light is green. Send it. */
            m__release_a_file_data_pdu (m, &fd);
        }
    }



  else if (event == RECEIVED_PUT_REQUEST)
    /* Received a Put Request */
    {
      /* Generate a Metadata pdu */
      aaa__build_metadata_from_put_req (*req_ptr, &m->hdr, &mp->md);

      /* Store some Header info for public viewing */
      mp->trans = m->hdr.trans;
      mp->partner_id = req_ptr->info.put.dest_id;

      /* 'Indication' callback allows engine user to respond to events */
      indication__ (IND_TRANSACTION, &(m->publik));
      indication__ (IND_MACHINE_ALLOCATED, &(m->publik));
      
      /* Queue the Metadata PDU for output */
      m->is_outgoing_md_buffered = YES;
    }



  else if (event == RECEIVED_SUSPEND_REQUEST)
    /* A Suspend Request was received; suspend */
    aaa__notice_of_suspension (m);



  else if (event == RECEIVED_RESUME_REQUEST)
    /* A Resume Request was received; resume */
    {
      /* 'Indication' callback allows engine user to respond to events */
      indication__ (IND_RESUMED, &(m->publik));
      mp->suspended = NO;
    }



  else if (event == RECEIVED_CANCEL_REQUEST)
    /* A Cancel Request was received; cancel the transaction */
    {
      mp->cancelled = YES;
      mp->condition_code = CANCEL_REQUEST_RECEIVED;
      m__ask_partner_to_cancel (m);
    }


  
  else if (event == RECEIVED_ABANDON_REQUEST)
    /* Abandon this transaction */
    aaa__abandon_this_transaction (m);



  else if (event == RECEIVED_REPORT_REQUEST)
    /* A Report Request was received; issue a Report */
    indication__ (IND_REPORT, &(m->publik));


  
  else if (event == RECEIVED_FREEZE_REQUEST)
    /* A "Freeze" was received, so Freeze */
    mp->frozen = YES;


  
  else if (event == RECEIVED_THAW_REQUEST)
    /* A "Thaw" was received, so Thaw */
    mp->frozen = NO;



  else if (event == EXTERNAL_FILE_TRANSFER_COMPLETED)
    /* This event was added in May 2007 to support high-speed (external)
     * file transfer by the engine user.  When this event fires, we know
     * that the engine user has completed sending the first round of
     * Filedata PDUs.  Therefore, we can send the EOF PDU.
     */
    {
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
    w_msg__ ("cfdp_engine: ignored event '%s' for trans '%s' <S1>.\n",
             event__event_as_string(event),
             cfdp_trans_as_string (mp->trans));
}
