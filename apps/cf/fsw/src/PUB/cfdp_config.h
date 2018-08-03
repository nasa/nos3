/* FILE: cfdp_config.h -- configuration of the CFDP library.
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
 * LAST MODIFIED:  2007_06_11
 * ORIGINAL PROGRAMMER:  Tim Ray 301-286-0581
 * SUMMARY:  The CFDP library user is free to modify this file as desired
 *   (all other files in the PUB and PRI directories are not to be changed).
 *   Use this file to configure the CFDP library.
 * CHANGES:
 *   2006_08_21 Tim Ray
 *     - Boiled 5 Filedata-size parameters down to 2.  
 *   2006_09_08 Tim Ray
 *     - Put "#ifndef" around each generic data type (to use the lib in
 *       ASIST).
 *   2007_05_17 Tim Ray
 *     - No longer allow a choice between enabling/disabling dynamic
 *       memory allocation.  Engine always uses static allocation.
 *   2007_06_11 Tim Ray
 *     - Sets default value for new MIB parameter 'save_incomplete_files'.
 */

#ifndef H_CFDP_CONFIG
#define H_CFDP_CONFIG 1

#include "osconfig.h"
#include "cf_platform_cfg.h"
#include "cf_defs.h"

/**************************/
/*** Generic data types ***/
/**************************/

/* The intent is to isolate any compiler-specific changes.
 * 'u_int_1' means "unsigned integer, 1 byte long",
 * 's_int_4' means "signed integer, 4 bytes long", etc.
 */

#ifndef boolean
#define  boolean  unsigned char
#endif

#ifndef u_int_1
#define  u_int_1  unsigned char
#endif

#ifndef u_int_2
#define  u_int_2  unsigned int
#endif

#ifndef u_int_4
#define  u_int_4  unsigned long int
#endif

#ifndef s_int_1
#define  s_int_1  signed char
#endif

#ifndef s_int_2
#define  s_int_2  signed int
#endif

#ifndef s_int_4
#define  s_int_4  signed long int
#endif



/********************/
/*** Memory usage ***/
/********************/

/* This section tells the CFDP library how much storage capacity is required.
 * All lengths are in bytes.  PDU = Protocol Data Unit.
 * (There used to be an option to choose whether or not dynamic memory
 * allocation is enabled.  There is no longer an option.  Dynamic memory
 * is always disabled.)
 */

/* The maximum number of concurrent transactions. */
#define MAX_CONCURRENT_TRANSACTIONS     CF_MAX_SIMULTANEOUS_TRANSACTIONS

/* The maximum number of file gaps that can be stored by each transaction */
#define MAX_GAPS_PER_TRANSACTION        20

/* The longest file name */
#define MAX_FILE_NAME_LENGTH            OS_MAX_PATH_LEN

/* Background:  The entity-id identifies a particular CFDP entity.
 * A typical Goddard mission will have 2 CFDP entities - one on the spacecraft
 * and one on the ground.  
 *   This parameter tells the library how many bytes to allow for storage of
 * entity-ids.  CFDP allows the entity-id to be anywhere from 1 to 8 bytes
 * in length.  The safest value for this parameter is 8, but if memory
 * usage is an issue, it can be set to a smaller value (if there are only two 
 * entities, a one-byte entity-id is sufficient).
 */
#define MAX_ID_LENGTH                   2

/* This parameter indicates the amount of storage required (in bytes) to 
 * hold the largest chunk of file-data that will ever be contained in a 
 * single incoming or outgoing Filedata PDU.
 * (CFDP limits this to just under 64K bytes, but any particular mission
 * may have a further limitation).
 */
#define MAX_FILE_CHUNK_SIZE             CF_MAX_OUTGOING_CHUNK_SIZE

/* This parameter indicates the amount of storage required (in bytes) to hold
 * the largest possible CFDP PDU.  The largest possible PDU will contain
 * a PDU-header (which can be up to 32 bytes long) plus the biggest possible
 * chunk of file-data.
 * (The lower communications layer must be able to accomodate this length)
 */
#define MAX_PDU_LENGTH                  CF_OUTGOING_PDU_BUF_SIZE


/* The storage capacity of the generic data structure.  Must hold the
 * largest possible PDU.
 */
#define MAX_DATA_LENGTH                 MAX_PDU_LENGTH

/*************************/
/*** Virtual Filestore ***/
/*************************/

/* The interface between the CFDP library and the Virtual Filestore is
 * defined in terms of a non-existent datatype called 'CFDP_FILE'.
 * Specify the real datatype here.
 * If the standard C filesystem is being used, then '#define CFDP_FILE FILE'
 * (so that the library uses variables like this:
 *      FILE *fp;
 * i.e. the declaration of a standard file-pointer).
 */
#define CFDP_FILE FILE

/* If the default 'temp_file_name' callback routine is used, all temp
 * files will start with this prefix.  
 */
#define DEFAULT_TEMP_FILE_NAME_PREFIX   CF_ENGINE_TEMP_FILE_PREFIX

/* The library will provide this much storage capacity for temporary file
 * names.  If the default 'temp_file_name' callback routine is used, this 
 * value must be at least 5 (characters) more than the string-length of the 
 * default temp file name prefix.
 */
#define MAX_TEMP_FILE_NAME_LENGTH       OS_MAX_FILE_NAME




/*****************************************/
/*** Management Information Base (MIB) ***/
/*****************************************/

/* The MIB contains the settings of configuration parameters used by CFDP
 * (refer to chapter 8 of the CFDP Blue Book at www.ccsds.org).
 * This section provides default values for all configuration parameters.
 * (timeouts are in seconds; lengths are in bytes).
 * Note:  These default values can be overridden at run-time.  The 
 * mechanism for doing that is defined in 'cfdp_provides.h'.
 */

/* Default values for 'Local Entity' parameters */
#define DEFAULT_ISSUE_EOF_RECV 0
#define DEFAULT_ISSUE_EOF_SENT 1
#define DEFAULT_ISSUE_FILE_SEGMENT_RECV 0
#define DEFAULT_ISSUE_FILE_SEGMENT_SENT 0
#define DEFAULT_ISSUE_RESUMED 1
#define DEFAULT_ISSUE_SUSPENDED 1
#define DEFAULT_ISSUE_TRANSACTION_FINISHED 0
#define DEFAULT_MY_ID "88"
#define DEFAULT_RESPONSE_TO_FAULT RESPONSE_CANCEL
#define DEFAULT_SAVE_INCOMPLETE_FILES 0

/* Default values for 'Remote Entity' parameters (timers) */
#define DEFAULT_ACK_LIMIT 5
#define DEFAULT_ACK_TIMEOUT 5
#define DEFAULT_INACTIVITY_TIMEOUT 60
#define DEFAULT_NAK_LIMIT 5
#define DEFAULT_NAK_TIMEOUT 5

/* This parameter determines how many bytes of Filedata are contained
 * in each outgoing Filedata PDU.  If you think of CFDP as transmitting 
 * a big file in multiple chunks, this is the "chunk size".
 */
#define DEFAULT_OUTGOING_FILE_CHUNK_SIZE    50

/***********************/
/*** Message Filters ***/
/***********************/

/* This section specifies the settings for message filters.  These
 * settings determine which messages are output by the CFDP library.
 * For each message-class, choose '1' to allow messages or '0' to suppress.
 * Note that error messages are not filtered.
 * Note:  As of July 2006, there is no way to override these default values
 * at run-time.  That may change in a future version...
 */

/* To output a simple text message in response to each CFDP Indication */
#define MSG_INDICATIONS 1

/* To monitor memory allocation */
#define MSG_DEBUG_MEMORY_USE 0 

/* To monitor assembly/disassembly of NAKs (lists of file gaps) */
#define MSG_DEBUG_NAK 0

/* To monitor conversion of raw PDUs to/from 'C' structure */
#define MSG_DEBUG_PDU 0

/* To monitor the Kernel's autonomous responses to incoming PDUs (i.e.
 * when the Kernel responds directly rather than passing the PDU to a
 * state machine).  These occur when an incoming PDU references a 
 * transaction for which there is no existing state machine, and no reason 
 * to start a new one.
 */
#define MSG_KERNEL_AUTONOMOUS 0

/* To monitor the Kernel's creation/deletion of state machines. */
#define MSG_KERNEL_TRANSACTION 0

/* To look inside Nak PDUs */
#define MSG_NAK_SUMMARY 0       /* Summary of file gaps in every Nak */
#define MSG_NAK_DETAILS 0       /* Details of file gaps in every Nak */

/* To see when PDUs are input/output */
#define MSG_PDU_FILEDATA 1      /* input of Filedata PDUs */
#define MSG_PDU_NON_FILEDATA 1  /* input/output of all other PDUs */
#define MSG_PDU_RETRANSMITTED_FD 1  /* Outgoing retrans Filedata PDUs */
#define MSG_PDU_RETRANSMITTED_MD 1  /* Outgoing retrans Metadata PDUs */

/* To keep abreast of the state of each active transaction */
#define MSG_STATE_ALL 0     /* One message per transaction in every cycle */
#define MSG_STATE_CHANGE 0  /* Message is only output when state changes */

/* To see each timer expiration (Ack, Nak, and Inactivity timers) */
#define MSG_TIMER_EXPIRED 0



/******************/
/*** Assertions ***/
/******************/

/* An assertion fires when the library encounters a situation that
 * "can't ever happen" -- it usually indicates a bug in the program,
 * and a program crash is likely.  The user of the CFDP library is
 * free to decide what response to take in response to assertions.
 * The intent is to use assertions to help find bugs during testing.
 * Once CFDP is operational, they can be 'disabled' (i.e. they still fire,
 * but the program is not exited).
 * Note:  Other than through assertions, the CFDP library does not ever
 * perform an exit.  
 */

/* Set this parameter to '1' for 'yes' or '0' for 'no'. */
#define IS_ASSERT_ALLOWED_TO_EXIT 1

#define ASSERT__(s) \
{ \
   if (!(s)) \
      { \
      msg__ ("Assertion failed in %s at line %u.\n", __func__, __LINE__); \
      if (IS_ASSERT_ALLOWED_TO_EXIT) \
         abort (); \
      } \
}

#define ASSERT__BUG_DETECTED \
{ \
   msg__ ("Bug detected in %s at line %u.\n", __func__, __LINE__); \
   if (IS_ASSERT_ALLOWED_TO_EXIT) \
      abort (); \
} \

#endif
