/* FILE: aaa.c   Action routines for CFDP state tables.
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
 * SPECS: aaa.h
 * ORIGINAL PROGRAMMER:  Tim Ray 301-286-0581
 * CHANGES:
 *   2007_07_31 Tim Ray (TR)
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 *   2007_11_01 TR
 *     - Updated 'aaa__display_state_and_event' to screen out *both* of
 *       the 'throttle' events.   (part of a bug fix)
 *   2007_11_05 TR
 *     - Made a small change to the way that Receiver state machines save
 *       incomplete temp files.  The old way had 5 places where this decision
 *       was made.  The new way has only 2 places:  transaction-finished and
 *       transaction-abandoned.  
 *   2007_11_09 TR
 *     - Better portability.  C++ uses 'public' as a keyword, so any use of
 *       'public' as a variable name was changed to 'publik'.
 *   2007_11_19 TR
 *     - Bug fix.  'memset' was misused in 2 places (args out of sequence).
 *   2007_12_03 TR
 *     - Bug fix.  'aaa__initialize' now initializes the 'direction' field
 *       in the pdu-header to 'toward_receiver' if our role in the transaction
 *       is 'sender'.  Previously, the 'direction' field was not set.
 *   2007_12_06 TR
 *     - Enhancement.  Message-class filtering can now be changed on-the-fly.
 *   2007_12_10 TR
 *     - Better coverage of all the cases in which the Receiver's temp-file
 *       should be saved.  A new boolean flag ('is_a_file_being_received')
 *       catches *all* the cases in which a file is being received (and,
 *       therefore, a temp file exists).
 *   2008_01_02 TR
 *     - Made assertions safer (avoid crashing if program continues).
 *   2008_01_24 TR
 *     - Used a public status field rather than a private status field
 *       (to indicate a transaction that exists solely to re-send an Ack-Fin).
 *     - Bug-fix: don't attempt to remove a non-existent file.
 *       This fix included cleaning up some code (much simpler logic now).
 *       Got rid of a few 'machine' variables and one local subroutine
 *       ('m__perhaps_retain_temp_file').
 *   2008_03_03 TR
 *     - Removed an 'fflush (stdout)' near line 1000.
 */

/* ***** FILE CHECKSUM *****
 * Background:  The file checksum is not your typical checksum.  The 
 *   typical checksum is one byte long and is an exclusive-or of all the
 *   data bytes.  The file checksum is 4 bytes long.  It is calculated by
 *   adding together all the bytes in a file using a 4-byte-wide addition.
 * Strategy used here:  For outgoing files, the file checksum calculation
 *   is performed on one chunk of file-data at a time AS PART OF THE
 *   INITIAL TRANSMISSION OF THE FILE.  This strategy uses much less
 *   system resources than performing the calculation in a SEPARATE READ
 *   of the whole file, but the algorithm is more complicated...
 * Complication:  The code here gets complicated because there is no
 *   guarantee that each chunk of file-data will be a multiple of 4 bytes
 *   in length. So, here are 2 examples.  The first illustrates the
 *   conceptual calculation.  The second shows how it is actually 
 *   performed in this module.  Both examples use a file that is 12 bytes
 *   long containing a "ramp" pattern (byte 1 is "01", byte 2 is "02", etc).
 * Example 1 -- Every chunk of filedata is a multiple of 4 bytes (this allows
 *   the checksum calculation to be simple):
 *       00 00 00 00   // Initial checksum value
 *       01 02 03 04   // First 4 bytes of the file
 *     + -----------
 *       01 02 03 04   // Interim checksum result
 *       05 06 07 08   // Second 4 bytes of the file
 *     + -----------   // 
 *       06 08 0A 0C   // Interim checksum result
 *       09 0A 0B 0C   // Final 4 bytes of the file
 *     + -----------
 *       0F 12 15 18   // Final checksum result
 * Example 2 -- Chunks of filedata are not a multiple of 4 bytes (in this
 *   example, every chunk is 6 bytes long):
 *       00 00 00 00   // Initial checksum value
 *       01 02 03 04   // first 4 bytes of file-data
 *       05 06 00 00   // 5th and 6th byte of file plus zero-fill to *right*
 *     + -----------
 *       06 08 03 04   // interim checksum result
 *       00 00 07 08   // 7th and 8th byte of file plus zero-fill to *left*
 *       09 0A 0B 0C   // Final 4 bytes of the file
 *     + -----------
 *       0F 12 15 18   // Final checksum result.
 * Summary:  This module does as much of the checksum calculation as it 
 *   can in 4-byte operations.  It may handle a few bytes at the beginning
 *   and/or end of a chunk of file-data as a special case as shown in the
 *   above examples.
 */

#include <netinet/in.h>
#include "cfdp.h"
#include "cfdp_private.h"

#define  NO  0
#define  YES  1




/*=r=************************************************************************/
ID m__partner_id (HDR hdr)
     /* WHAT IT DOES:  Given a PDU-header from an outgoing PDU, this routine
      *   determines the ID of the destination entity (and returns the ID).
      */
{
  ID     partner_id;
  /*------------------------------------------------------------*/

  if (cfdp_are_these_ids_equal (hdr.trans.source_id, mib__get_my_id()))
    /* Since I'm the source, the partner must be the destination */
    partner_id = hdr.dest_id;
  else
    /* Since I'm not the source, I must be the destination.  Therefore,
     * the partner must be the source.
     */
    partner_id = hdr.trans.source_id;
  return (partner_id);
}


/*=r=*************************************************************************/
void aaa__abandon_this_transaction (MACHINE *m)
{
  /*------------------------------------------------------------*/

  /* Update our bookkeeping */
  m->publik.abandoned = YES;
  m->publik.finished = YES;
  m->publik.final_status = FINAL_STATUS_ABANDONED;

  /* Let the user know about this */
  indication__ (IND_ABANDONED, &(m->publik));

  /* Shut down this state machine */
  aaa__shutdown (m);
}



/*=r=************************************************************************/
ACK aaa__build_ack_eof (MACHINE *m)
{
  ACK             ack;
  /*------------------------------------------------------------*/

  ack.directive_code = EOF_PDU;
  ack.directive_subtype_code = 0;
  ack.condition_code = m->eof.condition_code;
  ack.delivery_code = m->publik.delivery_code;
  ack.transaction_status = TRANS_ACTIVE;
  return (ack);
}



/*=r=************************************************************************/
ACK aaa__build_ack_fin (MACHINE *m)
{
  ACK           ack;
  /*------------------------------------------------------------*/

  ack.directive_code = FIN_PDU;
  ack.directive_subtype_code = 0;
  ack.condition_code = m->fin.condition_code;
  ack.delivery_code = DONT_CARE_0;
  if (m->publik.is_this_trans_solely_for_ack_fin)
    /* See section 5.2.4 of the CFDP Blue Book */
    ack.transaction_status = TRANS_UNDEFINED;
  else
    ack.transaction_status = TRANS_ACTIVE;
  return (ack);
}



/*=r=************************************************************************/
EOHF aaa__build_eof (CONDITION_CODE condition_code, 
                     u_int_4 file_checksum, u_int_4 file_size)
{
  EOHF         eof;
  /*------------------------------------------------------------*/

  eof.condition_code = condition_code;
  eof.spare = 0;
  eof.file_checksum = file_checksum;
  eof.file_size = file_size;
  return (eof);
}



/*=r=************************************************************************/
FIN aaa__build_fin (CONDITION_CODE condition_code, 
                    DELIVERY_CODE delivery_code)
{
  FIN           fin;
  /*------------------------------------------------------------*/
  fin.condition_code = condition_code;
  fin.end_system_status = 1;
  fin.delivery_code = delivery_code;
  fin.spare = 0;
  /* <NOT_SUPPORTED> Filestore Requests (Filestore Response here) */
  return (fin);
}



/*=r=************************************************************************/
void aaa__build_metadata_from_put_req (REQUEST request, HDR *hdr, MD *md)
   {
   /*------------------------------------------------------------*/

     /* Assemble the PDU-header */
     hdr->direction = TOWARD_RECEIVER;
     if (request.info.put.ack_required)
       hdr->mode = ACK_MODE;
     else
       hdr->mode = UNACK_MODE;
     hdr->use_crc = NO;   /* <NOT_SUPPORTED> CRC in PDUs */
     hdr->trans.source_id = mib__get_my_id ();
     /* (the transaction-id is chosen for me) */
     hdr->dest_id = request.info.put.dest_id;

     /* Assemble the Metadata directive that follows the PDU-header */
     md->file_transfer = request.info.put.file_transfer;
     memset (md->source_file_name, 0, MAX_FILE_NAME_LENGTH);
     memset (md->dest_file_name, 0, MAX_FILE_NAME_LENGTH);
     if (md->file_transfer)
       {
         COPY (md->source_file_name, request.info.put.source_file_name);
         COPY (md->dest_file_name, request.info.put.dest_file_name);
         md->file_size = file_size_callback (md->source_file_name);
       }
     md->segmentation_control = NO;  /* <NOT_SUPPORTED> Segmented files */
   }



/*=r=************************************************************************/
u_int_4 aaa__calculate_file_checksum (char *file_name)
   {
#define BUFFER_SIZE 1024
     unsigned char  buffer [BUFFER_SIZE];
     u_int_4        checksum = 0;
     FILE          *fp;
     int            i;
     u_int_4       *iptr;
     u_int_4        length;
   /*------------------------------------------------------------*/

     /*---------------------------------------*/
     /* Open the specified 'file' for reading */
     /*---------------------------------------*/

     fp = fopen_callback (file_name, "rb");
     if (fp == NULL)
       {
         e_msg__ ("cfdp_engine: Unable to open file (%s) to calculate "
                  "checksum.\n",
                  file_name);
         ASSERT__ (fp != NULL);
         /* Return a bogus answer */
         return (0xdeadbeef);
       }

     /*---------------------------------------------------------------------*/
     /* Perform the checksum calculation on one chunk of the file at a time */
     /*---------------------------------------------------------------------*/

     for (;;)
       {
         /* This 'memset' is critical.  It allows us to use the simple
          * "4 bytes at a time" method even if the length of the file
          * is not a multiple of 4 bytes.  
          * (If the very last "4 byte" piece of the file is actually
          * less than 4 bytes, the zeros will ensure that the checksum
          * result is still correct).
          */
         memset (buffer, 0, sizeof(buffer));

         /* Read the next chunk of the file */
         length = fread_callback (buffer, 1, BUFFER_SIZE, fp);
         if (length == 0)
           /* We've read the whole file, so we're done */
           break;

         /* We've read a chunk of the file -- all chunks (except the last)
          * will be 'BUFFER_SIZE' bytes in length; the last chunk will 
          * probably be less than 'BUFFER_SIZE' bytes.
          */
         /* Set up a pointer to walk through the buffer 4 bytes at a time */
         iptr = (u_int_4 *) buffer;
         for (i=0; i<length; i+=4)
           /* Add each 4 byte piece to the checksum. */
           {
             checksum += ntohl (*iptr);
             iptr ++;
           }
       }

     /* Close the file and return the checksum */
     fclose_callback (fp);
     return (checksum);
   }
#undef BUFFER_SIZE




/*=r=*************************************************************************/
void aaa__cancel_locally (MACHINE *m)
{
  /*------------------------------------------------------------*/

  /* Cancel all timers */
  timer__cancel (&m->ack_timer);
  timer__cancel (&m->nak_timer);
  timer__cancel (&m->inactivity_timer);

  /* Clear outgoing queues */
  m->is_outgoing_ack_buffered = NO;
  m->is_outgoing_eof_buffered = NO;
  m->is_outgoing_fin_buffered = NO;
  m->is_outgoing_md_buffered = NO;
  m->is_outgoing_nak_buffered = NO;

  /* Update bookkeeping */
  m->publik.cancelled = YES;
}



/*=r=************************************************************************/
void aaa__display_state_and_event (const char *which_state_table,
                                    STATE state, int event)
{
  /*------------------------------------------------------------*/

  if ((event == THROTTLE_OUTGOING_FILE_DIR_PDU) || 
      (event == THROTTLE_OUTGOING_FILE_DATA_PDU) || 
      (event == RECEIVED_FILEDATA_PDU))
    /* Don't display these events (they happen so frequently that they 
     * would hide all the interesting events)
     */
    ;

  else
    d_msg__ ("<%s> S%u + %s\n", 
             which_state_table, state, event__event_as_string (event));
}



/*=r=************************************************************************/
boolean aaa__is_file_checksum_valid (MACHINE *m)
   {
     TRANS_STATUS      *mp = &(m->publik);   /* useful shorthand */
   /*------------------------------------------------------------*/

     /* Calculate a checksum on the received (temporary) file */
     mp->file_checksum_as_calculated = 
       aaa__calculate_file_checksum (mp->temp_file_name);

     /* See if it matches the transmitted checksum */
     if (mp->file_checksum_as_calculated != m->eof.file_checksum)
       {
         e_msg__ ("cfdp_engine: checksum mismatch -- %0x / %0x "
                  "(eof/calculated)\n",
                  m->eof.file_checksum, mp->file_checksum_as_calculated);
         return (NO);
       }

     return (YES);
   }



/*=r=************************************************************************/
boolean aaa__is_file_size_valid (MACHINE *m)
   {
     TRANS_STATUS      *mp = &(m->publik);   /* useful shorthand */
   /*------------------------------------------------------------*/

     if (mp->received_file_size > m->eof.file_size)
       {
         e_msg__ ("cfdp_engine: file size mismatch -- %Lu / %lu "
                  "(eof/received))\n",
                  m->eof.file_size, mp->received_file_size);
         return (NO);
       }
     return (YES);
   }



/*=r=************************************************************************/
void aaa__initialize (MACHINE *m, REQUEST *req_ptr, HDR *hdr_ptr)
     /* NOTE: Some status is initialized the same for both Senders and
      *   Receivers (the first half of this routine); other status is
      *   specific to Senders and Receivers (the second half of this routine).
      */
   {
     TRANS_STATUS      *mp = &(m->publik);   /* useful shorthand */
   /*------------------------------------------------------------*/

     /* Public status */
     mp->abandoned = NO;
     mp->attempts = 0;
     mp->cancelled = NO;
     mp->condition_code = NO_ERROR;
     mp->delivery_code = DATA_INCOMPLETE;
     mp->fd_offset = 0;
     mp->file_checksum_as_calculated = 0;
     mp->final_status = FINAL_STATUS_UNKNOWN;
     mp->finished = NO;
     if (misc__are_any_partners_frozen ())
       mp->frozen = YES;
     else
       mp->frozen = NO;
     mp->has_md_been_received = NO;
     mp->how_many_naks = 0;
     mp->is_this_trans_solely_for_ack_fin = NO;
     memset (mp->md.source_file_name, 0, sizeof(mp->md.source_file_name));
     memset (mp->md.dest_file_name, 0, sizeof(mp->md.dest_file_name));
     mp->phase = 1;
     mp->received_file_size = 0;
     ;   /* "role" is set below */
     time (&(mp->start_time));
     mp->state = S1;
     mp->suspended = NO;
     strcpy (mp->md.dest_file_name, "Unknown");
     mp->md.segmentation_control = NO;

     /* Private status */
     m->ack_timer.mode = TIMER_OFF;
     m->has_eof_been_received = NO;
     m->has_eof_been_sent = NO;
     m->has_a_pdu_been_received = NO;
     m->has_a_put_request_been_received = NO;
     m->has_this_state_machine_finished = NO;
     m->have_we_initiated_a_cancel = NO;
     m->inactivity_timer.mode = TIMER_OFF;
     m->is_there_a_temp_file = NO;
     m->is_external_xfer_open = NO;
     m->is_outgoing_md_buffered = NO;
     m->is_outgoing_eof_buffered = NO;
     m->is_outgoing_ack_buffered = NO;
     m->is_outgoing_nak_buffered = NO;
     m->is_outgoing_fin_buffered = NO;
     m->is_there_an_open_file = NO;
     m->is_user_inside_the_engine_core = NO;
     /* (m->nak is initialized below) */
     m->nak_timer.mode = TIMER_OFF;
     m->should_external_xfer_be_closed = NO;

     /* We should be passed either a User Request or PDU, but not both */
     ASSERT__ (((req_ptr == NULL) && (hdr_ptr != NULL)) ||
               ((req_ptr != NULL) && (hdr_ptr == NULL)));

     /* If a User Request was passed in to this routine, then our role in
      * the transaction is Sender (because Sender transactions are 
      * initiated by Put Requests).
      */
     if (req_ptr != NULL)
       {
         /* Fill in the our role (i.e. Class 1 or Class 2 Sender) */
         if (req_ptr->info.put.ack_required)
           mp->role = S_2;
         else
           mp->role = S_1;

         nak__init (&(m->nak), I_AM_A_SENDER);

         /* Fill in the PDU header that will be used for all outgoing PDUs */
         m->hdr.direction = TOWARD_RECEIVER;
         m->hdr.trans.source_id = mib__get_my_id ();
         m->hdr.trans.number = misc__new_trans_seq_num ();
         m->hdr.dest_id = req_ptr->info.put.dest_id;
       }

     /* If a PDU-header was passed in to this routine, then our role in the 
      * transaction is Receiver (because Receiver transactions are
      * initiated by incoming Metatadata/Filedata/EOF PDUs).
      */
     else if (hdr_ptr != NULL)
       {
         /* Store the given PDU header */
         m->hdr = *hdr_ptr;  
         m->publik.role = pdu__receiver_role_from_pdu_hdr (*hdr_ptr);

         nak__init (&(m->nak), I_AM_A_RECEIVER);

         /* Start the Inactivity-timer */
         m->inactivity_timer = timer__start (mib__inactivity_timeout(NULL));
       }
   }



/*=r=************************************************************************/
boolean aaa__is_file_structure_valid (MACHINE *m)
   {
     TRANS_STATUS      *mp = &(m->publik);   /* useful shorthand */
   /*------------------------------------------------------------*/

     if ((mp->md.segmentation_control) &&
         (!is_file_segmented_callback (mp->md.source_file_name)))
       {
         e_msg__ ("cfdp_engine: invalid file structure (file should "
                  "be segmented)\n");
         return (NO);
       }

     else if ((!mp->md.segmentation_control) &&
         (is_file_segmented_callback (mp->md.source_file_name)))
       {
         e_msg__ ("cfdp_engine: invalid file structure (file should not "
                  "be segmented)\n");
         return (NO);
       }

     return (YES);
   }



/*=r=*************************************************************************/
void aaa__notice_of_suspension (MACHINE *m)
{
  /*------------------------------------------------------------*/

  if (!m->publik.suspended)
    {
      m->publik.suspended = YES;
      aaa__suspend_timers (m);
      indication__ (IND_SUSPENDED, &(m->publik));
    }
}



/*=r=************************************************************************/
boolean aaa__open_source_file (MACHINE *m)
   {
     TRANS_STATUS      *mp = &(m->publik);   /* useful shorthand */
   /*------------------------------------------------------------*/

     m->fp = fopen_callback (mp->md.source_file_name, "rb");
     if (m->fp == NULL)
       return NO;
     else
       {
         m->is_there_an_open_file = YES;
         return (YES);
       }
   }



/*=r=************************************************************************/
boolean aaa__open_temp_file (MACHINE *m)
   {
     int                i;
     TRANS_STATUS      *mp = &(m->publik);   /* useful shorthand */
     int                max_length;
     char               name [1024];
   /*------------------------------------------------------------*/

     /* <ENHANCEMENT> Add later!!!  make sure Receiver has room for a file
      * 'mp->md.file_size' bytes long that will be named
      * 'mp->md.dest_file_name'.
      */

     /* Get a name for the temporary file */
     /* Note:  It's kind of weird to have a 'for' loop here.  But the
      * general idea seems reasonable:  if the first file-name chosen for
      * the temp-file doesn't work, then don't give up -- try multiple 
      * times.
      */
     for (i=0; i<100; i++)
       {
         /* Generate a candidate name */
         if (sizeof(name) < sizeof(mp->temp_file_name))
           /* This should never happen, but if it does, avoid crashing */
           {
             e_msg__ ("cfdp_engine: MAX_TEMP_FILE_NAME_LENGTH is "
                      "too big (%u).\n",
                      MAX_TEMP_FILE_NAME_LENGTH);
             return (NO);
           }
         max_length = sizeof (name);
         memset (name, 0, max_length);
         strncpy (name, temp_file_name_callback(name), sizeof(name));
         name[max_length-1] = 0;

         if (strlen(name) > sizeof(mp->temp_file_name))
           {
             e_msg__ ("cfdp_engine: temp file name (%s) exceeds "
                      "storage capacity (%u)\n",
                      name, MAX_TEMP_FILE_NAME_LENGTH);
             /* Avoid crashing... */
             continue;
           }
         COPY (mp->temp_file_name, name);
         
         /* Open the chosen temp file name (for write access) */
         m->fp = fopen_callback (mp->temp_file_name, "wb");
         if (m->fp != NULL)
           /* Success */
           {
             m->is_there_a_temp_file = YES;
             return (YES);
           }
         else
           {
             w_msg__ ("cfdp_engine: candidate temp file (%s) open failed.\n",
                      mp->temp_file_name);
             continue;
           }
       }

     /* If we get here, something is wrong (many failed attempts) */
     e_msg__ ("cfdp_engine: Unable to open a temp file.\n");
     return (NO);
   }



/*=r=*************************************************************************/
void aaa__resume_timers (MACHINE *m)
{
   /*------------------------------------------------------------*/
      if (timer__is_timer_paused (&m->ack_timer))
        timer__resume (&m->ack_timer);
      if (timer__is_timer_paused (&m->nak_timer))
        timer__resume (&m->nak_timer);
      if (timer__is_timer_paused (&m->inactivity_timer))
        timer__resume (&m->inactivity_timer);
}



/*=r=************************************************************************/
void aaa__reuse_senders_first_hdr (HDR incoming_hdr, MACHINE *m)
{
  TRANS_STATUS       *mp = &(m->publik);   /* useful shorthand */
  /*------------------------------------------------------------*/

  if (!m->has_a_pdu_been_received)
    {
      /* Use this incoming header as the outgoing header... */
      m->hdr = incoming_hdr;
      mp->trans = m->hdr.trans;
      mp->partner_id = mp->trans.source_id;

      /* ... except that the direction is now reversed. */
      if (m->hdr.direction == TOWARD_RECEIVER)
        m->hdr.direction = TOWARD_SENDER;  
      else
        m->hdr.direction = TOWARD_RECEIVER;  
    }
}



/*=r=************************************************************************/
void aaa__send_ack (HDR hdr, ACK ack)
   {
     static PDU          pdu;
   /*------------------------------------------------------------*/

     /* If configured to do so, output a simple message */
     if (cfdp_is_message_class_enabled (CFDP_MSG_PDU_NON_FILEDATA))
       d_msg__ ("<---------- (%s) %s\n", 
                cfdp_trans_as_string (hdr.trans),
                ack_struct_as_string (ack));

     /* Create an ACK Pdu. */
      pdu__make_ack_pdu (hdr, ack, &pdu);

     /* If configured to do so, output a more detailed message */
     if (cfdp_is_message_class_enabled (CFDP_MSG_DEBUG_PDU))
       pdu__display_raw_pdu (pdu);

     /* Send the PDU immediately */
     pdu_output__send (hdr.trans, m__partner_id (hdr), &pdu);
   }



/*=r=************************************************************************/
void aaa__send_eof (HDR hdr, EOHF eof)
   {
     static PDU           pdu;
   /*------------------------------------------------------------*/

     /* If configured to do so, output a simple message */
     if (cfdp_is_message_class_enabled (CFDP_MSG_PDU_NON_FILEDATA))
       d_msg__ ("<---------- (%s) %s\n", 
                cfdp_trans_as_string (hdr.trans),
                eof_struct_as_string (eof));

     /* Create an EOF Pdu */
     pdu__make_eof_pdu (hdr, eof, &pdu);

     /* If configured to do so, output a more detailed message */
     if (cfdp_is_message_class_enabled (CFDP_MSG_DEBUG_PDU))
       pdu__display_raw_pdu (pdu);

     /* Send the PDU immediately */
     pdu_output__send (hdr.trans, hdr.dest_id, &pdu);
   }



/*=r=************************************************************************/
void aaa__send_finished (HDR hdr, FIN fin)
   {
     static PDU            pdu;
   /*------------------------------------------------------------*/

     /* If configured to do so, output a simple message */
     if (cfdp_is_message_class_enabled (CFDP_MSG_PDU_NON_FILEDATA))
       d_msg__ ("<---------- (%s) %s\n", 
                cfdp_trans_as_string (hdr.trans),
                fin_struct_as_string (fin));

     /* Build a Finished pdu from the HDR & FIN structs */
     pdu__make_fin_pdu (hdr, fin, &pdu);

     /* If configured to do so, output a more detailed message */
     if (cfdp_is_message_class_enabled (CFDP_MSG_DEBUG_PDU))
       pdu__display_raw_pdu (pdu);

     /* Send the pdu (immediately) */
     pdu_output__send (hdr.trans, hdr.trans.source_id, &pdu);
   }



/*=r=************************************************************************/
void aaa__send_metadata (HDR hdr, MD md)
   {
     static PDU            pdu;
   /*------------------------------------------------------------*/

     /* If configured to do so, output a simple message */
     if (cfdp_is_message_class_enabled (CFDP_MSG_PDU_NON_FILEDATA))
       d_msg__ ("<---------- (%s) %s\n", 
                cfdp_trans_as_string (hdr.trans),
                md_struct_as_string (md));

     /* Build a MD pdu from the HDR & MD structs */
     pdu__make_md_pdu (hdr, md, &pdu);

     /* If configured to do so, output a more detailed message */
     if (cfdp_is_message_class_enabled (CFDP_MSG_DEBUG_PDU))
       pdu__display_raw_pdu (pdu);

     /* Send the MD pdu (immediately) */
     pdu_output__send (hdr.trans, hdr.dest_id, &pdu);
   }



/*=r=************************************************************************/
void aaa__send_nak (HDR hdr, NAK nak)
   {
     static PDU            pdu;
   /*------------------------------------------------------------*/

     /* If configured to do so, output a simple message */
     if (cfdp_is_message_class_enabled (CFDP_MSG_PDU_NON_FILEDATA))
       d_msg__ ("<---------- (%s) %s\n", 
                cfdp_trans_as_string (hdr.trans),
                nak_struct_as_string (nak));

     /* Build a NAK pdu from the HDR & NAK structs */
     pdu__make_nak_pdu (hdr, nak, &pdu);

     /* If configured to do so, output a more detailed message */
     if (cfdp_is_message_class_enabled (CFDP_MSG_DEBUG_PDU))
       pdu__display_raw_pdu (pdu);

     /* Send the NAK pdu (immediately) */
     pdu_output__send (hdr.trans, hdr.trans.source_id, &pdu);
   }



/*=r=************************************************************************/
void aaa__send_one_file_data_pdu (MACHINE *m, FD *fd)
   {
     u_int_4           buffer_length;
     register u_int_4  checksum_in_register;
     u_int_1           four_byte_buffer [4];
     u_int_4          *four_byte_pointer;
     int               how_many_bytes_till_boundary;
     int               how_many_bytes_of_zero_fill;
     int               i;
     u_int_4          *i4ptr;
     TRANS_STATUS     *mp = &(m->publik);   /* useful shorthand */
     int               offset_modulo_four;
     static PDU        pdu;
     NODE             *ptr;
     size_t            result;
   /*------------------------------------------------------------*/

     /* Determine how much file-data to send: send as much of the first
      * data gap as will fit in one PDU (the whole data gap, if possible).
      */
     ptr = m->nak.head;   /* Point to the first data gap on the list */
     if (ptr == NULL)
       /* Avoid crashing if nak-list is empty (shouldn't ever happen) */
       {
         e_msg__ ("cfdp_engine: bug.  See 'aaa__send_one_file_data_pdu'.\n");
         /* Avoid crashing... */
         return;
       }
     fd->offset = ptr->begin;
     if (ptr->end - ptr->begin <= mib__outgoing_file_chunk_size (NULL))
       /* It all fits; take it all */
       fd->buffer_length = ptr->end - ptr->begin;
     else
       /* Won't all fit; use as much as allowed */
       fd->buffer_length = mib__outgoing_file_chunk_size (NULL);

     /* If this is a retransmission, seek to the required file offset
      * (during the first transmission, the end of the previous file data
      * is always the start of the current file data).
      */
     if (m->has_eof_been_sent)
       {
         if (cfdp_is_message_class_enabled (CFDP_MSG_PDU_RETRANSMITTED_FD))
           d_msg__ ("<aaa> Resending FD: %u bytes at offset %lu.\n",
                    fd->buffer_length, fd->offset);
         if (fseek_callback (m->fp, fd->offset, SEEK_SET))
           e_msg__ ("cfdp_engine: 'fseek' error (%s/%u).\n",
                    __func__, __LINE__);
       }

     /* ***OPTIMIZATION*** 
      *   This optimization is not yet implemented (it would break the
      *   current checksum-calculation algorithm).  Consider implementing
      *   it in the future...
      *   Instead of reading the file-data into a buffer and then
      *   building a PDU from that buffer, build the skeleton of the
      *   PDU and then read the file-data directly into the PDU.
      *   The straightforward approach uses 2 memcpy, the optimization 1.
      *   Careful - the checksum-calculation algorithm requires up to 3 
      *   bytes that follow the file-data to be zeroed out.
      */

     /* Read the data from the file and store it in the filedata-buffer.
      * Note:  It's important to zero out the filedata buffer before reading
      * (so that the algorithm used for checksum calculation can 
      * remain simple -- it assumes that zeros follow the real file-data).
      */

     memset (fd->buffer, 0, sizeof(fd->buffer));
     result = fread_callback (fd->buffer, (size_t) fd->buffer_length, 
                              1, m->fp);
     if (result != 1)
       e_msg__ ("cfdp_engine: 'fread' error (%s/%u).\n",  __func__, __LINE__);

     /* Generate a FD Pdu */
     pdu__make_fd_pdu (m->hdr, fd, &pdu);

     /*-----------------------------------------------------------------*/
     /* Please refer to the detailed note near the top of this file     */
     /* for an explanation of the file checksum and the algorithm used. */
     /*-----------------------------------------------------------------*/

     if (!m->has_eof_been_sent)
       /* We are still blasting the file (initial transmission) */
       {

         /*----------------------------------------------------------------*/
         /* Before entering the normal "4 bytes at a time" checksum        */
         /* calculation loop, get back onto a 4-byte boundary.             */
         /* (see example 2 in the detailed note near the top of this file) */
         /*----------------------------------------------------------------*/

         /* Determine how many bytes of file-data need to be processed to
          * reach a 4-byte boundary.  
          * Example:  If our current file-data starts at offset 5, then the
          * offset_modulo_four is 1, and 3 more bytes are needed to reach
          * the next 4-byte boundary (at offset = 8).
          */
         offset_modulo_four = fd->offset & 0x03;
         if (offset_modulo_four == 0)
           how_many_bytes_till_boundary = 0;
         else
           how_many_bytes_till_boundary = 4 - offset_modulo_four;

         /* Perform the (custom) processing required (if any) to get 
          * back to a 4-byte boundary.
          */
         if (how_many_bytes_till_boundary > 0)
           {
             /* Set up a 4 byte buffer with zero-fill on the left... */
             four_byte_buffer[0] = 0;
             four_byte_buffer[1] = 0;
             four_byte_buffer[2] = 0;
             four_byte_buffer[3] = 0;
             /* ... and the appropriate number of file-data bytes copied in a
              * "right-justified" position.
              */
             how_many_bytes_of_zero_fill = 4 - how_many_bytes_till_boundary;
             memcpy (&four_byte_buffer[how_many_bytes_of_zero_fill],
                     fd->buffer,
                     how_many_bytes_till_boundary);
             /* Add the 4-byte buffer to the current checksum */
             four_byte_pointer = (u_int_4 *) four_byte_buffer;
             mp->file_checksum_as_calculated += ntohl (*four_byte_pointer);
           }

         /* Adjust the file-data buffer length and data-pointer */
         buffer_length = fd->buffer_length - how_many_bytes_till_boundary;
         i4ptr = (u_int_4 *) &fd->buffer[how_many_bytes_till_boundary];

         /*------------------------------------------------------------*/
         /* This is the normal "4 bytes at a time" checksum algorithm. */
         /*------------------------------------------------------------*/
             
         /* Add one 4-byte wide piece at a time to the running checksum.
          * Note:  This algorithm assumes that there are at least 3 bytes
          * of zeros immediately beyond the file-data.  That's why the
          * file-data buffer is 3 bytes longer than "necessary", and why
          * the file-data buffer is zeroed out before each read.
          * ***OPTIMIZATION***
          *   use a temporary register variable inside the 'for' loop.
          */
         checksum_in_register = mp->file_checksum_as_calculated;
         for (i=0; i<buffer_length; i+=4)
           {
             /* Use 'ntohl' to avoid big-endian versus little-endian issues */
             checksum_in_register += ntohl (*i4ptr);
             /* Advance 4 more bytes into the buffer of file data */
             i4ptr ++;
           }
         mp->file_checksum_as_calculated = checksum_in_register;
       }

     /* If configured to do so, output a detailed message */
     if (cfdp_is_message_class_enabled (CFDP_MSG_DEBUG_PDU))
       pdu__display_disassembled_pdu (pdu);

     /* Send the Pdu */
     pdu_output__send (m->hdr.trans, m->hdr.dest_id, &pdu);
     mp->fd_length = fd->buffer_length;

     /* If configured to do so, issue an Indication */
     if (mib__issue_file_segment_sent ())
       indication__ (IND_FILE_SEGMENT_SENT, &(m->publik));

     /* During the initial send of the file, the next chunk of file data
      * to be sent will always immediately follow the current chunk
      */
     if (!m->has_eof_been_sent)
       mp->fd_offset = fd->offset + fd->buffer_length;
   }



/*=r=************************************************************************/
void aaa__shutdown (MACHINE *m)
   {
     GAP            *current;
     GAP            *next;
     TRANS_STATUS     *mp = &(m->publik);   /* useful shorthand */
   /*------------------------------------------------------------*/

     /* If a file is open, close it */
     if (m->is_there_an_open_file)
       {
         fclose_callback (m->fp);
         m->is_there_an_open_file = NO;
       }

     /* If a temp file exists at this point, then we are a Receiver in a 
      * transaction that wasn't entirely successful.  Delete the temp-file unless
      * the engine user has told us not to (i.e. they may want to look at what
      * they got even if it is is not complete).
      */
     if (m->is_there_a_temp_file)
       {
         if (mib__save_incomplete_files (NULL))
           /* Engine user wants us to save incomplete files, so don't delete it */
           d_msg__ ("cfdp_engine: saving incomplete temp-file (%s).\n",
                    m->publik.temp_file_name);
         else
           /* Ok to delete the temp-file */
           remove_callback (mp->temp_file_name);
       }

     mp->state = FINISHED;
     m->is_outgoing_md_buffered = NO;
     m->is_outgoing_eof_buffered = NO;
     m->is_outgoing_ack_buffered = NO;
     m->is_outgoing_nak_buffered = NO;
     m->is_outgoing_fin_buffered = NO;

     /* Free all memory in the Nak-list */
     for (current=m->nak.head; current!=NULL; current=next)
       {
         next = current->next;
         nak_mem__free (&(m->nak), current);
       }

     /* Stop all timers */
     timer__cancel (&m->ack_timer);
     timer__cancel (&m->nak_timer);
     timer__cancel (&m->inactivity_timer);

     /* Finally, set a flag indicating that this machine has finished. */
     m->has_this_state_machine_finished = YES;
   }



/*=r=************************************************************************/
void aaa__store_file_data (FILE *fp, FD *fd)
   {
     size_t          result;
   /*------------------------------------------------------------*/

     /* Seek to the specified file offset, and write the given data there */
     if (fseek_callback (fp, fd->offset, SEEK_SET))
       e_msg__ ("cfdp_engine: 'fseek' error (%s/%u).\n",
                __func__, __LINE__);
     else
       {
         result = fwrite_callback (fd->buffer, (size_t) fd->buffer_length, 
                                   1, fp);
         if (result != 1)
           e_msg__ ("cfdp_engine: 'fwrite' error (%s/%u).\n", 
                    __func__, __LINE__);
       }
   }



/*=r=*************************************************************************/
void aaa__suspend_timers (MACHINE *m)
{
  /*------------------------------------------------------------*/
  if (timer__is_timer_running (&m->ack_timer))
    timer__pause (&m->ack_timer);
  if (timer__is_timer_running (&m->nak_timer))
    timer__pause (&m->nak_timer);
  if (timer__is_timer_running (&m->inactivity_timer))
    timer__pause (&m->inactivity_timer);
}



/*=r=*************************************************************************/
void aaa__transaction_has_finished (MACHINE *m)
{
  /*------------------------------------------------------------*/

  /* Cancel all timers */
  timer__cancel (&m->ack_timer);
  timer__cancel (&m->nak_timer);
  timer__cancel (&m->inactivity_timer);

  /* Clear outgoing queues */
  m->is_outgoing_ack_buffered = NO;
  m->is_outgoing_eof_buffered = NO;
  m->is_outgoing_fin_buffered = NO;
  m->is_outgoing_md_buffered = NO;
  m->is_outgoing_nak_buffered = NO;

  /* Update bookkeeping */
  m->publik.finished = YES;
  if ((m->publik.role == R_1) && (!m->publik.has_md_been_received))
    /* This Class 1 Receiver finished, but never received a Metadata PDU.
     * Therefore, it wasn't successful.
     */
    m->publik.final_status = FINAL_STATUS_NO_METADATA;
  else if (m->publik.cancelled)
    m->publik.final_status = FINAL_STATUS_CANCELLED;
  else
    m->publik.final_status = FINAL_STATUS_SUCCESSFUL;

  /* Let user know what has happened */
  indication__ (IND_TRANSACTION_FINISHED, &(m->publik));

  /* Shut down this state machine */
  aaa__shutdown (m);
}
