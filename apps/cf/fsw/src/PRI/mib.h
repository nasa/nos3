/* FILE:  mib.h   Specs for Management Information Base (MIB) access.
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
 * BACKGROUND:  The MIB is a database of CFDP configuration parameters.
 *   CFDP defines two classes of MIB parameters:
 *     "Local entity" parameters apply to all transactions (regardless of who
 *   the CFDP partner is), and there is only one value for each parameter.
 *     "Remote entity" parameters apply to one specific CFDP partner.
 *   Each of our CFDP partners may have a different setting for these
 *   parameters.
 * SUMMARY:  This module allows clients to both set and get the value of
 *   any MIB parameter.  There is only one routine for setting an MIB
 *   parameter (see the first routine below); all the others are for
 *   getting the value of an MIB parameter.  
 * CHANGES:
 *   2007_07_31 Tim Ray
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 */

#ifndef H_MIB
#define H_MIB

#include "cfdp.h"
#include "cfdp_private.h"


/*-------------------------------------*/
/* Run-time modification of MIB values */
/*-------------------------------------*/

boolean mib__set_parameter (const char *param, const char *value);
/* WHAT IT DOES:  Matches the publicly available specs (see 
 *   'cfdp_provides.h').
 * NOTES:
 *   1) The parameter can be either a Local or Remote parameter.
 *   2) If given a Local parameter, the Local parameter set is modified.
 *   3) If given a Remote parameter, the default Remote parameter set is 
 *      modified.    (to modify a specific Remote entity's settings, 
 *      use 'cfdp_set_remote_parameter').
 */ 

boolean mib__get_parameter (const char *param_in, char *value);
/* WHAT IT DOES:  Matches the publicly available specs (see 
 *   'cfdp_provides.h').
 */


/*----------------------------------------*/
/* Local Entity configuration information */
/*----------------------------------------*/

boolean mib__issue_eof_recv (void);
boolean mib__issue_eof_sent (void);
boolean mib__issue_file_segment_recv (void);
boolean mib__issue_file_segment_sent (void);
boolean mib__issue_transaction_finished (void);
boolean mib__issue_suspended (void);
boolean mib__issue_resumed (void);
/* Response to each possible Protocol Error (aka "Condition Code") */
RESPONSE mib__response (CONDITION_CODE condition_code);


/*-----------------------------------------*/
/* Remote Entity configuration information */
/*-----------------------------------------*/

/* Each of these routines returns the value of a specific configuration
 * parameter for a specified remote CFDP entity.
 * Note:  The caller is allowed to provide a 'NULL' node_id; in this case,
 * each routine returns a default value.  
 * NOTE:  For now, the given node_id is ignored (i.e. the default value is
 * always returned); it is here so that the interface will support future 
 * enhancement.
 */
u_int_4 mib__ack_limit (ID *node_id);
u_int_4 mib__ack_timeout (ID *node_id);
u_int_4 mib__inactivity_timeout (ID *node_id);
u_int_4 mib__nak_limit (ID *node_id);
u_int_4 mib__nak_timeout (ID *node_id);

u_int_4 mib__outgoing_file_chunk_size (ID *node_id);
boolean mib__save_incomplete_files (ID *node_id);

/*----------------*/
/* Extra routines */
/*----------------*/

char *mib__as_string (void);
/* WHAT IT DOES:  Returns a character string that contains the setting of
 *   each mib parameter.
 */

ID mib__get_my_id (void);
/* WHAT IT DOES:  Returns my own (i.e. local) entity-ID. */

void mib__set_my_entity_id (char *entity_id_as_string);
/* WHAT IT DOES:  This routine allows the local entity-id to be set.
 * EXAMPLES:
 *    mib__set_my_entity_id ("23");
 *    mib__set_my_entity_id ("12.52.23.45");
 */

boolean mib__set_local_parameter (char *param, char *setting);
 /* WHAT IT DOES:  Sets the specified parameter to the given setting.
 *   The '#defines' below associate a string with selected MIB parameters.
 * EXAMPLE:  mib__set_parameter ("ACK_TIMEOUT", "10");    (10 seconds)
 * RETURN STATUS:  1 if successful; 0 otherwise.
 */

#endif
