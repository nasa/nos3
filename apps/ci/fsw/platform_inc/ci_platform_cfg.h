/******************************************************************************/
/** \file  ci_platform_cfg.h
*
*   \author Guy de Carufel (Odyssey Space Research), NASA, JSC, ER6
*
*   \brief Sample config file for CI Application with UDP socket
*
*   \par Limitations, Assumptions, External Events, and Notes:
*       - Make use of the setup.sh script to move / link this file to the 
*       {MISSION_HOME}/apps/to/fsw/platform_inc folder.
*
*   \par Modification History:
*     - 2015-01-09 | Guy de Carufel | Code Started
*******************************************************************************/
#ifndef _CI_PLATFORM_CFG_H_
#define _CI_PLATFORM_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
** Pragmas
*/

/*
** Local Defines
*/
/* Check new commands every 1s if not scheduled */
#define CI_WAKEUP_TIMEOUT  1000  

#define CI_SCH_PIPE_DEPTH  10
#define CI_CMD_PIPE_DEPTH  10
#define CI_TLM_PIPE_DEPTH  10


#define CI_CUSTOM_UDP_PORT 5010
#define CI_CUSTOM_UDP_ADDR IO_TRANS_UDP_INADDR_ANY
#define CI_CUSTOM_UDP_TIMEOUT 100
#define CI_CUSTOM_MAX_IP_STRING_SIZE  16 

#define CI_CUSTOM_BUFFER_SIZE 1000

#define CI_CUSTOM_TASK_STACK_PTR NULL
#define CI_CUSTOM_TASK_STACK_SIZE 0x4000
#define CI_CUSTOM_TASK_PRIO 118

/*
** Include Files
*/

/*
** Local Structure Declarations
*/

/*
** External Global Variables
*/

/*
** Global Variables
*/

/*
** Local Variables
*/

/*
** Local Function Prototypes
*/

#ifdef __cplusplus
}
#endif

#endif /* _CI_PLATFORM_CFG_H_ */

/*==============================================================================
** End of file ci_platform_cfg.h
**============================================================================*/
    
