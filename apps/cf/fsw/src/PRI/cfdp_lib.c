/* FILE: cfdp_lib.c  -- This module manages the CFDP State Machines.
 *   The "meaty" public routines are in this module.  (Less meaty public
 *   routines are in the 'utils' module).
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
 * SPECS:  PUB/cfdp_provides.h
 * ORIGINAL PROGRAMMER:  Tim Ray 301-286-0581
 * CHANGES:
 *   2007_07_31 Tim Ray (TR)
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 *   2007_11_01 TR
 *     - Rewrote the 'cycle' routine to loop through all the state machines
 *       twice (instead of once).  This ensures that outgoing File-Directives
 *       get priority over outgoing File-Data.    (part of a bug fix)
 *   2007_11_09 TR
 *     - Better portability.  C++ uses 'public' as a keyword, so any use of
 *       'public' as a variable name was changed to 'publik'.
 *   2007_12_06 TR
 *     - Enhancement.  Message-class filtering can now be changed on-the-fly.
 *     - If an incoming pdu is ignored, a debug message is *always* output.
 *       (i.e. got rid of the message-class 'kernel_autonomous')
 *   2008_01_04 TR
 *     - Bug fix.  In response to an incoming PDU, the inactivity-timer is
 *       no longer restarted IF the transaction is frozen or suspended.
 *     - Optimization:  If the source-file specified in a Put Request
 *       does not exist, reject the request (rather than starting a state
 *       machine and sending a Metadata PDU before discovering the problem).
 *   2008_03_03 TR
 *     - Fixed a typo in the contents of an error message.
 *       The fix is within 'm__is_pdu_sane', and comes from checking an
 *       _ACK_FIN_ pdu.
 *     - Bug fixes.  'cfdp_give_request' now returns 'unsuccessful' if:
 *       unable to start a new sender state machine
 *       (via the routine 'm__start_new_sender_machine')   OR
 *       unable to start a new receiver state machine
 *       (via the routine 'm__start_new_receiver_machine')   OR
 *       unable to restart an old (completed) sender state machine
 *       (via the routine 'm__restart_old_sender_machine').
 */

#include <ctype.h>
#include "cfdp.h"
#include "cfdp_private.h"

#define CFDP_ENGINE_VERSION "3.1a1"
#define REPORT_VERSION_ONCE \
   { if(!m_has_version_been_reported) m__report_version(); }
        
#define VALID_ROLE  ((m->publik.role == S_1) || \
                     (m->publik.role == S_2) || \
                     (m->publik.role == R_1) || \
                     (m->publik.role == R_2))
#define NO  0
#define YES  1

/* This one variable is shared by all the routines within this module */
static boolean            m_has_version_been_reported = NO;





/*=r=*************************************************************************/
static void m__report_version (void)
     /* WHAT IT DOES:  Reports the CFDP-engine version number */
{
  /*------------------------------------------------------------*/
  i_msg__ ("cfdp_engine: version = %s.\n", CFDP_ENGINE_VERSION);
  m_has_version_been_reported = YES;
}



/*=r=************************************************************************/
static void m__state_table (MACHINE *m, int event, 
                            PDU_AS_STRUCT *pdu_ptr, REQUEST *req_ptr)
     /* WHAT IT DOES:  Calls the appropriate state table routine (based
      *   upon the role that we are to play - e.g. Class 2 Sender).
      *   The given machine, event, PDU, and Request are all passed along 
      *   to the state table routine.  
      * NOTE:  Also enforces the restriction that the state tables are
      *   not re-entrant for any particular state machine.
      * NOTE:  If the given state machine finishes, it is deleted.
      */
{
  /*------------------------------------------------------------*/
  
  /* Avoid re-entering the engine core for any particular transaction */ 
  if (m->is_user_inside_the_engine_core)
    {
      w_msg__ ("cfdp_engine: user attempt to re-enter state tables denied.\n");
      return;
    }
  m->is_user_inside_the_engine_core = YES;
  
  /* Run the given state machine in the appropriate state table routine */
  if (m->publik.role == S_1)
    s1__state_table (m, event, pdu_ptr, req_ptr);
  else if (m->publik.role == S_2)
    s2__state_table (m, event, pdu_ptr, req_ptr);
  else if (m->publik.role == R_1)
    r1__state_table (m, event, pdu_ptr, req_ptr);
  else if (m->publik.role == R_2)
    r2__state_table (m, event, pdu_ptr, req_ptr);
  else
    e_msg__ ("cfdp_engine: unable to run state table; unknown Role.\n");

  m->is_user_inside_the_engine_core = NO;

  /* If the state machine has finished, delete it */
  if (m->has_this_state_machine_finished)
    {
      /* Engine-wide bookeeping */
      misc__update_summary_statistics (m);
      misc__set_last_condition_code (m->publik.condition_code);
      misc__set_last_trans_abandoned (m->publik.abandoned);
      /* Let the engine user know what has happened */
      indication__ (IND_MACHINE_DEALLOCATED, &(m->publik));
      /* Delete the state machine (give up the slot associated with it) */
      if (!machine_list__deallocate_slot (m))
        e_msg__ ("cfdp_engine: "
                 "attempt to delete completed state machine failed.\n");
    }
}



/*=r=*************************************************************************/
static MACHINE *m__restart_old_sender_machine (HDR hdr)
     /* WHAT IT DOES:  Given a PDU-header (that is presumably a Finished
      *   PDU), it attempts to open a comm-link to the CFDP partner,
      *   allocate a state machine slot, and initialize the state machine.
      *   If successful, returns a pointer to the new state machine;
      *   otherwise, returns NULL.
      */
{
  MACHINE           *m = NULL;
  ID                 partner_id;
  /*------------------------------------------------------------*/

  /* Attempt to open a comm-link to the CFDP partner entity.  Since we were 
   * (and are) the sender, our partner is the destination of this transaction.
   */
  partner_id = hdr.dest_id;   /* We're the sender, partner is dest-id */
  if (!pdu_output__open (mib__get_my_id(), partner_id))
    e_msg__ ("cfdp_engine: discarded incoming Fin PDU - "
             "unable to communicate with node '%s'\n",
             cfdp_id_as_string (partner_id));
  
  /* If successful, attempt to allocate a slot in the machine-list */
  else if ((m = machine_list__allocate_slot ()) == NULL)
    e_msg__ ("cfdp_engine: discarded incoming Fin PDU - "
             "can't exceed %lu transactions\n",
             MAX_CONCURRENT_TRANSACTIONS);

  /* If successful, initialize the state machine's MACHINE structure */
  else
    aaa__initialize (m, NULL, &hdr);

  return (m);
}



/*=r=*************************************************************************/
static MACHINE *m__start_new_receiver_machine (HDR hdr)
     /* WHAT IT DOES:  Given a PDU-header (presumably Metadata/Filedata/EOF
      *   PDU), it attempts to open a comm-link to the CFDP partner,
      *   allocate a state machine slot, and initialize the state machine.
      *   If successful, returns a pointer to the new state machine;
      *   otherwise, returns NULL.
      */
{
  MACHINE           *m = NULL;
  ID                 partner_id;
  /*------------------------------------------------------------*/

  /* Attempt to open a comm-link to the CFDP partner entity.  Since we are
   * the receiver, our partner is the source of this transaction.
   */
  partner_id = hdr.trans.source_id;
  if (!pdu_output__open (mib__get_my_id(), partner_id))
    e_msg__ ("cfdp_engine: discarded incoming PDU "
             "(unable to communicate with node '%s').\n",
             cfdp_id_as_string (partner_id));

  /* If successful, attempt to allocate a slot in the machine-list */
  else if ((m = machine_list__allocate_slot ()) == NULL)
    e_msg__ ("cfdp_engine: discarded incoming PDU - "
             "can't exceed %lu transactions\n",
             MAX_CONCURRENT_TRANSACTIONS);

  /* If successful, initialize the state machine's MACHINE structure */
  else
    aaa__initialize (m, NULL, &hdr);

  return (m);
}



/*=r=*************************************************************************/
static MACHINE *m__start_new_sender_machine (REQUEST put_request)
     /* WHAT IT DOES:  Given a User Request (presumably a Put Request)
      *   it attempts to open a comm-link to the CFDP partner,
      *   allocate a state machine slot, and initialize the state machine.
      *   If successful, returns a pointer to the new state machine;
      *   otherwise, returns NULL.
      */
{
  MACHINE           *m = NULL;
  ID                 partner_id;
  /*------------------------------------------------------------*/

  /* Attempt to open a comm-link to the CFDP partner entity.
   * (The Put Request tells us who our partner is)
   */
  partner_id = put_request.info.put.dest_id;
  if (!pdu_output__open (mib__get_my_id(), partner_id))
    e_msg__ ("cfdp_engine: discarded incoming Put Request - "
             "(unable to communicate with node '%s').\n",
             cfdp_id_as_string (partner_id));

  /* If successful, attempt to allocate a slot in the machine-list */
  else if ((m = machine_list__allocate_slot ()) == NULL)
    e_msg__ ("cfdp_engine: discarded incoming Put Request - "
             "can't exceed %lu transactions\n",
             MAX_CONCURRENT_TRANSACTIONS);

  /* If successful, initialize the state machine's MACHINE structure */
  else
    aaa__initialize (m, &put_request, NULL);

  return (m);
}



/*=r=************************************************************************/
static void m__give_request_to_one_trans (MACHINE *m, REQUEST_TYPE req_type)
     /* WHAT IT DOES:  Fires an event in the given state machine.
      *   The event is the one associated with the given request-type.
      */
{
  int               event;
  REQUEST           request;
  /*------------------------------------------------------------*/

  /* Build the Request structure */
  request.type = req_type;
  request.info.trans = m->hdr.trans;

  /* Fire the state table event associated with that request */
  event = event__determine_from_request (request);
  m__state_table (m, event, NULL, &request);
}



/*=r=************************************************************************/
static void m__give_request_to_all_trans (REQUEST_TYPE req_type)
     /* WHAT IT DOES:  Gives a User Request of the specified type to 
      *   each active transaction.  i.e. fires the event associated with
      *   the given Request-Type in every state machine.
      */
{
  MACHINE        *m;
  /*------------------------------------------------------------*/

  machine_list__open_a_walk ();
  while ((m=machine_list__get_next_machine()) != NULL)
    m__give_request_to_one_trans (m, req_type);
}



/*=r=************************************************************************/
static void m__give_pdu_to_trans (MACHINE *m, const PDU *pdu)
     /* WHAT IT DOES:  Converts the given raw PDU to a C structure,
      *   and fires the appropriate event in the given state machine.
      * NOTES:
      *   1) The appropriate event is determined by the PDU-type of the
      *      given PDU (e.g. Metadata, Filedata, EOF, etc).
      */
{
  int                     event;
  static PDU_AS_STRUCT    pdu_as_struct;
  /*------------------------------------------------------------*/
  
  /* Convert the given PDU from raw format to a C structure */
  pdu__convert_pdu_to_struct (pdu, &pdu_as_struct);

  /* Fire the appropriate state table event */
  event = event__determine_from_pdu_struct (&pdu_as_struct);

  m__state_table (m, event, &pdu_as_struct, NULL);

}



/*=r=************************************************************************/
static boolean m__is_pdu_sane (PDU_TYPE pdu_type, ROLE my_role)
     /* WHAT IT DOES:  Returns 1 if it makes sense for a PDU of the given
      *   type to be received by a transaction assigned the given role;
      *   otherwise, returns 0.
      */
{
  boolean         sane;
  /*------------------------------------------------------------*/

  sane = NO;

  if      (pdu_type == _MD_)
    {
      if ((my_role == R_1) || (my_role == R_2))
        sane = YES;
      else
        e_msg__ ("cfdp_engine: PDU rejected - "
                 "Metadata can only come from Sender.\n");
    }

  else if (pdu_type == _FD_)
    {
      if ((my_role == R_1) || (my_role == R_2))
        sane = YES;
      else
        e_msg__ ("cfdp_engine: PDU rejected - "
                 "Filedata can only come from Sender.\n");
    }

  else if (pdu_type == _EOF_)
    {
      if ((my_role == R_1) || (my_role == R_2))
        sane = YES;
      else
        e_msg__ ("cfdp_engine: PDU rejected - "
                 "EOF can only come from Sender.\n");
    }

  else if (pdu_type == _ACK_EOF_)
    {
      if (my_role == S_2)
        sane = YES;
      else
        e_msg__ ("cfdp_engine: PDU rejected - "
                 "Ack-EOF can only come from Class 2 Receiver.\n");
    }

  else if (pdu_type == _NAK_)
    {
      if (my_role == S_2)
        sane = YES;
      else
        e_msg__ ("cfdp_engine: PDU rejected - "
                 "Nak can only come from Class 2 Receiver.\n");
    }

  else if (pdu_type == _FIN_)
    {
      if (my_role == S_2)
        sane = YES;
      else
        e_msg__ ("cfdp_engine: PDU rejected - "
                 "Fin can only come from Class 2 Receiver.\n");
    }

  else if (pdu_type == _ACK_FIN_)
    {
      if (my_role == R_2)
        sane = YES;
      else
        e_msg__ ("cfdp_engine: PDU rejected - "
                 "Ack-Fin can only come from Class 2 Sender.\n");
    }

  else
    e_msg__ ("cfdp_engine: PDU rejected - unrecognized pdu-type.\n");

  return (sane);
}



/*=r=************************************************************************/
boolean cfdp_give_request (const char *request_string)
{
  /* CFDP_FILE            *fp;*/ /* See NOTE below */
  MACHINE              *m;
  REQUEST               request;
  boolean               successful = YES;
  /*------------------------------------------------------------*/

  REPORT_VERSION_ONCE;

  /*---------------------------------*/
  /* Validate the given User Request */
  /*---------------------------------*/

  if (request_string == NULL)
    {
      e_msg__ ("cfdp_engine: cfdp_give_request: given a null-pointer.\n");
      successful = NO;
    }

  else if (!utils__request_from_string (request_string, &request))
    /* The string does not represent a valid request */
    {
      e_msg__ ("cfdp_engine: request is invalid (%s).\n", request_string);
      successful = NO;
    }

  else if ((request.type == REQ_PUT) &&
           (cfdp_are_these_ids_equal (request.info.put.dest_id, 
                                      mib__get_my_id())))
    /* This Put Request is invalid */
    {
      e_msg__ ("cfdp_engine: Won't allow you to send a file to yourself!\n");
      successful = NO;
    }
  
  /* If the User Request is an "empty" one, just ignore it */
  else if (request.type == REQ_NONE)
    /* No action needed */
    d_msg__ ("cfdp_engine: ignoring an empty User Request.\n");
  

  /*---------------------------------------------------------
   * If the User Request is a Put Request, then create a new 
   * state machine, and pass the Put Request to that machine
   *---------------------------------------------------------*/
  
  else if (request.type == REQ_PUT)
    {

/* 
*  NOTE: Because file opens are taking 1ms(GPM rad750) and 99% of the time,  
*  this check passes, we'll do away with this open/close check. This check 
*  detects a non-existent file before the transfer begins. Without this check, 
*  the transfer will begin with a meta data pdu sent. Before the first file-data  
*  pdu is sent, a filestore rejection will occur followed by an EOF-Cancel.
*/

        /* Start a new state machine */
        if ((m = m__start_new_sender_machine (request)) != NULL)
            /* Fire the state table event associated with a Put Request */
            m__state_table (m, RECEIVED_PUT_REQUEST, NULL, &request);
        else
            successful = NO;


#if 0
      /* Optimization:  If the source-file doesn't exist, catch it now */
      fp = fopen_callback (request.info.put.source_file_name, "rb");
      if (fp == NULL)
        /* The source-file doesn't exist; no point in trying to transfer it */
        {
        
          e_msg__ ("cfdp_engine: can't transfer non-existent file (%s).\n",
                   request.info.put.source_file_name);
          successful = NO;
        }
      else
        {
          /* It does exist; close it and let the state machine re-open it */
          fclose_callback (fp);
          
          /* Start a new state machine */
          if ((m = m__start_new_sender_machine (request)) != NULL)
            /* Fire the state table event associated with a Put Request */
            m__state_table (m, RECEIVED_PUT_REQUEST, NULL, &request);
          else
            successful = NO;
        }
#endif

    }
  
  
  /*------------------------------------------------------------------
   * If the User Request applies to *all* transactions, then pass it
   * to all active state machines
   *------------------------------------------------------------------*/
  
  else if (request.type == REQ_REPORT)
    /* Report all active transactions */
    m__give_request_to_all_trans (request.type);
  
  else if (request.type == REQ_FREEZE)
    /* Freeze all active transactions, and any future ones too */
    {
      m__give_request_to_all_trans (request.type);
      misc__freeze_all_partners ();
    }
  
  else if (request.type == REQ_THAW)
    /* Thaw all active transactions, and any future ones too */
    {
      m__give_request_to_all_trans (request.type);
      misc__thaw_all_partners ();
    }
  
  else if (request.type == REQ_ABANDON_ALL_TRANSACTIONS)
    m__give_request_to_all_trans (REQ_ABANDON);
  
  else if (request.type == REQ_CANCEL_ALL_TRANSACTIONS)
    m__give_request_to_all_trans (REQ_CANCEL);
  
  else if (request.type == REQ_SUSPEND_ALL_TRANSACTIONS)
    m__give_request_to_all_trans (REQ_SUSPEND);
  
  else if (request.type == REQ_RESUME_ALL_TRANSACTIONS)
    m__give_request_to_all_trans (REQ_RESUME);
  
  
  /*-----------------------------------------------------------------
   * If the User Request applies to one transaction, then attempt to
   * find an active state machine assigned to that transaction, and
   * pass the Request to that state machine.
   *-----------------------------------------------------------------*/
  
  else if ((request.type == REQ_ABANDON) ||
           (request.type == REQ_CANCEL) ||
           (request.type == REQ_SUSPEND) ||
           (request.type == REQ_RESUME))
    {
      if ((m = machine_list__get_this_trans (request.info.trans)) !=  NULL)
        /* Success (a machine is assigned to this transaction) */
        m__give_request_to_one_trans (m, request.type);
      else
        /* No state machine assigned */
        {
          e_msg__ ("cfdp_engine: ignoring User-Request that references "
                   "unknown transaction (%s).\n",
                   cfdp_trans_as_string (request.info.trans));
          successful = NO;
        }
    }
  
  
  else
    {
      w_msg__ ("cfdp_engine: ignoring unrecognized User-Request (%s).\n",
               request_string);
      successful = NO;
    }
  
  return (successful);
}



/*=r=************************************************************************/
boolean cfdp_give_pdu (PDU pdu)
     /* NOTE:  The CFDP protocol requires a transaction's Inactivity-timer 
      *   to be started (or restarted) each time a PDU is received.  
      *   That action is taken here rather than in 25 different places in 
      *   all the state table modules.
      */
{
  HDR             hdr;
  MACHINE        *m;
  ROLE            my_role;
  _PDU_TYPE_      pdu_type;
  boolean         successful;
  /*------------------------------------------------------------*/
  
  REPORT_VERSION_ONCE;

  /*------------------*/
  /* Validate the pdu */
  /*------------------*/

  if (!pdu__is_this_pdu_acceptable (&pdu))
    return (0);

  pdu_type = pdu__what_type (&pdu);
  if (pdu_type == _DONT_KNOW_)
    /* PDUs such as 'Keepalive' are simply ignored */
    {
      d_msg__ ("cfdp_engine: ignored incoming PDU of unknown type.\n");
      return (1);
    }
  hdr = pdu__hdr_struct_from_pdu (&pdu);
  my_role = pdu__receiver_role_from_pdu_hdr (hdr);
  /* If we are the sender of the file, there are some pdu-types that our
   * partner should never send (for example, EOF).  If we are the receiver
   * of the file, there are other pdu-types that our partner should never
   * send (for example, Ack-EOF).
   */
  if (!m__is_pdu_sane (pdu_type, my_role))
    return (0);

  /* Perhaps put out some debug messages */
  if (cfdp_is_message_class_enabled (CFDP_MSG_DEBUG_PDU))
    {
      pdu__display_raw_pdu (pdu);
      pdu__display_disassembled_pdu (pdu);
    }
  
  /* Validation was successful; assume the response will also be successful
   * unless/until proven otherwise.
   */
  successful = YES;   

  /*----------------------------------------------------------------*/
  /* If an existing state machine is assigned to this transaction,  */
  /* pass the PDU to that state machine.                            */
  /*----------------------------------------------------------------*/

  if ((m = machine_list__get_this_trans (hdr.trans)) != NULL)
    {
      /* Conceptually, re-starting the inactivity-timer belongs in each
       * of the state tables, but it's done here for practical purposes
       * (it would be duplicated in 20+ places otherwise).
       */
      if (!m->publik.frozen && !m->publik.suspended)
        m->inactivity_timer = timer__start (mib__inactivity_timeout(NULL));
      m__give_pdu_to_trans (m, &pdu);
    }

  /*----------------------------------------------------------------*/
  /* If the incoming PDU is a MD/FD/EOF for a Receiver, then create */
  /* a new state machine and pass the PDU to that state machine.    */
  /*----------------------------------------------------------------*/

  else if (((pdu_type == _MD_) || (pdu_type == _FD_) || (pdu_type == _EOF_)) &&
           ((my_role == R_1) || (my_role == R_2)))
    {

      if ((m = m__start_new_receiver_machine (hdr)) != NULL)
        m__give_pdu_to_trans (m, &pdu);
      else
        successful = NO;
    }

  /*--------------------------------------------------------------------*/
  /* If the incoming PDU is a Finished PDU for a Class 2 Sender, then   */
  /* create a new state machine and pass the PDU to that state machine. */
  /*--------------------------------------------------------------------*/

  else if ((pdu_type == _FIN_) && (my_role == S_2))
    {
      if ((m = m__restart_old_sender_machine (hdr)) != NULL)
        m__give_pdu_to_trans (m, &pdu);
      else
        successful = NO;
    }

  /*----------------------------*/
  /* Any other PDUs are ignored */
  /*----------------------------*/

  else
    d_msg__ ("cfdp_engine: ignored an incoming PDU (trans %s).\n\n",
             cfdp_trans_as_string (hdr.trans));
             


  return (successful);
}



/*=r=************************************************************************/
void cfdp_cycle_each_transaction (void)
     /* NOTE:  In order to ensure that outgoing File-Directives get a
      *   higher priority than outgoing File-Data, it is necessary to
      *   loop through all the state machines twice.
      */
{
  MACHINE          *m;
  /*------------------------------------------------------------*/
  
  REPORT_VERSION_ONCE;

  /*--------------------------------------------------------*/
  /* Give each machine a chance to do higher-priority stuff */
  /*--------------------------------------------------------*/

  machine_list__open_a_walk ();
  while ((m=machine_list__get_next_machine()) != NULL)
    {

      /*------------------------------------------------------*/
      /* If a protocol timer has expired, initiate a response */
      /*------------------------------------------------------*/

      if (timer__expired (&m->inactivity_timer))
        /* The inactivity-timer expired */
        {
          timer__cancel (&m->inactivity_timer);
          m__state_table (m, INACTIVITY_TIMER_EXPIRED, NULL, NULL);
        }
  
      else if (timer__expired (&m->ack_timer))
        /* The ack-timer expired */
        {
          timer__cancel (&m->ack_timer);
          m__state_table (m, ACK_TIMER_EXPIRED, NULL, NULL);
        }
  
      else if (timer__expired (&m->nak_timer))
        /* The nak-timer expired */
        {
          timer__cancel (&m->nak_timer);
          m__state_table (m, NAK_TIMER_EXPIRED, NULL, NULL);
        }

      /*-------------------------------------------------------------*/
      /* Special case -- support high-speed (external) file transfer */
      /*-------------------------------------------------------------*/
      else if (m->should_external_xfer_be_closed)
        {
          m__state_table (m, EXTERNAL_FILE_TRANSFER_COMPLETED, NULL, NULL);
          m->should_external_xfer_be_closed = NO;
        }

      /*------------------------------------------------------------*/
      /* Allow up to one outgoing File-Directive PDU to be released */
      /* (Class 1 Receiver does not output PDUs)                    */
      /*------------------------------------------------------------*/
      else if (m->publik.role != R_1)
        m__state_table (m, THROTTLE_OUTGOING_FILE_DIR_PDU, NULL, NULL);
    }

  /*---------------------------------------------------*/
  /* Then allow each machine to output a File-Data PDU */
  /*---------------------------------------------------*/

  machine_list__open_a_walk ();
  while ((m=machine_list__get_next_machine()) != NULL)
    {
      /* (Only Senders output File-Data) */
      if ((m->publik.role == S_2) || (m->publik.role == S_1))
        m__state_table (m, THROTTLE_OUTGOING_FILE_DATA_PDU, NULL, NULL);
    }
}



/*=r=*************************************************************************/
boolean cfdp_open_external_file_xfer (TRANSACTION trans)
{
  MACHINE             *machine_ptr;
  /*------------------------------------------------------------*/

  /*----------------*/
  /* Validate input */
  /*----------------*/

  machine_ptr = machine_list__get_this_trans (trans);

  if (machine_ptr == NULL)
    /* Given transaction is not known to us. */
    return (0);

  else if ((machine_ptr->publik.role != CLASS_1_SENDER) &&
           (machine_ptr->publik.role != CLASS_2_SENDER))
    /* We can't disable Filedata output if we are not the Sender */
    return (0);

  /*----------------------------*/
  /* Input is valid, take action */
  /*----------------------------*/

  machine_ptr->publik.external_file_xfer = YES;
  machine_ptr->is_external_xfer_open = YES;
  return (1);
}



/*=r=*************************************************************************/
boolean cfdp_close_external_file_xfer (TRANSACTION trans)
{
  MACHINE             *machine_ptr;
  /*------------------------------------------------------------*/

  /*----------------*/
  /* Validate input */
  /*----------------*/

  machine_ptr = machine_list__get_this_trans (trans);

  if (machine_ptr == NULL)
    /* Given transaction is not known to us. */
    return (0);

  else if (!machine_ptr->is_external_xfer_open)
    /* We can't close something that isn't open */
    return (0);

  /*----------------------------*/
  /* Input is valid, take action */
  /*----------------------------*/

  machine_ptr->publik.external_file_xfer = NO;
  machine_ptr->should_external_xfer_be_closed = YES;
  return (1);
}



/*=r=*************************************************************************/
boolean cfdp_set_file_checksum (TRANSACTION trans, u_int_4 checksum)
{
  MACHINE             *machine_ptr;
   /*------------------------------------------------------------*/

  /*----------------*/
  /* Validate input */
  /*----------------*/

  machine_ptr = machine_list__get_this_trans (trans);

  if (machine_ptr == NULL)
    /* Given transaction is not known to us. */
    return (0);

  /*----------------------------*/
  /* Input is valid, take action */
  /*----------------------------*/

  machine_ptr->publik.file_checksum_as_calculated = checksum;
  return (1);
}
