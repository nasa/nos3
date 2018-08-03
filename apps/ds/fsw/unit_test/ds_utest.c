/************************************************************************
** File:
**   $Id: ds_utest.c 1.4.1.1 2015/02/28 17:13:54EST sstrege Exp  $
**
**  Copyright © 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** Purpose: 
**   This is a test driver used to unit test the CFS Data Storage (DS)
**   Application.
**
**   The CFS Data Storage (DS) is a table driven application
**   that provides the capability for telemetry packet storage
**   to Core Flight Executive (cFE) based systems. 
** 
**   Output can be directed either to screen or to file:
**   To direct output to screen, 
**      comment in '#define UTF_USE_STDOUT' statement in the
**      utf_custom.h file.
**
**   To direct output to file, 
**      comment out '#define UTF_USE_STDOUT' statement in 
**      utf_custom.h file.
** 
**   $Log: ds_utest.c  $
**   Revision 1.4.1.1 2015/02/28 17:13:54EST sstrege 
**   Added copyright information
**   Revision 1.4 2009/12/07 13:40:39EST lwalling 
**   Update DS unit tests, add unit test results files to MKS
**   Revision 1.3 2009/09/01 15:22:29EDT lwalling 
**   Add unit test for DS_AppMain()
**   Revision 1.2 2009/08/13 10:01:28EDT lwalling 
**   Updates to unit test source files
**   Revision 1.1 2009/05/26 13:37:47EDT lwalling 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/ds/fsw/unit_test/project.pj
** 
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/
#include "utf_custom.h"        /* UTF headers         */
#include "utf_types.h"
#include "utf_cfe_sb.h"
#include "utf_osapi.h"
#include "utf_osloader.h"
#include "utf_osfileapi.h"
#include "utf_cfe.h"

#include "ds_perfids.h"
#include "ds_msgids.h"
#include "ds_platform_cfg.h"

#include "ds_platform_cfg.h"
#include "ds_verify.h"

#include "ds_msg.h"
#include "ds_app.h"
#include "ds_cmds.h"
#include "ds_file.h"
#include "ds_table.h"
#include "ds_events.h"
#include "ds_version.h"

#include "cfe_es_cds.h"        /* cFE headers         */

#include <stdlib.h>            /* System headers      */

/************************************************************************
** Macro Definitions
*************************************************************************/
#define MESSAGE_FORMAT_IS_CCSDS

/************************************************************************
** DS global data external to this file
*************************************************************************/
extern  DS_AppData_t DS_AppData;   /* App data */


/*
** Entry point function for tests in ds_utest_app.c
*/
void Test_app(void);

/*
** Entry point function for tests in ds_utest_cmds.c
*/
void Test_cmds(void);

/*
** Entry point function for tests in ds_utest_file.c
*/
void Test_file(void);

/*
** Entry point function for tests in ds_utest_table.c
*/
void Test_table(void);


/************************************************************************
** Local data
*************************************************************************/

/*
** Global variables used by function hooks
*/
uint32   CFE_SB_SubscribeCallCount     = 1;
uint32   CFE_SB_SubscribeFailCount     = 1;
uint32   CFE_TBL_RegisterCallCount     = 1;
uint32   CFE_TBL_RegisterFailCount     = 1;
uint32   CFE_TBL_LoadCallCount         = 1;
uint32   CFE_TBL_LoadFailCount         = 1;
uint32   CFE_TBL_GetAddressCallCount   = 1; 
uint32   CFE_TBL_GetAddressFailCount   = 1;
uint32   CFE_TBL_GetAddressUpdateCount = 1;

uint32   UT_TotalTestCount = 0;
uint32   UT_TotalFailCount = 0;

uint16   UT_CommandPkt[256];

DS_NoopCmd_t        *UT_NoopCmd =        (DS_NoopCmd_t *)        &UT_CommandPkt[0];
DS_ResetCmd_t       *UT_ResetCmd =       (DS_ResetCmd_t *)       &UT_CommandPkt[0];
DS_AppStateCmd_t    *UT_AppStateCmd =    (DS_AppStateCmd_t *)    &UT_CommandPkt[0];
DS_FilterFileCmd_t  *UT_FilterFileCmd =  (DS_FilterFileCmd_t *)  &UT_CommandPkt[0];
DS_FilterTypeCmd_t  *UT_FilterTypeCmd =  (DS_FilterTypeCmd_t *)  &UT_CommandPkt[0];
DS_FilterParmsCmd_t *UT_FilterParmsCmd = (DS_FilterParmsCmd_t *) &UT_CommandPkt[0];
DS_DestTypeCmd_t    *UT_DestTypeCmd =    (DS_DestTypeCmd_t *)    &UT_CommandPkt[0];
DS_DestStateCmd_t   *UT_DestStateCmd =   (DS_DestStateCmd_t *)   &UT_CommandPkt[0];
DS_DestPathCmd_t    *UT_DestPathCmd =    (DS_DestPathCmd_t *)    &UT_CommandPkt[0];
DS_DestBaseCmd_t    *UT_DestBaseCmd =    (DS_DestBaseCmd_t *)    &UT_CommandPkt[0];
DS_DestExtCmd_t     *UT_DestExtCmd =     (DS_DestExtCmd_t *)     &UT_CommandPkt[0];
DS_DestSizeCmd_t    *UT_DestSizeCmd =    (DS_DestSizeCmd_t *)    &UT_CommandPkt[0];
DS_DestAgeCmd_t     *UT_DestAgeCmd =     (DS_DestAgeCmd_t *)     &UT_CommandPkt[0];
DS_DestCountCmd_t   *UT_DestCountCmd =   (DS_DestCountCmd_t *)   &UT_CommandPkt[0];
DS_CloseFileCmd_t   *UT_CloseFileCmd =   (DS_CloseFileCmd_t *)   &UT_CommandPkt[0];

DS_FilterTable_t     UT_FilterTbl;  
DS_DestFileTable_t   UT_DestFileTbl;

extern  DS_FilterTable_t     DS_FilterTable;
extern  DS_DestFileTable_t   DS_DestFileTable;

DS_FilterTable_t    *DS_FilterTblPtr = &DS_FilterTable;
DS_DestFileTable_t  *DS_DestFileTblPtr = &DS_DestFileTable;


/************************************************************************
** Local function prototypes
*************************************************************************/

void PrintHKPacket (uint8 source, void *packet);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* DS unit test program main                                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int main(void)
{
    /*
    ** Set up output results text file           
    */
    UTF_set_output_filename("ds_utest.out");

    /*
    ** Set up HK packet handler           
    */
    UTF_set_packet_handler(DS_HK_TLM_MID, (utf_packet_handler)PrintHKPacket);
    
    /*
    ** Initialize time data structures
    */
    UTF_init_sim_time(0.0);

    /*
    ** Initialize ES application data                         
    */
    UTF_ES_InitAppRecords();
    UTF_ES_AddAppRecord("DS",0);  
    CFE_ES_RegisterApp();

    /*
    ** Initialize CDS and table services data structures
    */
    CFE_ES_CDS_EarlyInit();
    CFE_TBL_EarlyInit();

    /*
     * Setup the virtual/physical file system mapping...
     *
     * The following local machine directory structure is required:
     *
     * ... ds/fsw/unit_test          <-- this is the current working directory
     * ... ds/fsw/unit_test/disk/TT  <-- physical location for virtual disk "/tt"
     */
    UTF_add_volume("/TT", "disk", FS_BASED, FALSE, FALSE, TRUE, "TT", "/tt", 0);

    /*
    ** Delete files created during previous tests
    */
    OS_remove("/tt/app00002000.tlm");
    OS_remove("/tt/app1980001000000.hk");
    OS_remove("/tt/b_00000000.x");
    OS_remove("/tt/b_99999999.x");

    /*
    ** Required setup prior to calling many CFE API functions
    */
    CFE_ES_RegisterApp();
    CFE_EVS_Register(NULL, 0, 0);

    /*
    ** Run DS application unit tests
    */
    UTF_put_text("\n*** DS -- Testing ds_cmds.c ***\n");
    Test_cmds();

    UTF_put_text("\n*** DS -- Testing ds_file.c ***\n");
    Test_file();

    UTF_put_text("\n*** DS -- Testing ds_table.c ***\n");
    Test_table();

    UTF_put_text("\n*** DS -- Testing ds_app.c ***\n");
    Test_app();

    UTF_put_text("\n*** DS -- Total test count = %d, total test errors = %d\n\n", UT_TotalTestCount, UT_TotalFailCount);

    /*
    ** Invoke the main loop "success" test now because the program
    **  will end when the last entry in the SB input file is read.
    */
	UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_RUNLOOP_PROC, TRUE);
    UTF_CFE_TBL_Set_Api_Return_Code (CFE_TBL_GETSTATUS_PROC, CFE_TBL_INFO_UPDATE_PENDING);
    DS_AppMain();

    return 0;
   
} /* End of main() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Prints out the current values in the LC housekeeping packet     */
/* data structure                                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void PrintHKPacket (uint8 source, void *packet)
{
    DS_HkPacket_t *HkPacket = (DS_HkPacket_t *) packet;
    
    /* Output the DS housekeeping data */
    UTF_put_text("\nDS HOUSEKEEPING DATA:\n");

    UTF_put_text("CmdAcceptedCounter = %d -- CmdRejectedCounter = %d\n",
                  HkPacket->CmdAcceptedCounter, HkPacket->CmdRejectedCounter);
    UTF_put_text("DisabledPktCounter = %d -- IgnoredPktCounter = %d\n",
                  HkPacket->DisabledPktCounter, HkPacket->IgnoredPktCounter);
    UTF_put_text("FilteredPktCounter = %d -- PassedPktCounter = %d\n",
                  HkPacket->FilteredPktCounter, HkPacket->PassedPktCounter);
    UTF_put_text("FileWriteCounter = %d -- FileWriteErrCounter = %d\n",
                  HkPacket->FileWriteCounter, HkPacket->FileWriteErrCounter);
    UTF_put_text("FileUpdateCounter = %d -- FileUpdateErrCounter = %d\n",
                  HkPacket->FileUpdateCounter, HkPacket->FileUpdateErrCounter);
    UTF_put_text("DestTblLoadCounter = %d -- DestTblErrCounter = %d\n",
                  HkPacket->DestTblLoadCounter, HkPacket->DestTblErrCounter);
    UTF_put_text("FilterTblLoadCounter = %d -- FilterTblErrCounter = %d\n",
                  HkPacket->FilterTblLoadCounter, HkPacket->FilterTblErrCounter);
    UTF_put_text("AppEnableState = %d\n", HkPacket->AppEnableState);
    UTF_put_text("\n");
   
} /* End of PrintHKPacket() */






