/* Copyright (C) 2009 - 2015 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

This software is provided "as is" without any warranty of any, kind either express, implied, or statutory, including, but not
limited to, any warranty that the software will conform to, specifications any implied warranties of merchantability, fitness
for a particular purpose, and freedom from infringement, and any warranty that the documentation will conform to the program, or
any warranty that the software will be error free.

In no event shall NASA be liable for any damages, including, but not limited to direct, indirect, special or consequential damages,
arising out of, resulting from, or in any way connected with the software or its documentation.  Whether or not based upon warranty,
contract, tort or otherwise, and whether or not loss was sustained from, or arose out of the results of, or use of, the software,
documentation or services provided hereunder

ITC Team
NASA IV&V
ivv-itc@lists.nasa.gov
*/

/************************************************************************
** File:
**    sample_stf1_app_events.h
**
** Purpose:
**  Define SAMPLE STF1 App Events IDs
**
** Notes:
**
*************************************************************************/

#ifndef _SAMPLE_STF1_APP_EVENTS_H_
#define _SAMPLE_STF1_APP_EVENTS_H_

/* define any custom app event IDs */
#define SAMPLE_STF1_RESERVED_EID              0
#define SAMPLE_STF1_STARTUP_INF_EID           1 
#define SAMPLE_STF1_COMMAND_ERR_EID           2
#define SAMPLE_STF1_COMMANDNOP_INF_EID        3 
#define SAMPLE_STF1_COMMANDRST_INF_EID        4
#define SAMPLE_STF1_INVALID_MSGID_ERR_EID     5 
#define SAMPLE_STF1_LEN_ERR_EID               6 
#define SAMPLE_STF1_PIPE_ERR_EID              7

#endif