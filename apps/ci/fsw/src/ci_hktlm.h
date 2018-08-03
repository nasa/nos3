/******************************************************************************/
/** \file  ci_hktlm.h
*
*   \author Guy de Carufel (Odyssey Space Research), NASA, JSC, ER6
*
*   \brief Default HK Telemetry
*
*   \par
*       This header contains the definition of the default HK Telemetry.
*
*   \par Limitations, Assumptions, External Events, and Notes:
*     - Include this file in your MISSION_ci_types.h or define your own.
*     - If a custom HK tlm is required, make sure to include all parameters
*       in this default HK packet in your custom implementation.
*
*   \par Modification History:
*     - 2015-10-16 | Guy de Carufel | Code Started
*******************************************************************************/
#ifndef _CI_HKTLM_H_
#define _CI_HKTLM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "cfe.h"

typedef struct
{
    uint8   ucTlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint16  usCmdCnt;           /**< Count of all commands received           */
    uint16  usCmdErrCnt;        /**< Count of command errors                  */
} CI_HkTlm_t;

#ifdef __cplusplus
}
#endif

#endif /* _CI_HKTLM_H_ */

/*==============================================================================
** End of file ci_hktlm.h
**============================================================================*/
