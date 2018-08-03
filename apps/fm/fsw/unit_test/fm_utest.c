/************************************************************************
** File:
**   $Id: fm_utest.c 1.8 2014/12/04 17:52:08EST lwalling Exp  $
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
**   $Log: fm_utest.c  $
**   Revision 1.8 2014/12/04 17:52:08EST lwalling 
**   Removed unused CommandWarnCounter
**   Revision 1.7 2014/11/13 17:03:56EST lwalling 
**   Modified unit tests to remove temp directories and files after unit tests complete
**   Revision 1.6 2010/03/08 15:49:14EST lwalling 
**   Remove uint64 data type from free space packet
**   Revision 1.5 2010/02/25 13:45:45EST lwalling 
**   Fix print statement for 64 bit free space values
**   Revision 1.4 2009/12/02 14:31:17EST lwalling 
**   Update FM unit tests to match UTF changes
**   Revision 1.3 2009/11/20 15:40:40EST lwalling 
**   Unit test updates
**   Revision 1.2 2009/11/13 16:25:35EST lwalling 
**   Updated unit tests, modified macro names
**   Revision 1.1 2009/11/09 18:15:51EST lwalling 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/fsw/unit_test/project.pj
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

#include "fm_perfids.h"
#include "fm_msgids.h"
#include "fm_platform_cfg.h"

#include "fm_verify.h"

#include "fm_defs.h"
#include "fm_msg.h"
#include "fm_msgdefs.h"
#include "fm_app.h"
#include "fm_cmds.h"
#include "fm_cmd_utils.h"
#include "fm_tbl.h"
#include "fm_events.h"
#include "fm_version.h"

#include "cfe_es_cds.h"        /* cFE headers         */

#include <stdlib.h>            /* System headers      */

/************************************************************************
** Macro Definitions
*************************************************************************/
#define MESSAGE_FORMAT_IS_CCSDS

/************************************************************************
** FM global data external to this file
*************************************************************************/
extern  FM_GlobalData_t FM_GlobalData;   /* App data */

void CreateTestFile(char *Filename, int SizeInKs, boolean LeaveOpen);


/************************************************************************
** Local copies of global functions
*************************************************************************/
int32 DecompressResult = CFE_SUCCESS;
int32 CFE_FS_Decompress( char * srcFileName, char * tgtFileName )
{
    return DecompressResult;
}

char FillBuffer[1024];






/*
** Entry point function for tests in fm_utest_app.c
*/
void Test_app(void);

/*
** Entry point function for tests in fm_utest_cmds.c
*/
void Test_cmds(void);

/*
** Entry point function for tests in fm_utest_utils.c
*/
void Test_utils(void);

/*
** Entry point function for tests in fm_utest_child.c
*/
void Test_child(void);

/*
** Entry point function for tests in fm_utest_tbl.c
*/
void Test_tbl(void);


/************************************************************************
** Local data
*************************************************************************/

int OpenFileHandle = 0;

/*
** Global variables used by function hooks
*/
int32 CFE_SB_SubscribeHook(CFE_SB_MsgId_t  MsgId, CFE_SB_PipeId_t PipeId);
int32 SubscribeHook = 0;

uint32   UT_TotalTestCount = 0;
uint32   UT_TotalFailCount = 0;

uint16   UT_CommandPkt[1024];

FM_HousekeepingCmd_t     *UT_HousekeepingCmd =  (FM_HousekeepingCmd_t *) &UT_CommandPkt[0];

FM_NoopCmd_t             *UT_NoopCmd          =  (FM_NoopCmd_t *)          &UT_CommandPkt[0];
FM_ResetCmd_t            *UT_ResetCmd         =  (FM_ResetCmd_t *)         &UT_CommandPkt[0];
FM_CopyFileCmd_t         *UT_CopyFileCmd      =  (FM_CopyFileCmd_t *)      &UT_CommandPkt[0];
FM_MoveFileCmd_t         *UT_MoveFileCmd      =  (FM_MoveFileCmd_t *)      &UT_CommandPkt[0];
FM_RenameFileCmd_t       *UT_RenameFileCmd    =  (FM_RenameFileCmd_t *)    &UT_CommandPkt[0];
FM_DeleteFileCmd_t       *UT_DeleteFileCmd    =  (FM_DeleteFileCmd_t *)    &UT_CommandPkt[0];
FM_DeleteAllCmd_t        *UT_DeleteAllCmd     =  (FM_DeleteAllCmd_t *)     &UT_CommandPkt[0];
FM_DecompressCmd_t       *UT_DecompressCmd    =  (FM_DecompressCmd_t *)    &UT_CommandPkt[0];
FM_ConcatCmd_t           *UT_ConcatCmd        =  (FM_ConcatCmd_t *)        &UT_CommandPkt[0];
FM_GetFileInfoCmd_t      *UT_GetFileInfoCmd   =  (FM_GetFileInfoCmd_t *)   &UT_CommandPkt[0];
FM_GetOpenFilesCmd_t     *UT_GetOpenFilesCmd  =  (FM_GetOpenFilesCmd_t *)  &UT_CommandPkt[0];
FM_CreateDirCmd_t        *UT_CreateDirCmd     =  (FM_CreateDirCmd_t *)     &UT_CommandPkt[0];
FM_DeleteDirCmd_t        *UT_DeleteDirCmd     =  (FM_DeleteDirCmd_t *)     &UT_CommandPkt[0];
FM_GetDirFileCmd_t       *UT_GetDirFileCmd    =  (FM_GetDirFileCmd_t *)    &UT_CommandPkt[0];
FM_GetDirPktCmd_t        *UT_GetDirPktCmd     =  (FM_GetDirPktCmd_t *)     &UT_CommandPkt[0];
FM_GetFreeSpaceCmd_t     *UT_GetFreeSpaceCmd  =  (FM_GetFreeSpaceCmd_t *)  &UT_CommandPkt[0];
FM_SetTableStateCmd_t    *UT_SetTableStateCmd =  (FM_SetTableStateCmd_t *) &UT_CommandPkt[0];


/************************************************************************
** Local function prototypes
*************************************************************************/

void PrintHKPacket (uint8 source, void *packet);
void PrintInfoPacket (uint8 source, void *packet);
void PrintListPacket (uint8 source, void *packet);
void PrintOpenPacket (uint8 source, void *packet);
void PrintFreePacket (uint8 source, void *packet);

/*
** Prototypes for function hooks
*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* DS unit test program main                                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int main(void)
{

    /*
    ** API function hook handlers
    */
    UTF_SB_set_function_hook(CFE_SB_SUBSCRIBE_HOOK, (void *)&CFE_SB_SubscribeHook);


    /*
    ** Set up output file and HK packet handler
    */
    UTF_set_output_filename("fm_utest.out");
    UTF_set_packet_handler(FM_HK_TLM_MID, (utf_packet_handler)PrintHKPacket);
    UTF_set_packet_handler(FM_DIR_LIST_TLM_MID, (utf_packet_handler)PrintListPacket);
    UTF_set_packet_handler(FM_FILE_INFO_TLM_MID, (utf_packet_handler)PrintInfoPacket);
    UTF_set_packet_handler(FM_OPEN_FILES_TLM_MID, (utf_packet_handler)PrintOpenPacket);
    UTF_set_packet_handler(FM_FREE_SPACE_TLM_MID, (utf_packet_handler)PrintFreePacket);

    /*
    ** Initialize time data structures
    */
    UTF_init_sim_time(0.0);

    /*
    ** Initialize ES application data
    */
    UTF_ES_InitAppRecords();
    UTF_ES_AddAppRecord("FM",0);
    CFE_ES_RegisterApp();

    /*
    ** Initialize CDS and table services data structures
    */
    CFE_ES_CDS_EarlyInit();
    CFE_TBL_EarlyInit();

    CFE_EVS_Register(NULL, 0, 0);
    /*
     * Setup the virtual/physical file system mapping...
     *
     * The following local machine directory structure is required:
     *
     * ... fm/fsw/unit_test      <-- this is the current working directory
     * ... fm/fsw/unit_test/ram  <-- physical location for virtual disk "/ram"
     */
    UTF_add_volume("/", "ram", FS_BASED, FALSE, FALSE, TRUE, "RAM", "/ram", 0);

    OS_mkdir("/ram/sub",0);
    OS_mkdir("/ram/sub2",0);
    OS_mkdir("/ram/sub2/sub22",0);
    OS_mkdir("/ram/sub3",0);

    /*
    ** Run FM application unit tests
    */
    UT_TotalTestCount = 0;
    UT_TotalFailCount = 0;

    UTF_put_text("\n*** FM -- Testing fm_app.c ***\n");
    Test_app();

    UTF_put_text("\n*** FM -- Testing fm_cmds.c ***\n");
    Test_cmds();

    UTF_put_text("\n*** FM -- Testing fm_cmd_utils.c ***\n");
    Test_utils();

    UTF_put_text("\n*** FM -- Testing fm_child.c ***\n");
    Test_child();

    UTF_put_text("\n*** FM -- Testing fm_tbl.c ***\n");
    Test_tbl();

    UTF_put_text("\n*** FM -- Total test count = %d, total test errors = %d\n\n", UT_TotalTestCount, UT_TotalFailCount);

    /*
    ** Remove directories created for these tests...
    */
    OS_rmdir("/ram/sub3");
    OS_rmdir("/ram/sub2/sub22");
    OS_rmdir("/ram/sub2");
    OS_rmdir("/ram/sub");

    /*
    ** Invoke the main loop test now because the program will end
    **  when the last entry in the SB sim input file is read.
    */
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_RUNLOOP_PROC, TRUE);
    FM_AppMain();
    UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_RUNLOOP_PROC);

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
    FM_HousekeepingPkt_t *HousekeepingPkt = (FM_HousekeepingPkt_t *) packet;

    /* Output the FM housekeeping data */
    UTF_put_text("\nFM HOUSEKEEPING DATA:\n");
    UTF_put_text("CommandCounter = %d\n", HousekeepingPkt->CommandCounter);
    UTF_put_text("CommandErrCounter = %d\n", HousekeepingPkt->CommandErrCounter);
    UTF_put_text("NumOpenFiles = %d\n", HousekeepingPkt->NumOpenFiles);
    UTF_put_text("ChildCmdCounter = %d\n", HousekeepingPkt->ChildCmdCounter);
    UTF_put_text("ChildCmdErrCounter = %d\n", HousekeepingPkt->ChildCmdErrCounter);
    UTF_put_text("ChildCmdWarnCounter = %d\n", HousekeepingPkt->ChildCmdWarnCounter);
    UTF_put_text("ChildQueueCount = %d\n", HousekeepingPkt->ChildQueueCount);
    UTF_put_text("\n");

} /* End of PrintHKPacket() */



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Display the contents of a Directory List packet                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void PrintListPacket (uint8 source, void *packet)
{
    FM_DirListPkt_t *DirListPkt = (FM_DirListPkt_t *) packet;
    int i;

    /* Output the FM housekeeping data */
    UTF_put_text("\nFM DIRECTORY LIST PACKET:\n");
    UTF_put_text("DirName = %s\n", DirListPkt->DirName);
    UTF_put_text("TotalFiles = %d\n", DirListPkt->TotalFiles);
    UTF_put_text("PacketFiles = %d\n", DirListPkt->PacketFiles);
    UTF_put_text("FirstFile = %d\n", DirListPkt->FirstFile);
    for (i = 0; i < DirListPkt->PacketFiles; i++)
        UTF_put_text("File = %s, Size = %d, Time = %d\n", DirListPkt->FileList[i].EntryName,
                                                          DirListPkt->FileList[i].EntrySize,
                                                          DirListPkt->FileList[i].ModifyTime);
    UTF_put_text("\n");

} /* End of PrintListPacket() */




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Display the contents of a File Info Status packet               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void PrintInfoPacket (uint8 source, void *packet)
{
    FM_FileInfoPkt_t *FileInfoPkt = (FM_FileInfoPkt_t *) packet;
    char *ps;

    /* Output the FM housekeeping data */
    UTF_put_text("\nFM FILE INFO STATUS PACKET:\n");

    if (FileInfoPkt->FileStatus == FM_NAME_IS_INVALID) ps = "invalid name";
    if (FileInfoPkt->FileStatus == FM_NAME_IS_NOT_IN_USE) ps = "does not exist";
    if (FileInfoPkt->FileStatus == FM_NAME_IS_FILE_OPEN) ps = "open file";
    if (FileInfoPkt->FileStatus == FM_NAME_IS_FILE_CLOSED) ps = "closed file";
    if (FileInfoPkt->FileStatus == FM_NAME_IS_DIRECTORY) ps = "directory";
    UTF_put_text("FileStatus = %d, %s\n", FileInfoPkt->FileStatus, ps);
    UTF_put_text("CRC_Computed = %d\n", FileInfoPkt->CRC_Computed);
    UTF_put_text("CRC = 0x%08X\n", FileInfoPkt->CRC);
    UTF_put_text("FileSize = %d\n", FileInfoPkt->FileSize);
    UTF_put_text("LastModifiedTime = %d\n", FileInfoPkt->LastModifiedTime);
    UTF_put_text("Filename = %s\n", FileInfoPkt->Filename);
    UTF_put_text("\n");

} /* End of PrintInfoPacket() */



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Display the contents of a Open File List packet                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void PrintOpenPacket (uint8 source, void *packet)
{
    FM_OpenFilesPkt_t *OpenFilesPkt = (FM_OpenFilesPkt_t *) packet;
    int i;

    /* Output the FM open files data */
    UTF_put_text("\nFM OPEN FILE LIST PACKET:\n");
    UTF_put_text("NumOpenFiles = %d\n", OpenFilesPkt->NumOpenFiles);
    for (i = 0; i < OpenFilesPkt->NumOpenFiles; i++)
        UTF_put_text("App = %s, File = %s\n", OpenFilesPkt->OpenFilesList[i].AppName,
                                              OpenFilesPkt->OpenFilesList[i].LogicalName);
    UTF_put_text("\n");


} /* End of PrintOpenPacket() */



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Display the contents of a Free Space packet                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void PrintFreePacket (uint8 source, void *packet)
{
    FM_FreeSpacePkt_t *FreeSpacePkt = (FM_FreeSpacePkt_t *) packet;
    int i;

    /* Output the FM free space data */
    UTF_put_text("\nFM FREE SPACE PACKET:\n");
    for (i = 0; i < FM_TABLE_ENTRY_COUNT; i++)
      if (FreeSpacePkt->FileSys[i].Name[0] != '\0')
        UTF_put_text("Name = %s, SizeA = 0x%X, SizeB = 0x%X\n", FreeSpacePkt->FileSys[i].Name,
                     FreeSpacePkt->FileSys[i].FreeSpace_A, FreeSpacePkt->FileSys[i].FreeSpace_B);
    UTF_put_text("\n");


} /* End of PrintFreePacket() */



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Creates a file -                                                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CreateTestFile(char *Filename, int SizeInKs, boolean LeaveOpen)
{
    int FileHandle;
    int i;

    FileHandle = OS_creat(Filename, OS_READ_WRITE);
    if (FileHandle >= OS_SUCCESS)
    {
        if (FillBuffer[0] == 0)
        {
            CFE_PSP_MemSet(FillBuffer, 0xA5, sizeof(FillBuffer));
        }

        for (i = 0; i < SizeInKs; i++)
        {
            OS_write(FileHandle, FillBuffer, sizeof(FillBuffer));
        }

        if (LeaveOpen)
        {
            OpenFileHandle = FileHandle;
        }
        else
        {
            OS_close(FileHandle);
        }
    }
    else
    {
        UTF_put_text("\nERROR CREATING TEST FILE %s\n", Filename);
    }

    return;

} /* End of CreateTestFile() */


int32 CFE_SB_SubscribeHook(CFE_SB_MsgId_t  MsgId, CFE_SB_PipeId_t PipeId)
{
    SubscribeHook++;

    if ((SubscribeHook == 1) || (SubscribeHook == 3))
        return (-1);
    else
        return (CFE_SUCCESS);

}/* end CFE_SB_Subscribe */

