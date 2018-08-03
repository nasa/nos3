/* FILE: validate_user_config.h -- this is a non-typical header file.
 *   It does not define the specs for a module.  
 *   Instead, it attempts to validate the contents of a header file 
 *   (cfdp_config.h) that is set up by the library user.  The idea is
 *   to notice things at compile-time rather that could cause problems
 *   at run-time.  (The validation is performed *within* this header file).
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
 *   2007_07_31 Tim Ray
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 */


#ifndef H_VALIDATE_USER_CONFIG
#define H_VALIDATE_USER_CONFIG 1

/*-----------------------------------------------*/
/* Validate library configuration (as set up by  */
/* the library user in the file 'cfdp_config.h') */
/* Mostly this involves ensuring that a value    */
/* has been supplied for all required parameters.*/
/*-----------------------------------------------*/

#include "cfdp_config.h"

#ifndef boolean
#error 'boolean' must be defined in 'cfdp_config.h'.
#endif

#ifndef u_int_1
#error 'u_int_1' must be defined in 'cfdp_config.h'.
#endif

#ifndef u_int_2
#error 'u_int_2' must be defined in 'cfdp_config.h'.
#endif

#ifndef u_int_4
#error 'u_int_4' must be defined in 'cfdp_config.h'.
#endif

#ifndef s_int_1
#error 's_int_1' must be defined in 'cfdp_config.h'.
#endif

#ifndef s_int_2
#error 's_int_2' must be defined in 'cfdp_config.h'.
#endif

#ifndef s_int_4
#error 's_int_4' must be defined in 'cfdp_config.h'.
#endif

#ifndef MAX_CONCURRENT_TRANSACTIONS
#error 'MAX_CONCURRENT_TRANSACTIONS' must be defined in 'cfdp_config.h'.
#endif

#ifndef MAX_GAPS_PER_TRANSACTION
#error 'MAX_GAPS_PER_TRANSACTION' must be defined in 'cfdp_config.h'.
#endif

#ifndef MAX_FILE_NAME_LENGTH
#error 'MAX_FILE_NAME_LENGTH' must be defined in 'cfdp_config.h'.
#endif

#ifndef MAX_ID_LENGTH
#error 'MAX_ID_LENGTH' must be defined in 'cfdp_config.h'.
#endif

#ifndef MAX_FILE_CHUNK_SIZE
#error 'MAX_FILE_CHUNK_SIZE' must be defined in 'cfdp_config.h'.
#endif

#ifndef MAX_PDU_LENGTH
#error 'MAX_PDU_LENGTH' must be defined in 'cfdp_config.h'.
#endif

#ifndef MAX_DATA_LENGTH
#error 'MAX_DATA_LENGTH' must be defined in 'cfdp_config.h'.
#endif

#ifndef CFDP_FILE
#error 'CFDP_FILE' must be defined in 'cfdp_config.h'.
#endif

#ifndef DEFAULT_TEMP_FILE_NAME_PREFIX
#error 'DEFAULT_TEMP_FILE_NAME_PREFIX' must be defined in 'cfdp_config.h'.
#endif

#ifndef MAX_TEMP_FILE_NAME_LENGTH
#error 'MAX_TEMP_FILE_NAME_LENGTH' must be defined in 'cfdp_config.h'.
#endif

#ifndef DEFAULT_MY_ID
#error 'DEFAULT_MY_ID' must be defined in 'cfdp_config.h'.
#endif

#ifndef DEFAULT_RESPONSE_TO_FAULT
#error 'DEFAULT_RESPONSE_TO_FAULT' must be defined in 'cfdp_config.h'.
#endif

#ifndef DEFAULT_ACK_LIMIT
#error 'DEFAULT_ACK_LIMIT' must be defined in 'cfdp_config.h'.
#endif

#ifndef DEFAULT_ACK_TIMEOUT
#error 'DEFAULT_ACK_TIMEOUT' must be defined in 'cfdp_config.h'.
#endif

#ifndef DEFAULT_INACTIVITY_TIMEOUT
#error 'DEFAULT_INACTIVITY_TIMEOUT' must be defined in 'cfdp_config.h'.
#endif

#ifndef DEFAULT_NAK_LIMIT
#error 'DEFAULT_NAK_LIMIT' must be defined in 'cfdp_config.h'.
#endif

#ifndef DEFAULT_NAK_TIMEOUT
#error 'DEFAULT_NAK_TIMEOUT' must be defined in 'cfdp_config.h'.
#endif

#ifndef DEFAULT_OUTGOING_FILE_CHUNK_SIZE
#error 'DEFAULT_OUTGOING_FILE_CHUNK_SIZE' must be defined in 'cfdp_config.h'.
/* For this parameter, the *value* is also validated: */
#elif DEFAULT_OUTGOING_FILE_CHUNK_SIZE > MAX_FILE_CHUNK_SIZE
#error 'DEFAULT_OUTGOING_FILE_CHUNK_SIZE' cannot be greater than\
 'MAX_FILE_CHUNK_SIZE'.
#endif

#ifndef DEFAULT_SAVE_INCOMPLETE_FILES
#error 'DEFAULT_SAVE_INCOMPLETE_FILES' must be defined in 'cfdp_config.h'.
#endif

#if IS_DYNAMIC_ALLOCATION_ENABLED==1
#error The engine no longer supports DYNAMIC_ALLOCATION (see 'cfdp_config.h').
#endif

#endif
