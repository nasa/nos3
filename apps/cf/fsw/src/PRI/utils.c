/* FILE: utils.c -- CFDP utility routines.
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
 * SPECS:  see utils.h
 * ORIGINAL PROGRAMMER:  Tim Ray x0581
 * CHANGES:
 *   2007_07_31 Tim Ray (TR)
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 *   2007_10_15 TR
 *     - Bug fix.  In 'cfdp_transaction_timer_status', a typo resulted in
 *       a pointer being modified instead of 'the value it pointed to'.
 *       i.e.   should have been:    *type = NO_TIMER;
 *              instead of:           type = NO_TIMER;
 *     - Poor programming practice fixed (IVV scan).
 *       'cfdp_transaction_status' no longer uses an uninitialized var.
 *   2007_11_09 TR
 *     - Improved portability.  'sscanf' reads of '%lu' format no longer put 
 *       the data directly into an 'u_int_4' variable.  This fix avoids a
 *       core dump if the compiler uses 8 bytes for 'long int' variables
 *       (i.e. avoids writing 8 bytes into a 4 byte variable).
 *     - Better portability.  C++ uses 'public' as a keyword, so any use of
 *       'public' as a variable name was changed to 'publik'.
 *   2007_11_19 TR
 *     - Bug fix.  'memset' was misused in 2 places (args out of sequence).
 *   2007_12_05 TR
 *     - Enhancement.  Added support for on-the-fly changes to message
 *       classes (i.e. which classes are currently enabled for output?).
 */

#include <ctype.h>
#include "cfdp.h"
#include "cfdp_private.h"

#define STRING_TERMINATOR 0



/*=r=************************************************************************/
static boolean m__do_strings_match (const char *s1, const char *s2)
     /* WHAT IT DOES:  Returns 1 if the strings match; otherwise, 0.
      * NOTE:  Comparison is case-insensitive.
      */
{
  int            i;
  /*------------------------------------------------------------*/
  if (strlen(s1) != strlen(s2))
    return (NO);

  for (i=0; i<strlen(s1); i++)
    if (tolower ((int) s1[i]) != tolower ((int) s2[i]))
      return (NO);

  return (YES);
}



/*****************************/
/*****************************/
/*****************************/
/***** Private utilities *****/
/*****************************/
/*****************************/
/*****************************/


/*=r=************************************************************************/
boolean utils__request_from_string (const char *request_string, 
                                    REQUEST *request)
{
  char             arg1 [MAX_REQUEST_STRING_LENGTH+1];
  char             arg2 [MAX_REQUEST_STRING_LENGTH+1];
  char             arg3 [MAX_REQUEST_STRING_LENGTH+1];
  char             arg4 [MAX_REQUEST_STRING_LENGTH+1];
  char             arg5 [MAX_REQUEST_STRING_LENGTH+1];
  int              i;
  ID               id;
  char             string [MAX_REQUEST_STRING_LENGTH+1];
  /*------------------------------------------------------------*/

  if (strlen (request_string) > MAX_REQUEST_STRING_LENGTH)
    /* Invalid input string */
    {
      e_msg__ ("cfdp_engine: Request-string (%s) is too long (%u max).\n",
               request_string, MAX_REQUEST_STRING_LENGTH);
      return (0);
    }

  /* Initialize the Request to be 'none' */
  request->type = REQ_NONE;

  /* Extract args (tokens) from the given input */
  memset (arg1, 0, sizeof(arg1));
  memset (arg2, 0, sizeof(arg2));
  memset (arg3, 0, sizeof(arg3));
  memset (arg4, 0, sizeof(arg4));
  memset (arg5, 0, sizeof(arg5));
  sscanf (request_string, "%128s %128s %128s %128s %128s", 
          arg1, arg2, arg3, arg4, arg5);

  /* Concatentate the args to produce a string for error messages */
  COPY (string, arg1);
  if (strlen(arg2) > 0)
    APPEND (string, " ");
  APPEND (string, arg2);
  if (strlen(arg3) > 0)
    APPEND (string, " ");
  APPEND (string, arg3);
  if (strlen(arg4) > 0)
    APPEND (string, " ");
  APPEND (string, arg4);
  if (strlen(arg5) > 0)
    APPEND (string, " ");
  APPEND (string, arg5);

  /* Convert the first token to uppercase (case-insensitive syntax) */
  for (i=0; i<strlen(arg1); i++)
    arg1[i] = (char) toupper ((int) arg1[i]);


  if (!strcmp (arg1, PUT_REQUEST))
    /* A Put Request */
    {
      request->type = REQ_PUT;
      request->info.put.file_transfer = YES;

      /* A valid Put Request has at least 2 more args */
      if (strlen(arg3) == 0)
        {
          e_msg__ ("cfdp_engine: Invalid Put Request (%s); not enough args.\n",
                   string);
          return (0);
        }

      /* Does the Request specify the optional Service Class 1? */
      if (m__do_strings_match (arg2, "-class1"))
        /* The request is for Service Class 1 (no acknowledgement) */
        {
          request->info.put.ack_required = NO;
          /* Shift all the remaining args forward */
          COPY (arg2, arg3);
          COPY (arg3, arg4);
          COPY (arg4, arg5);
        }
      else
        request->info.put.ack_required = YES;

      /* (Also accept a "-class2" option) (Even though it is the default) */
      if (m__do_strings_match (arg2, "-class2"))
        /* The request is for Service Class 2 (with acknowledgement) */
        {
          request->info.put.ack_required = YES;
          /* Shift all the remaining args forward */
          COPY (arg2, arg3);
          COPY (arg3, arg4);
          COPY (arg4, arg5);
        }

      /* Take note of the source file */
      /* (but first, validate it) */
      if (strlen (arg2) > MAX_FILE_NAME_LENGTH)
        {
          e_msg__ ("cfdp_engine: Invalid source file ('%s'), name exceeds "
                   "MAX_FILE_NAME_LENGTH chars.\n", arg2);
          return (0);
        }
      memset (request->info.put.source_file_name, 0, MAX_FILE_NAME_LENGTH);
      COPY (request->info.put.source_file_name, arg2);

      /* Does the request include a valid destination-ID? */
      if (cfdp_id_from_string (arg3, &id))
        request->info.put.dest_id = id;
      else
        {
          e_msg__ ("cfdp_engine: Invalid ID (%s) in Put Request.\n", arg3);
          return (0);
        }

      /* Does the request specify a different dest file-name, or simply
       * the same file-name at both ends?
       */
      if (strlen(arg4) > 0)
        /* Dest file-name specified */
        {
          /* Is it valid?... */
          if (strlen (arg4) > MAX_FILE_NAME_LENGTH)
            {
              e_msg__ ("cfdp_engine: Invalid dest file ('%s'), name exceeds "
                       "MAX_FILE_NAME_LENGTH chars.\n", arg4);
              return (0);
            }
          /* ...Yes, so copy it. */
          memset (request->info.put.dest_file_name, 0, MAX_FILE_NAME_LENGTH);
          COPY (request->info.put.dest_file_name, arg4);
        }
      else
        /* Same file-name at both ends */
        COPY (request->info.put.dest_file_name, 
              request->info.put.source_file_name);
    }
  

  else if (!strcmp (arg1, SUSPEND_REQUEST))
    {
      request->type = REQ_SUSPEND;

      /* A valid Suspend Request has at least 1 more args */
      if (strlen(arg2) == 0)
        {
          e_msg__ ("cfdp_engine: Invalid Suspend Request (%s); "
                   "not enough args.\n",
                   string);
          return (0);
        }

      /* Does the request include a valid transaction */
      if (m__do_strings_match (arg2, "all"))
        /* Blanket request for all transactions */
        request->type = REQ_SUSPEND_ALL_TRANSACTIONS;
      else if (!cfdp_trans_from_string (arg2, &(request->info.trans)))
        {
          e_msg__ ("cfdp_engine: Invalid trans (%s) in Suspend Request.\n", 
                   arg2);
          return (0);
        }
    }


  else if (!strcmp (arg1, RESUME_REQUEST))
    {
      request->type = REQ_RESUME;

      /* A valid Resume Request has at least 1 more args */
      if (strlen(arg2) == 0)
        {
          e_msg__ ("cfdp_engine: Invalid Resume Request (%s); "
                   "not enough args.\n",
                   string);
          return (0);
        }

      /* Does the request include a valid transaction */
      if (m__do_strings_match (arg2, "all"))
        /* Blanket request for all transactions */
        request->type = REQ_RESUME_ALL_TRANSACTIONS;
      else if (!cfdp_trans_from_string (arg2, &(request->info.trans)))
        {
          e_msg__ ("cfdp_engine: Invalid trans (%s) in Resume Request.\n", 
                   arg2);
          return (0);
        }
    }


  else if (!strcmp (arg1, CANCEL_REQUEST))
    {
      request->type = REQ_CANCEL;

      /* A valid Cancel Request has at least 1 more args */
      if (strlen(arg2) == 0)
        {
          e_msg__ ("cfdp_engine: Invalid Cancel Request (%s); "
                   "not enough args.\n",
                   string);
          return (0);
        }

      /* Does the request include a valid transaction */
      if (m__do_strings_match (arg2, "all"))
        /* Blanket request for all transactions */
        request->type = REQ_CANCEL_ALL_TRANSACTIONS;
      else if (!cfdp_trans_from_string (arg2, &(request->info.trans)))
        {
          e_msg__ ("cfdp_engine: Invalid trans (%s) in Cancel Request.\n", 
                   arg2);
          return (0);
        }
    }


  else if (!strcmp (arg1, ABANDON_REQUEST))
    {
      request->type = REQ_ABANDON;

      /* A valid Abandon Request has at least 1 more args */
      if (strlen(arg2) == 0)
        {
          e_msg__ ("cfdp_engine: Invalid Abandon Request (%s); "
                   "not enough args.\n",
                   string);
          return (0);
        }

      /* Does the request include a valid transaction */
      if (m__do_strings_match (arg2, "all"))
        /* Blanket request for all transactions */
        request->type = REQ_ABANDON_ALL_TRANSACTIONS;
      else if (!cfdp_trans_from_string (arg2, &(request->info.trans)))
        {
          e_msg__ ("cfdp_engine: Invalid trans (%s) in Abandon Request.\n",
                   arg2);
          return (0);
        }
    }


  else if (!strcmp (arg1, REPORT_REQUEST))
    {
      request->type = REQ_REPORT;

      /* A valid Report Request has at least 1 more args */
      if (strlen(arg2) == 0)
        {
          e_msg__ ("cfdp_engine: Invalid Report Request (%s); "
                   "not enough args.\n",
                   string);
          return (0);
        }

      /* Does the request include a valid transaction */
      if (m__do_strings_match (arg2, "all"))
        /* Blanket request for all transactions */
        /*!!!  "REPORT" should not imply *all* transactions...
        request->type = REQ_REPORT_ALL_TRANSACTIONS;  
        */ ;
      else if (!cfdp_trans_from_string (arg2, &(request->info.trans)))
        {
          e_msg__ ("cfdp_engine: Invalid trans (%s) in Report Request.\n", 
                   arg2);
          return (0);
        }
    }


  else if (!strcmp (arg1, FREEZE_REQUEST))
     request->type = REQ_FREEZE;


  else if (!strcmp (arg1, THAW_REQUEST))
     request->type = REQ_THAW;

  
  else
    {
      e_msg__ ("cfdp_engine: Unrecognized request (%s).\n", arg1);
      return (0);
    }

  return (1);
}



/*=r=************************************************************************/
boolean utils__strncpy (char *output, const char *input, size_t max_storage)
{
  boolean            was_entire_string_copied;
  /*------------------------------------------------------------*/

  was_entire_string_copied = NO;

  strncpy (output, input, max_storage);
  if (strlen(input) < max_storage)
    was_entire_string_copied = YES;
  else
    {
      output[max_storage-1] = STRING_TERMINATOR;
      w_msg__ ("cfdp_engine: Unable to copy entire string "
               "(%u of %u chars copied).\n",
               max_storage-1, strlen(input));
    }
  
  return (was_entire_string_copied);
}



/*=r=************************************************************************/
boolean utils__strncat (char *string1, const char *string2, size_t max_storage)
{
  size_t             amount_to_concatenate;
  size_t             original_length_of_string1;
  size_t             total_string_length;
  boolean            was_entire_string_concatenated;
  /*------------------------------------------------------------*/
  
  was_entire_string_concatenated = NO;
  original_length_of_string1 = strlen(string1);

  /* If there is enough storage, simply use 'strcat' */
  total_string_length = original_length_of_string1 + strlen(string2);
  if (max_storage > total_string_length)
    {
      strcat (string1, string2);
      was_entire_string_concatenated = YES;
    }
  
  /* Otherwise, be careful... */
  else
    {
      /* Figure out how much of string2 can be concatenated */
      amount_to_concatenate = max_storage - original_length_of_string1 - 1;
      /* Append that portion of string2 to string1 */
      strncat (string1, string2, amount_to_concatenate);
      /* Ensure that string1 is null-terminated */
      string1[max_storage-1] = STRING_TERMINATOR;
      w_msg__ ("cfdp_engine: Unable to concatenate entire string "
               "(%u of %u chars copied).\n",
               amount_to_concatenate, strlen(string2));
    }

  return (was_entire_string_concatenated);
}



/****************************/
/****************************/
/****************************/
/***** Public utilities *****/
/****************************/
/****************************/
/****************************/



/***************************************/
/*** Configure the engine on-the-fly ***/
/***************************************/



/*=r=************************************************************************/
boolean cfdp_set_mib_parameter (const char *param, const char *value)
{
  /*------------------------------------------------------------*/

  if ((param == NULL) || (value == NULL))
    {
      e_msg__ ("cfdp_set_mib_parameter: given a null-pointer.\n");
      return (0);
    }

  return (mib__set_parameter (param, value));
}



/*=r=************************************************************************/
boolean cfdp_get_mib_parameter (const char *param, char *value)
{
  /*------------------------------------------------------------*/

  if (value == NULL)
    {
      e_msg__ ("cfdp_get_mib_parameter: given a null-pointer.\n");
      return (0);
    }

  else if (param == NULL)
    {
      strcpy (value, "null-pointer");
      e_msg__ ("cfdp_get_mib_parameter: given a null-pointer.\n");
      return (0);
    }

  return (mib__get_parameter (param, value));
}



/*=r=************************************************************************/
char *cfdp_mib_as_string (void)
{
  /*------------------------------------------------------------*/
  return (mib__as_string());
}



/*=r=*************************************************************************/
boolean cfdp_enable_message_class (int class)
{
  /*------------------------------------------------------------*/
  return (message_class__enable (class));
}



/*=r=*************************************************************************/
boolean cfdp_disable_message_class (int class)
{
  /*------------------------------------------------------------*/
  return (message_class__disable (class));
}



/*=r=*************************************************************************/
boolean cfdp_is_message_class_enabled (int class)
{
  /*------------------------------------------------------------*/
  return (message_class__enabled (class));
}



/********************************/
/*** Get status of the engine ***/
/********************************/



/*=r=************************************************************************/
SUMMARY_STATUS cfdp_summary_status (void)
{
  static SUMMARY_STATUS    answer;
  MACHINE                 *m;
  TRANS_STATUS            *mp;
  SUMMARY_STATISTICS       stats;
  /*------------------------------------------------------------*/

  /*---------------------------------------------------------*/
  /* Step One -- statistics on currently active transactions */
  /*---------------------------------------------------------*/

  /* Initialize all the relevant counters */
  answer.how_many_senders = 0;
  answer.how_many_receivers = 0;
  answer.how_many_frozen = 0;
  answer.how_many_suspended = 0;


  /* Update the counters based on each transaction's status */
  machine_list__open_a_walk ();
  while ((m=machine_list__get_next_machine()) != NULL)
    {
      mp = &(m->publik);
      if ((mp->role == S_1) || (mp->role == S_2))
        answer.how_many_senders ++;
      else if ((mp->role == R_1) || (mp->role == R_2))
        answer.how_many_receivers ++;
      if (mp->frozen)
        answer.how_many_frozen ++;
      if (mp->suspended)
        answer.how_many_suspended ++;
    }

  /*-----------------------------------*/
  /* Step Two -- cumulative statistics */
  /*-----------------------------------*/

  stats = misc__get_summary_statistics ();
  answer.total_files_sent = stats.successful_sender_count;
  answer.total_files_received = stats.successful_receiver_count;
  answer.total_unsuccessful_senders = stats.unsuccessful_sender_count;
  answer.total_unsuccessful_receivers = stats.unsuccessful_receiver_count;

  /*----------------------------------------------------------*/
  /* Step Three -- is the link to any of our partners frozen? */
  /*----------------------------------------------------------*/

  answer.are_any_partners_frozen = misc__are_any_partners_frozen ();

  return (answer);
}



/*=r=************************************************************************/
CONDITION_CODE cfdp_last_condition_code (void)
{
  /*------------------------------------------------------------*/
  return (misc__get_last_condition_code());
}



/************************************************/
/*** Get status of one particular transaction ***/
/************************************************/



/*=r=*************************************************************************/
boolean cfdp_transaction_status (TRANSACTION trans, TRANS_STATUS *status)
{
  MACHINE             *machine_ptr;
  /*------------------------------------------------------------*/

  /*----------------*/
  /* Validate input */
  /*----------------*/

  if (status == NULL)
    {
      e_msg__ ("cfdp_transaction_status: given a null-pointer.\n");
      return (0);
    }

  machine_ptr = machine_list__get_this_trans (trans);

  if (machine_ptr == NULL)
    /* Given transaction is not known to us. */
    {
      e_msg__ ("cfdp_transaction_status:  unknown transaction (%s).\n",
               cfdp_trans_as_string (trans));
      return (0);
    }

  /*-----------------------------*/
  /* Input is valid; take action */
  /*-----------------------------*/
  
  *status = machine_ptr->publik;
  return (1);
}



/*=r=*************************************************************************/
boolean cfdp_transaction_timer_status (TRANSACTION trans,
                                       TIMER_TYPE *type,
                                       u_int_4 *seconds_remaining)
{
  MACHINE             *m;
  /*------------------------------------------------------------*/

  /*----------------*/
  /* Validate input */
  /*----------------*/

  /* Validate input pointers */
  if ((type == NULL) || (seconds_remaining == NULL))
    {
      e_msg__ ("cfdp_transaction_timer_status: given a null-pointer.\n");
      return (0);
    }

  /* Now it's ok to initialize results */
  *type = NO_TIMER;
  *seconds_remaining = 0;

  m = machine_list__get_this_trans (trans);

  if (m == NULL)
    /* Given transaction is not known to us. */
    {
      e_msg__ ("cfdp_transaction_timer_status:  unknown transaction (%s).\n",
               cfdp_trans_as_string (trans));
      return (0);
    }

  /*-----------------------------*/
  /* Input is valid; take action */
  /*-----------------------------*/
  
  if (m->ack_timer.mode != TIMER_OFF)
    /* Ack-timer is active */
    {
      *type = ACK_TIMER;
      *seconds_remaining = timer__read (&(m->ack_timer));
    }

  else if (m->nak_timer.mode != TIMER_OFF)
    /* Nak-timer is active */
    {
      *type = NAK_TIMER;
      *seconds_remaining = timer__read (&(m->nak_timer));
    }
  
  else if (m->inactivity_timer.mode != TIMER_OFF)
    /* Inactivity-timer is active */
    {
      *type = INACTIVITY_TIMER;
      *seconds_remaining = timer__read (&(m->inactivity_timer));
    }

  else
    /* No timer is active */
    *type = NO_TIMER;
  
  return (1);
}



/*=r=*************************************************************************/
boolean cfdp_transaction_progress (TRANSACTION trans,
                                   u_int_4 *bytes_transferred,
                                   u_int_4 *file_size)
{
  u_int_4              bytes_missing;
  MACHINE             *m;
  /*------------------------------------------------------------*/

  /*----------------*/
  /* Validate input */
  /*----------------*/

  if ((bytes_transferred == NULL) || (file_size == NULL))
    {
      e_msg__ ("cfdp_transaction_progress: given a null-pointer.\n");
      return (0);
    }

  /* Now it's ok to initialize results */
  *bytes_transferred = 0;
  *file_size = 0;

  m = machine_list__get_this_trans (trans);

  if (m == NULL)
    /* Given transaction is not known to us. */
    {
      e_msg__ ("cfdp_transaction_progress:  unknown transaction (%s).\n",
               cfdp_trans_as_string (trans));
      return (0);
    }

  /*-----------------------------*/
  /* Input is valid; take action */
  /*-----------------------------*/

  /* If we are the Sender */
  if ((m->publik.role == CLASS_1_SENDER) || 
      (m->publik.role == CLASS_2_SENDER))
    {
      *file_size = m->publik.md.file_size;
      bytes_missing = nak__how_many_bytes_missing (&(m->nak));
      if (bytes_missing > *file_size)
        /* Shouldn't ever happen, but avoid giving nonsense answer */
        *bytes_transferred = 0;
      else
        *bytes_transferred = *file_size - bytes_missing;
    }

  /* Otherwise, we are the Receiver */
  else
    {
      /* Copy the file-size from the Metadata PDU if possible... */
      if (m->publik.has_md_been_received)
        *file_size = m->publik.md.file_size;

      /* ... if not, copy the file-size from the EOF PDU if possible... */
      else if (m->has_eof_been_received)
        /* Copy the file-size from the EOF pdu */
        *file_size = m->eof.file_size;

      /* ... if not, then the file-size is unknown (set it to zero). */
      else
        *file_size = 0;

      *bytes_transferred = nak__how_many_bytes_received (&(m->nak));
    }
  
  return (1);
}



/*=r=*************************************************************************/
boolean cfdp_transaction_gaps_as_string (TRANSACTION trans, char *string,
                                         int max_string_length)
{
  MACHINE             *m;
  /*------------------------------------------------------------*/

  /*----------------*/
  /* Validate input */
  /*----------------*/

  if (string == NULL)
    {
      e_msg__ ("cfdp_transaction_gaps_as_string: given a null-pointer.\n");
      return (0);
    }

  m = machine_list__get_this_trans (trans);

  if (m == NULL)
    /* Given transaction is not known to us. */
    {
      e_msg__ ("cfdp_transaction_gaps_as_string:  unknown transaction (%s).\n",
               cfdp_trans_as_string (trans));
      return (0);
    }

  /*-----------------------------*/
  /* Input is valid; take action */
  /*-----------------------------*/

  nak__gaps_as_string (&(m->nak), string, max_string_length);
  return (1);
}



/*****************************************************************/
/*** Convert between "computer" data and readable text strings ***/
/*****************************************************************/



/*=r=************************************************************************/
char *cfdp_condition_as_string (CONDITION_CODE cc)
     /* WHAT IT DOES:  Returns the Condition-Code as a string. */
{
  static char        string [64];
  /*------------------------------------------------------------*/
  if (cc == 0)
    strcpy (string, "No error");
  else if (cc == 1)
    strcpy (string, "Positive Ack limit reached");
  else if (cc == 2)
    strcpy (string, "Keep alive limit reached");
  else if (cc == 3)
    strcpy (string, "Invalid transmission mode");
  else if (cc == 4)
    strcpy (string, "Filestore rejection");
  else if (cc == 5)
    strcpy (string, "File checksum failure");
  else if (cc == 6)
    strcpy (string, "File size error");
  else if (cc == 7)
    strcpy (string, "NAK limit reached");
  else if (cc == 8)
    strcpy (string, "Inactivity detected");
  else if (cc == 9)
    strcpy (string, "Invalid file structure");
  else if ((cc >= 10) && (cc <= 13))
    strcpy (string, "Reserved");
  else if (cc == 14)
    strcpy (string, "Suspend.request received");
  else if (cc == 15)
    strcpy (string, "Cancel.request received");
  else
    strcpy (string, "?");
  
  return (string);
}



/*=r=************************************************************************/
char *cfdp_id_as_string (ID id)
{
  int              i;
  char             substring [8];
  static char      string [MAX_AS_STRING_LENGTH+1];
  /*------------------------------------------------------------*/

  /* Screen for valid input */
  if (id.length < 1)
    /* No CFDP ID can ever be this short */
    {
      e_msg__ ("cfdp_id_as_string: ID is shorter than CFDP allows! (%u)\n", 
               id.length);
      return ("<<<Invalid_ID>>>");
    }
  else if (id.length > 8)
    /* No CFDP ID can ever be this long */
    {
      e_msg__ ("cfdp_id_as_string: ID is longer than CFDP allows! (%u)\n", 
               id.length);
      return ("<<<Invalid_ID>>>");
    }

  /* Generate the 'dotted-decimal' format (e.g. "128.183.53") */
  memset (string, 0, sizeof(string));
  sprintf (string, "%u", id.value[0]);
  for (i=1; i<id.length; i++)
    {
      sprintf (substring, ".%u", (u_int_2) id.value[i]);
      APPEND (string, substring);
    }

  return (string);
}



/*=r=************************************************************************/
boolean cfdp_id_from_string (const char *value_as_dotted_string, ID *id)
{
  int            digit;
  char          *ptr;
  char           string [MAX_AS_STRING_LENGTH+1];
  /*------------------------------------------------------------*/

  if ((value_as_dotted_string == NULL) || (id == NULL))
    {
      e_msg__ ("cfdp_id_from_string: given a null-pointer.\n");
      return (0);
    }
      
  else if (strlen(value_as_dotted_string) == 0)
    /* Invalid input string -- empty */
    {
      e_msg__ ("cfdp_id_from_string: ID-string is empty (zero-length).\n");
      return (0);
    }
    
  else if (strlen(value_as_dotted_string) > MAX_AS_STRING_LENGTH)
    /* Invalid input string -- too long */
    {
      e_msg__ ("cfdp_id_from_string: ID-string (%s) is too long (%u max).\n",
               value_as_dotted_string, MAX_AS_STRING_LENGTH);
      return (0);
    }

  else if (!isdigit ((int) value_as_dotted_string[0]))
    /* Invalid input string -- id must start with a number */
    {
      e_msg__ ("cfdp_id_from_string: ID-string (%s) does not start with "
               "a number.\n",
               value_as_dotted_string);
      return (0);
    }
    

  id->length = 0;


  /* Copying the input string allows 'strtok' to modify it */
  COPY (string, value_as_dotted_string);
  
  ptr = strtok (string, ".");
  
  while (ptr)
    /* Parse the string into tokens, each of which should be a number 0-255 */
    {
      if (id->length == MAX_ID_LENGTH)
        {
          e_msg__ ("cfdp_id_from_string: full conversion of id-string (%s) "
                   "would violate max-id-length (%u).\n",
                   value_as_dotted_string, MAX_ID_LENGTH);
          return (0);
        }

      /* Convert this numeric-string to a one-byte number */
      if (!isdigit ((int) *ptr))
        /* Invalid numeric string */
        {
          e_msg__ ("cfdp_id_from_string: ID-string (%s) contains "
                   "non-numeric substring.\n",
                   value_as_dotted_string);
          return (0);
        }
      digit = atoi (ptr);
      if (digit > 255)
        /* Each number in a valid ID will fit in one byte */
        {
          e_msg__ ("cfdp_id_from_string: ID-string (%s) contains number "
                   "that is too big (>255).\n",
                   value_as_dotted_string);
          return (0);
        }
      id->value[id->length] = digit;

      id->length++;
      ptr = strtok ('\0', ".");
    }
  
  return (1);
}



/*=r=************************************************************************/
char *cfdp_indication_type_as_string (INDICATION_TYPE it)
     /* WHAT IT DOES:  Returns the given Indication-Type as a string. */
{
  static char        string [64];
  /*------------------------------------------------------------*/

  if (it == IND_ABANDONED)
    strcpy (string, "Abandoned");
  else if (it == IND_ACK_TIMER_EXPIRED)
    strcpy (string, "Ack_Timer_Expired");
  else if (it == IND_EOF_RECV)
    strcpy (string, "EOF_Recv");
  else if (it == IND_EOF_SENT)
    strcpy (string, "EOF_Sent");
  else if (it == IND_FAULT)
    strcpy (string, "Fault");
  else if (it == IND_FILE_SEGMENT_SENT)
    strcpy (string, "File_Segment_Sent");
  else if (it == IND_FILE_SEGMENT_RECV)
    strcpy (string, "File_Segment_Recv");
  else if (it == IND_INACTIVITY_TIMER_EXPIRED)
    strcpy (string, "Inactivity_Timer_Expired");
  else if (it == IND_MACHINE_ALLOCATED)
    strcpy (string, "Machine_Allocated");
  else if (it == IND_MACHINE_DEALLOCATED)
    strcpy (string, "Machine_Deallocated");
  else if (it == IND_METADATA_RECV)
    strcpy (string, "Metadata_Recv");
  else if (it == IND_METADATA_SENT)
    strcpy (string, "Metadata_Sent");
  else if (it == IND_NAK_TIMER_EXPIRED)
    strcpy (string, "Nak_Timer_Expired");
  else if (it == IND_REPORT)
    strcpy (string, "Report");
  else if (it == IND_RESUMED)
    strcpy (string, "Resumed");
  else if (it == IND_SUSPENDED)
    strcpy (string, "Suspended");
  else if (it == IND_TRANSACTION)
    strcpy (string, "Transaction");
  else if (it == IND_TRANSACTION_FINISHED)
    strcpy (string, "Transaction_Finished");
  else
    strcpy (string, "?");

  return (string);
}



/*=r=************************************************************************/
char *cfdp_response_as_string (RESPONSE response)
     /* WHAT IT DOES:  Returns the Condition-Code as a string. */
{
  static char        string [32];
  /*------------------------------------------------------------*/
  if (response == 0)
    strcpy (string, "reserved_0");
  else if (response == 1)
    strcpy (string, "cancel");
  else if (response == 2)
    strcpy (string, "suspend");
  else if (response == 3)
    strcpy (string, "ignore");
  else if (response == 4)
    strcpy (string, "abandon");
  else if (response >= 5)
    strcpy (string, "reserved");
  else
    /* This should never happen */
    strcpy (string, "?");
  
  return (string);
}



/*=r=************************************************************************/
char *cfdp_role_as_string (ROLE role)
{
  /*------------------------------------------------------------*/
  if (role == S_1)
    return ("S1");
  else if (role == R_1)
    return ("R1");
  else if (role == S_2)
    return ("S2");
  else if (role == R_2)
    return ("R2");
  else
    return ("Role_Undefined");
}



/*=r=************************************************************************/
char *cfdp_trans_as_string (TRANSACTION transaction)
{
  char             substring [16];
  static char      string [MAX_AS_STRING_LENGTH+1];
  /*------------------------------------------------------------*/

  /* (Safely) generate a "%s_%lu" format */
  COPY (string, cfdp_id_as_string(transaction.source_id));
  APPEND (string, "_");
  sprintf (substring, "%lu", transaction.number);
  APPEND (string, substring);
  return (string);
}



/*=r=************************************************************************/
boolean cfdp_trans_from_string (const char *string_in, TRANSACTION *trans)
{
  int                     i;
  char                    id_string [MAX_AS_STRING_LENGTH+1];
  unsigned long int       long_int;
  char                    num_string [MAX_AS_STRING_LENGTH+1];
  boolean                 am_i_parsing_the_id;
  /*------------------------------------------------------------*/

  if ((string_in == NULL) || (trans == NULL))
    {
      e_msg__ ("cfdp_trans_from_string: given a null-pointer.\n");
      return (0);
    }
    
  if (strlen (string_in) > MAX_AS_STRING_LENGTH)
    /* Invalid input string */
    {
      e_msg__ ("cfdp_trans_from_string: input (%s) is too long"
               "(%u max).\n",
               string_in, MAX_AS_STRING_LENGTH);
      return (0);
    }
  
  else if (strlen (string_in) == 0)
    {
      e_msg__ ("cfdp_trans_from_string: input is zero-lenth.\n");
      return (0);
    }
     
  else if (!isdigit ((int) string_in[0]))
    {
      e_msg__ ("cfdp_trans_from_string: input (%s) must start "
               "with a number.\n", string_in);
      return (0);
    }


  /* The input string format is supposed to be "<id>_<number>".
   * Copy the ID portion into id_string and the number portion into num_string.
   */
  memset (id_string, 0, sizeof(id_string));
  memset (num_string, 0, sizeof(num_string));
  am_i_parsing_the_id = YES;
  for (i=0; i<strlen(string_in); i++)
    {
      if (string_in[i] == '_')
        /* Underscore marks the boundary between ID and number */
        am_i_parsing_the_id = NO;
      else if (am_i_parsing_the_id)
        id_string[strlen(id_string)] = string_in[i];
      else
        num_string[strlen(num_string)] = string_in[i];
    }

  /* Convert the ID-string into the transaction's source-ID */
  if (!cfdp_id_from_string (id_string, &(trans->source_id)))
    {
      e_msg__ ("cfdp_trans_from_string: string (%s) has invalid ID.\n", 
               string_in);
      return (0);
    }
  
  /* Convert the num-string into the transaction's number */
  if (strlen(num_string) == 0)
    /* A valid transaction-id includes a transaction-sequence-number */
    {
      e_msg__ ("cfdp_trans_from_string: string (%s) does not include "
               "a sequence-number.\n", string_in);
      return (0);
    }
  else if (!isdigit ((int) num_string[0]))
    /* The seq-num must be a number */
    {
      e_msg__ ("cfdp_trans_from_string: string (%s) has a non-numeric "
               "sequence-number.\n", string_in);
      return (0);
    }

  sscanf (num_string, "%lu", &long_int);
  trans->number = (u_int_4) long_int;

  return (1);
}



/*********************/
/*** Miscellaneous ***/
/*********************/



/*=r=************************************************************************/
boolean cfdp_are_these_ids_equal (ID id1, ID id2)
{
  int            i;
  int            larger_length;
  int            smaller_length;
  /*------------------------------------------------------------*/

  /* Determine how many bytes need to be compared (normally the 2 ids will
   * be the same length, but that is not guaranteed)
   */
  if (id1.length == id2.length)
    /* The usual case */
    larger_length = smaller_length = id1.length;
  else if (id1.length > id2.length)
    {
      larger_length = id1.length;
      smaller_length = id2.length;
    }
  else 
    {
      larger_length = id2.length;
      smaller_length = id1.length;
    }

  /* Do a byte-by-byte comparison up to the smaller-length.
   * The indexing is weird because of how the bytes are stored in the array:
   * For example:                 byte 0     byte 1
   *       "23" stored as:          23         --
   *       "0.23" stored as:         0         23
   */
  for (i=1; i<=smaller_length; i++)
    if (id1.value[id1.length-i] != id2.value[id2.length-i])
      return (NO);

  /* If the ids are of different lengths, the one with the longer length
   * only matches if the "extra bytes" are zeros.    
   * For example, "0.23" matches "23", but "88.23" doesn't.
   * Again, indexing is weird because of how the bytes are stored.
   */
  for (i=smaller_length+1; i<=larger_length; i++)
    {
      if (id1.length > id2.length)
        {
          if (id1.value[id1.length-i] != 0)
            return (NO);
        }
      else
        {
          if (id2.value[id2.length-i] != 0)
            return (NO);
        }
    }

  return (YES);
}



/*=r=************************************************************************/
boolean  cfdp_are_these_trans_equal (TRANSACTION t1, TRANSACTION t2)
{
  /*------------------------------------------------------------*/
  if ((t1.number == t2.number) &&
      cfdp_are_these_ids_equal (t1.source_id, t2.source_id))
    return (YES);
  else
    return (NO);
}



/*=r=************************************************************************/
u_int_4 cfdp_memory_use_per_trans (void)
{
  /*------------------------------------------------------------*/
  return (sizeof(MACHINE));
}



/*=r=*************************************************************************/
void cfdp_reset_totals (void)
{
   /*------------------------------------------------------------*/
  misc__reset_summary_statistics ();
}



/*=r=*************************************************************************/
void cfdp_set_trans_seq_num (u_int_4 value)
{
  /*------------------------------------------------------------*/

  misc__set_trans_seq_num (value);
}



/*=r=************************************************************************/
u_int_4 cfdp_how_many_active_trans (void)
{
  /*------------------------------------------------------------*/
  return (machine_list__how_many_machines ());
}



/*---------------------------------------*/
/* Are any transactions being cancelled? */
/*---------------------------------------*/

/*=r=*************************************************************************/
boolean cfdp_is_a_trans_being_cancelled (void)
{
  boolean        answer;
  MACHINE       *m;
  /*------------------------------------------------------------*/

  answer = NO;
  machine_list__open_a_walk ();
  while ((m = machine_list__get_next_machine ()) != NULL)
    if (m->publik.cancelled)
      answer = YES;

  return (answer);
}
