/* FILE: cfdp.h -- a single 'include' file for CFDP library interface.
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
 * LAST MODIFIED:  2006_07_11
 * ORIGINAL PROGRAMMER:  Tim Ray 301-286-0581
 * SUMMARY:  This one file includes the entire CFDP library interface.
 * OFFICIAL CFDP SPECS:
 *   The official CFDP specs are published in the CFDP Blue Book,
 *   CCSDS document #727.0-B-3, available at 'www.ccsds.org'.
 *   There are also two official Green Books, which help explain what
 *   CFDP is and how to implement it:
 *     720.1-G-2  CFDP Introduction and Overview
 *     720.2-G-2  CFDP Implementers Guide.
 * OTHER HELPFUL INFO:
 *   There is a CFDP library Users Guide.
 *   There is a powerpoint slide presentation that summarizes CFDP.
 *   Both are available from Tim Ray at 301-286-0581 or tim.ray@nasa.gov.
 * CONFIGURING THIS LIBRARY:
 *   The library user is free to modify the file 'cfdp_config.h'; the
 *   other library interface files should not be modified.
 */

/* List of CFDP library interface files:
 *    cfdp_config.h            - compile-time configuration of the library
 *    cfdp_data_structures.h   - CFDP-related data structures 
 *    cfdp_provides.h          - services provided by the library
 *    cfdp_requires.h          - services required by the library
 *    cfdp_syntax.h            - syntax of character string Requests/Directives
 */


#ifndef H_CFDP
#define H_CFDP

#include "cfdp_provides.h"
#include "cfdp_requires.h"

#endif
