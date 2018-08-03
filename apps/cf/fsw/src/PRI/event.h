/* FILE: event.h    Specs for module that determines event from PDU or Request.
 *   'Event' refers to an event in the CFDP state tables.
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
 *   2007_11_01 Tim Ray
 *     - Broke the single 'throttle_outgoing_pdus' event into 2 separate
 *       events.   (part of a bug fix)
 */

#ifndef H_EVENT
#define H_EVENT 1

#include "cfdp.h"


typedef enum 
  {
    /* Errors (to assist in debugging results of event-decoding) */
    CFDP_EVENT_INVALID_ACK,
    CFDP_EVENT_INVALID_DIR_CODE,
    CFDP_EVENT_UNKNOWN_REQUEST,
    /* Outgoing PDUs are throttled by the engine user's traffic signal
     * callback routine.  These next two events each allow up to one 
     * outgoing PDU to be released each time they occur.  
     * There are 2 events so that File Directives can get priority over
     * File Data.
     */
    THROTTLE_OUTGOING_FILE_DIR_PDU,
    THROTTLE_OUTGOING_FILE_DATA_PDU,
    /* Incoming User Requests */
    RECEIVED_PUT_REQUEST,
    RECEIVED_SUSPEND_REQUEST,
    RECEIVED_RESUME_REQUEST,
    RECEIVED_CANCEL_REQUEST,
    RECEIVED_ABANDON_REQUEST,
    RECEIVED_REPORT_REQUEST,
    RECEIVED_FREEZE_REQUEST,
    RECEIVED_THAW_REQUEST,
    /* Incoming PDUs from our CFDP Partner */
    RECEIVED_METADATA_PDU,
    RECEIVED_FILEDATA_PDU,
    RECEIVED_EOF_NO_ERROR_PDU,
    RECEIVED_ACK_EOF_NO_ERROR_PDU,
    RECEIVED_EOF_CANCEL_PDU,
    RECEIVED_ACK_EOF_CANCEL_PDU,
    RECEIVED_NAK_PDU,
    RECEIVED_FINISHED_NO_ERROR_PDU,
    RECEIVED_ACK_FIN_NO_ERROR_PDU,
    RECEIVED_FINISHED_CANCEL_PDU,
    RECEIVED_ACK_FIN_CANCEL_PDU,
    /* Expiration of protocol timers */
    ACK_TIMER_EXPIRED,
    NAK_TIMER_EXPIRED,
    INACTIVITY_TIMER_EXPIRED,
    /* A special case to support external file transfer by the engine user */
    EXTERNAL_FILE_TRANSFER_COMPLETED
  } CFDP_EVENT;


CFDP_EVENT event__determine_from_pdu_struct (const PDU_AS_STRUCT *pdu_struct);
/* WHAT IT DOES:  Returns the state-table event associated with
 *   the receipt of the given PDU.
 * NOTE: If a bug is detected, it will return UNKNOWN_CFDP_EVENT.
 */

CFDP_EVENT event__determine_from_request (REQUEST request);
/* WHAT IT DOES:  Returns the state-table event associated with
 *   the receipt of the given request.
 * NOTE: If a bug is detected, it will return UNKNOWN_CFDP_EVENT.
 */

char *event__event_as_string (CFDP_EVENT event);
/* WHAT IT DOES:  Converts the given event to a char-string */
#endif
