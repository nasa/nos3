/************************************************************************
** File: to_sub_table.h
**
** Purpose: 
**  Define TO Lab CPU specific subscription table 
**
** Notes:
**
** $Log: to_lab_sub_table.h  $
** Revision 1.3 2014/07/16 14:44:45GMT-05:00 acudmore 
** Member renamed from to_sub_table.h to to_lab_sub_table.h in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/to_lab/fsw/platform_inc/project.pj.
** Revision 1.2 2014/07/16 14:44:45ACT acudmore 
** Updated TO_LAB subscription table header file ( comments, clean up )
**
*************************************************************************/

/* 
** Add the proper include file for the message IDs below
*/
#include "cfe_msgids.h"

/*
** Common CFS app includes below are commented out
*/
#include "ci_lab_msgids.h"

//#include "sample_app_msgids.h"
#include "cam_msgids.h"
#include "nav_msgids.h"

#if 0
#include "hs_msgids.h"
#include "fm_msgids.h"
#include "sc_msgids.h"
#include "ds_msgids.h"
#include "lc_msgids.h"
#endif

static TO_subsciption_t  TO_SubTable[] =
{
            /* CFS App Subscriptions */
            {TO_LAB_HK_TLM_MID,     {0,0},  4},
            {TO_LAB_DATA_TYPES_MID, {0,0},  4},
            {CI_LAB_HK_TLM_MID,     {0,0},  4},
            //{SAMPLE_APP_HK_TLM_MID, {0,0},  4},
			{CAM_HK_TLM_MID,        {0,0},  10},
            {CAM_EXP_TLM_MID,       {0,0}, 100},
            {NAV_SEND_HK_TLM,       {0,0},  10},
            {TO_UNUSED,              {0,0},  0},

#if 0
            /* Add these if needed */
            {HS_HK_TLM_MID,         {0,0},  4},
            {FM_HK_TLM_MID,         {0,0},  4},
            {SC_HK_TLM_MID,         {0,0},  4},
            {DS_HK_TLM_MID,         {0,0},  4},
            {LC_HK_TLM_MID,         {0,0},  4},
#endif
            /* cFE Core subsciptions */
            {CFE_ES_HK_TLM_MID,     {0,0},  4},
            {CFE_EVS_HK_TLM_MID,    {0,0},  4},
            {CFE_SB_HK_TLM_MID,     {0,0},  4},
            {CFE_TBL_HK_TLM_MID,    {0,0},  4},
            {CFE_TIME_HK_TLM_MID,   {0,0},  4},
            {CFE_TIME_DIAG_TLM_MID, {0,0},  4},
            {CFE_SB_STATS_TLM_MID,  {0,0},  4},
            {CFE_TBL_REG_TLM_MID,   {0,0},  4},
            {CFE_EVS_EVENT_MSG_MID, {0,0},  32},
            {CFE_ES_SHELL_TLM_MID,  {0,0},  32},
            {CFE_ES_APP_TLM_MID,    {0,0},   4},
            {CFE_ES_MEMSTATS_TLM_MID,{0,0},  4},

            {TO_UNUSED,              {0,0},  0},
            {TO_UNUSED,              {0,0},  0},
            {TO_UNUSED,              {0,0},  0}
};

/************************
** End of File Comment ** 
************************/
