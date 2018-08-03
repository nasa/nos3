/* FILE: nak.h    Specs for CFDP Nak module.
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
 * BIGGER PICTURE:  In the formal CFDP protocol, the Nak is a Protocol Data
 *   Unit indicating requests for retransmissions.  Within this software
 *   library, the Nak structure has the same contents as a Nak PDU, but it
 *   is used in a broader way.  The Sender uses the Nak structure to indicate
 *   what data needs to be sent (throughout the life of the transaction, not
 *   just when feedback is received from the Receiver).  (The Receiver uses
 *   the Nak structure in the usual way).
 * SUMMARY:  This module maintains NAK-Pdu data (represented as a C structure);
 *   i.e. it keeps track of:
 *     1) whether or not Metadata needs to be retransmitted
 *     2) any gaps in the file-data (which File-data needs to be retrans).
 *   This module is designed so that separate transactions can maintain
 *   separate Nak-data (the Nak-data is passed in/out with each routine).
 * NOTE:  Only 'deferred' (comprehensive) Nak-mode is supported!
 * USAGE:  
 *   Call 'nak__init' once for each transaction before calling any of the 
 *   other routines.  
 *   When copying a Nak structure, use the copy_nak routine below (unless
 *   you enjoy looking through core dumps).
 * CHANGES:
 *   2007_07_31 Tim Ray
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 */


#ifndef H_NAK
#define H_NAK 1

#include "cfdp.h"

#define I_AM_A_SENDER 1
#define I_AM_A_RECEIVER 2

void nak__copy_nak (NAK *dest_ptr, NAK *src_ptr);
/* WHAT IT DOES:  Copies the given (src) Nak to the specified destination. */

void nak__init (NAK *nak, int sender_or_receiver);
/* WHAT IT DOES:  First, initializes the given Nak (to be empty, with a
 *   scope of 0-0).  If the caller has indicated that they are a Receiver
 *   (sender_or_receiver = I_AM_A_RECEIVER), then the Nak is also updated
 *   to indicate that Metadata has not yet been received.
 */

void nak__data_received (NAK *nak_ptr, u_int_4 begin, u_int_4 end);
/* WHAT IT DOES:  Updates the given Nak based upon the given data range.
 *   This data range may close gaps and/or open new ones, and may also
 *   extend the scope of the Nak.
 */

void nak__data_sent (NAK *nak_ptr, u_int_4 begin, u_int_4 end);
/* WHAT IT DOES:  Removes the given data range from the given Nak.
 *   This data range may also extend the scope of the Nak.
 */

void nak__set_end_of_scope (NAK *nak_ptr, u_int_4 end);
/* WHAT IT DOES:  Sets the end_of_scope for the given Nak.  If the new
 *   end_of_scope extends the old one, then a new data gap is added
 *   (i.e. the data range between the two is missing).
 */

void nak__metadata_sent_or_received (NAK *nak_ptr);
/* WHAT IT DOES:  Updates the given Nak to indicate that Metadata is no
 *   longer missing.
 */


u_int_4 nak__how_many_filedata_gaps (NAK *nak);
/* WHAT IT DOES:  Returns the current number of Filedata gaps in the given 
 *   nak-list.  (Does not include missing Metadata)
 */

boolean nak__is_list_empty (NAK *nak);
/* WHAT IT DOES:  Returns 1 if the given Nak-list is empty (i.e. the Nak
 *   does not request the transmission of any PDUs); otherwise, 0.
 */

char *nak__list_as_string (NAK *nak);
/* WHAT IT DOES:  Given Nak-data, it returns a character string containing
 *    the Nak-data.  The scope (begin-end) is in parentheses, perhaps
 *    followed by the word "Metadata" (if Metadata is missing), followed by
 *    a list of Filedata gaps.
 * INTENT:  Useful for debugging/validation of this module.
 * EXAMPLES:
 *   "(0-100)"   -- Highest offset received is 100; no gaps.
 *   "(0-500) 100-200 250-300"   -- Highest offset received is 500; 2 gaps.
 *   "(0-10000) Metadata 100-200 250-300" -- same as above, plus Metadata
 *                                           is missing.
 */

void nak__gaps_as_string (NAK *nak_ptr, char *string,
                                      int max_string_length);
/* WHAT IT DOES:  Similar to the previous routine, except that it only
 *   includes filedata-gaps.
 * INPUTS:  The nak-list, a pointer to the string to write into, and 
 *   the length of the given string-array.
 * OUTPUTS:  A string (containing the list of filedata gaps).
 * NOTE:  This routine will truncate the list if necessary to ensure
 *   that the length of the returned string does not exceed max_string_length.
 * EXAMPLES:
 *   ""          -- no gaps
 *   "100-200"   -- one gap
 *   "100-200 3500-3800 5600-5700"   -- 3 gaps
 */

u_int_4 nak__how_many_bytes_missing (NAK *nak_ptr);
/* WHAT IT DOES:  Adds up the number of bytes in each gap on the given 
 *   nak-list, and returns the total.
 */

u_int_4 nak__how_many_bytes_received (NAK *nak_ptr);
/* WHAT IT DOES:  Based on the scope and what is missing, determines how
 *   many bytes are "not missing", and returns that number.
 */

#endif
