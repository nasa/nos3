/* FILE: cfdp_requires.h -- subroutines required by the CFDP library.
 *   the CFDP library.
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
 * LAST MODIFIED:  2007_05_21
 * ORIGINAL PROGRAMMER:  Tim Ray 301-286-0581
 * SUMMARY:
 *     The library uses a 'callback' mechanism for the implementation-dependent
 *   services that it requires.  The calling interface for each callback
 *   routine is defined in this file.  The library user calls the routines
 *   in this file to register their callback routines.  
 * NOTES:
 *     1) There are 3 callback routines that the library user must register;
 *   these handle the output of PDUs.  The library user must register 
 *   these 3 callbacks before requesting any services from the library.
 *     2) In all other cases, the library includes a default callback.
 *   The library user may either accept the default (no action required)
 *   or override the default by registering their own callback.
 * CHANGES:
 *   2006_08_24 Tim Ray
 *     - Goal: Protect the library's internal data structures from being
 *       modified within a callback routine.
 *       So, made the 'TRANS_STATUS' pointer a 'const' argument in the
 *       callback routine that handles Indications.
 *   2006_09_08 Tim Ray
 *     - The name of the generic data structure that is passed to the
 *       routine "pdu_output_send" is now "CFDP_DATA" rather than "DATA".
 *       This avoids a name-conflict within the ASIST ground system.
 *   2007_05_08 Tim Ray
 *     - Modified the prototype for the 'pdu_output__ready' callback
 *       (so that engine user has more information).
 *   2007_05_11 Tim Ray
 *     - Modified the prototype for the Indication callback.  The
 *       transaction-status is now passed by-value rather than by-reference.
 *   2007_05_21 Tim Ray
 *     - Modified the prototype for the 'pdu_output__send' callback: the
 *       pdu is now passed by reference rather than by value.
 */

/* List of callback routines that can be registered with the CFDP library:
 *
 *    ---- Required ----
 *    pdu_output_open          - connect to specified CFDP partner
 *    pdu_output_ready         - ok to send PDU to this partner now?
 *    pdu_output_send          - send given PDU to specified partner
 *
 *    ---- Optional ----
 *    indication               - respond to given Indication
 *    printf                   - output a message (actually several levels)
 *    fopen                    - Posix file access
 *    fseek                    -   "
 *    fread                    -   "
 *    fwrite                   -   "
 *    feof                     -   "
 *    fclose                   -   "
 *    rename                   - Posix (rename a file)
 *    remove                   - Posix (remove a file)
 *    tmpnam                   - Posix (choose temporary file name)
 *    file_size                - what is this file's size (in bytes)
 *    is_file_segmented        - is this file stored as segments?
 */


#ifndef H_CFDP_REQUIRES
#define H_CFDP_REQUIRES 1

#include "cfdp_data_structures.h"



/***************************************/
/*** Protocol Data Unit (PDU) output ***/
/***************************************/

/* This section specifies the interface for output of CFDP PDUs from 
 * 'my' CFDP entity to partner entities.  The library user must implement
 * these three callback routines (and register them before requesting any
 * services from the library).
 */

void register_pdu_output_open (boolean (*function) (ID my_id, ID partner_id));
/* WHEN:  The callback routine is called once at the start of each transaction.
 * WHAT:  If it has not already done so, it attempts to initialize the 
 *   communication path between my CFDP entity and the specified partner 
 *   entity.  If successful, returns 1; otherwise, 0.
 */

void register_pdu_output_ready 
(boolean (*function) (PDU_TYPE pdu_type, TRANSACTION trans_id, ID partner_id));
/* WHEN:  The callback routine is called at least once prior to the output of
 *   each CFDP PDU.  
 * WHAT:  It returns 1 if the communication path to the specified CFDP partner
 * is ready for another PDU of the specified type; otherwise, 0.   
 * NOTE: The id of the transaction requesting this info is also provided.
 */

void register_pdu_output_send (void (*function) 
			       (TRANSACTION trans, 
				ID partner_id, 
				CFDP_DATA *pdu));
/* WHEN:  The callback routine is called each time the library outputs a PDU.
 * WHAT:  The callback routine is responsible for outputting the given PDU
 *   to the specified partner.  The Transaction-ID argument is provided as a
 *   courtesy to the library user; it can be ignored.
 * NOTE:  The library meters out PDUs (i.e. it doesn't call the 
 *   'pdu_output_send' callback routine until getting a green-light from 
 *   the 'pdu_output_ready' callback routine).
 */



/*******************/
/*** Indications ***/
/*******************/

/* This callback routine allows the library user to provide their own
 * response to Indications (either in place of, or in addition to, the
 * default library response).
 * BACKGROUND:  CFDP specifies a discrete set of Indications that are to
 *   be issued by the protocol to the User.  For example, when a transaction
 *   completes, an Indication is issued.  CFDP does not specify what actions to
 *   take in response to each Indication; that is implementation-dependent.  
 * NOTE:  Independent of this callback routine, the library will output 
 *   a simple text message in response to each Indication (unless the 
 *   'MSG_INDICATIONS' message-class has been disabled in 'cfdp_config.h').
 */

void register_indication (void (*function) 
			  (INDICATION_TYPE, TRANS_STATUS));
/* WHEN:  This callback routine is called whenever the CFDP protocol issues
 *   an Indication (e.g. at transaction startup and shutdown).
 * WHAT:  The callback routine is given an indication-type and transaction 
 *   status. The action taken is up to the implementer of the callback routine;
 *   nothing is required by CFDP.
 */



/**************/
/*** Printf ***/
/**************/

/* The library uses callback routines to output messages.  There are 4
 * callback routines - one for each severity-level (debug, info, warning,
 * and error).  All of the message callbacks match the POSIX 'printf'
 * routine.  The library user may register a *separate* callback routine for 
 * each severity level.  Or, register a *single* callback routine to handle
 * all levels.  Or, accept the default callback, which is 'printf'.
 */

void register_printf (int (*function) (const char *, ...));
/* WHEN:  The callback routine is called whenever the library has a message
 *   to output to the user.
 * WHAT:  Up to the implementer.
 * NOTE:  This routines registers a *single* callback routine that will
 *   be used for all messages (i.e. all severity levels).
 */

/* These routines allow the library user to register a *separate* 
 * callback routine for each severity-level.
 */
void register_printf_debug (int (*function) (const char *, ...));
void register_printf_info (int (*function) (const char *, ...));
void register_printf_warning (int (*function) (const char *, ...));
void register_printf_error (int (*function) (const char *, ...));


/*************************/
/*** Virtual Filestore ***/
/*************************/

/* The CFDP library provides a default Virtual Filestore implementation
 * that is intended to be acceptable as-is for those library users with
 * a standard filesystem.
 * NOTE:  An alias is used (i.e. 'CFDP_FILE' in place of 'FILE').  
 *   This alias is defined at compile-time in 'cfdp_config.h'.
 */


/* This first set matches the C library file-access routines, and the
 * default callbacks are the actual C library routines.
 */

void register_fopen (CFDP_FILE *(*function) (const char *name, 
                                             const char *mode));

void register_fseek (int (*function) (CFDP_FILE *file, long int offset, 
                                      int whence));

void register_fread (size_t (*function) (void *buffer, size_t size, 
                                         size_t count, CFDP_FILE *file));

void register_fwrite (size_t (*function) (const void *buff, size_t size, 
                                          size_t count, CFDP_FILE *file));

void register_feof (int (*function) (CFDP_FILE *file));

void register_fclose (int (*function) (CFDP_FILE *file));


/* This second set matches Posix-compliant routines.  The default callbacks
 * are the Posix routines for 'rename' and 'remove', and a custom routine
 * for 'tmpnam'.
 */

void register_rename (int (*function) (const char *current, const char *new));
/* WHEN:  The callback is called once each time a file is received and
 *   accepted.   (Renames the temporary file to its permanent name)
 * WHAT:  Renames a file from the given current name to the specified new name.
 */

void register_remove (int (*function) (const char *name));
/* WHEN:  The callback is called once each time a file is received and
 *   rejected (i.e. the file was not successfully received).
 * WHAT:  Deletes the specified file.
 */

void register_tmpnam (char *(*function) (char *string));
/* WHEN:  The callback is called once at the beginning of each incoming
 *   file transfer.
 * WHAT:  Determines a unused file-name that may be used as a temporary file,
 *   stores it in the given string.  The 'return status' is a pointer to the
 *   given string.
 */


/* This third set consists of specialized routines. */

void register_file_size (u_int_4 (const char *file_name));
/* WHEN:  The callback is called once at the beginning of each outgoing
 *   file transfer.
 * WHAT:  Returns the file size (in bytes) of the specified file.
 */

void register_is_file_segmented (boolean (const char *file_name));
/* WHEN:  Not currently used; this is a placeholder for possible future use. */

#endif
