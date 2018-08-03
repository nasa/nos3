/* FILE:  machine.h -- defines a data structure to hold the status of a
 *   CFDP state machine.
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
 *   2007_07_31 Tim Ray (TR)
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 *   2007_11_09 TR
 *     - Better portability.  C++ uses 'public' as a keyword, so any use of
 *       'public' as a variable name was changed to 'publik'.
 *   2007_12_10 TR
 *     - Added the field 'is_a_file_being_received'.
 *   2007_01_24 TR
 *     - Removed the field 'is_this_simply_an_ack..." (moved it to the public
 *       trans_status structure, and renamed it 'is_this_trans_solely...').
 *     - Bug-fix: don't attempt to remove a non-existent file.
 *       This fix included cleaning up some code (much simpler logic now).
 *       Got rid of a few 'machine' variables, and renamed one variable
 *       ('is_a_file_being_received' to 'is_there_a_temp_file')
 */

#ifndef H_MACHINE
#define H_MACHINE 1

#include "cfdp.h"


typedef struct
{
  /* Publicly available items */
  TRANS_STATUS    publik;
  /* Boolean flags */
  boolean         has_a_pdu_been_received:1;
  boolean         has_a_put_request_been_received:1;
  boolean         has_eof_been_received:1;
  boolean         has_eof_been_sent:1;
  boolean         has_this_state_machine_finished:1;
  boolean         have_we_initiated_a_cancel:1;
  boolean         is_external_xfer_open:1;
  boolean         is_outgoing_ack_buffered:1;
  boolean         is_outgoing_eof_buffered:1;
  boolean         is_outgoing_fin_buffered:1;
  boolean         is_outgoing_md_buffered:1;
  boolean         is_outgoing_nak_buffered:1;
  boolean         is_there_a_temp_file:1;
  boolean         is_there_an_open_file:1;
  boolean         is_user_inside_the_engine_core:1;
  boolean         should_external_xfer_be_closed:1;
  /* Some pdu contents are stored for later use */
  HDR             hdr;
  EOHF            eof;
  FIN             fin;
  ACK             ack;
  NAK             nak;
  /* Protocol timers */
  TIMER           ack_timer;
  TIMER           inactivity_timer;
  TIMER           nak_timer;
  /* Other machine variables */
  FILE           *fp;
} MACHINE;

#endif




