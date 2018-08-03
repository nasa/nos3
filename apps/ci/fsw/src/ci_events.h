/******************************************************************************/
/** \file  ci_events.h
*  
*   \author Guy de Carufel (Odyssey Space Research), NASA, JSC, ER6
*
*   \brief ID Header File for CI Application
*
*   \par
*       This header file contains definitions of the CI Event IDs
*
*   \par Modification History:
*     - 2015-01-09 | Guy de Carufel | Code Started
*******************************************************************************/
    
#ifndef _CI_EVENTS_H_
#define _CI_EVENTS_H_

#ifdef __cplusplus
extern "C" {
#endif

/** Event IDs */
typedef enum
{
    CI_RESERVED_EID       =   0,
    CI_INF_EID            =   1,
    CI_INIT_INF_EID       =   2,
    CI_CMD_INF_EID        =   3,
    CI_CUSTOM_INF_EID     =   4,
    CI_ERR_EID            =   5,
    CI_INIT_ERR_EID       =   6,
    CI_CMD_ERR_EID        =   7,
    CI_PIPE_ERR_EID       =   8,
    CI_MSGID_ERR_EID      =   9,
    CI_MSGLEN_ERR_EID     =  10,
    CI_CUSTOM_ERR_EID     =  11,
    CI_EVT_CNT
} CI_Events_t;

#ifdef __cplusplus
}
#endif

#endif /* _CI_EVENTS_H_ */

/*==============================================================================
** End of file ci_events.h
**============================================================================*/
    
