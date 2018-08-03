/* FILE:  aaa.h   Specs for CFDP state table 'action' routines.
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
 * BACKGROUND:
 *     My philosophy with this module is different than usual.  
 *   My usual approach is to make lower-level modules as close to general-
 *   purpose utilities as practical.  Instead, this module is tightly coupled
 *   to the needs of the state table modules (so that the state table logic
 *   is easier to grasp).
 *     The purpose of this module is to provide the details for carrying out 
 *   many conceptually straightforward actions that are called out within
 *   the CFDP state tables (see s1.c, s2.c, r1.c, and r2.c).  
 *     All of the routines in this module work on one state machine at a time.
 *   In most cases, a pointer to the state machine is supplied by the caller.
 *   In many cases, the routine interface could have been more specific
 *   (instead of passing the entire state machine), but that would have made
 *   the state table logic less readable.
 * CHANGES:
 *   2007_07_31 Tim Ray (TR)
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 *   2007_11_05 TR
 *     - Removed the routine that retained incomplete files (this routine
 *       is now private).
 */

#ifndef H_AAA
#define H_AAA 1

#include "cfdp.h"
#include "cfdp_private.h"


void aaa__abandon_this_transaction (MACHINE *m);
/* WHAT IT DOES:  Updates our bookkeeping, lets the user know what has 
 *   happened, and marks the state-machine as 'finished'.
 */

/* A group of routines that build the File-Directive portion of a PDU.
 * Note:  These routines do not build the entire PDU.
 */
ACK aaa__build_ack_eof (MACHINE *m);
ACK aaa__build_ack_fin (MACHINE *m);
EOHF aaa__build_eof (CONDITION_CODE condition_code, 
                     u_int_4 file_checksum, u_int_4 file_size);
FIN aaa__build_fin (CONDITION_CODE condition_code, 
                    DELIVERY_CODE delivery_code);

void aaa__build_metadata_from_put_req (REQUEST req, HDR *hdr, MD *md);
/* WHAT IT DOES:  Builds Header & Metadata structs from the given Put Request.
 */

u_int_4 aaa__calculate_file_checksum (char *file_name);
/* WHAT IT DOES:  Calculates (and returns) the checksum of the given file */

void aaa__cancel_locally (MACHINE *m);
/* WHAT IT DOES:  Cancels all timers, clears all queues, and updates
 *   the machine's bookkeeping.
 */

void aaa__display_state_and_event (const char *which_state_table,
                                   STATE state, int event);
/* WHAT IT DOES:  (Intended for debugging only) 
 *   Outputs a debug message containing the given info
 */

void aaa__initialize (MACHINE *m, REQUEST *req_ptr, HDR *hdr_ptr);
/* WHAT IT DOES:  Initializes the given state machine's variables.
 *   If the machine is a Sender, the caller must supply the
 *   Put Request that initiated the transaction (and hdr_ptr is null).
 *   If the machine is a Receiver, the caller must supply the
 *   PDU-header from the incoming MD/FD/EOF PDU that initiated the
 *   transaction (and req_ptr is null).
 */

boolean aaa__is_file_checksum_valid (MACHINE *m);
/* WHAT IT DOES:  (Used by Receivers only)  Performs the checksum operation
 *   on the received file, and compares the result to the checksum given in
 *   the EOF pdu.  If they match, returns 1; otherwise returns 0.
 */

boolean aaa__is_file_size_valid (MACHINE *m);
/* WHAT IT DOES:  (Used by Receivers only)  Compares the size of the
 *   received file with the size specified in the EOF pdu.  If the received 
 *   file is bigger than specified in the EOF, returns 0; otherwise 1.
 */

boolean aaa__is_file_structure_valid (MACHINE *m);
/* WHAT IT DOES:  (Related to Segmented files)  Returns 1 if the structure
 *   of the file to be sent matches the structure advertised in the Metadata
 *   PDU (either Segmented or not).
 */

void aaa__notice_of_suspension (MACHINE *m);
/* WHAT IT DOES:  If the machine is not already suspended, suspends the
 *   timers and issues a 'Suspended' Indication.
 */

boolean aaa__open_source_file (MACHINE *m);

boolean aaa__open_temp_file (MACHINE *m);

void aaa__resume_timers (MACHINE *m);
/* WHAT IT DOES:  Resumes all the protocol timers for the given machine. */

void aaa__reuse_senders_first_hdr (HDR incoming_hdr, MACHINE *m);
/* WHAT IT DOES:  This routine is only intended for use by the Receiver.
 *   It ensures that the Header from the first incoming Pdu is copied so
 *   that outgoing Pdus have the appropriate Header info.
 * NOTE:  The Header 'direction' field is flipped (i.e. 'Toward_Sender').
 */

void aaa__send_eof (HDR hdr, EOHF eof);
/* WHAT IT DOES:  Builds an EOF Pdu from the given Header & EOF structs,
 *   and immediately sends it out via the lower-layer comm.
 */

void aaa__send_finished (HDR hdr, FIN fin);
/* WHAT IT DOES:  Builds a Finished Pdu from the given Header & Fin structs,
 *   and immediately sends it out via the lower-layer comm.
 */

void aaa__send_metadata (HDR hdr, MD md);
/* WHAT IT DOES:  Builds a MD Pdu from the given Header & Metadata structs,
 *   and immediately sends it out via the lower-layer comm.
 */

void aaa__send_nak (HDR hdr, NAK nak);
/* WHAT IT DOES:  Builds a Nak Pdu from the given Header & Nak structs,
 *   and immediately sends it out via the lower-layer comm.
 */

void aaa__send_one_file_data_pdu (MACHINE *m, FD *fd);
/* WHAT IT DOES:  Generates and sends a file-data pdu.  Also updates the
 *   machine status.  The caller is passed back a copy of the FD structure.
 */

void aaa__send_finished (HDR hdr, FIN fin);
/* WHAT IT DOES:  Builds a Finished Pdu from the given Header & Fin structs,
 *   and immediately sends it out via the lower-layer comm.
 */

void aaa__send_ack (HDR hdr, ACK ack);
/* WHAT IT DOES:  Builds an ACK Pdu from the given Header & Ack structs,
 *   and immediately sends it out via the lower-layer comm.
 */

void aaa__shutdown (MACHINE *m);
/* WHAT IT DOES:  Shuts down the given machine (and removes it from the
 *   list of active machines).  
 * WARNING:  Upon return, the pointer no longer points to a valid machine.
 *   Don't use it!
 */

void aaa__store_file_data (FILE *fp, FD *fd);
/* WHAT IT DOES:  Given file-data (via the variable 'fd'), it stores the
 *   file-data in the given file.
 */
void aaa__suspend_timers (MACHINE *m);
/* WHAT IT DOES:  Suspends all the protocol timers for the given machine. */

void aaa__transaction_has_finished (MACHINE *m);
/* WHAT IT DOES:  Cancels all timers, clears outgoing queues, updates
 *   bookkeeping, lets the user know what that the transaction has finished,
 *   and shuts down the state machine.
 */

#endif
