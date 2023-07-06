/*==============================================================================
** File Name: to_config.c
**
** Copyright 2017 United States Government as represented by the Administrator
** of the National Aeronautics and Space Administration.  No copyright is
** claimed in the United States under Title 17, U.S. Code.
** All Other Rights Reserved.
**
** Title:     TO table definition
**
** $Author: $
** $Revision: $
** $Date:  $
**
** Purpose:   To provide the table for default data config.
**
** Functions Contained:
**    None
**
**
** Limitations, Assumptions, External Events, and Notes:
**  1.   None
**
**
**==============================================================================
*/

/*
#ifndef _TO_CONFIG_
#define _TO_CONFIG_


#ifdef   __cplusplus
extern "C" {
#endif
*/

/*
** Pragmas
*/

/*
** Include Files
*/
#include "cfe.h"
#include "cfe_tbl_filedef.h"
#include "to_platform_cfg.h"
#include "to_mission_cfg.h"
#include "to_app.h"
#include "to_tbldefs.h"
#include "to_grpids.h"

#include "cfe_msgids.h"

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
#include "nav_msgids.h"
#include "sample_msgids.h"
#include "generic_adcs_msgids.h"


static CFE_TBL_FileDef_t CFE_TBL_FileDef =
{
    "to_ConfigTable", "TO.to_config", "TO config table",
    "to_config.tbl", sizeof(TO_ConfigTable_t)
};

/*
** Default TO iLoad table data
*/

TO_ConfigTable_t to_ConfigTable =
{
   {
       /* 0 - 9 */
       {CF_CONFIG_TLM_MID,            {0,0},  5,   0xffff,     TO_GROUP_CFE | TO_MGROUP_ONE, 0,1},
       {CF_HK_TLM_MID,                {0,0},  5,   0xffff,     TO_GROUP_CFE | TO_MGROUP_ONE, 0,1},
       {CF_SPACE_TO_GND_PDU_MID,      {0,0},  64,  0xffff,     TO_GROUP_CFE | TO_MGROUP_ONE, 0,1},
       {CF_TRANS_TLM_MID,             {0,0},  5,   0xffff,     TO_GROUP_CFE | TO_MGROUP_ONE, 0,1},
       {CFE_ES_APP_TLM_MID,           {0,0},  5,   0xffff,     TO_GROUP_CFE | TO_MGROUP_ONE, 0,1},
       {CFE_ES_HK_TLM_MID,            {0,0},  5,   0xffff,     TO_GROUP_CFE | TO_MGROUP_ONE, 0,1},
       {CFE_ES_MEMSTATS_TLM_MID,      {0,0},  5,   0xffff,     TO_GROUP_CFE | TO_MGROUP_ONE, 0,1},
       {CFE_ES_SHELL_TLM_MID,         {0,0},  32,  0xffff,     TO_GROUP_CFE | TO_MGROUP_ONE, 0,1},
       {CFE_EVS_EVENT_MSG_MID,        {0,0},  32,  0xffff,     TO_GROUP_CFE | TO_MGROUP_ONE, 0,1},
       {CFE_EVS_HK_TLM_MID,           {0,0},  5,   0xffff,     TO_GROUP_CFE | TO_MGROUP_ONE, 0,1},
                                                         
       /* 10 - 19 */                                     
       {CFE_SB_ALLSUBS_TLM_MID,       {0,0},  5,   0xffff,     TO_GROUP_CFE | TO_MGROUP_ONE, 0,1},
       {CFE_SB_HK_TLM_MID,            {0,0},  5,   0xffff,     TO_GROUP_CFE | TO_MGROUP_ONE, 0,1},
       {CFE_SB_ONESUB_TLM_MID,        {0,0},  5,   0xffff,     TO_GROUP_CFE | TO_MGROUP_ONE, 0,1},
       {CFE_SB_STATS_TLM_MID,         {0,0},  5,   0xffff,     TO_GROUP_CFE | TO_MGROUP_ONE, 0,1},
       {CFE_TBL_HK_TLM_MID,           {0,0},  5,   0xffff,     TO_GROUP_CFE | TO_MGROUP_ONE, 0,1},
       {CFE_TBL_REG_TLM_MID,          {0,0},  5,   0xffff,     TO_GROUP_CFE | TO_MGROUP_ONE, 0,1},
       {CFE_TIME_DIAG_TLM_MID,        {0,0},  5,   0xffff,     TO_GROUP_CFE | TO_MGROUP_ONE, 0,1},
       {CFE_TIME_HK_TLM_MID,          {0,0},  5,   0xffff,     TO_GROUP_CFE | TO_MGROUP_ONE, 0,1},
       {TO_HK_TLM_MID,                {0,0},  5,   0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {SCH_DIAG_TLM_MID,             {0,0},  5,   0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
                                                         
       /* 20 - 29 */                                     
       {SCH_HK_TLM_MID,               {0,0},  5,   0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {CI_HK_TLM_MID,                {0,0},  5,   0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {TO_DATA_TYPE_MID,             {0,0},  5,   0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {FM_HK_TLM_MID,                {0,0},  5,   0x0001,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {FM_FILE_INFO_TLM_MID,         {0,0},  5,   0x0001,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {FM_DIR_LIST_TLM_MID,          {0,0},  5,   0x0001,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {FM_OPEN_FILES_TLM_MID,        {0,0},  5,   0x0001,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {FM_FREE_SPACE_TLM_MID,        {0,0},  5,   0x0001,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {SC_HK_TLM_MID,                {0,0},  5,   0x0001,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {LC_HK_TLM_MID,                {0,0},  5,   0x0001,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},

       /* 30 - 39 */
       {DS_HK_TLM_MID,                {0,0},  5,   0x0001,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {CAM_HK_TLM_MID,               {0,0},  32,  0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {CAM_EXP_TLM_MID,              {0,0},  32,  0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {GENERIC_EPS_HK_TLM_MID,       {0,0},  32,  0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {GENERIC_RW_APP_HK_TLM_MID,    {0,0},  32,  0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {GENERIC_TORQUER_HK_TLM_MID,   {0,0},  32,  0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {NAV_SEND_HK_TLM,              {0,0},  32,  0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {SAMPLE_HK_TLM_MID,            {0,0},  32,  0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {SAMPLE_DEVICE_TLM_MID,        {0,0},  32,  0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {GENERIC_FSS_HK_TLM_MID,       {0,0},  32,  0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {GENERIC_FSS_DEVICE_TLM_MID,   {0,0},  32,  0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       
       /* 40 - 49 */
       {GENERIC_CSS_HK_TLM_MID,       {0,0},  32,  0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {GENERIC_CSS_DEVICE_TLM_MID,   {0,0},  32,  0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {GENERIC_RADIO_HK_TLM_MID,     {0,0},  32,  0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {GENERIC_IMU_HK_TLM_MID,       {0,0},  32,  0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {GENERIC_IMU_DEVICE_TLM_MID,   {0,0},  32,  0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {GENERIC_MAG_HK_TLM_MID,       {0,0},  32,  0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {GENERIC_MAG_DEVICE_TLM_MID,   {0,0},  32,  0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {GENERIC_ADCS_HK_TLM_MID,      {0,0},  32,  0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},

       // Commented out to limited ADCS messages sent via radio
       //{GENERIC_ADCS_DI_MID,          {0,0},  32,  0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       //{GENERIC_ADCS_AD_MID,          {0,0},  32,  0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       //{GENERIC_ADCS_GNC_MID,         {0,0},  32,  0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       //{GENERIC_ADCS_AC_MID,          {0,0},  32,  0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       //{GENERIC_ADCS_DO_MID,          {0,0},  32,  0xffff,     TO_GROUP_APP | TO_MGROUP_ONE, 0,1},
       
       /* 50 - 59 */
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},

       /* 60 - 69 */
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},

       /* 70 - 79 */
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},

       /* 80 - 89 */
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},

       /* 90 - 99 */
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0},
       {TO_UNUSED_ENTRY,              {0,0},  0,   0x0000,     TO_GROUP_NONE,            0,0}
    }
};

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

/*
#ifdef   __cplusplus
}
#endif

#endif
*/

/* _TO_CONFIG_ */
