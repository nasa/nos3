/* FILE: cfdp_provides.h -- subroutines provided by the CFDP library.
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
 * LAST MODIFIED:  2007_07_09
 * ORIGINAL PROGRAMMER:  Tim Ray 301-286-0581
 * SUMMARY:  This file specifies all subroutines provided by the CFDP library.
 * CHANGES:
 *   2006_08_14 Tim Ray
 *     - Added a '#define' that identifies the engine version.
 *       Note:  I'm not going to add a 'change' when this '#define' is 
 *       updated in the future.   (See 'CFDP_ENGINE_VERSION').
 *   2006_09_08 Tim Ray
 *     - Changed 'DATA' to 'CFDP_DATA'.  (Solves ASIST name-conflict)
 *   2006_11_27 Tim Ray
 *     - Moved the #define of CFDP_ENGINE_VERSION to the private code.
 *   2007_05_03 Tim Ray
 *     - Added routines to support external file transfer.
 *   2007_05_10 Tim Ray
 *     - Added a routine to provide access to the status of any transaction
 *       at any time.
 *     - Added a routine to convert an Indication-Type to a char-string.
 *   2007_05_16 Tim Ray
 *     - Organized the routines better.
 *     - Added a few new routines -- 'cfdp_transaction_timer_status' and
 *       'cfdp_transaction_progress'.
 *   2007_05_30 Tim Ray
 *     - Added the routine 'cfdp_reset_totals' (resets statistical totals).
 *   2007_06_01 Tim Ray
 *     - Added 'cfdp_transaction_gaps_as_string'.
 *   2007_06_04 Tim Ray
 *     - Added 2 routines:  'cfdp_are_these_ids_equal' and
 *       'cfdp_are_these_trans_equal'.
 *   2007_06_17 Tim Ray
 *     - Added 'cfdp_is_a_trans_being_cancelled' to enable automated testing.
 *   2007_12_04 Tim Ray
 *     - Added support for enabling/disabling message classes on-the-fly.
 */

/* List of routines provided for library users:
 *     
 *    ---- core routines ---
 *    cfdp_give_pdu                    - to pass an incoming PDU to the lib
 *    cfdp_give_request                - to pass a User Request to the lib
 *    cfdp_cycle_all_transactions      - meter outgoing PDUs, check timers.
 *
 *    ---- configure the engine on-the-fly ----
 *    cfdp_set_mib_parameter           - to set an MIB parameter
 *    cfdp_get_mib_parameter           - to get the value of an MIB param
 *    cfdp_mib_as_string               - entire MIB as a character string
 *    cfdp_enable_message_class        - enable a class of messages
 *    cfdp_disable_message_class       - disable a class of messages
 *    cfdp_is_message_class_enabled    - check if message class is enabled
 *
 *    ---- get status of the engine -----
 *    cfdp_summary_status              - summary of all transactions' status
 *
 *    ---- get status of one particular transaction ----
 *    cfdp_transaction_status          - overall status
 *    cfdp_transaction_timer_status    - Ack/Nak/Inactivity-timer status
 *    cfdp_transaction_progress        - progress of file-data transfer
 *    cfdp_transaction_gaps_as_string  - list of filedata gaps
 *
 *    ---- convert between "computer" data and readable text strings ----
 *    cfdp_condition_as_string         - converts enumerated type to string
 *    cfdp_id_as_string                - converts ID structure to string
 *    cfdp_id_from_string              - converts string to ID structure
 *    cfdp_indication_type_as_string   - converts enumerated type to string
 *    cfdp_response_as_string          - converts structure to string
 *    cfdp_role_as_string              - converts enumerated type to string
 *    cfdp_trans_as_string             - converts structure to string
 *    cfdp_trans_from_string           - converts string to structure
 *
 *    ---- miscellaneous ----
 *    cfdp_are_these_ids_equal         - compares 2 ids
 *    cfdp_are_these_trans_equal       - compares 2 transaction-ids
 *    cfdp_memory_use_per_trans        - (for storage of transaction status)
 *    cfdp_reset_totals                - resets the statistical totals
 *
 *    ---- support for external file transfer ----
 *    cfdp_open_external_file_xfer     - engine user will output Filedata
 *    cfdp_close_external_file_xfer    - to have engine resume normal ops
 *    cfdp_set_file_checksum           - to give file checksum to engine
 *
 *    ---- needed for automated unit testing of the engine ----
 *    cfdp_is_a_trans_being_cancelled  - is a transaction being cancelled?
 *    cfdp_last_condition_code         - status of last transaction to complete
 *    cfdp_set_trans_seq_num           - sets the transaction-sequence-number
 */


#ifndef H_CFDP_PROVIDES
#define H_CFDP_PROVIDES 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cfdp_data_structures.h"
#include "cfdp_syntax.h"



/*********************/
/*** Core routines ***/
/*********************/

/* All CFDP applications that are based on this library must call all
 * of these core routines.
 */

boolean cfdp_give_pdu (CFDP_DATA pdu);
/* BIGGER PICTURE:  The library user receives incoming PDUs from CFDP
 *   partner entities, and calls this routine to pass each incoming PDU
 *   to the library.
 * WHAT IT DOES:  Responds to the PDU as required by the CFDP protocol.
 * RETURN STATUS:  '1' indicates success.  '0' indicates failure
 */

boolean cfdp_give_request (const char *request_as_string);
/* BIGGER PICTURE:  The library user receives User Requests from the
 *   User, and calls this routine to pass each User Request to the library.
 * WHAT IT DOES:  Responds to the Request as required by the CFDP protocol.
 *   (the syntax of the character strings is described in 'cfdp_syntax.h')
 * RETURN STATUS:  '1' indicates success.  '0' indicates failure (e.g.
 *   syntax error).
 * NOTE:  The input string may be up to 'MAX_REQUEST_STRING_LENGTH' chars.
 */
#define MAX_REQUEST_STRING_LENGTH 128

void cfdp_cycle_each_transaction (void);
/* BIGGER PICTURE:  The library uses a polling mechanism to meter out chunks 
 *   of file-data as fast as the library user's 'pdu_output' routines allow.
 *   Calling this routine frequently ensures that the output of one file-chunk
 *   is quickly followed by the output of the next file-chunk.
 * WHAT IT DOES:  Performs the polling required in this CFDP implementation.
 */



/***************************************/
/*** Configure the engine on-the-fly ***/
/***************************************/

/* The MIB contains the values of CFDP configuration parameters.
 * Library users can specify the default value of every MIB parameter
 * (see 'cfdp_config.h').  The two routines in this section allow
 * the library user to modify and monitor the MIB settings at run-time.
 */

boolean cfdp_set_mib_parameter (const char *param, const char *value);
/* WHAT IT DOES:  Attempts to set the specified parameter to the specified
 *   value.  If successful, returns 1; otherwise, 0.
 *   (the syntax of the character strings is described in 'cfdp_syntax.h')
 * NOTES:
 *   1) The given param string may be up to 'MAX_MIB_PARAMETER_LENGTH' chars.
 *   2) The given value string may be up to 'MAX_MIB_VALUE_LENGTH' chars.
 */ 

boolean cfdp_get_mib_parameter (const char *param, char *value);
/* WHAT IT DOES:  If the given 'param' string is valid, the return status 
 *   is 1 and the specified parameter's current value is returned via the 
 *   'value' argument.  Otherwise, the return status is 0.
 *   (the syntax of the character strings is described in 'cfdp_syntax.h')
 * NOTES:  
 *   1) The given param string may be up to 'MAX_MIB_PARAMETER_LENGTH' chars.
 *   2) The returned string may be up to 'MAX_MIB_VALUE_LENGTH' chars long.
 */
#define MAX_MIB_PARAMETER_LENGTH 64
#define MAX_MIB_VALUE_LENGTH 64

char *cfdp_mib_as_string (void);
/* WHAT IT DOES:  Returns the current MIB settings represented as a 
 *   single character string (of unspecified format).
 * NOTE:  The returned char string may be up to 'MAX_MIB_AS_STRING_LENGTH' 
 *   chars long.
 */
#define MAX_MIB_AS_STRING_LENGTH 10240

boolean cfdp_enable_message_class (int class);
/* WHAT IT DOES:  If the given message-class is recognized, the message-
 *   class is enabled and the return status is 1.  Otherwise, no action
 *   is taken and the return status is 0.   (Classes defined below)
 * NOTE:  The class 0 is a special case that means "all classes".
 */

boolean cfdp_disable_message_class (int class);
/* WHAT IT DOES:  If the given message-class is recognized, the message-
 *   class is disabled and the return status is 1.  Otherwise, no action
 *   is taken and the return status is 0.   (Classes defined below)
 * NOTE:  The class 0 is a special case that means "all classes".
 */

boolean cfdp_is_message_class_enabled (int class);
/* WHAT IT DOES:  If the given message-class is recognized, the return
 *   status is either 1 (enabled) or 0 (disabled).  Otherwise, 0 is returned.
 */

/* The following message classes are defined... */

/* If enabled, a message is output each time an Indication occurs */
#define CFDP_MSG_INDICATIONS 1

/* This message class provides information needed to debug problems within
 * the engine (related to memory use for storing 'Nak' PDUs).
 */
#define CFDP_MSG_DEBUG_MEMORY_USE 2

/* This message class provides information needed to debug problems within
 * the engine (related to maintenance of lists of missing data).
 */
#define CFDP_MSG_DEBUG_NAK 3

/* If enabled, the raw contents of incoming and outgoing PDUs are displayed. */
#define CFDP_MSG_DEBUG_PDU 4

/* If enabled, a message is output each time a Filedata PDU is received */
#define CFDP_MSG_PDU_FILEDATA 5

/* If enabled, a message is output each time a File Directive PDU is sent
 * or received.
 */
#define CFDP_MSG_PDU_NON_FILEDATA 6

/* If enabled, a message is output each time a Filedata PDU is retransmitted */
#define CFDP_MSG_PDU_RETRANSMITTED_FD 7

/* If enabled, a message is output every time a state machine runs 
 * (lots of messages!).
 */
#define CFDP_MSG_STATE_ALL 8

/* If enabled, a message is output each time their is a state change within
 * a state machine.
 */
#define CFDP_MSG_STATE_CHANGE 9
    


/********************************/
/*** Get status of the engine ***/
/********************************/

SUMMARY_STATUS cfdp_summary_status (void);
/* WHAT IT DOES:  Returns a summary of the status of all transactions. */

/* Each routine in this group expresses a given data structure as a
 * character string.  (The exact string format is not specified)
 * WARNING:  All returns from a particular routine point to the same
 *   character string.  In other words, each call to a particular routine
 *   writes over the result of the previous call to that same routine.
 *   Therefore, be sure to copy (or print) the result from one call before 
 *   making another call to the same routine.
 * NOTES:
 *   1) Each returned character string may be up to 'MAX_AS_STRING_LENGTH'
 *      bytes long.
 *   2) When an ID is expressed as a string, it is in 'dotted-decimal' format.
 *      Each byte's value is expressed as a decimal number, and a dot is
 *      put between decimal numbers.  Examples:  "10"  "101.151"
 *   3) When a TRANSACTION is expressed as a string, the format is 
 *      id-underscore-transaction_sequence_number.   Examples:
 *             "10_1" -- transaction number 1 at CFDP entity 10.
 *             "101.151_803" -- transaction number 803 at CFDP entity 101.151.
 */



/************************************************/
/*** Get status of one particular transaction ***/
/************************************************/

/* The structure 'TRANS_STATUS' holds general status of a transaction.
 * The first routine returns that status.  The remaining routines return
 * more specialized information.
 */

boolean cfdp_transaction_status (TRANSACTION transaction, 
				 TRANS_STATUS *trans_status);
/* WHAT IT DOES:  If the specified transaction exists, the return-status
 *   is '1' and the engine returns the transaction's status in 'trans_status'.
 *   Otherwise, the return-status is '0', and the 'trans_status' argument
 *   is meaningless.
 */

boolean cfdp_transaction_timer_status (TRANSACTION transaction,
				       TIMER_TYPE *type, 
				       u_int_4 *seconds_remaining);
/* WHAT IT DOES:  If the specified transaction exists, the return-status
 *   is '1' and the engine returns the timer status.
 *   Otherwise, the return-status is '0', and the timer-status
 *   is meaningless.
 * OUTPUTS:  'type' tells you which timer is running, and 'seconds-remaining'
 *   the current countdown value.  e.g. Ack-timer is running and has 5 
 *   seconds until it expires.
 */

boolean cfdp_transaction_progress (TRANSACTION transaction,
				   u_int_4 *bytes_transferred,
				   u_int_4 *file_size);
/* WHAT IT DOES:  If the specified transaction exists, then the return-status
 *   is '1', 'bytes_transferred' indicates how many bytes of file-data 
 *   have been transferred, and 'file_size' indicates the size of the file
 *   in bytes.
 *   Otherwise, the return-status is '0', and the data returned
 *   is meaningless.
 * NOTES:  If the local CFDP node is the sender, then 'bytes-transferred'
 *   indicates the number of bytes sent (but not necessarily received).
 *   If the local CFDP node is the receiver, then 'bytes-transferred'
 *   indicates the number of bytes received.
 *     If the local CFDP node is the receiver, the 'file_size' will be
 *   unknown until a Metadata or EOF pdu is received.  If the file-size
 *   is unknown, it will be given the value of zero.
 */

boolean cfdp_transaction_gaps_as_string (TRANSACTION trans, char *string,
					 int max_string_length);
/* WHAT IT DOES:  If the specified transaction exists, then the return-status
 *   is '1', and 'string' will contain a list of the filedata gaps for the
 *   given transaction.  Otherwise, the return-status is '0', and the 
 *   data returned is meaningless.
 * INPUTS:  First, which transaction is of interest?  Second, a pointer to
 *   memory where the output string will be written.  Third, the size of
 *   the memory (this routine will not write beyond the specified length).
 * EXAMPLES OF THE OUTPUT STRING:
 *  ""         -- no gaps
 *  "100-200"  -- one gap
 *  "100-200 5000-5100 8800-8900"  -- three gaps
 */



/*****************************************************************/
/*** Convert between "computer" data and readable text strings ***/
/*****************************************************************/

/* Each of these routines converts a structure to a string.  The string
 * may be up to 'max-as-string-length' chars long.
 */
#define MAX_AS_STRING_LENGTH 128
char *cfdp_condition_as_string (CONDITION_CODE cc);
char *cfdp_id_as_string (ID id);
char *cfdp_indication_type_as_string (INDICATION_TYPE indication_type);
char *cfdp_response_as_string (RESPONSE response);
char *cfdp_role_as_string (ROLE role);
char *cfdp_trans_as_string (TRANSACTION transaction);

/* Each of these routines performs the reverse conversion (string to struct).
 * The return status is 1 if successful; otherwise, 0.
 */
boolean cfdp_id_from_string (const char *value_as_dotted_string, ID *id);
boolean cfdp_trans_from_string (const char *string, TRANSACTION *trans);



/*********************/
/*** Miscellaneous ***/
/*********************/

boolean cfdp_are_these_ids_equal (ID id1, ID id2);
/* WHAT IT DOES:  Returns 1 if the given IDs are equal; 0 otherwise. */

boolean  cfdp_are_these_trans_equal (TRANSACTION t1, TRANSACTION t2);
/* WHAT IT DOES:  Returns '1' if the given transactions are equal (i.e.
 *   same source-ID and transaction-sequence-number); otherwise, '0'.
 */

u_int_4 cfdp_memory_use_per_trans (void);
/* WHAT IT DOES:  Answers the question "How much memory is used per
 *   transaction?".  (the answer will always be the same within a given
 *   program run)
 */

void cfdp_reset_totals (void);
/* WHAT IT DOES:  Resets those summary statistics that are "total counts"
 *   (for example, total_files_sent).   (Counts are reset to zero).
 */



/******************************/
/*** External File Transfer ***/
/******************************/

/* Caution:  These routines are for a special purpose.  They allow the
 * engine's implementation of the CFDP protocol to be disabled during the
 * initial Filedata transmissions and then resumed for the remainder of the
 * protocol.
 * Context:  Suppose you had to transfer files at a much higher datarate
 * than could be accomplished with a software implementation of CFDP 
 * running on a flight processor (say, 100 megabits/second).  You could
 * disable the engine's normal protocol while hardware blasts the initial
 * transmission of the file, then re-enable the engine to send the EOF
 * and respond to acks and naks.
 * Note:  If the engine does not output Filedata PDUs, it will not calculate
 * a file checksum.  If external file transfer is used, the engine user 
 * must calculate the file checksum and give it to the engine (by calling
 * the routine cfdp_set_file_checksum).
 */

boolean cfdp_open_external_file_xfer (TRANSACTION trans);
/* WHAT IT DOES:  For one particular transaction, tells the engine to disable
 *   its output of Filedata PDUs.
 * INPUT:  Which transaction?
 * RETURN STATUS:  '1' indicates acceptance.  '0' indicates rejection.
 *   Acceptance will only occur if the given transaction is known to the
 *   engine, the engine's role is 'Sender', and the engine has not sent
 *   an EOF PDU.
 * NOTES:  
 *   1) This routine should be called in response to the 'Transaction'
 *   Indication (or, in response to the 'Metadata_Sent' Indication).
 *   2) The engine still responds to Cancel Requests and Finished-Cancel PDUs
 *   for the given transaction.
 */

boolean cfdp_close_external_file_xfer (TRANSACTION trans);
/* WHAT IT DOES:  For one particular transaction, tells the engine that the
 *   engine user is done sending Filedata PDUs.  The engine resumes normal
 *   implementation of the CFDP protocol (it sends the EOF PDU and responds
 *   to acks and naks).
 * INPUT:  Which transaction?
 * RETURN STATUS:  1' indicates acceptance.  '0' indicates rejection.
 *   Acceptance will only occur if the given transaction is known to the
 *   engine, and external file transfer is open for that transaction.
 */

boolean cfdp_set_file_checksum (TRANSACTION trans, u_int_4 checksum);
/* WHAT IT DOES:  For one particular transaction, sets the file_checksum
 *   (that is put into the EOF PDU).
 * INPUT:  Which transaction?
 * RETURN STATUS:  1' indicates acceptance.  '0' indicates rejection.
 *   Acceptance will occur if the given transaction is known to the engine.
 */



/********************************************************/
/*** Support for automated unit testing of the engine ***/
/********************************************************/

boolean cfdp_is_a_trans_being_cancelled (void);
/* WHAT IT DOES:  Returns '1' if any tranaction is currently being 
 *   cancelled; otherwise, returns '0'.
 */

CONDITION_CODE cfdp_last_condition_code (void);
/* WHAT IT DOES:  Answers the question "what was the Completion-Code of 
 *   the most recently completed transaction?".  (answer via return status)
 */

void cfdp_set_trans_seq_num (u_int_4 value);
/* WHAT IT DOES:  This should only be used for regression-testing of the
 *   engine.  It is dangerous to call this routine.
 */

#endif
