/* FILE: cfdp_private.h -- specs that are private to the CFDP library.
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
 *   2007_07_31 Tim Ray (TR)
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 *   2007_12_06 TR
 *     - Removed the macro 'msg__enabled'.
 */

#ifndef H_CFDP_PRIVATE
#define H_CFDP_PRIVATE 1

#include "cfdp.h"

/*-----------------------------------------------*/
/* Validate library configuration (as set up by  */
/* the library user in the file 'cfdp_config.h') */
/*-----------------------------------------------*/

#include "validate_user_config.h"

/* May 2007:  Took out public option to enable/disable dynamic memory 
 * allocation, so set it privately.
 */
#define IS_DYNAMIC_ALLOCATION_ENABLED 0

/*---------------------------------------------------------------------*/
/* As of 2006_09_08, the name of the generic data type is 'CFDP_DATA'  */
/* rather than 'DATA'.  This '#define' makes them equivalent within    */
/* the library (avoids changing numerous 'DATA' to 'CFDP_DATA'.        */
/*---------------------------------------------------------------------*/
typedef CFDP_DATA DATA;
typedef CFDP_DATA PDU;

/*---------------------------------------------------------*/
/* Include all the specification files from this one place */
/*---------------------------------------------------------*/
#include "structures.h"
#include "message_class.h"
#include "timer.h"
#include "event.h"
#include "pdu.h"
#include "pdu_as_string.h"
#include "machine.h"
#include "machine_list.h"
#include "mib.h"
#include "misc.h"
#include "nak.h"
#include "nak_mem.h"
#include "utils.h"
#include "aaa.h"
#include "r1.h"
#include "r2.h"
#include "s1.h"
#include "s2.h"
#include "callbacks.h"

/* These macros allow the code that builds character strings to be easier to
 * read and understand.
 */
#define COPY(str1,str2) utils__strncpy (str1, str2, sizeof(str1))
#define APPEND(str1,str2) utils__strncat (str1, str2, sizeof(str1))

/* This has to be declared somewhere !!! */
void indication__ (INDICATION_TYPE type, const TRANS_STATUS *info);

/* Shortcuts for the 'full names' of the enumerated type 'ROLE' */
#define S_1 CLASS_1_SENDER
#define R_1 CLASS_1_RECEIVER
#define S_2 CLASS_2_SENDER
#define R_2 CLASS_2_RECEIVER

#endif
