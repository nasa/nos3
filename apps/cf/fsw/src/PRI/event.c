/* FILE: event.c   Determines event from PDU or Request.
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
 * SPECS: event.h
 * ORIGINAL PROGRAMMER:  Tim Ray 301-286-0581
 * CHANGES:
 *   2007_07_31 Tim Ray (TR)
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 *   2007_11_01 TR
 *     - The 'event__event_as_string' routine had to be updated because the
 *       single 'throttle_outgoing...' event was broken into 2 events.
 *   2007_12_06 TR
 *     - Enhancement.  Message-class filtering can now be changed on-the-fly.
 *   2008_03_03 TR
 *     - Editorial.  Removed an unneccessary 'return (event)' from
 *       the routine 'event__determine_from_pdu_struct'.
 */

#include "cfdp.h"
#include "cfdp_private.h"



/*=r=************************************************************************/
CFDP_EVENT event__determine_from_pdu_struct (const PDU_AS_STRUCT 
                                             *pdu_as_struct)
   {
     CFDP_EVENT      event;
     boolean         is_msg_needed;
     FILE_DIR_CODE   which_dir_is_being_acked;
   /*------------------------------------------------------------*/

     /*---------------------------------------------------------------*/
     /* If configured to do so, output a message summarizing this PDU */
     /*---------------------------------------------------------------*/
     if (pdu_as_struct->is_this_a_file_data_pdu)
       {
         if (cfdp_is_message_class_enabled (CFDP_MSG_PDU_FILEDATA))
           is_msg_needed = YES;
         else
           is_msg_needed = NO;
       }
     else if (cfdp_is_message_class_enabled (CFDP_MSG_PDU_NON_FILEDATA))
       is_msg_needed = YES;
     else
       is_msg_needed = NO;
     if (is_msg_needed)
       d_msg__ ("----------> %s\n", pdu_struct_as_string (*pdu_as_struct));

     /*----------------------------------------------------*/
     /* Determine which event corresponds to the given PDU */
     /*----------------------------------------------------*/

     if (pdu_as_struct->is_this_a_file_data_pdu)
       event = RECEIVED_FILEDATA_PDU;

     else if (pdu_as_struct->dir_code == MD_PDU)
       event = RECEIVED_METADATA_PDU;

     else if (pdu_as_struct->dir_code == EOF_PDU)
       {
         /* Is the PDU an EOF (No Error) or an EOF (Cancel)? */
         if (pdu_as_struct->data_field.eof.condition_code == NO_ERROR)
           event = RECEIVED_EOF_NO_ERROR_PDU;
         else 
           event = RECEIVED_EOF_CANCEL_PDU;
       }

     else if (pdu_as_struct->dir_code == FIN_PDU)
       {
         /* Is the PDU is a Finished (No Error) or a Finished (Cancel)? */
         if (pdu_as_struct->data_field.fin.condition_code == NO_ERROR)
           event = RECEIVED_FINISHED_NO_ERROR_PDU;
         else 
           event = RECEIVED_FINISHED_CANCEL_PDU;
       }
     
     else if (pdu_as_struct->dir_code == ACK_PDU)
       {
         /* Is this PDU an Ack of an EOF PDU or an Ack of a Finished PDU? */
         which_dir_is_being_acked = 
           pdu_as_struct->data_field.ack.directive_code;
         if (which_dir_is_being_acked == EOF_PDU)
           {
             /* Is this Ack-EOF PDU an ack of an EOF-no-error or EOF-Cancel?*/
             if (pdu_as_struct->data_field.ack.condition_code == NO_ERROR)
               event = RECEIVED_ACK_EOF_NO_ERROR_PDU;
             else
               event = RECEIVED_ACK_EOF_CANCEL_PDU;
           }
         else if (which_dir_is_being_acked == FIN_PDU)
           {
             /* Is this Ack-Fin PDU an ack of a Fin-no-error or Fin-Cancel?*/
             if (pdu_as_struct->data_field.ack.condition_code == NO_ERROR)
               event = RECEIVED_ACK_FIN_NO_ERROR_PDU;
             else
               event = RECEIVED_ACK_FIN_CANCEL_PDU;
           }
         else
           /* Invalid Ack - this "can't happen unless there is a bug" */
           {
             e_msg__ ("cfdp_engine: received Ack of unknown dir-type (%u).\n",
                      pdu_as_struct->dir_code);
             ASSERT__BUG_DETECTED;
             event = CFDP_EVENT_INVALID_ACK;
           }
       }

     else if (pdu_as_struct->dir_code == NAK_PDU)
       event = RECEIVED_NAK_PDU;

     else
       /* The Directive Code is unrecognized */
       {
         w_msg__ ("cfdp_engine: received PDU with unknown dir-code (%u).\n",
                  pdu_as_struct->dir_code);
         event = CFDP_EVENT_INVALID_DIR_CODE;
       }
     
     return (event);
   }



/*=r=************************************************************************/
CFDP_EVENT event__determine_from_request (REQUEST request)
   {
     int        event;
   /*------------------------------------------------------------*/

     if (request.type == REQ_PUT)
       event = RECEIVED_PUT_REQUEST;

     else if (request.type == REQ_ABANDON)
       event = RECEIVED_ABANDON_REQUEST;

     else if (request.type == REQ_SUSPEND)
       event = RECEIVED_SUSPEND_REQUEST;

     else if (request.type == REQ_RESUME)
       event = RECEIVED_RESUME_REQUEST;

     else if (request.type == REQ_CANCEL)
       event = RECEIVED_CANCEL_REQUEST;

     else if (request.type == REQ_REPORT)
       event = RECEIVED_REPORT_REQUEST;

     else if (request.type == REQ_FREEZE)
       event = RECEIVED_FREEZE_REQUEST;

     else if (request.type == REQ_THAW)
       event = RECEIVED_THAW_REQUEST;

     else
       {
         w_msg__ ("cfdp_engine: User Request (%u) is unknown.\n",
                  request.type);
         ASSERT__BUG_DETECTED;
         event = CFDP_EVENT_UNKNOWN_REQUEST;
       }

     return (event);
   }



/*=r=*************************************************************************/
char *event__event_as_string (CFDP_EVENT event)
{
  static char         string [128];
  /*------------------------------------------------------------*/
  
  if (event == CFDP_EVENT_INVALID_ACK)
    strcpy (string, "invalid ack");

  else if (event == CFDP_EVENT_INVALID_DIR_CODE)
    strcpy (string, "invalid dir-code");

  else if (event == CFDP_EVENT_UNKNOWN_REQUEST)
    strcpy (string, "unknown request");

  else if (event == RECEIVED_PUT_REQUEST)
    strcpy (string, "received Put Request");

  else if (event == RECEIVED_SUSPEND_REQUEST)
    strcpy (string, "received Suspend Request");

  else if (event == RECEIVED_RESUME_REQUEST)
    strcpy (string, "received Resume Request");

  else if (event == RECEIVED_CANCEL_REQUEST)
    strcpy (string, "received Cancel Request");

  else if (event == RECEIVED_ABANDON_REQUEST)
    strcpy (string, "received Abandon Request");

  else if (event == RECEIVED_REPORT_REQUEST)
    strcpy (string, "received Report Request");

  else if (event == RECEIVED_FREEZE_REQUEST)
    strcpy (string, "received Freeze Request");

  else if (event == RECEIVED_THAW_REQUEST)
    strcpy (string, "received Thaw Request");

  else if (event == RECEIVED_METADATA_PDU)
    strcpy (string, "received Metadata PDU");

  else if (event == RECEIVED_FILEDATA_PDU)
    strcpy (string, "received Filedata PDU");

  else if (event == RECEIVED_EOF_NO_ERROR_PDU)
    strcpy (string, "received EOF-no-error PDU");

  else if (event == RECEIVED_ACK_EOF_NO_ERROR_PDU)
    strcpy (string, "received Ack-EOF-no-error PDU");

  else if (event == RECEIVED_EOF_CANCEL_PDU)
    strcpy (string, "received EOF-Cancel PDU");

  else if (event == RECEIVED_ACK_EOF_CANCEL_PDU)
    strcpy (string, "received Ack-EOF-Cancel PDU");

  else if (event == RECEIVED_NAK_PDU)
    strcpy (string, "received Nak PDU");

  else if (event == RECEIVED_FINISHED_NO_ERROR_PDU)
    strcpy (string, "received Finished-no-error PDU");

  else if (event == RECEIVED_ACK_FIN_NO_ERROR_PDU)
    strcpy (string, "received Ack-Finished-no-error PDU");

  else if (event == RECEIVED_FINISHED_CANCEL_PDU)
    strcpy (string, "received Finished-Cancel PDU");

  else if (event == RECEIVED_ACK_FIN_CANCEL_PDU)
    strcpy (string, "received Ack-Finished-Cancel PDU");

  else if (event == ACK_TIMER_EXPIRED)
    strcpy (string, "ack-timer expired");

  else if (event == NAK_TIMER_EXPIRED)
    strcpy (string, "nak-timer expired");

  else if (event == INACTIVITY_TIMER_EXPIRED)
    strcpy (string, "inactivity-timer expired");

  else if (event == THROTTLE_OUTGOING_FILE_DIR_PDU)
    strcpy (string, "throttle outgoing File-Dir PDU");

  else if (event == THROTTLE_OUTGOING_FILE_DATA_PDU)
    strcpy (string, "throttle outgoing File-Data PDU");

  else if (event == EXTERNAL_FILE_TRANSFER_COMPLETED)
    strcpy (string, "external file transfer completed");

  else
    strcpy (string, "?");

  return (string);
}



