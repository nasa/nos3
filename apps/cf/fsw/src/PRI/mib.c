/* FILE: mib.c
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
 * SPECS:  see mib.h
 * ORIGINAL PROGRAMMER:  Tim Ray 301-286-0581
 * CHANGES:
 *   2007_07_31 Tim Ray (TR)
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 *   2007_10_22 TR
 *     - Bug fix: in mib__get_parameter, a string-copy was done to a 
 *       maximum length of 'MAX_MIB_VALUE_LENGTH+1' instead of 
 *       'MAX_MIB_VALUE_LENGTH'.  This could write beyond the end of an array.
 *   2007_11_09 TR
 *     - Improved portability.  'sscanf' reads of '%lu' format no longer put 
 *       the data directly into an 'u_int_4' variable.  This fix avoids a
 *       core dump if the compiler uses 8 bytes for 'long int' variables
 *       (i.e. avoids writing 8 bytes into a 4 byte variable).
 *   2008_01_02 TR
 *     - Removed unneeded assertion.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cfdp.h"
#include "cfdp_private.h"

#define INITIALIZE_ONCE if (!m_initialized) { m__init(); m_initialized=1; }
#define STRING_TERMINATOR '\0'

/* Local entity parameters */
typedef struct
{
  boolean         issue_eof_recv;
  boolean         issue_eof_sent;
  boolean         issue_file_segment_recv;
  boolean         issue_file_segment_sent;
  boolean         issue_resumed;
  boolean         issue_suspended;
  boolean         issue_transaction_finished;
  ID              my_id;
  /* Response to each possible Protocol Error (aka "Condition Code") */
  u_int_1         response [NUMBER_OF_CONDITION_CODES];
} MIB_LOCAL;

/* Remote entity parameters */
typedef struct
{
  u_int_4         ack_limit;
  u_int_4         ack_timeout;
  u_int_4         inactivity_timeout;
  u_int_4         nak_limit;
  u_int_4         nak_timeout;
  u_int_4         outgoing_file_chunk_size;
  boolean         save_incomplete_files;
} MIB_REMOTE;

/* These variables are only known with this source file, but are shared
 * by all the routines within this source file.
 */
static boolean               m_initialized = NO;
static MIB_LOCAL             m_mib_local;
static MIB_REMOTE            m_mib_remote_default;





/*=r=************************************************************************/
static char *m__convert_string_to_uppercase (const char *string)
     /* WHAT IT DOES:  Converts the given char string to uppercase letters,
      *   and returns the result via the return status.
      */
#define MAX_LENGTH 256
   {
     int            i;
     int            length;
     static char    uppercase [MAX_LENGTH+1];
   /*------------------------------------------------------------*/

     /* Avoid writing past the allocated memory */
     length = strlen(string);
     if (length > MAX_LENGTH)
       {
         w_msg__ ("cfdp_engine: MIB string truncated (%u->%u)\n",
                  length, MAX_LENGTH);
       }

     /* Make the conversion */
     memset (uppercase, STRING_TERMINATOR, sizeof(uppercase));
     for (i=0; i<length; i++)
       uppercase[i] = (char) toupper ((int) string[i]);
     return (uppercase);
   }
#undef MAX_LENGTH



/*=r=************************************************************************/
static void m__init (void)
     /* WHAT IT DOES:  Initializes the settings of the Local Entity parameters 
      *   and the default Remote Entity parameters.
      */
   {
     int            i;
   /*------------------------------------------------------------*/

     /* Local-entity values */
     m_mib_local.issue_eof_recv = DEFAULT_ISSUE_EOF_RECV;
     m_mib_local.issue_eof_sent = DEFAULT_ISSUE_EOF_SENT;
     m_mib_local.issue_file_segment_recv = DEFAULT_ISSUE_FILE_SEGMENT_RECV;
     m_mib_local.issue_file_segment_sent = DEFAULT_ISSUE_FILE_SEGMENT_SENT;
     m_mib_local.issue_resumed = DEFAULT_ISSUE_RESUMED;
     m_mib_local.issue_suspended = DEFAULT_ISSUE_SUSPENDED;
     for (i=0; i<NUMBER_OF_CONDITION_CODES; i++)
       m_mib_local.response[i] = DEFAULT_RESPONSE_TO_FAULT;

     /* Default Remote-entity values */
     m_mib_remote_default.ack_limit = DEFAULT_ACK_LIMIT;
     m_mib_remote_default.ack_timeout = DEFAULT_ACK_TIMEOUT;
     m_mib_remote_default.inactivity_timeout = DEFAULT_INACTIVITY_TIMEOUT;
     m_mib_remote_default.nak_limit = DEFAULT_NAK_LIMIT;
     m_mib_remote_default.nak_timeout = DEFAULT_NAK_TIMEOUT;
     m_mib_remote_default.outgoing_file_chunk_size = 
       DEFAULT_OUTGOING_FILE_CHUNK_SIZE;
     m_mib_remote_default.save_incomplete_files = 
       DEFAULT_SAVE_INCOMPLETE_FILES;
   }



/*=r=************************************************************************/
static RESPONSE m__string_as_response (char *string)
     /* WHAT IT DOES:  Given a char string, it returns a Response.
      * EXAMPLE:  If given "CANCEL" it returns RESPONSE_CANCEL.
      */
   {
     static RESPONSE    response;
   /*------------------------------------------------------------*/
     if (!strcmp (string, "ABANDON"))
       response = RESPONSE_ABANDON;
     else if (!strcmp (string, "CANCEL"))
       response = RESPONSE_CANCEL;
     else if (!strcmp (string, "IGNORE"))
       response = RESPONSE_IGNORE;
     else if (!strcmp (string, "SUSPEND"))
       response = RESPONSE_SUSPEND;
     else
       {
         w_msg__ ("cfdp_engine: MIB: unrecognized fault response (%s) "
                  "converted to 'Cancel'\n", 
                  string);
         response = RESPONSE_CANCEL;
       }

     return (response);
   }



/*=r=************************************************************************/
char *mib__as_string (void)
{
  static char             string [MAX_MIB_AS_STRING_LENGTH];
  char                    substring [128];
  /*------------------------------------------------------------*/
  INITIALIZE_ONCE;
  strcpy (string, "*** MIB parameter settings ***\n");

  strcpy (substring, "   Entity-ID = ");
  APPEND (substring, cfdp_id_as_string(m_mib_local.my_id));
  APPEND (substring, "\n");
  APPEND (string, substring);

  sprintf (substring, "   Issue EOF-Sent Indication? = %u\n", 
           m_mib_local.issue_eof_sent);
  APPEND (string, substring);

  sprintf (substring, "   Issue EOF-Received Indication? = %u\n", 
           m_mib_local.issue_eof_recv);
  APPEND (string, substring);

  sprintf (substring, "   Issue File-Segment-Sent Indication? = %u\n", 
           m_mib_local.issue_file_segment_sent);
  APPEND (string, substring);

  sprintf (substring, "   Issue File-Segment-Received Indication? = %u\n", 
           m_mib_local.issue_file_segment_recv);
  APPEND (string, substring);

  /* <ADD_CODE> include the prescribed response to each possible Fault */

  /* <ADD_CODE> include "CRCs required on transmission" */

  sprintf (substring, "   (default) Ack-limit = %lu\n",
           m_mib_remote_default.ack_limit);
  APPEND (string, substring);

  sprintf (substring, "   (default) Ack-timeout = %lu\n",
           m_mib_remote_default.ack_timeout);
  APPEND (string, substring);

  sprintf (substring, "   (default) Nak-limit = %lu\n",
           m_mib_remote_default.nak_limit);
  APPEND (string, substring);

  sprintf (substring, "   (default) Nak-timeout = %lu\n",
           m_mib_remote_default.nak_timeout);
  APPEND (string, substring);

  sprintf (substring, "   (default) Inactivity-timeout = %lu\n",
           m_mib_remote_default.inactivity_timeout);
  APPEND (string, substring);

  sprintf (substring, 
           "   (default) Outgoing file chunk size = %lu (bytes)\n",
           m_mib_remote_default.outgoing_file_chunk_size);
  APPEND (string, substring);

  sprintf (substring, "   (default) Save-incomplete-files = %u\n",
           m_mib_remote_default.save_incomplete_files);
  APPEND (string, substring);

  return (string);
}



/*=r=************************************************************************/
u_int_4 mib__ack_limit (ID *node_id)
{
  /*------------------------------------------------------------*/
  INITIALIZE_ONCE;
  return (m_mib_remote_default.ack_limit);
}



/*=r=************************************************************************/
u_int_4 mib__ack_timeout (ID *node_id)
{
  /*------------------------------------------------------------*/
  INITIALIZE_ONCE;
  return (m_mib_remote_default.ack_timeout);
}



/*=r=************************************************************************/
u_int_4 mib__inactivity_timeout (ID *node_id)
{
  /*------------------------------------------------------------*/
  INITIALIZE_ONCE;
  return (m_mib_remote_default.inactivity_timeout);
}



/*=r=************************************************************************/
boolean mib__issue_eof_recv (void)
{
  /*------------------------------------------------------------*/
  INITIALIZE_ONCE;
  return (m_mib_local.issue_eof_recv);
}



/*=r=************************************************************************/
boolean mib__issue_eof_sent (void)
{
  /*------------------------------------------------------------*/
  INITIALIZE_ONCE;
  return (m_mib_local.issue_eof_sent);
}



/*=r=************************************************************************/
boolean mib__issue_file_segment_recv (void)
{
  /*------------------------------------------------------------*/
  INITIALIZE_ONCE;
  return (m_mib_local.issue_file_segment_recv);
}



/*=r=************************************************************************/
boolean mib__issue_file_segment_sent (void)
{
  /*------------------------------------------------------------*/
  INITIALIZE_ONCE;
  return (m_mib_local.issue_file_segment_sent);
}



/*=r=************************************************************************/
u_int_4 mib__outgoing_file_chunk_size (ID *node_id)
{
  /*------------------------------------------------------------*/
  INITIALIZE_ONCE;
  return (m_mib_remote_default.outgoing_file_chunk_size);
}



/*=r=************************************************************************/
boolean mib__save_incomplete_files (ID *node_id)
{
  /*------------------------------------------------------------*/
  INITIALIZE_ONCE;
  return (m_mib_remote_default.save_incomplete_files);
}



/*=r=************************************************************************/
ID mib__get_my_id (void)
{
  /*------------------------------------------------------------*/
  INITIALIZE_ONCE;
  return (m_mib_local.my_id);
}



/*=r=************************************************************************/
u_int_4 mib__nak_limit (ID *node_id)
{
  /*------------------------------------------------------------*/
  INITIALIZE_ONCE;
  return (m_mib_remote_default.nak_limit);
}



/*=r=************************************************************************/
u_int_4 mib__nak_timeout (ID *node_id)
{
  /*------------------------------------------------------------*/
  INITIALIZE_ONCE;
  return (m_mib_remote_default.nak_timeout);
}



/*=r=************************************************************************/
RESPONSE mib__response (CONDITION_CODE condition_code)
{
  /*------------------------------------------------------------*/
  INITIALIZE_ONCE;

  /* Validate input */
  if ((condition_code < 0) || (condition_code > 15))
    /* Invalid input; do something 'reasonable' */
    {
      w_msg__ ("cfdp_engine: Invalid input to 'mib__response' (%u); "
               "should be 0-15.\n",
               condition_code);
      return (RESPONSE_CANCEL);
    }

  return (m_mib_local.response[condition_code]);
}



/*=r=************************************************************************/
boolean mib__set_parameter (const char *param_in, const char *value_in)
   {
     int                i;
     ID                 id;
     unsigned long int  long_int;
     char               param [MAX_MIB_PARAMETER_LENGTH+1];
     u_int_4            requested_size;
     char               value [MAX_MIB_VALUE_LENGTH+1];
   /*------------------------------------------------------------*/
     INITIALIZE_ONCE;

     /*--------------------------*/
     /* Validate input arguments */
     /*--------------------------*/

     if (strlen(param_in) == 0)
       {
         e_msg__ ("cfdp_engine: can't set MIB parameter; "
                  "no parameter given.\n");
         return (0);
       }
     else if (strlen(param_in) > MAX_MIB_PARAMETER_LENGTH)
       {
         e_msg__ ("cfdp_engine: can't set MIB parameter; "
                  "parameter-string (%s) is too long.\n",
                  param_in);
         return (0);
       }
     else if (strlen(value_in) == 0)
       {
         e_msg__ ("cfdp_engine: can't set MIB parameter; no value given.\n");
         return (0);
       }
     else if (strlen(value_in) > MAX_MIB_PARAMETER_LENGTH)
       {
         e_msg__ ("cfdp_engine: can't set MIB parameter; "
                  "value-string (%s) is too long.\n",
                  value_in);
         return (0);
       }

     COPY (param, m__convert_string_to_uppercase (param_in));
     COPY (value, m__convert_string_to_uppercase (value_in));

     /*-----------------------------------------*/
     /* Compare input with each Local parameter */
     /*-----------------------------------------*/

     if (!strcmp (param, MIB_MY_ID))
       {
         if (cfdp_id_from_string (value, &id))
           {
             m_mib_local.my_id = id;
             i_msg__ ("cfdp_engine: entity-id set to '%s'.\n",
                      cfdp_id_as_string (m_mib_local.my_id));
           }
         else
           {
             e_msg__ ("cfdp_engine: can't set entity-id to illegal value "
                      "(%s).\n",
                      value);
             return (0);
           }
       }

     else if (!strcmp (param, MIB_ISSUE_EOF_RECV))
       {
         if (!strcmp (value, "YES"))
           {
             m_mib_local.issue_eof_recv = YES;
             i_msg__ ("cfdp_engine: 'issue_eof_recv' set to 'yes'.\n");
           }
         else if (!strcmp (value, "NO"))
           {
             m_mib_local.issue_eof_recv = NO;
             i_msg__ ("cfdp_engine: 'issue_eof_recv' set to 'no'.\n");
           }
         else
           {
             e_msg__ ("cfdp_engine: can't set 'issue_eof_recv'; "
                      "value (%s) must be 'yes' or 'no'.\n",
                      value_in); 
             return (0);
           }
       }

     else if (!strcmp (param, MIB_ISSUE_EOF_SENT))
       {
         if (!strcmp (value, "YES"))
           {
             m_mib_local.issue_eof_sent = YES;
             i_msg__ ("cfdp_engine: 'issue_eof_sent' set to 'yes'.\n");
           }
         else if (!strcmp (value, "NO"))
           {
             m_mib_local.issue_eof_sent = NO;
             i_msg__ ("cfdp_engine: 'issue_eof_sent' set to 'no'.\n");
           }
         else
           {
             e_msg__ ("cfdp_engine: can't set 'issue_eof_sent'; "
                      "value (%s) must be 'yes' or 'no'.\n",
                      value_in); 
             return (0);
           }
       }

     else if (!strcmp (param, MIB_ISSUE_FILE_SEGMENT_RECV))
       {
         if (!strcmp (value, "YES"))
           {
             m_mib_local.issue_file_segment_recv = YES;
             i_msg__ ("cfdp_engine: 'issue_file_segment_recv' set to 'yes'\n");
           }
         else if (!strcmp (value, "NO"))
           {
             m_mib_local.issue_file_segment_recv = NO;
             i_msg__ ("cfdp_engine: 'issue_file_segment_recv' set to 'no'.\n");
           }
         else
           {
             e_msg__ ("cfdp_engine: can't set 'issue_file_segment_recv'; "
                      "value (%s) must be 'yes' or 'no'.\n",
                      value_in); 
             return (0);
           }
       }

     else if (!strcmp (param, MIB_ISSUE_FILE_SEGMENT_SENT))
       {
         if (!strcmp (value, "YES"))
           {
             m_mib_local.issue_file_segment_sent = YES;
             i_msg__ ("cfdp_engine: 'issue_file_segment_sent' set to 'yes'\n");
           }
         else if (!strcmp (value, "NO"))
           {
             m_mib_local.issue_file_segment_sent = NO;
             i_msg__ ("cfdp_engine: 'issue_file_segment_sent' set to 'no'.\n");
           }
         else
           {
             e_msg__ ("cfdp_engine: can't set 'issue_file_segment_sent'; "
                      "value (%s) must be 'yes' or 'no'.\n",
                      value_in); 
             return (0);
           }
       }

     else if (!strcmp (param, MIB_ISSUE_RESUMED))
       {
         if (!strcmp (value, "YES"))
           {
             m_mib_local.issue_resumed = YES;
             i_msg__ ("cfdp_engine: 'issue_resumed' set to 'yes'.\n");
           }
         else if (!strcmp (value, "NO"))
           {
             m_mib_local.issue_resumed = NO;
             i_msg__ ("cfdp_engine: 'issue_resumed' set to 'no'.\n");
           }
         else
           {
             e_msg__ ("cfdp_engine: can't set 'issue_resumed'; "
                      "value (%s) must be 'yes' or 'no'.\n",
                      value_in); 
             return (0);
           }
       }

     else if (!strcmp (param, MIB_ISSUE_SUSPENDED))
       {
         if (!strcmp (value, "YES"))
           {
             m_mib_local.issue_suspended = YES;
             i_msg__ ("cfdp_engine: 'issue_suspended' set to 'yes'.\n");
           }
         else if (!strcmp (value, "NO"))
           {
             m_mib_local.issue_suspended = NO;
             i_msg__ ("cfdp_engine: 'issue_suspended' set to 'no'.\n");
           }
         else
           {
             e_msg__ ("cfdp_engine: can't set 'issue_suspended'; "
                      "value (%s) must be 'yes' or 'no'.\n",
                      value_in); 
             return (0);
           }
       }

     else if (!strcmp (param, MIB_ISSUE_TRANSACTION_FINISHED))
       {
         if (!strcmp (value, "YES"))
           {
             m_mib_local.issue_transaction_finished = YES;
             i_msg__ ("cfdp_engine: 'issue_transaction_finished' "
                      "set to 'yes'.\n");
           }
         else if (!strcmp (value, "NO"))
           {
             m_mib_local.issue_transaction_finished = NO;
             i_msg__ ("cfdp_engine: 'issue_transaction_finished' "
                      "set to 'no'.\n");
           }
         else
           {
             e_msg__ ("cfdp_engine: can't set 'issue_transaction_finished'; "
                      "value (%s) must be 'yes' or 'no'.\n",
                      value_in); 
             return (0);
           }
       }

     else if (!strcmp (param, MIB_RESPONSE_TO_FAULT))
       {
         if ((!strcmp (value, "IGNORE")) ||
             (!strcmp (value, "SUSPEND")) ||
             (!strcmp (value, "CANCEL")) ||
             (!strcmp (value, "ABANDON")))
           {
             for (i=0; i<NUMBER_OF_CONDITION_CODES; i++)
               /* WARNING:  This library uses the same response for all faults.
                *   The CFDP protocol allows the user to choose a different
                *   response to each fault.  So, this library is not fully
                *   compliant with the CFDP protocol.  However, the library
                *   developer considers this to be a practical shortcut that
                *   simplifies things.
                */
               m_mib_local.response[i] = m__string_as_response (value);
             i_msg__ ("cfdp_engine: 'response_to_fault' set to '%s'.\n",
                      cfdp_response_as_string (m_mib_local.response[0]));
           }
         else
           {
             e_msg__ ("cfdp_engine: can't set 'response_to_fault'; "
                      "value (%s) is invalid.\n",
                      value_in);
             return (0);
           }
       }

     /*------------------------------------------*/
     /* Compare input with each Remote parameter */
     /*------------------------------------------*/

     else if (!strcmp (param, MIB_ACK_LIMIT))
       {
         if (!isdigit ((int) value_in[0]))
           {
             e_msg__ ("cfdp_engine: can't set 'ack_limit'; "
                      "value (%s) must be numeric.\n",
                      value_in);
             return (0);
           }
         sscanf (value, "%lu", &long_int);
         m_mib_remote_default.ack_limit = (u_int_4) long_int;
         i_msg__ ("cfdp_engine: 'ack_limit' set to '%lu'.\n",
                  m_mib_remote_default.ack_limit);
       }

     else if (!strcmp (param, MIB_ACK_TIMEOUT))
       {
         if (!isdigit ((int) value_in[0]))
           {
             e_msg__ ("cfdp_engine: can't set 'ack_timeout'; "
                      "value (%s) must be numeric.\n",
                      value_in);
             return (0);
           }
         sscanf (value, "%lu", &long_int);
         m_mib_remote_default.ack_timeout = (u_int_4) long_int;
         i_msg__ ("cfdp_engine: 'ack_timeout' set to '%lu'.\n",
                  m_mib_remote_default.ack_timeout);
       }

     else if (!strcmp (param, MIB_INACTIVITY_TIMEOUT))
       {
         if (!isdigit ((int) value_in[0]))
           {
             e_msg__ ("cfdp_engine: can't set 'inactivity_timeout'; "
                      "value (%s) must be numeric.\n",
                      value_in);
             return (0);
           }
         sscanf (value, "%lu", &long_int);
         m_mib_remote_default.inactivity_timeout = (u_int_4) long_int;
         i_msg__ ("cfdp_engine: 'inactivity_timeout' set to '%lu'.\n",
                  m_mib_remote_default.inactivity_timeout);
       }

     else if (!strcmp (param, MIB_OUTGOING_FILE_CHUNK_SIZE))
       {
         if (!isdigit ((int) value_in[0]))
           {
             e_msg__ ("cfdp_engine: can't set 'outgoing_file_chunk_size'; "
                      "value (%s) must be numeric.\n",
                      value_in);
             return (0);
           }
         sscanf (value, "%lu", &long_int);
         requested_size = (u_int_4) long_int;
         if (requested_size > MAX_FILE_CHUNK_SIZE)
           {
             e_msg__ ("cfdp_engine: can't set 'outgoing_file_chunk_size'; "
                      "value (%s) exceeds max (%lu).\n",
                      value_in, MAX_FILE_CHUNK_SIZE);
             return (0);
           }
         else
           {
             m_mib_remote_default.outgoing_file_chunk_size = requested_size;
             i_msg__ ("cfdp_engine: 'outgoing_file_chunk_size' "
                      "set to '%lu'.\n",
                      m_mib_remote_default.outgoing_file_chunk_size);
           }
       }

     else if (!strcmp (param, MIB_NAK_LIMIT))
       {
         if (!isdigit ((int) value_in[0]))
           {
             e_msg__ ("cfdp_engine: can't set 'nak_limit'; "
                      "value (%s) must be numeric.\n",
                      value_in);
             return (0);
           }
         sscanf (value, "%lu", &long_int);
         m_mib_remote_default.nak_limit = (u_int_4) long_int;
         i_msg__ ("cfdp_engine: 'nak_limit' set to '%lu'.\n",
                  m_mib_remote_default.nak_limit);
       }

     else if (!strcmp (param, MIB_NAK_TIMEOUT))
       {
         if (!isdigit ((int) value_in[0]))
           {
             e_msg__ ("cfdp_engine: can't set 'nak_timeout'; "
                      "value (%s) must be numeric.\n",
                      value_in);
             return (0);
           }
         sscanf (value, "%lu", &long_int);
         m_mib_remote_default.nak_timeout = (u_int_4) long_int;
         i_msg__ ("cfdp_engine: 'nak_timeout' set to '%lu'.\n",
                  m_mib_remote_default.nak_timeout);
       }

     else if (!strcmp (param, MIB_SAVE_INCOMPLETE_FILES))
       {
         if (!strcmp (value, "YES"))
           {
             m_mib_remote_default.save_incomplete_files = YES;
             i_msg__ ("cfdp_engine: 'save_incomplete_files' set to 'yes'.\n");
           }
         else if (!strcmp (value, "NO"))
           {
             m_mib_remote_default.save_incomplete_files = NO;
             i_msg__ ("cfdp_engine: 'save_incomplete_files' set to 'no'.\n");
           }
         else
           {
             e_msg__ ("cfdp_engine: can't set 'save_incomplete_files'; "
                      "value (%s) must be 'yes' or 'no'.\n",
                      value_in); 
             return (0);
           }
       }

     else
       {
         e_msg__ ("cfdp_engine: can't set unrecognized MIB parameter "
                  "(%s).\n",
                  param);
         return (0);
       }

     return (1);
   }



/*=r=************************************************************************/
boolean mib__get_parameter (const char *param_in, char *value)
   {
     char             param [MAX_MIB_PARAMETER_LENGTH+1];
   /*------------------------------------------------------------*/
     INITIALIZE_ONCE;

     COPY (param, m__convert_string_to_uppercase (param_in));

     /*-----------------------------------------*/
     /* Compare input with each Local parameter */
     /*-----------------------------------------*/

     if (!strcmp (param, MIB_MY_ID))
       utils__strncpy (value, 
                       cfdp_id_as_string (m_mib_local.my_id),
                       MAX_MIB_VALUE_LENGTH);

     else if (!strcmp (param, MIB_ISSUE_EOF_RECV))
       {
         if (m_mib_local.issue_eof_recv)
           strcpy (value, "YES");
         else
           strcpy (value, "NO");
       }

     else if (!strcmp (param, MIB_ISSUE_EOF_SENT))
       {
         if (m_mib_local.issue_eof_sent)
           strcpy (value, "YES");
         else
           strcpy (value, "NO");
       }

     else if (!strcmp (param, MIB_ISSUE_FILE_SEGMENT_RECV))
       {
         if (m_mib_local.issue_file_segment_recv)
           strcpy (value, "YES");
         else
           strcpy (value, "NO");
       }

     else if (!strcmp (param, MIB_ISSUE_FILE_SEGMENT_SENT))
       {
         if (m_mib_local.issue_file_segment_sent)
           strcpy (value, "YES");
         else
           strcpy (value, "NO");
       }

     else if (!strcmp (param, MIB_ISSUE_RESUMED))
       {
         if (m_mib_local.issue_resumed)
           strcpy (value, "YES");
         else
           strcpy (value, "NO");
       }

     else if (!strcmp (param, MIB_ISSUE_SUSPENDED))
       {
         if (m_mib_local.issue_suspended)
           strcpy (value, "YES");
         else
           strcpy (value, "NO");
       }

     else if (!strcmp (param, MIB_ISSUE_TRANSACTION_FINISHED))
       {
         if (m_mib_local.issue_transaction_finished)
           strcpy (value, "YES");
         else
           strcpy (value, "NO");
       }

     else if (!strcmp (param, MIB_RESPONSE_TO_FAULT))
       {
         /* NOTE:  This implementation assumes that the response to
          * all faults will be the same.  
          */
         utils__strncpy (value, 
                         cfdp_response_as_string (m_mib_local.response[0]),
                         MAX_MIB_VALUE_LENGTH);
       }

     /*------------------------------------------*/
     /* Compare input with each Remote parameter */
     /*------------------------------------------*/

     else if (!strcmp (param, MIB_ACK_LIMIT))
       sprintf (value, "%lu", m_mib_remote_default.ack_limit);

     else if (!strcmp (param, MIB_ACK_TIMEOUT))
       sprintf (value, "%lu", m_mib_remote_default.ack_timeout);

     else if (!strcmp (param, MIB_INACTIVITY_TIMEOUT))
       sprintf (value, "%lu", m_mib_remote_default.inactivity_timeout);

     else if (!strcmp (param, MIB_NAK_LIMIT))
       sprintf (value, "%lu", m_mib_remote_default.nak_limit);

     else if (!strcmp (param, MIB_NAK_TIMEOUT))
       sprintf (value, "%lu", m_mib_remote_default.nak_timeout);

     else if (!strcmp (param, MIB_OUTGOING_FILE_CHUNK_SIZE))
       sprintf (value, "%lu", 
                m_mib_remote_default.outgoing_file_chunk_size);

     else if (!strcmp (param, MIB_SAVE_INCOMPLETE_FILES))
       {
         if (m_mib_remote_default.save_incomplete_files)
           strcpy (value, "YES");
         else
           strcpy (value, "NO");
       }

     else
       {
         w_msg__ ("cfdp_engine: can't get value of unrecognized MIB "
                  "parameter (%s).\n",
                  param);
         return (0);
       }

     return (1);
   }




