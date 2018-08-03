/* FILE: r1.c   CFDP Class 1 Receiver.
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
 * SPECS: r1.h
 * ORIGINAL PROGRAMMER:  Tim Ray 301-286-0581
 * CHANGES:
 *   2007_07_31 Tim Ray (TR)
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 *   2007_11_05 TR
 *     - Moved the 'retain temp file' logic to aaa.c.
 *   2007_11_08 TR
 *     - Enhancement.  A "list of gaps" is now maintained.
 *     - "What was I thinking?"  Previously, if the Metadata PDU was missed
 *       then all File-data was discarded.  Now, the File-data is stored.
 *       (The temp-file must be opened first)
 *   2007_11_09 TR
 *     - Better portability.  C++ uses 'public' as a keyword, so any use of
 *       'public' as a variable name was changed to 'publik'.
 *   2007_12_06 TR
 *     - Enhancement.  Message-class filtering can now be changed on-the-fly.
 *   2007_12_07 TR
 *     - Added error messages to help explain why filestore-rejection occurs.
 *   2008_01_02 TR
 *     - Made assertions safer (avoid crashing if program continues).
 *   2008_01_24 TR
 *     - Bug-fix: don't attempt to remove a non-existent file.  This module
 *       now clears the variable 'is_there_a_temp_file' if the temp-file is
 *       successfully renamed to its final destination.
 */

#include "cfdp.h"
#include "cfdp_private.h"

#define NO 0
#define YES 1




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
      /* We never give feedback to our partner, so just wrap up. */
      aaa__transaction_has_finished (m);
    } 

  else if (mib__response (fault) == RESPONSE_SUSPEND) 
    aaa__notice_of_suspension (m);

  else if (mib__response (fault) == RESPONSE_IGNORE) 
    ok_for_caller_to_continue = YES;

  else if (mib__response (fault) == RESPONSE_ABANDON) 
    aaa__abandon_this_transaction (m); 

  else 
    e_msg__ ("cfdp_engine: problem with the Fault mechanism in <R1>.\n");
  
  return (ok_for_caller_to_continue);
}





/*=r=************************************************************************/
void r1__state_table (MACHINE *m, int event,
                      PDU_AS_STRUCT *pdu_ptr, REQUEST *req_ptr)
     /* NOTE:  Conceptually, this state table has 2 states:
      *    S1 = Waiting for a Metadata PDU to arrive
      *    S2 = Waiting for an EOF PDU to arrive
      */
{
  TRANS_STATUS      *mp = &(m->publik);   /* useful shorthand */
  /*------------------------------------------------------------*/
  
  /* Possibly display some debug info */
  if (cfdp_is_message_class_enabled (CFDP_MSG_STATE_ALL))
    aaa__display_state_and_event ("R1", m->publik.state, event);
  

  /*------------------------------------------------------------*/
  /* Here is the logic for responding to each Event in State S1 */
  /*------------------------------------------------------------*/
  

  if (mp->state == S1)
    /* In state S1, we are waiting for a Metadata PDU.  As soon as a
     * Metadata PDU is received, we move to state S2.  Normally, the
     * transition to state S2 happens quickly because the first PDU 
     * received is typically Metadata.  However, there are many other
     * possibilities, and each of these is captured as an event below.
     */
    {



      if (event == RECEIVED_CANCEL_REQUEST)
        /* Received a Cancel Request, meaning that someone/something
         * on *our* end (as opposed to our partner) decided to cancel 
         * this transaction.
         */
        {
          mp->condition_code = CANCEL_REQUEST_RECEIVED;
          aaa__cancel_locally (m); 
          /* We never give feedback to our partner, so just wrap up. */
          aaa__transaction_has_finished (m);
        }


      
      else if (event == RECEIVED_ABANDON_REQUEST)
        /* Abandon this transaction */
        aaa__abandon_this_transaction (m);



      else if (event == RECEIVED_REPORT_REQUEST)
        /* Received a Report Request */
        indication__ (IND_REPORT, &(m->publik));


      
      else if (event == RECEIVED_METADATA_PDU)
        /* Received Metadata Pdu.   NOMINAL PATH STARTS HERE... */
        {
          /* Store both the pdu-header and the MD data-field */
          m->hdr = pdu_ptr->hdr;
          mp->trans = m->hdr.trans;
          mp->partner_id = mp->trans.source_id;
          ASSERT__ (pdu_ptr->dir_code == MD_PDU);
          mp->md = pdu_ptr->data_field.md;
          mp->has_md_been_received = YES;

          /* Update the Nak-list to indicate that Metadata has been received */
          nak__metadata_sent_or_received (&(m->nak));

          /* Issue the Machine-Allocated Indication once per transaction */
          if (!m->has_a_pdu_been_received)
            {
              indication__ (IND_MACHINE_ALLOCATED, &(m->publik));
              m->has_a_pdu_been_received = YES;
            }

          /* 'Indication' callback allows engine user to respond to events */
          indication__ (IND_METADATA_RECV, &(m->publik));

          /* If a file is to be transferred, open a temp file to hold the
           * incoming file-data.
           */
          if (mp->md.file_transfer)
            {
              if (aaa__open_temp_file (m))
                m->is_there_an_open_file = YES;
              else
                {
                  e_msg__ ("cfdp_engine: unable to open a temp file (MD).\n");
                  if (!m__fault_handler (m, FILESTORE_REJECTION))
                    return;
                }
            }

          /* <NOT_SUPPORTED> Metadata TLVs */

          /* Once we receive a Metadata PDU, we move to state S2 */
          mp->state = S2;
          if (cfdp_is_message_class_enabled (CFDP_MSG_STATE_CHANGE))
            d_msg__ ("<R1>  (S2) 'Wait for an EOF Pdu'\n");
        }


      
      else if (event == RECEIVED_FILEDATA_PDU)
        /* Received File-data Pdu (before Metadata) */
        {
          /* Save the header info */
          m->hdr = pdu_ptr->hdr;
          mp->trans = m->hdr.trans;
          mp->partner_id = mp->trans.source_id;

          /* Issue the Machine-Allocated Indication once per transaction */
          if (!m->has_a_pdu_been_received)
            {
              indication__ (IND_MACHINE_ALLOCATED, &(m->publik));
              m->has_a_pdu_been_received = YES;
            }

          /* "Accept" (copy) the received Pdu */
          ASSERT__ (pdu_ptr->is_this_a_file_data_pdu);
          mp->fd_offset = pdu_ptr->data_field.fd.offset;
          mp->fd_length = pdu_ptr->data_field.fd.buffer_length;
          
          if (mib__issue_file_segment_recv())
            indication__ (IND_FILE_SEGMENT_RECV, &(m->publik));

          /* Open a temp file (once) to hold the incoming file-data. */
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

          /* Store the incoming file-data */
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
        }


      
      else if (event == RECEIVED_EOF_NO_ERROR_PDU)
        /* Received EOF Pdu (before Metadata); shut down */
        {
          m->hdr = pdu_ptr->hdr;
          mp->trans = m->hdr.trans;
          mp->partner_id = mp->trans.source_id;

          /* Issue the Machine-Allocated Indication once per transaction */
          if (!m->has_a_pdu_been_received)
            {
              indication__ (IND_MACHINE_ALLOCATED, &(m->publik));
              m->has_a_pdu_been_received = YES;
            }

          i_msg__ ("cfdp_engine: Class 1 Receiver finished, "
                   "but never received Metadata.\n");
          aaa__transaction_has_finished (m);
        }


      
      else if (event == RECEIVED_EOF_CANCEL_PDU)
        /* Received EOF-Cancel Pdu, meaning our partner has cancelled 
         * this transaction.
         */
        {
          mp->cancelled = YES;

          /* Copy the Condition Code, which tells us *why* they cancelled */
          mp->condition_code = 
            pdu_ptr->data_field.eof.condition_code;

          /* Issue the Machine-Allocated Indication once per transaction */
          if (!m->has_a_pdu_been_received)
            {
              indication__ (IND_MACHINE_ALLOCATED, &(m->publik));
              m->has_a_pdu_been_received = YES;
            }
          
          /* Wrap up and shut down */
          aaa__transaction_has_finished (m);
        }

      

      else if (event == INACTIVITY_TIMER_EXPIRED)
        /* Inactivity-timer expired */
        {
          m->inactivity_timer = timer__start 
            (mib__inactivity_timeout(NULL));
          if (!m__fault_handler (m, INACTIVITY_DETECTED))
            return;
        }


      
      else
        w_msg__ ("cfdp_engine: ignored event '%s' in state S1 "
                 "for trans '%s' <R1>.\n",
                 event__event_as_string(event),
                 cfdp_trans_as_string (mp->trans));
    }
  


  
  /*------------------------------------------------------------*/
  /* Here is the logic for responding to each Event in State S2 */
  /*------------------------------------------------------------*/
  
  else if (mp->state == S2)
    /* Wait for an EOF Pdu */
    {



      if (event == RECEIVED_CANCEL_REQUEST)
        /* Received a Cancel Request */
        {
          mp->condition_code = CANCEL_REQUEST_RECEIVED;
          aaa__cancel_locally (m); 
          /* We never give feedback to our partner, so just wrap up. */
          aaa__transaction_has_finished (m);
        }
      


      else if (event == RECEIVED_ABANDON_REQUEST)
        /* Abandon this transaction */
        aaa__abandon_this_transaction (m);



      else if (event == RECEIVED_REPORT_REQUEST)
        /* Received a Report Request */
        indication__ (IND_REPORT, &(m->publik));


      
      else if (event == RECEIVED_FILEDATA_PDU)
        /* Received a File-data Pdu; store the file data. ...NOMINAL PATH... */
        {
          /* "Accept" (copy) the received Pdu */
          ASSERT__ (pdu_ptr->is_this_a_file_data_pdu);
          mp->fd_offset = pdu_ptr->data_field.fd.offset;
          mp->fd_length = pdu_ptr->data_field.fd.buffer_length;
          
          /* The rest matches the published state table */
          if (mp->md.file_transfer)
            {
              if (mib__issue_file_segment_recv())
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
            }            
          else
            /* Our CFDP partner violated the CFDP protocol.  Their Metadata
             * PDU told us that there would be no file transfer.
             */
            d_msg__ ("cfdp_engine: Class 1 Receiver discarded Filedata "
                     "because Metadata indicated 'no file transfer'.\n");
        }


      
      else if (event == RECEIVED_EOF_NO_ERROR_PDU)
        /* Received EOF Pdu.  ...NOMINAL PATH ENDS HERE. */
        {
          /* Store the EOF Pdu */
          m->eof = pdu_ptr->data_field.eof;
          if (mib__issue_eof_recv())
            indication__ (IND_EOF_RECV, &(m->publik));
          
          if (mp->md.file_transfer)
            {
              /* Close the temporary file */
              ASSERT__ (m->fp != NULL);
              if (m->fp == NULL)
                /* This should never happen, but if it does, avoid crashing */
                ;
              else
                fclose_callback (m->fp);
              m->is_there_an_open_file = NO;

              if (!aaa__is_file_size_valid (m))
                {
                  if (!m__fault_handler (m, FILE_SIZE_ERROR))
                    return;
                }
              if (!aaa__is_file_checksum_valid (m))
                {
                  if (!m__fault_handler (m, FILE_CHECKSUM_FAILURE))
                    return;
                }
              mp->delivery_code = DATA_COMPLETE;

              /* Move the temporary file to the real destination */
              if (!rename_callback (mp->temp_file_name, mp->md.dest_file_name))
                /* Success */
                m->is_there_a_temp_file = NO;
              else
                {
                  e_msg__ ("cfdp_engine: unable to rename '%s' to '%s'.\n",
                           mp->temp_file_name, mp->md.dest_file_name);
                  e_msg__ ("cfdp_engine: are they on separate filesystems?\n");
                  if (!m__fault_handler (m, FILESTORE_REJECTION))
                    return;
                }

              /* <NOT_SUPPORTED> Filestore Request execution */
            }  

          /* Wrap up and shut down */
          aaa__transaction_has_finished (m);
        }


      
      else if (event == RECEIVED_EOF_CANCEL_PDU)
        /* Received EOF-Cancel Pdu */
        {
          mp->cancelled = YES;

          mp->condition_code = 
            pdu_ptr->data_field.eof.condition_code;
          /* Give some feedback to the User, and shut down */
          aaa__transaction_has_finished (m);
        }


      
      else if (event == INACTIVITY_TIMER_EXPIRED)
        /* Inactivity-timer expired */
        {
          m->inactivity_timer = timer__start 
            (mib__inactivity_timeout (NULL));
          if (!m__fault_handler (m, INACTIVITY_DETECTED))
            return;
        }
      


      else
        w_msg__ ("cfdp_engine: ignored event '%s' in state S2 "
                 "for trans '%s' <R1>.\n",
                 event__event_as_string(event),
                 cfdp_trans_as_string (mp->trans));
    }
  

  
  else 
    /* This shouldn't happen; report it */
    w_msg__ ("cfdp_engine: ignored event '%s' in trans '%s' "
             "because state is flaky (%u) <R1>.\n",
             event__event_as_string(event),
             cfdp_trans_as_string (mp->trans), mp->state);
}
