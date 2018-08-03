/*
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
** Purpose: This program prints the Message IDs used by the cFE
**
** Author: W. Moleski - NASA/GSFC Code 582
**
** Notes:
**
** $Log: gen_msgids.c  $
** Revision 1.2 2012/01/13 13:37:08GMT-05:00 acudmore 
** Added license/copyright text
** Revision 1.1 2008/11/19 08:51:02EST wfmoleski 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/build/cpu1/gen_msgids/project.pj
**
*/


/*
** Required header files...
*/
#include "gen_msgids.h"
#include <stdio.h>

int main(void)
{
/*********************
int i;

  for (i=4; i<TST_TBL_MAX_NUM_TABLES; i++)
  {
  }
*********************/
FILE *fp;
int cmdAppId=0, cmdMsgId=0;

  fp = fopen("cFEMsgIDs.h","w");
  if (fp != NULL)
  {
    /* Print the cFE Command Message IDs */
    fprintf(fp,"/* cFE Command MsgIds for CPU1 - Processor 1 */\n");

    cmdAppId = 1;
#ifdef CFE_CMD_APPID_BASE_CPU1
    cmdAppId = CFE_CMD_APPID_BASE_CPU1;
#endif

    /* CFE_EVS_CMD_MID */
    fprintf(fp,"CFE_EVS_CMD_MID 0x%04X   %d\n",CFE_EVS_CMD_MID,cmdAppId);
    cmdMsgId = CFE_EVS_CMD_MID;

    /* CFE_SB_CMD_MID */
    cmdAppId += (CFE_SB_CMD_MID - cmdMsgId);
    fprintf(fp,"CFE_SB_CMD_MID 0x%04X   %d\n",CFE_SB_CMD_MID,cmdAppId);
    cmdMsgId = CFE_SB_CMD_MID;

    /* CFE_TBL_CMD_MID */
    cmdAppId += (CFE_TBL_CMD_MID - cmdMsgId);
    fprintf(fp,"CFE_TBL_CMD_MID 0x%04X   %d\n",CFE_TBL_CMD_MID,cmdAppId);
    cmdMsgId = CFE_TBL_CMD_MID;

    /* CFE_TIME_CMD_MID */
    cmdAppId += (CFE_TIME_CMD_MID - cmdMsgId);
    fprintf(fp,"CFE_TIME_CMD_MID 0x%04X   %d\n",CFE_TIME_CMD_MID,cmdAppId);
    cmdMsgId = CFE_TIME_CMD_MID;

    /* CFE_ES_CMD_MID */
    cmdAppId += (CFE_ES_CMD_MID - cmdMsgId);
    fprintf(fp,"CFE_ES_CMD_MID 0x%04X   %d\n",CFE_ES_CMD_MID,cmdAppId);
    cmdMsgId = CFE_ES_CMD_MID;

    /* CFE_ES_SEND_HK_MID */
    cmdAppId += (CFE_ES_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"CFE_ES_SEND_HK_MID 0x%04X   %d\n",CFE_ES_SEND_HK_MID,cmdAppId);
    cmdMsgId = CFE_ES_SEND_HK_MID;

    /* CFE_EVS_SEND_HK_MID */
    cmdAppId += (CFE_EVS_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"CFE_EVS_SEND_HK_MID 0x%04X   %d\n",CFE_EVS_SEND_HK_MID,cmdAppId);
    cmdMsgId = CFE_EVS_SEND_HK_MID;

    /* CFE_SB_SEND_HK_MID */
    cmdAppId += (CFE_SB_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"CFE_SB_SEND_HK_MID 0x%04X   %d\n",CFE_SB_SEND_HK_MID,cmdAppId);
    cmdMsgId = CFE_SB_SEND_HK_MID;

    /* CFE_TBL_SEND_HK_MID */
    cmdAppId += (CFE_TBL_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"CFE_TBL_SEND_HK_MID 0x%04X   %d\n",CFE_TBL_SEND_HK_MID,cmdAppId);
    cmdMsgId = CFE_TBL_SEND_HK_MID;

    /* CFE_TIME_SEND_HK_MID */
    cmdAppId += (CFE_TIME_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"CFE_TIME_SEND_HK_MID 0x%04X   %d\n",CFE_TIME_SEND_HK_MID,cmdAppId);
    cmdMsgId = CFE_TIME_SEND_HK_MID;

    /* CFE_TIME_TONE_CMD_MID */
    cmdAppId += (CFE_TIME_TONE_CMD_MID - cmdMsgId);
    fprintf(fp,"CFE_TIME_TONE_CMD_MID 0x%04X   %d\n",CFE_TIME_TONE_CMD_MID,cmdAppId);
    cmdMsgId = CFE_TIME_TONE_CMD_MID;

    /* CFE_TIME_1HZ_CMD_MID */
    cmdAppId += (CFE_TIME_1HZ_CMD_MID - cmdMsgId);
    fprintf(fp,"CFE_TIME_1HZ_CMD_MID 0x%04X   %d\n",CFE_TIME_1HZ_CMD_MID,cmdAppId);

    /* Print the cFE Telemetry  Message IDs */
    fprintf(fp,"\n/* cFE Telemetry MsgIds for CPU1 - Processor 1 */\n");
    cmdAppId = 0;
#ifdef CFE_TLM_APPID_BASE_CPU1
    cmdAppId = CFE_TLM_APPID_BASE_CPU1;
#endif

    /* CFE_ES_HK_TLM_MID */
    fprintf(fp,"CFE_ES_HK_TLM_MID 0x%04X   %d\n",CFE_ES_HK_TLM_MID,cmdAppId);
    cmdMsgId = CFE_ES_HK_TLM_MID;

    /* CFE_EVS_HK_TLM_MID */
    cmdAppId += (CFE_EVS_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"CFE_EVS_HK_TLM_MID 0x%04X   %d\n",CFE_EVS_HK_TLM_MID,cmdAppId);
    cmdMsgId = CFE_EVS_HK_TLM_MID;

    /* CFE_SB_HK_TLM_MID */
    cmdAppId += (CFE_SB_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"CFE_SB_HK_TLM_MID 0x%04X   %d\n",CFE_SB_HK_TLM_MID,cmdAppId);
    cmdMsgId = CFE_SB_HK_TLM_MID;

    /* CFE_TBL_HK_TLM_MID */
    cmdAppId += (CFE_TBL_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"CFE_TBL_HK_TLM_MID 0x%04X   %d\n",CFE_TBL_HK_TLM_MID,cmdAppId);
    cmdMsgId = CFE_TBL_HK_TLM_MID;

    /* CFE_TIME_HK_TLM_MID */
    cmdAppId += (CFE_TIME_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"CFE_TIME_HK_TLM_MID 0x%04X   %d\n",CFE_TIME_HK_TLM_MID,cmdAppId);
    cmdMsgId = CFE_TIME_HK_TLM_MID;

    /* CFE_TIME_DIAG_TLM_MID */
    cmdAppId += (CFE_TIME_DIAG_TLM_MID - cmdMsgId);
    fprintf(fp,"CFE_TIME_DIAG_TLM_MID 0x%04X   %d\n",CFE_TIME_DIAG_TLM_MID,cmdAppId);
    cmdMsgId = CFE_TIME_DIAG_TLM_MID;

    /* CFE_EVS_EVENT_MSG_MID */
    cmdAppId += (CFE_EVS_EVENT_MSG_MID - cmdMsgId);
    fprintf(fp,"CFE_EVS_EVENT_MSG_MID 0x%04X   %d\n",CFE_EVS_EVENT_MSG_MID,cmdAppId);
    cmdMsgId = CFE_EVS_EVENT_MSG_MID;

    /* CFE_SB_STATS_TLM_MID */
    cmdAppId += (CFE_SB_STATS_TLM_MID - cmdMsgId);
    fprintf(fp,"CFE_SB_STATS_TLM_MID 0x%04X   %d\n",CFE_SB_STATS_TLM_MID,cmdAppId);
    cmdMsgId = CFE_SB_STATS_TLM_MID;

    /* CFE_ES_APP_TLM_MID */
    cmdAppId += (CFE_ES_APP_TLM_MID - cmdMsgId);
    fprintf(fp,"CFE_ES_APP_TLM_MID 0x%04X   %d\n",CFE_ES_APP_TLM_MID,cmdAppId);
    cmdMsgId = CFE_ES_APP_TLM_MID;

    /* CFE_TBL_REG_TLM_MID */
    cmdAppId += (CFE_TBL_REG_TLM_MID - cmdMsgId);
    fprintf(fp,"CFE_TBL_REG_TLM_MID 0x%04X   %d\n",CFE_TBL_REG_TLM_MID,cmdAppId);
    cmdMsgId = CFE_TBL_REG_TLM_MID;

    /* CFE_ES_MEMSTATS_TLM_MID */
    cmdAppId += (CFE_ES_MEMSTATS_TLM_MID - cmdMsgId);
    fprintf(fp,"CFE_ES_MEMSTATS_TLM_MID 0x%04X   %d\n",CFE_ES_MEMSTATS_TLM_MID,cmdAppId);

    /* Print the Application Command Message IDs (if they are defined)*/
    fprintf(fp,"\n/* Application Command MsgIds for CPU1 - Processor 1 */\n");

    cmdAppId = 128;
#ifdef APP_CMD_APPID_BASE_CPU1
    cmdAppId = APP_CMD_APPID_BASE_CPU1;
#endif

#ifdef TO_CMD_MID
    fprintf(fp,"TO_CMD_MID 0x%04X   %d\n",TO_CMD_MID,cmdAppId);
    cmdMsgId = TO_CMD_MID;
#endif

#ifdef TO_SEND_HK_MID
    cmdAppId += (TO_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TO_SEND_HK_MID 0x%04X   %d\n",TO_SEND_HK_MID,cmdAppId);
    cmdMsgId = TO_SEND_HK_MID;
#endif

#ifdef CI_CMD_MID
    cmdAppId += (CI_CMD_MID - cmdMsgId);
    fprintf(fp,"CI_CMD_MID 0x%04X   %d\n",CI_CMD_MID,cmdAppId);
    cmdMsgId = CI_CMD_MID;
#endif

#ifdef CI_SEND_HK_MID
    cmdAppId += (CI_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"CI_SEND_HK_MID 0x%04X   %d\n",CI_SEND_HK_MID,cmdAppId);
    cmdMsgId = CI_SEND_HK_MID;
#endif

    fprintf(fp,"\n/* Test Application Command MsgIds for CPU1 - Processor 1 */\n");

    /* Setup the Application IDs for the TST App Commands */
    cmdAppId = 256;
#ifdef APP_TST_CMD_APPID_BASE_CPU1
    cmdAppId = APP_TST_CMD_APPID_BASE_CPU1;
#endif

    /* Print the Test Application Command Message IDs (if they are defined)*/
#ifdef TST_EVS_CMD_MID
    fprintf(fp,"TST_EVS_CMD_MID 0x%04X   %d\n",TST_EVS_CMD_MID,cmdAppId);
    cmdMsgId = TST_EVS_CMD_MID;
#endif

#ifdef TST_TIME_CMD_MID
    cmdAppId += (TST_TIME_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_TIME_CMD_MID 0x%04X   %d\n",TST_TIME_CMD_MID,cmdAppId);
    cmdMsgId = TST_TIME_CMD_MID;
#endif

#ifdef TST_TIME_SEND_HK_MID
    cmdAppId += (TST_TIME_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_TIME_SEND_HK_MID 0x%04X   %d\n",TST_TIME_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_TIME_SEND_HK_MID;
#endif

#ifdef TST_SB_CMD_MID
    cmdAppId += (TST_SB_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_SB_CMD_MID 0x%04X   %d\n",TST_SB_CMD_MID,cmdAppId);
    cmdMsgId = TST_SB_CMD_MID;
#endif

#ifdef TST_SB_SEND_HK_MID
    cmdAppId += (TST_SB_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_SB_SEND_HK_MID 0x%04X   %d\n",TST_SB_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_SB_SEND_HK_MID;
#endif

#ifdef TST_TBL_CMD_MID
    cmdAppId += (TST_TBL_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_TBL_CMD_MID 0x%04X   %d\n",TST_TBL_CMD_MID,cmdAppId);
    cmdMsgId = TST_TBL_CMD_MID;
#endif

#ifdef TST_TBL_SEND_HK_MID
    cmdAppId += (TST_TBL_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_TBL_SEND_HK_MID 0x%04X   %d\n",TST_TBL_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_TBL_SEND_HK_MID;
#endif

#ifdef TST_TBL2_CMD_MID
    cmdAppId += (TST_TBL2_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_TBL2_CMD_MID 0x%04X   %d\n",TST_TBL2_CMD_MID,cmdAppId);
    cmdMsgId = TST_TBL2_CMD_MID;
#endif

#ifdef TST_TBL2_SEND_HK_MID
    cmdAppId += (TST_TBL2_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_TBL2_SEND_HK_MID 0x%04X   %d\n",TST_TBL2_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_TBL2_SEND_HK_MID;
#endif

#ifdef TST_ES_CMD_MID
    cmdAppId += (TST_ES_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_ES_CMD_MID 0x%04X   %d\n",TST_ES_CMD_MID,cmdAppId);
    cmdMsgId = TST_ES_CMD_MID;
#endif

#ifdef TST_ES_SEND_HK_MID
    cmdAppId += (TST_ES_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_ES_SEND_HK_MID 0x%04X   %d\n",TST_ES_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_ES_SEND_HK_MID;
#endif

#ifdef TST_ES2_CMD_MID
    cmdAppId += (TST_ES2_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_ES2_CMD_MID 0x%04X   %d\n",TST_ES2_CMD_MID,cmdAppId);
    cmdMsgId = TST_ES2_CMD_MID;
#endif

#ifdef TST_ES2_SEND_HK_MID
    cmdAppId += (TST_ES2_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_ES2_SEND_HK_MID 0x%04X   %d\n",TST_ES2_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_ES2_SEND_HK_MID;
#endif

#ifdef TST_ES3_CMD_MID
    cmdAppId += (TST_ES3_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_ES3_CMD_MID 0x%04X   %d\n", TST_ES3_CMD_MID,cmdAppId);
    cmdMsgId = TST_ES3_CMD_MID;
#endif

#ifdef TST_ES3_SEND_HK_MID
    cmdAppId += (TST_ES3_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_ES3_SEND_HK_MID 0x%04X   %d\n", TST_ES3_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_ES3_SEND_HK_MID;
#endif

#ifdef TST_ES4_CMD_MID
    cmdAppId += (TST_ES4_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_ES4_CMD_MID 0x%04X   %d\n",TST_ES4_CMD_MID,cmdAppId);
    cmdMsgId = TST_ES4_CMD_MID;
#endif

#ifdef TST_ES4_SEND_HK_MID
    cmdAppId += (TST_ES4_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_ES4_SEND_HK_MID 0x%04X   %d\n",TST_ES4_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_ES4_SEND_HK_MID;
#endif

#ifdef TST_ES5_CMD_MID
    cmdAppId += (TST_ES5_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_ES5_CMD_MID 0x%04X   %d\n",TST_ES5_CMD_MID,cmdAppId);
    cmdMsgId = TST_ES5_CMD_MID;
#endif

#ifdef TST_ES5_SEND_HK_MID
    cmdAppId += (TST_ES5_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_ES5_SEND_HK_MID 0x%04X   %d\n",TST_ES5_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_ES5_SEND_HK_MID;
#endif

#ifdef TST_ES6_CMD_MID
    cmdAppId += (TST_ES6_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_ES6_CMD_MID 0x%04X   %d\n",TST_ES6_CMD_MID,cmdAppId);
    cmdMsgId = TST_ES6_CMD_MID;
#endif

#ifdef TST_ES6_SEND_HK_MID
    cmdAppId += (TST_ES6_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_ES6_SEND_HK_MID 0x%04X   %d\n",TST_ES6_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_ES6_SEND_HK_MID;
#endif

#ifdef TST_ES7_CMD_MID
    cmdAppId += (TST_ES7_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_ES7_CMD_MID 0x%04X   %d\n",TST_ES7_CMD_MID,cmdAppId);
    cmdMsgId = TST_ES7_CMD_MID;
#endif

#ifdef TST_ES7_SEND_HK_MID
    cmdAppId += (TST_ES7_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_ES7_SEND_HK_MID 0x%04X   %d\n",TST_ES7_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_ES7_SEND_HK_MID;
#endif

#ifdef TST_ES8_CMD_MID
    cmdAppId += (TST_ES8_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_ES8_CMD_MID 0x%04X   %d\n",TST_ES8_CMD_MID,cmdAppId);
    cmdMsgId = TST_ES8_CMD_MID;
#endif

#ifdef TST_ES8_SEND_HK_MID
    cmdAppId += (TST_ES8_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_ES8_SEND_HK_MID 0x%04X   %d\n",TST_ES8_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_ES8_SEND_HK_MID;
#endif

#ifdef TST_ES9_CMD_MID
    cmdAppId += (TST_ES9_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_ES9_CMD_MID 0x%04X   %d\n",TST_ES9_CMD_MID,cmdAppId);
    cmdMsgId = TST_ES9_CMD_MID;
#endif

#ifdef TST_ES9_SEND_HK_MID
    cmdAppId += (TST_ES9_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_ES9_SEND_HK_MID 0x%04X   %d\n",TST_ES9_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_ES9_SEND_HK_MID;
#endif

#ifdef TST_ES10_CMD_MID
    cmdAppId += (TST_ES10_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_ES10_CMD_MID 0x%04X   %d\n",TST_ES10_CMD_MID,cmdAppId);
    cmdMsgId = TST_ES10_CMD_MID;
#endif

#ifdef TST_ES10_SEND_HK_MID
    cmdAppId += (TST_ES10_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_ES10_SEND_HK_MID 0x%04X   %d\n",TST_ES10_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_ES10_SEND_HK_MID;
#endif

#ifdef TST_ES11_CMD_MID
    cmdAppId += (TST_ES11_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_ES11_CMD_MID 0x%04X   %d\n",TST_ES11_CMD_MID,cmdAppId);
    cmdMsgId = TST_ES11_CMD_MID;
#endif

#ifdef TST_ES11_SEND_HK_MID
    cmdAppId += (TST_ES11_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_ES11_SEND_HK_MID 0x%04X   %d\n",TST_ES11_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_ES11_SEND_HK_MID;
#endif

#ifdef TST_ES12_CMD_MID
    cmdAppId += (TST_ES12_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_ES12_CMD_MID 0x%04X   %d\n",TST_ES12_CMD_MID,cmdAppId);
    cmdMsgId = TST_ES12_CMD_MID;
#endif

#ifdef TST_ES12_SEND_HK_MID
    cmdAppId += (TST_ES12_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_ES12_SEND_HK_MID 0x%04X   %d\n",TST_ES12_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_ES12_SEND_HK_MID;
#endif

#ifdef TST_ES13_CMD_MID
    cmdAppId += (TST_ES13_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_ES13_CMD_MID 0x%04X   %d\n",TST_ES13_CMD_MID,cmdAppId);
    cmdMsgId = TST_ES13_CMD_MID;
#endif

#ifdef TST_ES13_SEND_HK_MID
    cmdAppId += (TST_ES13_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_ES13_SEND_HK_MID 0x%04X   %d\n",TST_ES13_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_ES13_SEND_HK_MID;
#endif

#ifdef TST_ES14_CMD_MID
    cmdAppId += (TST_ES14_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_ES14_CMD_MID 0x%04X   %d\n",TST_ES14_CMD_MID,cmdAppId);
    cmdMsgId = TST_ES14_CMD_MID;
#endif

#ifdef TST_ES14_SEND_HK_MID
    cmdAppId += (TST_ES14_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_ES14_SEND_HK_MID 0x%04X   %d\n",TST_ES14_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_ES14_SEND_HK_MID;
#endif

#ifdef TST_ES15_CMD_MID
    cmdAppId += (TST_ES15_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_ES15_CMD_MID 0x%04X   %d\n",TST_ES15_CMD_MID,cmdAppId);
    cmdMsgId = TST_ES15_CMD_MID;
#endif

#ifdef TST_ES15_SEND_HK_MID
    cmdAppId += (TST_ES15_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_ES15_SEND_HK_MID 0x%04X   %d\n",TST_ES15_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_ES15_SEND_HK_MID;
#endif

#ifdef TST_ES16_CMD_MID
    cmdAppId += (TST_ES16_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_ES16_CMD_MID 0x%04X   %d\n",TST_ES16_CMD_MID,cmdAppId);
    cmdMsgId = TST_ES16_CMD_MID;
#endif

#ifdef TST_ES16_SEND_HK_MID
    cmdAppId += (TST_ES16_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_ES16_SEND_HK_MID 0x%04X   %d\n",TST_ES16_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_ES16_SEND_HK_MID;
#endif

#ifdef TST_ES17_CMD_MID
    cmdAppId += (TST_ES17_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_ES17_CMD_MID 0x%04X   %d\n",TST_ES17_CMD_MID,cmdAppId);
    cmdMsgId = TST_ES17_CMD_MID;
#endif

#ifdef TST_ES17_SEND_HK_MID
    cmdAppId += (TST_ES17_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_ES17_SEND_HK_MID 0x%04X   %d\n",TST_ES17_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_ES17_SEND_HK_MID;
#endif

#ifdef TST_ES18_CMD_MID
    cmdAppId += (TST_ES18_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_ES18_CMD_MID 0x%04X   %d\n",TST_ES18_CMD_MID,cmdAppId);
    cmdMsgId = TST_ES18_CMD_MID;
#endif

#ifdef TST_ES18_SEND_HK_MID
    cmdAppId += (TST_ES18_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_ES18_SEND_HK_MID 0x%04X   %d\n",TST_ES18_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_ES18_SEND_HK_MID;
#endif

#ifdef TST_ES19_CMD_MID
    cmdAppId += (TST_ES19_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_ES19_CMD_MID 0x%04X   %d\n",TST_ES19_CMD_MID,cmdAppId);
    cmdMsgId = TST_ES19_CMD_MID;
#endif

#ifdef TST_ES19_SEND_HK_MID
    cmdAppId += (TST_ES19_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_ES19_SEND_HK_MID 0x%04X   %d\n",TST_ES19_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_ES19_SEND_HK_MID;
#endif

#ifdef TST_ES20_CMD_MID
    cmdAppId += (TST_ES20_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_ES20_CMD_MID 0x%04X   %d\n",TST_ES20_CMD_MID,cmdAppId);
    cmdMsgId = TST_ES20_CMD_MID;
#endif

#ifdef TST_ES20_SEND_HK_MID
    cmdAppId += (TST_ES20_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_ES20_SEND_HK_MID 0x%04X   %d\n",TST_ES20_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_ES20_SEND_HK_MID;
#endif

#ifdef TST_ES21_CMD_MID
    cmdAppId += (TST_ES21_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_ES21_CMD_MID 0x%04X   %d\n",TST_ES21_CMD_MID,cmdAppId);
    cmdMsgId = TST_ES21_CMD_MID;
#endif

#ifdef TST_ES21_SEND_HK_MID
    cmdAppId += (TST_ES21_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_ES21_SEND_HK_MID 0x%04X   %d\n",TST_ES21_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_ES21_SEND_HK_MID;
#endif

#ifdef TST_ES22_CMD_MID
    cmdAppId += (TST_ES22_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_ES22_CMD_MID 0x%04X   %d\n",TST_ES22_CMD_MID,cmdAppId);
    cmdMsgId = TST_ES22_CMD_MID;
#endif

#ifdef TST_ES22_SEND_HK_MID
    cmdAppId += (TST_ES22_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_ES22_SEND_HK_MID 0x%04X   %d\n",TST_ES22_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_ES22_SEND_HK_MID;
#endif

#ifdef TST_ES23_CMD_MID
    cmdAppId += (TST_ES23_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_ES23_CMD_MID 0x%04X   %d\n",TST_ES23_CMD_MID,cmdAppId);
    cmdMsgId = TST_ES23_CMD_MID;
#endif

#ifdef TST_ES23_SEND_HK_MID
    cmdAppId += (TST_ES23_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_ES23_SEND_HK_MID 0x%04X   %d\n",TST_ES23_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_ES23_SEND_HK_MID;
#endif

#ifdef TST_ES24_CMD_MID
    cmdAppId += (TST_ES24_CMD_MID - cmdMsgId);
    fprintf(fp,"TST_ES24_CMD_MID 0x%04X   %d\n",TST_ES24_CMD_MID,cmdAppId);
    cmdMsgId = TST_ES24_CMD_MID;
#endif

#ifdef TST_ES24_SEND_HK_MID
    cmdAppId += (TST_ES24_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"TST_ES24_SEND_HK_MID 0x%04X   %d\n",TST_ES24_SEND_HK_MID,cmdAppId);
    cmdMsgId = TST_ES24_SEND_HK_MID;
#endif

#ifdef QQ_CMD_MID
    cmdAppId += (QQ_CMD_MID - cmdMsgId);
    fprintf(fp,"QQ_CMD_MID 0x%04X   %d\n",QQ_CMD_MID,cmdAppId);
    cmdMsgId = QQ_CMD_MID;
#endif

#ifdef QQ_SEND_HK_MID
    cmdAppId += (QQ_SEND_HK_MID - cmdMsgId);
    fprintf(fp,"QQ_SEND_HK_MID 0x%04X   %d\n",QQ_SEND_HK_MID,cmdAppId);
#endif


    /* Setup the Application IDs for Telemetry */
    cmdAppId = 128;
#ifdef APP_TLM_APPID_BASE_CPU1
    cmdAppId = APP_TLM_APPID_BASE_CPU1;
#endif

    /* Print the Application Telemetry Message IDs (if they are defined)*/
    fprintf(fp,"\n/* Application Telemetry MsgIds for CPU1 - Processor 1 */\n");

#ifdef TO_HK_TLM_MID
    fprintf(fp,"TO_HK_TLM_MID 0x%04X   %d\n",TO_HK_TLM_MID,cmdAppId);
    cmdMsgId = TO_HK_TLM_MID;
#endif

#ifdef TO_DATA_TYPES_MID
    cmdAppId += (TO_DATA_TYPES_MID - cmdMsgId);
    fprintf(fp,"TO_DATA_TYPES_MID 0x%04X   %d\n",TO_DATA_TYPES_MID,cmdAppId);
    cmdMsgId = TO_DATA_TYPES_MID;
#endif

#ifdef CI_HK_TLM_MID
    cmdAppId += (CI_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"CI_HK_TLM_MID 0x%04X   %d\n",CI_HK_TLM_MID,cmdAppId);
#endif

    /* Setup the Application IDs for Test Application Telemetry */
    cmdAppId = 256;
#ifdef APP_TST_TLM_APPID_BASE_CPU1
    cmdAppId = APP_TST_TLM_APPID_BASE_CPU1;
#endif

    fprintf(fp,"\n/* Test Application Telemetry MsgIds for CPU1 - Processor 1 */\n");
#ifdef TST_SB_HK_TLM_MID
    fprintf(fp,"TST_SB_HK_TLM_MID 0x%04X   %d\n",TST_SB_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_SB_HK_TLM_MID;
#endif

#ifdef TST_SB_PIPE0_TLM_MID
    cmdAppId += (TST_SB_PIPE0_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_SB_PIPE0_TLM_MID 0x%04X   %d\n",TST_SB_PIPE0_TLM_MID,cmdAppId);
    cmdMsgId = TST_SB_PIPE0_TLM_MID;
#endif

#ifdef TST_TBL_HK_TLM_MID
    cmdAppId += (TST_TBL_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_TBL_HK_TLM_MID 0x%04X   %d\n",TST_TBL_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_TBL_HK_TLM_MID;
#endif

#ifdef TST_TBL_MAXTBLST_TLM_MID
    cmdAppId += (TST_TBL_MAXTBLST_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_TBL_MAXTBLST_TLM_MID 0x%04X   %d\n",TST_TBL_MAXTBLST_TLM_MID,cmdAppId);
    cmdMsgId = TST_TBL_MAXTBLST_TLM_MID;
#endif

#ifdef TST_TBL_INFO_TLM_MID
    cmdAppId += (TST_TBL_INFO_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_TBL_INFO_TLM_MID 0x%04X   %d\n",TST_TBL_INFO_TLM_MID,cmdAppId);
    cmdMsgId = TST_TBL_INFO_TLM_MID;
#endif

#ifdef TST_TBL2_HK_TLM_MID
    cmdAppId += (TST_TBL2_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_TBL2_HK_TLM_MID 0x%04X   %d\n",TST_TBL2_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_TBL2_HK_TLM_MID;
#endif

#ifdef TST_TBL2_MAXTBLST_TLM_MID
    cmdAppId += (TST_TBL2_MAXTBLST_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_TBL2_MAXTBLST_TLM_MID 0x%04X   %d\n",TST_TBL2_MAXTBLST_TLM_MID,cmdAppId);
    cmdMsgId = TST_TBL2_MAXTBLST_TLM_MID;
#endif

#ifdef TST_ES_HK_TLM_MID
    cmdAppId += (TST_ES_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_ES_HK_TLM_MID 0x%04X   %d\n",TST_ES_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_ES_HK_TLM_MID;
#endif

#ifdef TST_ES2_HK_TLM_MID
    cmdAppId += (TST_ES2_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_ES2_HK_TLM_MID 0x%04X   %d\n",TST_ES2_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_ES2_HK_TLM_MID;
#endif

#ifdef TST_ES3_HK_TLM_MID
    cmdAppId += (TST_ES3_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_ES3_HK_TLM_MID 0x%04X   %d\n",TST_ES3_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_ES3_HK_TLM_MID;
#endif

#ifdef TST_ES4_HK_TLM_MID
    cmdAppId += (TST_ES4_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_ES4_HK_TLM_MID 0x%04X   %d\n",TST_ES4_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_ES4_HK_TLM_MID;
#endif

#ifdef TST_ES5_HK_TLM_MID
    cmdAppId += (TST_ES5_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_ES5_HK_TLM_MID 0x%04X   %d\n",TST_ES5_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_ES5_HK_TLM_MID;
#endif

#ifdef TST_ES6_HK_TLM_MID
    cmdAppId += (TST_ES6_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_ES6_HK_TLM_MID 0x%04X   %d\n",TST_ES6_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_ES6_HK_TLM_MID;
#endif

#ifdef TST_ES7_HK_TLM_MID
    cmdAppId += (TST_ES7_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_ES7_HK_TLM_MID 0x%04X   %d\n",TST_ES7_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_ES7_HK_TLM_MID;
#endif

#ifdef TST_ES8_HK_TLM_MID
    cmdAppId += (TST_ES8_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_ES8_HK_TLM_MID 0x%04X   %d\n",TST_ES8_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_ES8_HK_TLM_MID;
#endif

#ifdef TST_ES9_HK_TLM_MID
    cmdAppId += (TST_ES9_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_ES9_HK_TLM_MID 0x%04X   %d\n",TST_ES9_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_ES9_HK_TLM_MID;
#endif

#ifdef TST_ES10_HK_TLM_MID
    cmdAppId += (TST_ES10_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_ES10_HK_TLM_MID 0x%04X   %d\n",TST_ES10_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_ES10_HK_TLM_MID;
#endif

#ifdef TST_ES11_HK_TLM_MID
    cmdAppId += (TST_ES11_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_ES11_HK_TLM_MID 0x%04X   %d\n",TST_ES11_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_ES11_HK_TLM_MID;
#endif

#ifdef TST_ES12_HK_TLM_MID
    cmdAppId += (TST_ES2_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_ES12_HK_TLM_MID 0x%04X   %d\n",TST_ES12_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_ES12_HK_TLM_MID;
#endif

#ifdef TST_ES13_HK_TLM_MID
    cmdAppId += (TST_ES13_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_ES13_HK_TLM_MID 0x%04X   %d\n",TST_ES13_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_ES13_HK_TLM_MID;
#endif

#ifdef TST_ES14_HK_TLM_MID
    cmdAppId += (TST_ES14_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_ES14_HK_TLM_MID 0x%04X   %d\n",TST_ES14_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_ES14_HK_TLM_MID;
#endif

#ifdef TST_ES15_HK_TLM_MID
    cmdAppId += (TST_ES15_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_ES15_HK_TLM_MID 0x%04X   %d\n",TST_ES15_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_ES15_HK_TLM_MID;
#endif

#ifdef TST_ES16_HK_TLM_MID
    cmdAppId += (TST_ES16_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_ES16_HK_TLM_MID 0x%04X   %d\n",TST_ES16_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_ES16_HK_TLM_MID;
#endif

#ifdef TST_ES17_HK_TLM_MID
    cmdAppId += (TST_ES17_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_ES17_HK_TLM_MID 0x%04X   %d\n",TST_ES17_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_ES17_HK_TLM_MID;
#endif

#ifdef TST_ES18_HK_TLM_MID
    cmdAppId += (TST_ES18_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_ES18_HK_TLM_MID 0x%04X   %d\n",TST_ES18_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_ES18_HK_TLM_MID;
#endif

#ifdef TST_ES19_HK_TLM_MID
    cmdAppId += (TST_ES19_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_ES19_HK_TLM_MID 0x%04X   %d\n",TST_ES19_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_ES19_HK_TLM_MID;
#endif

#ifdef TST_ES20_HK_TLM_MID
    cmdAppId += (TST_ES20_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_ES20_HK_TLM_MID 0x%04X   %d\n",TST_ES20_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_ES20_HK_TLM_MID;
#endif

#ifdef TST_ES21_HK_TLM_MID
    cmdAppId += (TST_ES21_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_ES21_HK_TLM_MID 0x%04X   %d\n",TST_ES21_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_ES21_HK_TLM_MID;
#endif

#ifdef TST_ES22_HK_TLM_MID
    cmdAppId += (TST_ES22_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_ES22_HK_TLM_MID 0x%04X   %d\n",TST_ES22_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_ES22_HK_TLM_MID;
#endif

#ifdef TST_ES23_HK_TLM_MID
    cmdAppId += (TST_ES23_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_ES23_HK_TLM_MID 0x%04X   %d\n",TST_ES23_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_ES23_HK_TLM_MID;
#endif

#ifdef TST_ES24_HK_TLM_MID
    cmdAppId += (TST_ES24_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_ES24_HK_TLM_MID 0x%04X   %d\n",TST_ES24_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_ES24_HK_TLM_MID;
#endif

#ifdef TST_TIME_HK_TLM_MID
    cmdAppId += (TST_TIME_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_TIME_HK_TLM_MID 0x%04X   %d\n",TST_TIME_HK_TLM_MID,cmdAppId);
    cmdMsgId = TST_TIME_HK_TLM_MID;
#endif

#ifdef TST_TIME_TAI_TLM_MID
    cmdAppId += (TST_TIME_TAI_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_TIME_TAI_TLM_MID 0x%04X   %d\n",TST_TIME_TAI_TLM_MID,cmdAppId);
    cmdMsgId = TST_TIME_TAI_TLM_MID;
#endif

#ifdef TST_TIME_UTC_TLM_MID
    cmdAppId += (TST_TIME_UTC_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_TIME_UTC_TLM_MID 0x%04X   %d\n",TST_TIME_UTC_TLM_MID,cmdAppId);
    cmdMsgId = TST_TIME_UTC_TLM_MID;
#endif

#ifdef TST_TIME_ST_TLM_MID
    cmdAppId += (TST_TIME_ST_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_TIME_ST_TLM_MID 0x%04X   %d\n",TST_TIME_ST_TLM_MID,cmdAppId);
    cmdMsgId = TST_TIME_ST_TLM_MID;
#endif

#ifdef TST_TIME_MET_TLM_MID
    cmdAppId += (TST_TIME_MET_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_TIME_MET_TLM_MID 0x%04X   %d\n",TST_TIME_MET_TLM_MID,cmdAppId);
    cmdMsgId = TST_TIME_MET_TLM_MID;
#endif

#ifdef TST_TIME_METSEC_TLM_MID
    cmdAppId += (TST_TIME_METSEC_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_TIME_METSEC_TLM_MID 0x%04X   %d\n",TST_TIME_METSEC_TLM_MID,cmdAppId);
    cmdMsgId = TST_TIME_METSEC_TLM_MID;
#endif

#ifdef TST_TIME_METSUB_TLM_MID
    cmdAppId += (TST_TIME_METSUB_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_TIME_METSUB_TLM_MID 0x%04X   %d\n",TST_TIME_METSUB_TLM_MID,cmdAppId);
    cmdMsgId = TST_TIME_METSUB_TLM_MID;
#endif

#ifdef TST_TIME_STCF_TLM_MID
    cmdAppId += (TST_TIME_STCF_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_TIME_STCF_TLM_MID 0x%04X   %d\n",TST_TIME_STCF_TLM_MID,cmdAppId);
    cmdMsgId = TST_TIME_STCF_TLM_MID;
#endif

#ifdef TST_TIME_LPSEC_TLM_MID
    cmdAppId += (TST_TIME_LPSEC_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_TIME_LPSEC_TLM_MID 0x%04X   %d\n",TST_TIME_LPSEC_TLM_MID,cmdAppId);
    cmdMsgId = TST_TIME_LPSEC_TLM_MID;
#endif

#ifdef TST_TIME_CLKST_TLM_MID
    cmdAppId += (TST_TIME_CLKST_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_TIME_CLKST_TLM_MID 0x%04X   %d\n",TST_TIME_CLKST_TLM_MID,cmdAppId);
    cmdMsgId = TST_TIME_CLKST_TLM_MID;
#endif

#ifdef TST_TIME_ADD_TLM_MID
    cmdAppId += (TST_TIME_ADD_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_TIME_ADD_TLM_MID 0x%04X   %d\n",TST_TIME_ADD_TLM_MID,cmdAppId);
    cmdMsgId = TST_TIME_ADD_TLM_MID;
#endif

#ifdef TST_TIME_SUBTIME_TLM_MID
    cmdAppId += (TST_TIME_SUBTIME_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_TIME_SUBTIME_TLM_MID 0x%04X   %d\n",TST_TIME_SUBTIME_TLM_MID,cmdAppId);
    cmdMsgId = TST_TIME_SUBTIME_TLM_MID;
#endif

#ifdef TST_TIME_COMP_TLM_MID
    cmdAppId += (TST_TIME_COMP_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_TIME_COMP_TLM_MID 0x%04X   %d\n",TST_TIME_COMP_TLM_MID,cmdAppId);
    cmdMsgId = TST_TIME_COMP_TLM_MID;
#endif

#ifdef TST_TIME_SUBMICRO_TLM_MID
    cmdAppId += (TST_TIME_SUBMICRO_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_TIME_SUBMICRO_TLM_MID 0x%04X   %d\n",TST_TIME_SUBMICRO_TLM_MID,cmdAppId);
    cmdMsgId = TST_TIME_SUBMICRO_TLM_MID;
#endif

#ifdef TST_TIME_MICROSUB_TLM_MID
    cmdAppId += (TST_TIME_MICROSUB_TLM_MID - cmdMsgId);
    fprintf(fp,"TST_TIME_MICROSUB_TLM_MID 0x%04X   %d\n",TST_TIME_MICROSUB_TLM_MID,cmdAppId);
    cmdMsgId = TST_TIME_MICROSUB_TLM_MID;
#endif

#ifdef QQ_HK_TLM_MID
    cmdAppId += (QQ_HK_TLM_MID - cmdMsgId);
    fprintf(fp,"QQ_HK_TLM_MID 0x%04X   %d\n",QQ_HK_TLM_MID,cmdAppId);
#endif
  }
  else 
    printf("could not open file for writing");
}
