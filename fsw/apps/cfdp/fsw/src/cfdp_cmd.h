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
 *
 * CF command processing function declarations
 */

#ifndef CFDP_CMD_H
#define CFDP_CMD_H

#include "cfe.h"
#include "cfdp_app.h"

CFE_Status_t CFDP_NoopCmd(void);

void CFDP_ResetCounters(void);

void CFDP_DownloadFromSatellite(const CFE_MSG_Message_t *MsgPtr);
void CFDP_UploadToSatellite(const CFE_MSG_Message_t *MsgPtr);
void CFDP_UploadToSatelliteConfirm(const CFE_MSG_Message_t *MsgPtr);

#endif