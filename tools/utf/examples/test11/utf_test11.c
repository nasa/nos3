
/*
** File: utf_test11.c
**
** Purpose: This test makes calls to OS bsp support api.
**
** References:
**
** Assumptions and Notes:
**
** $Date: 2009/10/13 14:23:36GMT-05:00 $
** $Revision: 1.4 $
** $Log: utf_test11.c  $
** Revision 1.4 2009/10/13 14:23:36GMT-05:00 wmoleski 
** 
** Revision 1.3 2009/06/24 10:05:09EDT rmcgraw 
** DCR8290:4 OS_BSP to CFE_PSP_  API name changes
** Revision 1.2 2008/07/28 11:07:22EDT wfmoleski 
** Added code to allow the return code setting capability of the UTF to be used on the 2 new BSP functions.
** Also, testing of this capability was added to test11 of the UTF examples.
** Revision 1.1 2008/04/17 08:08:59EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/examples/test11/project.pj
** Revision 1.2 2006/08/01 08:37:19EDT wfmoleski 
** Changes made to upgrade this example to cFE 3.3.0
** Revision 1.1 2006/04/10 16:31:59EDT sslegel 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/examples/test11/project.pj
**
*/

#include "cfe.h"
#include "utf_custom.h"
#include "utf_types.h"
#include "utf_osapi.h"
#include "cfe_psp.h"
#include "utf_cfe_psp.h"

int main(void)
{

    /* Set up output file and initial time */
    UTF_set_output_filename("utf_test11.ActualOutput");
    UTF_init_sim_time(1.5);

    /* CFE_PSP_Restart Test */
    UTF_put_text("CFE_PSP_Restart - Currently does nothing\n");
    CFE_PSP_Restart(0);
    
    /* CFE_PSP_FlushCaches Test */
    UTF_put_text("CFE_PSP_FlushCaches - Currently does nothing\n");
    CFE_PSP_FlushCaches(1, 2, 3);
    
    /* CFE_PSP_GetProcessorId Test */
    UTF_put_text("CFE_PSP_GetProcessorId Default Return Value %x\n", CFE_PSP_GetProcessorId());

    UTF_put_text("Set the processor id to 0x24\n");
    UTF_set_processor_id(0x24);
    UTF_put_text("CFE_PSP_GetProcessorId New Return Value %x\n", CFE_PSP_GetProcessorId());

    /* CFE_PSP_GetSpacecraftId Test */
    UTF_put_text("CFE_PSP_GetSpacecraftId Default Return Value %x\n", CFE_PSP_GetSpacecraftId());

    UTF_put_text("Set the spacecraft id to 0x155\n");
    UTF_set_spacecraft_id(0x155);
    UTF_put_text("CFE_PSP_GetSpacecraftId New Return Value %x\n", CFE_PSP_GetSpacecraftId());

/* CFE_PSP_GetRestartType Test - Currently only returns CFE_ES_POWERON_RESET */
    uint32 reset_subtype;
    UTF_put_text("CFE_PSP_GetRestartType Return Value %d\n", CFE_PSP_GetRestartType(&reset_subtype));
    UTF_put_text("CFE_PSP_GetRestartType reset subtype %d\n", reset_subtype);

    /* Test the new Info functions */
    UTF_put_text("\nGet Segment Info API Tests\n");
    uint32 sizeValue;
    uint32 segmentPtr;
    UTF_put_text("CFE_PSP_GetKernelTextSegmentInfo Return Value %x\n",
		CFE_PSP_GetKernelTextSegmentInfo(&segmentPtr,&sizeValue));
    UTF_put_text("-- SegmentPtr = %x; Size = %d\n",segmentPtr, sizeValue);
    UTF_put_text("CFE_PSP_GetCFETextSegmentInfo Return Value %x\n",
		CFE_PSP_GetCFETextSegmentInfo(&segmentPtr,&sizeValue));
    UTF_put_text("-- SegmentPtr = %x; Size = %d\n",segmentPtr, sizeValue);
	
    /* Test the return code setting of the above functions */
    UTF_put_text("CFE_PSP_GetKernelTextSegmentInfo Return Code Setting Test\n");
    UTF_PSP_Set_Api_Return_Code(CFE_PSP_GETKERNELTEXTSEGMENTINFO_PROC,OS_ERROR);
    UTF_put_text("CFE_PSP_GetKernelTextSegmentInfo Return Code %x, expected %x\n",
    CFE_PSP_GetKernelTextSegmentInfo(&segmentPtr,&sizeValue),OS_ERROR);
    UTF_PSP_Use_Default_Api_Return_Code(CFE_PSP_GETKERNELTEXTSEGMENTINFO_PROC);
    UTF_put_text("CFE_PSP_GetKernelTextSegmentInfo Return Code %x, expected %x\n",
    CFE_PSP_GetKernelTextSegmentInfo(&segmentPtr,&sizeValue),OS_SUCCESS);

    UTF_put_text("CFE_PSP_GetCFETextSegmentInfo Return Code Setting Test\n");
    UTF_PSP_Set_Api_Return_Code(CFE_PSP_GETCFETEXTSEGMENTINFO_PROC,OS_ERROR);
    UTF_put_text("CFE_PSP_GetCFETextSegmentInfo Return Code %x, expected %x\n",
    CFE_PSP_GetCFETextSegmentInfo(&segmentPtr,&sizeValue),OS_ERROR);
    UTF_PSP_Use_Default_Api_Return_Code(CFE_PSP_GETCFETEXTSEGMENTINFO_PROC);
    UTF_put_text("CFE_PSP_GetCFETextSegmentInfo Return Code %x, expected %x\n",
    CFE_PSP_GetCFETextSegmentInfo(&segmentPtr,&sizeValue),OS_SUCCESS);

    exit(0);
}
