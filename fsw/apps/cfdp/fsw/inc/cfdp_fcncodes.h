/************************************************************************
 * NASA Docket No. GSC-18,447-1, and identified as “CFS CFDP (CF)
 * Application version 3.0.0”
 *
 * Copyright (c) 2019 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * @file
 *   Specification for the CFS CFDP (CF) command function codes
 *
 * @note
 *   This file should be strictly limited to the command/function code (CC)
 *   macro definitions.  Other definitions such as enums, typedefs, or other
 *   macros should be placed in the msgdefs.h or msg.h files.
 */
#ifndef CFDP_FCNCODES_H
#define CFDP_FCNCODES_H

/************************************************************************
 * Macro Definitions
 ************************************************************************/

/**
 * \defgroup cfscfcmdcodes CFS CFDP Command Codes
 * \{
 */

typedef enum
{
    CFDP_NOOP_CC = 0,
    CFDP_RST_CTR_CC  = 1,
    CFDP_DOWNLOADFILE_CC = 29,
    CFDP_UPLOADFILE_CC = 30,
    CFDP_UPLOADFILE_DATA_CC = 31,
} CFDP_CMDS;

/**\}*/

#endif