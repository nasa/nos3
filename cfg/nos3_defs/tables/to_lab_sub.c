/************************************************************************
**
**      GSC-18128-1, "Core Flight Executive Version 6.7"
**
**      Copyright (c) 2006-2002 United States Government as represented by
**      the Administrator of the National Aeronautics and Space Administration.
**      All Rights Reserved.
**
**      Licensed under the Apache License, Version 2.0 (the "License");
**      you may not use this file except in compliance with the License.
**      You may obtain a copy of the License at
**
**        http://www.apache.org/licenses/LICENSE-2.0
**
**      Unless required by applicable law or agreed to in writing, software
**      distributed under the License is distributed on an "AS IS" BASIS,
**      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**      See the License for the specific language governing permissions and
**      limitations under the License.
**
** File: to_lab_sub_table.c
**
** Purpose:
**  Define TO Lab CPU specific subscription table
**
** Notes:
**
*************************************************************************/

/*
** Include Files
*/
#include "cfe_tbl_filedef.h"  /* Required to obtain the CFE_TBL_FILEDEF macro definition */
#include "to_lab_sub_table.h"
#include "to_lab_msgids.h"
#include "ci_lab_msgids.h"

#include "ci_msgids.h"
#include "cf_msgids.h"
#include "ds_msgids.h"
#include "fm_msgids.h"
//#include "hs_msgids.h"
//#include "hk_msgids.h"
#include "lc_msgids.h"
#include "sc_msgids.h"
#include "sch_msgids.h"
#include "to_msgids.h"

/*
** Component Include Files
*/
#include "cam_msgids.h"
#include "generic_css_msgids.h"
#include "generic_eps_msgids.h"
#include "generic_fss_msgids.h"
#include "generic_imu_msgids.h"
#include "generic_mag_msgids.h"
#include "generic_radio_msgids.h"
#include "generic_reaction_wheel_msgids.h"
#include "generic_torquer_msgids.h"
#include "novatel_oem615_msgids.h"
#include "sample_msgids.h"
#include "generic_adcs_msgids.h"
#include "generic_star_tracker_msgids.h"

/*
** Local Structure Declarations
*/
#define CF_CONFIG_TLM_MID 0x08B2
#define CF_PDU_TLM_MID    0x0FFD

TO_LAB_Subs_t TO_LAB_Subs =
{
    .Subs =
    {
        /* CFS App Subscriptions */
        {CFE_SB_MSGID_WRAP_VALUE(TO_LAB_HK_TLM_MID), {0, 0}, 4},
        {CFE_SB_MSGID_WRAP_VALUE(TO_LAB_DATA_TYPES_MID), {0, 0}, 4},
        {CFE_SB_MSGID_WRAP_VALUE(CI_LAB_HK_TLM_MID), {0, 0}, 4},

        /* Add these if needed */
        {CFE_SB_MSGID_WRAP_VALUE(CF_CONFIG_TLM_MID), {0,0}, 4},
        {CFE_SB_MSGID_WRAP_VALUE(CF_HK_TLM_MID), {0,0}, 4},
        {CFE_SB_MSGID_WRAP_VALUE(FM_HK_TLM_MID), {0,0}, 4},
        {CFE_SB_MSGID_WRAP_VALUE(SC_HK_TLM_MID), {0,0}, 4},
        {CFE_SB_MSGID_WRAP_VALUE(DS_HK_TLM_MID), {0,0}, 4},
        {CFE_SB_MSGID_WRAP_VALUE(LC_HK_TLM_MID), {0,0}, 4},

        /* cFE Core subscriptions */
        {CFE_SB_MSGID_WRAP_VALUE(CFE_ES_HK_TLM_MID), {0, 0}, 4},
        {CFE_SB_MSGID_WRAP_VALUE(CFE_EVS_HK_TLM_MID), {0, 0}, 4},
        {CFE_SB_MSGID_WRAP_VALUE(CFE_SB_HK_TLM_MID), {0, 0}, 4},
        {CFE_SB_MSGID_WRAP_VALUE(CFE_TBL_HK_TLM_MID), {0, 0}, 4},
        {CFE_SB_MSGID_WRAP_VALUE(CFE_TIME_HK_TLM_MID), {0, 0}, 4},
        {CFE_SB_MSGID_WRAP_VALUE(CFE_TIME_DIAG_TLM_MID), {0, 0}, 4},
        {CFE_SB_MSGID_WRAP_VALUE(CFE_SB_STATS_TLM_MID), {0, 0}, 4},
        {CFE_SB_MSGID_WRAP_VALUE(CFE_TBL_REG_TLM_MID), {0, 0}, 4},
        {CFE_SB_MSGID_WRAP_VALUE(CFE_EVS_LONG_EVENT_MSG_MID), {0, 0}, 32},

        /* Component Specifics */
        {CFE_SB_MSGID_WRAP_VALUE(CAM_HK_TLM_MID),               {0,0},  32},
        {CFE_SB_MSGID_WRAP_VALUE(CAM_EXP_TLM_MID),              {0,0},  32},
        {CFE_SB_MSGID_WRAP_VALUE(GENERIC_CSS_HK_TLM_MID),       {0,0},  32},
        {CFE_SB_MSGID_WRAP_VALUE(GENERIC_CSS_DEVICE_TLM_MID),   {0,0},  32},
        {CFE_SB_MSGID_WRAP_VALUE(GENERIC_EPS_HK_TLM_MID),       {0,0},  32},
        {CFE_SB_MSGID_WRAP_VALUE(GENERIC_FSS_HK_TLM_MID),       {0,0},  32},
        {CFE_SB_MSGID_WRAP_VALUE(GENERIC_FSS_DEVICE_TLM_MID),   {0,0},  32},
        {CFE_SB_MSGID_WRAP_VALUE(GENERIC_IMU_HK_TLM_MID),       {0,0},  32},
        {CFE_SB_MSGID_WRAP_VALUE(GENERIC_IMU_DEVICE_TLM_MID),   {0,0},  32},
        {CFE_SB_MSGID_WRAP_VALUE(GENERIC_MAG_HK_TLM_MID),       {0,0},  32},
        {CFE_SB_MSGID_WRAP_VALUE(GENERIC_MAG_DEVICE_TLM_MID),   {0,0},  32},
        {CFE_SB_MSGID_WRAP_VALUE(GENERIC_RADIO_HK_TLM_MID),     {0,0},  32},
        {CFE_SB_MSGID_WRAP_VALUE(GENERIC_RW_APP_HK_TLM_MID),    {0,0},  32},
	    {CFE_SB_MSGID_WRAP_VALUE(GENERIC_TORQUER_HK_TLM_MID),   {0,0},  32},
        {CFE_SB_MSGID_WRAP_VALUE(NOVATEL_OEM615_HK_TLM_MID),    {0,0},  32},
        {CFE_SB_MSGID_WRAP_VALUE(NOVATEL_OEM615_DEVICE_TLM_MID),{0,0},  32},
        {CFE_SB_MSGID_WRAP_VALUE(SAMPLE_HK_TLM_MID),            {0,0},  32},
        {CFE_SB_MSGID_WRAP_VALUE(SAMPLE_DEVICE_TLM_MID),        {0,0},  32},
        {CFE_SB_MSGID_WRAP_VALUE(GENERIC_ADCS_HK_TLM_MID),      {0,0},  32},
        {CFE_SB_MSGID_WRAP_VALUE(GENERIC_ADCS_DI_MID),          {0,0},  32},
        {CFE_SB_MSGID_WRAP_VALUE(GENERIC_ADCS_AD_MID),          {0,0},  32},
        {CFE_SB_MSGID_WRAP_VALUE(GENERIC_ADCS_GNC_MID),         {0,0},  32},
        {CFE_SB_MSGID_WRAP_VALUE(GENERIC_ADCS_AC_MID),          {0,0},  32},
        {CFE_SB_MSGID_WRAP_VALUE(GENERIC_ADCS_DO_MID),          {0,0},  32},
        {CFE_SB_MSGID_WRAP_VALUE(GENERIC_STAR_TRACKER_HK_TLM_MID),{0,0},  32},
        {CFE_SB_MSGID_WRAP_VALUE(GENERIC_STAR_TRACKER_DEVICE_TLM_MID),{0,0},  32},

    }
};

CFE_TBL_FILEDEF(TO_LAB_Subs, TO_LAB_APP.TO_LAB_Subs, TO Lab Sub Tbl, to_lab_sub.tbl)
