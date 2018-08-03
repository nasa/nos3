/*
 *  Copyright (c) 2004-2015, United States government as represented by the
 *  administrator of the National Aeronautics Space Administration.
 *  All rights reserved. This software was created at NASA Glenn
 *  Research Center pursuant to government contracts.
 *
 *  This is governed by the NASA Open Source Agreement and may be used,
 *  distributed and modified only according to the terms of that agreement.
 */

/**
 * \file cfe_evs_log_typedef.h
 *
 *  Created on: Jan 22, 2015
 *      Author: joseph.p.hickey@nasa.gov
 *
 * Definition of the CFE_EVS_Log structure type.
 * This was moved into its own header file since it is referenced by multiple CFE core apps.
 */

#ifndef CFE_EVS_LOG_TYPEDEF_H_
#define CFE_EVS_LOG_TYPEDEF_H_

#include <common_types.h>
#include "cfe_evs_msg.h"  /* Required for CFE_EVS_Packet_t definition */

/*
** \brief  EVS Log type definition. This is declared here so ES can include it
**  in the reset area structure
*/
typedef struct {
    uint16 Next;                              /**< \brief Index of the next entry in the local event log */
    uint16 LogCount;                          /**< \brief Local Event Kog counter */
    uint8  LogFullFlag;                       /**< \brief Local Event Log full flag */
    uint8  LogMode;                           /**< \brief Local Event Logging mode (overwrite/discard) */
    uint16 LogOverflowCounter;                /**< \brief Local Event Log overflow counter */
    CFE_EVS_Packet_t LogEntry[CFE_EVS_LOG_MAX];/**< \brief The actual Local Event Log entry */

} CFE_EVS_Log_t;



#endif /* CFE_EVS_LOG_TYPEDEF_H_ */
