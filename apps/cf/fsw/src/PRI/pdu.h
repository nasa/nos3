/* FILE: pdu.h -- specs for a module that handles PDUs (e.g. assembles,
 *   disassembles, and displays PDUs).
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
 * BIG PICTURE:  The intent is to allow most of the CFDP library to work
 *   with PDUs as if they were standard "C" data structures.  Each incoming
 *   (raw) PDU is immediately converted to a C data structure, and each
 *   outgoing PDU is converted from the internal data structure to raw bytes 
 *   at the last possible moment.
 *   Hopefully, this allows the library source code to be easier to read,
 *   and, perhaps, more efficient as well.
 * SUMMARY:  There are routines for converting from C data structures to
 *   raw PDU bytes (assembly), and vice versa (disassembly).
 *   Plus, there are a few special purpose routines.
 * CHANGES:
 *   2007_07_31 Tim Ray
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 */


#ifndef H_PDU
#define H_PDU 1

#include "cfdp.h"
#include "cfdp_private.h"


/*-------------------------------------------------------*/
/* PDU assembly (generating a raw pdu from C structures) */
/*-------------------------------------------------------*/

/* Each of these routines makes a raw PDU from the given data structures */
void pdu__make_md_pdu (HDR header, MD md, PDU *pdu);
void pdu__make_fd_pdu (HDR header, FD *fd, PDU *pdu);
void pdu__make_eof_pdu (HDR header, EOHF eof, PDU *pdu);
void pdu__make_fin_pdu (HDR header, FIN fin, PDU *pdu);
void pdu__make_ack_pdu (HDR header, ACK ack, PDU *pdu);
void pdu__make_nak_pdu (HDR header, NAK nak, PDU *pdu);


/*----------------------------------------------------------*/
/* PDU disassembly (generating C structures from a raw pdu) */
/*----------------------------------------------------------*/

/* This returns YES if the given pdu is valid and can be stored within
 * our 'PDU_AS_STRUCT' structure; otherwise, returns NO.
 */
boolean pdu__is_this_pdu_acceptable (PDU *pdu);

/* To represent the entire raw PDU as a C structure */
void pdu__convert_pdu_to_struct (const PDU *pdu, PDU_AS_STRUCT *p_a_s);

/* To represent just the Header portion of the PDU as a C structure */
HDR pdu__hdr_struct_from_pdu (const PDU *pdu);

/* To represent just the data-field portion of the PDU as a C structure */
MD pdu__md_struct_from_md_pdu (const PDU *pdu);
void pdu__fd_struct_from_fd_pdu (const PDU *pdu, FD *fd);
EOHF pdu__eof_struct_from_eof_pdu (const PDU *pdu);
FIN pdu__fin_struct_from_fin_pdu (const PDU *pdu);
ACK pdu__ack_struct_from_ack_pdu (const PDU *pdu);
void pdu__nak_struct_from_nak_pdu (const PDU *pdu, NAK *nak);

/* This returns the official PDU-type (either File-data or File-directive) */
PDU_TYPE pdu__type (const PDU *pdu);

/* This returns an unofficial PDU-type that is more practical */
_PDU_TYPE_ pdu__what_type (const PDU *pdu);

/* This routine is given a Directive Pdu and returns the file-directive code
 * (e.g. Metadata, EOF, Ack, Nak, Finished)
 */
FILE_DIR_CODE pdu__file_dir_code (const PDU *pdu);


/*-----------------*/
/* Special routine */
/*-----------------*/

/* This routine is given a C data structure (representing the PDU header), 
 * and returns the role played by the <receiver> of the PDU 
 * (e.g. Class 1 Sender).
 */
ROLE pdu__receiver_role_from_pdu_hdr (HDR hdr);


/*---------------*/
/* Display a PDU */
/*---------------*/

void pdu__display_raw_pdu (PDU pdu);
/* WHAT IT DOES:  Displays the given PDU as raw hexadecimal bytes */

void pdu__display_disassembled_pdu (PDU pdu);
/* WHAT IT DOES:  Attempts to interpret the contents of the given PDU,
 *   and display this interpretation.
 */

#endif
