/************************************************************************
** File:
**
** Purpose: 
**  Define TO Lab Application header file
**
** Notes:
**
** $Log: to_lab_app.h  $
** Revision 1.4 2010/11/17 08:12:27GMT-05:00 wmoleski 
** The TO_Lab_APP, CI_Lab_APP and SCH_Lab_APP did not compile with cFE6.1.0. An include statement of cfe_error.h needed to be added to each app.
** Revision 1.3 2008/09/22 13:57:46EDT apcudmore 
** Added RunLoop call to TO_LAB app.
** Revision 1.2 2008/04/30 15:31:41EDT rjmcgraw 
** DCR1718:2 Fixed compiler errors
** Revision 1.1 2008/04/30 14:44:51EDT rjmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/to_lab/fsw/src/project.pj
**
*************************************************************************/

#ifndef _to_lab_app_h_
#define _to_lab_app_h_

#include "network_includes.h"

#include "cfe_error.h"
#include "cfe_sb.h"
#include "cfe_time.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "common_types.h"
#include "osapi.h"

/*****************************************************************************/

#define TO_TASK_MSEC             500          /* run at 2 Hz */
#define TO_UNUSED                  0

#define cfgTLM_ADDR "192.168.1.81"
#define cfgTLM_PORT 1235
#define TO_LAB_VERSION_NUM "5.1.0"

/******************************************************************************/

/*
** Prototypes Section
*/
void TO_Lab_AppMain(void);

/******************************************************************************/

#endif  /* _to_lab_app_h_ */

