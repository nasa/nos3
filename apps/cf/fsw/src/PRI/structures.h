/* FILE:  structures.h -- private data structures; public data structures
 *   are defined in 'cfdp_data_structures.h'.
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
 * SUMMARY:  (PDU = Protocol Data Unit; i.e. a CFDP message)
 *   This file defines a single data structure that can hold any CFDP PDU,
 *   plus data structures to represent various CFDP concepts.
 *   Each PDU contains a header plus data, where data is either
 *   metadata (MD), file-data (FD), EOF, ack, nak, etc.  I have defined
 *   one structure to hold the pdu-header, structures to hold what follows
 *   the pdu-header (i.e. one structure each for metadata, file-data, 
 *   EOF, etc), and one "bottom-line" structure that can contain
 *   any CFDP PDU.  (Plus there are "building block" data-types that go
 *   inside PDUs; e.g. the CFDP Condition Code)
 * PURPOSE:  These definitions exist in the hope that their use allows
 *   the CFDP source code to be simpler (and easier to read).  
 *   The intent is that each incoming PDU is immediately converted to a 
 *   structure (which allows easier access to fields within the PDU).  
 *   Similarly, each outgoing PDU is generated using the structure, and is 
 *   converted to bits just prior to release.
 * CHANGES:
 *   2007_07_31 Tim Ray (TR)
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 *   2007_11_19 TR
 *     - Bug fix.  'max_pdu_header_length' calculation was fixed.
 *       (Previously, assumption was that entity-id length = trans-num length)
 */


#ifndef H_STRUCTURES
#define H_STRUCTURES 1

#include "cfdp_data_structures.h"    /* Public definitions */



/*********************/
/*** User Requests ***/
/*********************/

/* CFDP provides services in response to User Requests.  CFDP defines
 * a discrete set of User Requests.  All of those are expressed here,
 * plus a few extras that allow *all* transactions to be 
 * suspended/resumed/cancelled via a single Request.
 */

/* A list of the possible Request types */
typedef enum 
{ 
  REQ_NONE, 
  REQ_PUT, 
  REQ_SUSPEND, 
  REQ_RESUME, 
  REQ_CANCEL, 
  REQ_ABANDON,
  REQ_FREEZE, 
  REQ_THAW,
  REQ_REPORT, 
  REQ_SUSPEND_ALL_TRANSACTIONS,          
  REQ_RESUME_ALL_TRANSACTIONS,
  REQ_CANCEL_ALL_TRANSACTIONS,
  REQ_ABANDON_ALL_TRANSACTIONS
} REQUEST_TYPE;

/* A structure to hold a Put Request */   
typedef struct
{
  boolean      ack_required;
  ID           dest_id;
  boolean      file_transfer;
  char         source_file_name [MAX_FILE_NAME_LENGTH+1];
  char         dest_file_name [MAX_FILE_NAME_LENGTH+1];
  /* <NOT_SUPPORTED> TLVs such as Filestore Directives */
} PUT_INFO;

/* A structure that can hold any Request */
typedef struct
{
  REQUEST_TYPE  type;
  union
  {
    PUT_INFO       put;          /* Applies to Put Request only */
    TRANSACTION    trans;        /* Applies to Cancel/Suspend/Resume only */
  } info;
} REQUEST;



/*********************/
/*** Miscellaneous ***/
/*********************/

/* The rough status of each CFDP transaction is one of these */
typedef enum 
{
  TRANS_UNDEFINED, TRANS_ACTIVE, TRANS_TERMINATED, TRANS_UNRECOGNIZED
} TRANS_STAT;

/* Each PDU consists of a 'header' plus a 'data field'.  
 * Other parts of this file need to know the longest possible data field
 * length.  Here is the calculation (in bytes):
 */
/* First, determine the longest possible 'header'... */
#define TRANS_NUM_LENGTH 4    /* currently, hard-coded to 4 byte integer */
#define MAX_PDU_HEADER_LENGTH (4 + (2 * MAX_ID_LENGTH) + TRANS_NUM_LENGTH)
/* ... leaving this many bytes for the PDU 'data field' (2 bytes for CRC): */
#define MAX_PDU_DATA_FIELD_LENGTH (MAX_PDU_LENGTH - MAX_PDU_HEADER_LENGTH - 2)

/* This enumerated type short-circuits the more formal methods */
typedef enum 
{ 
  _DONT_KNOW_, _MD_, _FD_, _EOF_, _ACK_EOF_, _NAK_, _FIN_, _ACK_FIN_ 
} _PDU_TYPE_;



/*****************/
/*** File Gaps ***/
/*****************/

/* In order for CFDP to reliably deliver files, the Receiver must keep
 * track of gaps in the received file data (and report those gaps to
 * the Sender).
 */

/* This structure specifies a single gap, with support for a linked list */
struct gap
{
  u_int_4            begin;
  u_int_4            end;
  struct gap        *next;
} ;
typedef struct gap GAP;

/* If there are lots of gaps, they may not all fit in one NAK PDU.
 * This next macro defines the max gaps that will fit in one Nak PDU.
 * To understand the calculation below:
 *   1) The part of a CFDP PDU that follows the PDU-header is called the 
 *      'PDU data field'.
 *   2) The PDU data-field for a Nak PDU has 9 bytes, plus 8 bytes per gap.
 *      (Byte #1 specifies that this is a Nak PDU)
 *      (Bytes #2-9 specify the start-of-scope and end-of-scope; 
 *       i.e. to what portion of the file does this Nak apply?)
 *      (Each additional 8 bytes specify start-offset and end-offset of a gap)
 */
#define MAX_GAPS_PER_NAK_PDU ((MAX_PDU_DATA_FIELD_LENGTH - 9) / 8)



/* For outgoing PDUs, the transaction-sequence-number field is hard-coded.
 * Strictly speaking, this violates the CFDP standard because this field
 * is variable-length, and up to 8 bytes long.
 * However, a 4-byte counter can accomodate one new transaction every second
 * for over 100 years before rolling over (i.e. 100 years worth of 
 * transactions will each have a unique ID).
 */
#define HARD_CODED_TRANS_SEQ_NUM_LENGTH 4




/****************************************/
/****************************************/
/********** PDU data structure **********/
/****************************************/
/****************************************/

/* Every PDU has one of these two structures:
 *       header + file-data
 *       header + file-directive-code + file-directive
 * Whatever follows the header is referred to as the 'data field'.
 */



/******************/
/*** PDU Header ***/
/******************/

/* ('PDU_TYPE' is defined publicly) */

/* Each CFDP PDU is either sent to the Receiver or back to the Sender */
typedef enum {TOWARD_RECEIVER, TOWARD_SENDER} PDU_DIRECTION;

/* Each CFDP transaction uses either Acknowledged or Unacknowledged Service */
typedef enum {ACK_MODE, UNACK_MODE} PDU_MODE;

/* Each CFDP PDU has a generic header; the header contains these fields */
typedef struct
{
  PDU_TYPE        pdu_type;
  PDU_DIRECTION   direction;
  PDU_MODE        mode;
  boolean         use_crc;
  TRANSACTION     trans;
  ID              dest_id;
} HDR;



/******************/
/*** Data field ***/
/******************/

typedef enum 
{
  CFDP_FILE_DIR_RESERVED_0, 
  CFDP_FILE_DIR_RESERVED_1, 
  CFDP_FILE_DIR_RESERVED_2, 
  CFDP_FILE_DIR_RESERVED_3,
  EOF_PDU, 
  FIN_PDU, 
  ACK_PDU, 
  MD_PDU, 
  NAK_PDU, 
  PROMPT_PDU, 
  SUSPEND_PDU, 
  RESUME_PDU,
  KEEPALIVE_PDU, 
  CFDP_FILE_DIR_RESERVED_13,
  CFDP_FILE_DIR_RESERVED_14, 
  CFDP_FILE_DIR_RESERVED_15
} FILE_DIR_CODE;

/* Metadata */
/* (defined publicly) */

/* File-data */
typedef struct
{
  u_int_4         offset;
  u_int_1         buffer [MAX_FILE_CHUNK_SIZE+4];
  u_int_2         buffer_length;
} FD;

/* EOF.  Note that "EOF" is used in 'stdio.h', so the name is "EOHF" */
typedef struct
{
  u_int_1         condition_code : 4;
  u_int_1         spare          : 4;
  u_int_4         file_checksum;
  u_int_4         file_size;
} EOHF;

/* Finished */
typedef struct
{
  u_int_1         condition_code    : 4;
  u_int_1         end_system_status : 1;
  u_int_1         delivery_code     : 1;
  u_int_1         spare             : 2;
  /* <NOT_SUPPORTED> TLVs (i.e. Filestore Responses) */
} FIN;

/* Ack */
typedef struct
{
  u_int_1         directive_code          : 4;
  u_int_1         directive_subtype_code  : 4;
  u_int_1         condition_code          : 4;
  u_int_1         delivery_code           : 2;
  u_int_1         transaction_status      : 2;   /* see 'TRANS_STAT' above */
} ACK;

/* Nak.
 * Note:  The current implementation of the Class 2 Receiver uses this
 *   as a Nak-list (i.e. if dynamic memory allocation is enabled, then
 *   there is no limit to the number of gaps that can be stored).
 * Note:  A Nak-list may hold more gaps than can fit in one Nak PDU.
 *   (Consider the case where 'MAX_GAPS_PER_TRANSACTION' is greater than 
 *   'MAX_GAPS_PER_NAK_PDU')
 */
typedef struct
{
  u_int_4         start_of_scope;
  u_int_4         end_of_scope;
  boolean         is_metadata_missing : 1;
  GAP            *head;
#if IS_DYNAMIC_ALLOCATION_ENABLED==0
  boolean         in_use [MAX_GAPS_PER_TRANSACTION];
  GAP             gap [MAX_GAPS_PER_TRANSACTION];
#elif IS_DYNAMIC_ALLOCATION_ENABLED==1
#else
  #error Enable or disable dynamic memory allocation in 'memory_use.h'.
#endif
} NAK;

/* In my view, the 'data field' is either filedata or one of the file
 * directives.
 */
typedef union
{
  MD            md;
  FD            fd;
  EOHF          eof;
  FIN           fin;
  ACK           ack;
  NAK           nak;
} DATA_FIELD;



/***********/
/*** PDU ***/
/***********/

/* Finally, a single structure that can be used to store any CFDP PDU.
 * 'hdr' always contains the pdu-header.  
 * If 'is_this_a_file_data_pdu' is set, then the pdu contains Filedata.
 * If not, then the pdu contains a File Directive, 
 * and 'dir_code' tells us which type of File Directive.
 * 'data_field' contains the actual Filedata or File Directive.
 */

typedef struct
{
  HDR             hdr;
  boolean         is_this_a_file_data_pdu;
  FILE_DIR_CODE   dir_code;
  DATA_FIELD      data_field;
} PDU_AS_STRUCT;

#endif


