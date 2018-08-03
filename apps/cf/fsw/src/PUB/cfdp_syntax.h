/* FILE: cfdp_syntax.h -- character string syntax used by the CFDP library.
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
 * LAST MODIFIED:  2007_06_11
 * ORIGINAL PROGRAMMER:  Tim Ray 301-286-0581
 * SUMMARY:  
 *     The syntax defined here is relevant to the library routines 
 *   'cfdp_give_request', 'cfdp_get_mib_parameter', and 
 *   'cfdp_set_mib_parameter' (see the file 'cfdp_provides.h').  
 *     Those routines use character-strings to represent User Requests,
 *   MIB parameter names, and MIB parameter values.  This file defines
 *   the syntax of the character-strings.
 * CHANGES:
 *   2006_08_21 Tim Ray
 *     - Replaced 2 File chunk-size parameters with 1.
 *   2007_05_30 Tim Ray
 *     - Added a new Request (Abandon).
 *   2007_06_11 Tim Ray
 *     - Added a new MIB parameter (Save-Incomplete-Files).
 */

#ifndef H_CFDP_SYNTAX
#define H_CFDP_SYNTAX 1



/*********************/
/*** User Requests ***/
/*********************/

/* This section applies to the routine 'cfdp_give_directive' */

/* Argument #1 of each request is one of these tokens.
 * (Uppercase/lowercase does not matter)
 */
#define PUT_REQUEST "PUT"
#define SUSPEND_REQUEST "SUSPEND"
#define RESUME_REQUEST "RESUME"
#define CANCEL_REQUEST "CANCEL"
#define ABANDON_REQUEST "ABANDON"
#define REPORT_REQUEST "REPORT"
#define FREEZE_REQUEST "FREEZE"
#define THAW_REQUEST "THAW"

/* Put Requests use this syntax:
 *     Put [-class1] <source_file> <destination_id> [dest_file]
 * Examples:
 *     Put myfile 23 yourfile  -- sends 'myfile' to CFDP node 23, and
 *                                calls it 'yourfile'.  (Service class 2)
 *     Put sun.data 23         -- sends 'sun.data' to CFDP node 23, and 
 *                                calls it 'sun.data'.  (Service class 2)
 *     Put -class1 sun.data 23 -- same as previous, except Service Class 1
 *                                (i.e. no retransmissions by CFDP)
 *     Put sun.data 101.158    -- if the partner's Entity-ID exceeds 1 byte,
 *                                use dotted-decimal format.
 *     Put -class1 abc 23 def  -- Uses service class 1 to transfer 'abc'
 *                                to node 23 and call it 'def'.
 */

/* Suspend, Resume, Cancel, Abandon, and Report Requests all use the 
 * same syntax.   (Only 'cancel' will be shown):
 *     Cancel <transaction>
 * Examples:
 *     Cancel 23_808    -- cancels the transaction which CFDP node 23 started
 *                         and assigned transaction-sequence-number = 808.
 *                         (a transaction is uniquely identified by the 
 *                         combination of the Source-ID and trans-seq-number)
 *     Cancel 101.158_3 -- ID=101.158, trans-seq-number=3.
 *     Cancel all       -- cancels all transactions.
 */

/* The Freeze and Thaw Requests take no arguments:
 *     Freeze
 *     Thaw
 */



/***********/
/*** MIB ***/
/***********/

/* This section applies to the routines 'cfdp_get_mib_parameter' and
 * 'cfdp_set_mib_parameter'.
 */

/* Char string representations of MIB Local parameters (case-insensitive) */
#define MIB_MY_ID "MY_ID"
#define MIB_ISSUE_EOF_RECV "ISSUE_EOF_RECV"
#define MIB_ISSUE_EOF_SENT "ISSUE_EOF_SENT"
#define MIB_ISSUE_FILE_SEGMENT_RECV "ISSUE_FILE_SEGMENT_RECV"
#define MIB_ISSUE_FILE_SEGMENT_SENT "ISSUE_FILE_SEGMENT_SENT"
#define MIB_ISSUE_RESUMED "ISSUE_RESUMED"
#define MIB_ISSUE_SUSPENDED "ISSUE_SUSPENDED"
#define MIB_ISSUE_TRANSACTION_FINISHED "ISSUE_TRANSACTION_FINISHED"
#define MIB_RESPONSE_TO_FAULT "RESPONSE_TO_FAULT"

/* Char string representations of MIB Remote parameters (case-insensitive) */
#define MIB_ACK_LIMIT "ACK_LIMIT"
#define MIB_ACK_TIMEOUT "ACK_TIMEOUT"
#define MIB_INACTIVITY_TIMEOUT "INACTIVITY_TIMEOUT"
#define MIB_NAK_LIMIT "NAK_LIMIT"
#define MIB_NAK_TIMEOUT "NAK_TIMEOUT"
#define MIB_SAVE_INCOMPLETE_FILES "SAVE_INCOMPLETE_FILES"

/* Char string representation of File Chunk size parameter */
#define MIB_OUTGOING_FILE_CHUNK_SIZE "OUTGOING_FILE_CHUNK_SIZE"

/* Char string representations of MIB parameter values
 *   boolean - "YES" or "NO"
 *   entity-ID - a dotted-decimal string; e.g. "10", "11", "101.34".
 *   numbers - read/written via calls to sscanf/sprintf using "%lu" format.
 */

/* Examples:
 *   cfdp_set_mib_parameter (MIB_MY_ID, "10");   
 *   cfdp_set_mib_parameter (MIB_ACK_TIMEOUT, "15");    // 15 seconds
 *   cfdp_set_mib_parameter (MIB_ISSUE_EOF_SENT, "YES");
 */

#endif
