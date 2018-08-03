/*******************************************************************************
** File: nav_app.h
**
** Purpose:
**   Navigation (NAV) App header file
**
*******************************************************************************/

#ifndef _nav_app_h_
#define _nav_app_h_

#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include "nav_msg.h"
#include "hwlib.h"

#define NAV_PIPE_DEPTH 8
#define NAV_PIPE_NAME  "NAV_CMD_PIPE"

/**
**  \brief NAV App Global Data Structure
*/
typedef struct
{
    CFE_SB_MsgPtr_t         MsgPtr;			/**< \brief Pointer to msg received on software bus */
    CFE_SB_PipeId_t         CmdPipe;		/**< \brief Pipe Id for NAV command pipe */
    uint32   				RunStatus;		/**< \brief NAV App run status */
    NAV_HkTlm_t 			hk;				/**< \brief NAV App HK Tlm */
} NAV_AppData_t;


/**
 * NAV App Prototypes
 */
void NAV_AppMain(void);
void NAV_AppInit(void);
void NAV_ProcessCommandPacket(uint8 CmdCode);
void NAV_ProcessGroundCommand(void);
void NAV_ReportHousekeeping(void);
void NAV_ResetCounters(void);

#endif
