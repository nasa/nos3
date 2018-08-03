/* FILE: pdu_as_string.h    Specs for module that provides a text summary of
 *   various Protocol Data Unit (PDU) structures.
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
 * SUMMARY:
 *   Each routine in this module returns a text string that summarizes the
 *   data structure given to it.  For each routine, the resulting text string
 *   may be up to 'MAX_PDU_AS_STRING_LENGTH' chars long.
 * CHANGES:
 *   2007_07_31 Tim Ray
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 */


#ifndef H_PDU_AS_STRING
#define H_PDU_AS_STRING 1

#include "cfdp.h"

#define MAX_PDU_AS_STRING_LENGTH 1024

char *pdu_struct_as_string (PDU_AS_STRUCT pdu_as_struct);

char *ack_struct_as_string (ACK ack);

char *eof_struct_as_string (EOHF eof);

char *fd_struct_as_string (FD *fd);

char *fin_struct_as_string (FIN fin);

char *md_struct_as_string (MD md);

char *nak_struct_as_string (NAK nak);

#endif
