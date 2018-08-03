/* FILE: cfdp_data_structures.h -- CFDP-related data structures
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
 * LAST MODIFIED:  2008_01_24
 * ORIGINAL PROGRAMMER:  Tim Ray 301-286-0581
 * SUMMARY:  CFDP-related concepts/data are expressed as 'C' structures
 *   and enumerated types.
 * MORE INFO:  Check the official CFDP Blue Book:
 *   CCSDS Document #727.0-B-2 (www.ccsds.org)
 * CHANGES:
 *   2006_08_18 Tim Ray
 *     - Added 2 fields to the 'summary_status' structure (so that
 *       unsuccessful transactions can be tracked).
 *   2006_08_24 Tim Ray
 *     - Renamed the enumerated values of the 'ROLE' datatype to be 
 *       meaningful to someone besides me.
 *       (e.g. S_1 is now CLASS_1_SENDER).
 *   2006_09_08 Tim Ray
 *     - The name of the structure that holds generic data is now 
 *       'CFDP_DATA' rather than 'DATA'.  This avoids a name-conflict
 *       within the ASIST ground system.
 *   2007_05_03 Tim Ray
 *     - Added a field to the TRANS_STATUS to support external file transfer.
 *   2007_05_07 Tim Ray
 *     - Added the 'are_any_partners_frozen' field to SUMMARY_STATUS.
 *   2007_05_08 Tim Ray
 *     - Added 'PDU_TYPE' definition.
 *   2007_05_10 Tim Ray
 *     - Added 2 new Indications - machine_allocated and machine_deallocated.
 *   2007_05_16 Tim Ray
 *     - Added 'TIMER_TYPE'.
 *   2007_06_05 Tim Ray
 *     - Added 'phase' to the TRANS_STATUS.
 *   2007_06_07 Tim Ray
 *     - Added 'start_time' to the TRANS_STATUS.
 *     - Added 'abandoned', 'attempts', 'cancelled', and 
 *       'file_checksum_as_calculated' to the TRANS_STATUS.
 *   2007_06_08 Tim Ray
 *     - Added new enumerated type 'FINAL_STATUS'.
 *     - Added 'finished' and 'final_status' to TRANS_STATUS.
 *     - Added 'how_many_naks' to TRANS_STATUS.
 *   2007_06_12 Tim Ray
 *     - Changed the include of time.h from "time.h" to <time.h>
 *   2008_01_24 Tim Ray
 *     - Added the field 'is_this_trans_solely_for_ack_fin' to the trans_status
 *       data structure.  (allows the engine user to distinguish between a 
 *       nominal transaction and one that exists solely to retransmit an
 *       Ack-Finished PDU that was apparently dropped the first time around).
 */

/* List of data structures:
 *
 *    ---- 'Pure' CFDP ----
 *    ID                       - identity of a CFDP entity
 *    TRANSACTION              - identity of a specific transaction
 *    PDU_TYPE                 - each PDU is either Filedata or directive.
 *    CONDITION_CODE           - list of possible transaction status
 *    RESPONSE                 - list of possible responses to 'faults'
 *    MD                       - Metadata for a transaction
 *    STATE                    - list of possible transaction states
 *    ROLE                     - Sender Class 1, Sender Class 2, etc.
 *    DELIVERY_CODE            - Has transaction delivered all data?
 *    TIMER_TYPE               - list of protocol timers
 *    INDICATION_TYPE          - list of Indication types
 *
 *    ---- Utility ----
 *    CFDP_DATA                - generic data container
 *    FINAL_STATUS             - the final status of any one transaction
 *    TRANSACTION_STATUS       - one transaction's status
 *    SUMMARY_STATUS           - summary of all transactions' status
 */



#ifndef H_CFDP_DATA_STRUCTURES
#define H_CFDP_DATA_STRUCTURES 1

#include <time.h>
#include "cfdp_config.h"   /* Compile-time configuration of CFDP */

#define NO 0
#define YES 1



/*****************/
/*** Entity-ID ***/
/*****************/

/* Each CFDP Entity is supposed to have a unique ID.  This ID is 
 * variable-length, and up to 8 bytes long.  
 */

typedef struct
{
  u_int_1      length;
  u_int_1      value[MAX_ID_LENGTH];
} ID;



/***************************************/
/*** CFDP Transaction identification ***/
/***************************************/

/* Each CFDP Entity assigns a transaction-sequence-number to each
 * transaction that it initiates.
 * Each CFDP transaction is uniquely identified by the combination of
 * the Entity-ID of the source entity plus the transaction-sequence-number.
 */

typedef struct
{
  ID              source_id;
  u_int_4         number;
} TRANSACTION;



/****************/
/*** PDU Type ***/
/****************/

/* Each CFDP PDU contains either a File-Directive or File-Data */
typedef enum {FILE_DIR_PDU, FILE_DATA_PDU} PDU_TYPE;



/**********************/
/*** Condition Code ***/
/**********************/

/* The Condition-Code is a field within several of the Protocol Data Units.  
 * It indicates the current condition of a transaction.  
 * Nominally, it is 'NO_ERROR'.
 */

#define  NUMBER_OF_CONDITION_CODES  16

typedef enum 
{ 
  NO_ERROR, 
  POSITIVE_ACK_LIMIT_REACHED, 
  KEEP_ALIVE_LIMIT_REACHED, 
  INVALID_TRANSMISSION_MODE, 
  FILESTORE_REJECTION, 
  FILE_CHECKSUM_FAILURE, 
  FILE_SIZE_ERROR, 
  NAK_LIMIT_REACHED,
  INACTIVITY_DETECTED, 
  INVALID_FILE_STRUCTURE, 
  RESERVED_BY_CCSDS_10, 
  RESERVED_BY_CCSDS_11, 
  RESERVED_BY_CCSDS_12, 
  RESERVED_BY_CCSDS_13,
  SUSPEND_REQUEST_RECEIVED, 
  CANCEL_REQUEST_RECEIVED 
} CONDITION_CODE;



/****************/
/*** Response ***/
/****************/

/* In response to error conditions (which CFDP calls 'faults'), CFDP 
 * defines a discrete set of possible responses:
 *    Ignore -- simply ignore that the fault occurred
 *    Suspend -- suspend the transaction (the User can resume it later)
 *    Cancel -- cancel the transaction (transaction partner is alerted)
 *    Abandon -- kill the transaction without telling the partner
 *    (all the other values are reserved for future use)
 */

typedef enum 
{ 
  RESPONSE_RESERVED_0, 
  RESPONSE_CANCEL, 
  RESPONSE_SUSPEND, 
  RESPONSE_IGNORE, 
  RESPONSE_ABANDON, 
  RESPONSE_RESERVED_5, RESPONSE_RESERVED_6, RESPONSE_RESERVED_7,
  RESPONSE_RESERVED_8, RESPONSE_RESERVED_9, RESPONSE_RESERVED_10, 
  RESPONSE_RESERVED_11, RESPONSE_RESERVED_12, RESPONSE_RESERVED_13,
  RESPONSE_RESERVED_14, RESPONSE_RESERVED_15 
} RESPONSE;



/****************/
/* Metadata PDU */
/****************/

/* This structure contains info that is useful for reports.  It also
 * represents the Metadata PDU that is sent at the beginning of each
 * transaction.
 */

typedef struct
{
  boolean         file_transfer : 1;
  boolean         segmentation_control : 1;
  u_int_4         file_size;
  char            source_file_name [MAX_FILE_NAME_LENGTH+1];
  char            dest_file_name [MAX_FILE_NAME_LENGTH+1];
  /* <NOT_SUPPORTED> TLVs (e.g. Filestore Directives) */
} MD;



/*********************/
/*** Miscellaneous ***/
/*********************/

/* Each transaction is in one of these states */
typedef enum 
{ 
  UNINITIALIZED, S1, S2, S3, S4, S5, S6, FINISHED 
} STATE;

/* For each transaction, my entity's role is one of these.
 * (Sender Class 1, Receiver Class 1, Sender Class 2, Receiver Class 2)
 */
typedef enum 
{ 
  ROLE_UNDEFINED, 
  CLASS_1_SENDER,
  CLASS_1_RECEIVER,
  CLASS_2_SENDER,
  CLASS_2_RECEIVER
} ROLE;

/* For each CFDP transaction, the state of data delivery is one of these */
typedef enum 
{
  DONT_CARE_0, DONT_CARE_1, DATA_COMPLETE, DATA_INCOMPLETE
} DELIVERY_CODE;

/* The CFDP protocol defines 3 timers */
typedef enum
{
  NO_TIMER,
  ACK_TIMER,
  NAK_TIMER,
  INACTIVITY_TIMER
} TIMER_TYPE;



/*******************/
/*** Indications ***/
/*******************/

/* CFDP defines a discrete set of Indications that are to be issued to 
 * the User by the core protocol.  For example, when a transaction completes, 
 * an Indication is issued.  
 */

typedef enum
{
  IND_ABANDONED,
  IND_ACK_TIMER_EXPIRED,
  IND_EOF_RECV,
  IND_EOF_SENT,
  IND_FAULT,
  IND_FILE_SEGMENT_SENT,
  IND_FILE_SEGMENT_RECV,
  IND_INACTIVITY_TIMER_EXPIRED,
  IND_MACHINE_ALLOCATED,
  IND_MACHINE_DEALLOCATED,
  IND_METADATA_RECV,        /* Typical transaction startup at Receiving end */
  IND_METADATA_SENT,
  IND_NAK_TIMER_EXPIRED,
  IND_REPORT,
  IND_RESUMED,
  IND_SUSPENDED,
  IND_TRANSACTION,          /* Transaction startup at Sending end */
  IND_TRANSACTION_FINISHED
} INDICATION_TYPE;



/******************************/
/*** Generic data container ***/
/******************************/

/* This structure is a generic container for holding data.  The data is stored
 * in 'content', and 'length' tells how many bytes long it is.
 */

typedef struct
{
  u_int_4     length;
  u_int_1     content [MAX_DATA_LENGTH];
} CFDP_DATA;



/*********************************************/
/*** Final status (of any one transaction) ***/
/*********************************************/

typedef enum 
{ 
  FINAL_STATUS_UNKNOWN, 
  FINAL_STATUS_SUCCESSFUL,
  FINAL_STATUS_CANCELLED, 
  FINAL_STATUS_ABANDONED,
  FINAL_STATUS_NO_METADATA
} FINAL_STATUS;



/**************************/
/*** Transaction status ***/
/**************************/

/* This info applies to a single transaction (each transaction has its
 * own status info).  This info may be useful for reporting purposes.  
 * To get general information about the transaction:
 *    1) Look at 'trans' to determine the transaction-id.
 *    2) Look at 'role' to determine the role of the local entity 
 *       (e.g. Class 2 Sender)
 *    3) Look inside 'md' to determine the source and destination files.
 *       (But, if local entity is the receiver, 'md' will not contain
 *       real info until 'has_md_been_received' is set)
 *    4) Look at 'start_time' to determine when the transaction started.
 * To see if the transaction has finished:
 *    1) Look at 'finished'.  If it is set, the transaction is over;
 *       otherwise, the transaction is still in-progress.
 * To monitor an in-progress transaction:
 *    1) Look at 'frozen' to see if the transaction is frozen.
 *    2) Look at 'suspended' to see if the transaction is suspended.
 *    3) Look at 'cancelled' to see if the transaction is being cancelled.
 *    3a) If the transaction is being cancelled, look at 'condition_code'
 *        to determine why (e.g. 'Nak limit reached').
 *    4) Look at 'phase' to see what phase the transaction is in.
 *    5) Look at 'how_many_naks' to see how many Nak PDUs have been
 *       sent (by the Receiver) or received (by the Sender).  
 *    6) Call one or more of the 'cfdp_transaction_...' routines
 *       to get transaction-progress, see what timers are running, etc.
 * To determine the outcome of a transaction that has finished:
 *    1) Look at 'final_status' (should be either successful, cancelled,
 *       or abandoned).
 *    2) If the final status is not successful, look at 'condition_code'
 *       to find out why (e.g. 'Nak limit reached').
 *       Note:  If the transaction was abandoned due to a local User Request
 *       to abandon, then the condition-code will be 'no-error'.
 */

typedef struct
{
  TRANSACTION     trans;          /* This identifies the transaction */
  boolean         abandoned:1;    /* Has this transaction been abandoned? */
  u_int_4         attempts;       /* How many attempts to send current PDU? */
  boolean         cancelled:1;    /* Has this transaction been cancelled? */
  CONDITION_CODE  condition_code;    /* See the 'Blue Book' */
  DELIVERY_CODE   delivery_code;     /* See the 'Blue Book' */
  boolean         external_file_xfer:1;  /* Is engine user sending Filedata? */
  u_int_4         fd_offset;      /* Offset of last Filedata sent/received */
  u_int_4         fd_length;      /* Length of last Filedata sent/received */
  u_int_4         file_checksum_as_calculated;
  FINAL_STATUS    final_status;   
  boolean         finished:1;     /* Has this trans finished? */
  boolean         frozen:1;       /* Is this transaction currently frozen? */
  boolean         has_md_been_received:1;
  u_int_1         how_many_naks;  /* How many Nak PDUs have been sent/recd? */
  boolean         is_this_trans_solely_for_ack_fin:1;  /* re-start of old trans */
  MD              md;             /* Metadata; includes dest file name */
  ID              partner_id;     /* Who is this transaction with? */
  u_int_1         phase;          /* Either 1, 2, 3, or 4 */
  u_int_4         received_file_size;
  ROLE            role;           /* (e.g. Receiver Class 1) */
  STATE           state;
  time_t          start_time;     /* When was this transaction started? */
  boolean         suspended:1;    /* Is this trans currently suspended? */
  char            temp_file_name [MAX_TEMP_FILE_NAME_LENGTH+1];
} TRANS_STATUS;



/**********************/
/*** SUMMARY STATUS ***/
/**********************/

/* This structure contains a summary status for all transactions.
 * Most items answer a "How many ..." question.
 */

typedef struct
{
  boolean         are_any_partners_frozen; /* Can be true even if there are
					    * no transactions in-progress.
					    */
  u_int_4         how_many_senders;        /* ...active Senders? */
  u_int_4         how_many_receivers;      /* ...active Receivers? */
  u_int_4         how_many_frozen;         /* ...trans are frozen? */
  u_int_4         how_many_suspended;      /* ...trans are suspended? */
  u_int_4         total_files_sent;        /* ...files sent succesfully */
  u_int_4         total_files_received;    /* ...files received successfully */
  u_int_4         total_unsuccessful_senders;
  u_int_4         total_unsuccessful_receivers;
} SUMMARY_STATUS;
  
#endif
