/*
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
** File:
** $Id: ut_bsp_stubs.c 1.9 2014/05/28 09:21:51GMT-05:00 wmoleski Exp  $
**
** Purpose:
** Unit test stubs for BSP routines
** 
** Notes:
** Minimal work is done, only what is required for unit testing
**
** $Data:$
** $Revision: 1.9 $
** $Log: ut_bsp_stubs.c  $
** Revision 1.9 2014/05/28 09:21:51GMT-05:00 wmoleski 
** Overwriting cFE Unit Test files with the updated JSC files.
** Revision 1.8 2012/01/13 13:59:29EST acudmore 
** Added license text
** Revision 1.7 2011/11/30 15:44:39EST jmdagost 
** Added CFE_PSP_MemValidateRange stub function.
** Revision 1.6 2010/11/23 13:17:48EST jmdagost 
** Added CFE_PSP_MemRead8() stub.
** Revision 1.5 2010/09/09 15:02:57EDT jmdagost 
** Updated funciton CFE_PSP_GetCFETextSegmentInfo() to avoid segment fault in CentOS VM environment.
** Also made dummy CFE segment CfeData a non-zero value.
** Revision 1.4 2009/06/25 14:14:47EDT rmcgraw 
** DCR8290:5 Removed #include os_bsp.h
** Revision 1.3 2009/06/25 12:46:02EDT rmcgraw 
** DCR8290:5 cFE API changes to unit test files
** Revision 1.2 2008/08/15 11:27:02EDT njyanchik 
** Check in of ES Unit Test
** Revision 1.1 2008/04/17 13:05:44BST ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/unit-test/project.pj
** Revision 1.15 2007/09/26 15:00:47EDT njyanchik 
** This is the first update of the ES unit tests for 5.0
** Revision 1.14 2007/05/30 15:15:18EDT njyanchik 
** check in of ES Unit test
** Revision 1.13 2007/05/30 08:56:41EDT njyanchik 
** Update
** Revision 1.12 2007/05/24 15:48:08EDT njyanchik 
** Update for ES Unit Test
** Revision 1.11 2007/05/22 13:03:57EDT njyanchik 
** Updated changes to not lose my work
** Revision 1.10 2007/05/16 11:14:36EDT njyanchik 
** Update ES's unit test driver to match code for build 4.1
** Revision 1.9 2007/05/04 09:10:32EDT njyanchik 
** Check in of Time UT and related changes
** Revision 1.8 2007/05/01 13:28:24EDT njyanchik 
** I updated the ut stubs to get the each of the subsytems to compile under the unit test. I did not
** change the unit tests themselves to cover more of the files, however.
** Revision 1.7 2007/02/27 15:17:10EST njyanchik 
** There was some code in ut_bsp_stubs.c that would cause linking errors on all OS's but linux.
** This has been resolved
** Revision 1.6 2007/01/17 10:04:20EST njyanchik 
** This CP Checks in cleaned-up versions of the files checked in for the ES unit test
** Revision 1.5 2007/01/17 09:26:08GMT-05:00 njyanchik 
** Check in of ES Unit Test changed files
** Revision 1.4 2006/11/06 15:10:06GMT-05:00 apcudmore 
** Removed OS_BSPGetRestartType. The routine is obsolete.
** Revision 1.3 2006/11/02 13:53:57EST njyanchik 
** Unit test for TIME was updated to match the changes made for this DCR
** Revision 1.2 2006/11/01 12:46:53GMT-05:00 njyanchik 
** Changed the Unit test to reflect the changes from removing the CDS functionality from TIME
** Revision 1.1 2006/03/02 15:10:24GMT-05:00 jjhageman 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/fsw/cfe-core/unit-test/project.pj
*/

/*
** Includes
*/
#include <string.h>
#include "cfe.h"
#include "osapi.h"
#include "common_types.h"
#include "ut_stubs.h"

/*
** Macro definitions
*/
#define CFE_PSP_CDS_SIZE CFE_ES_CDS_SIZE

/*
** Global variables
*/
uint8 *CFE_PSP_CDSPtr = 0;
uint8 *CFE_PSP_ResetAreaPtr = 0;
uint8 *CFE_PSP_UserReservedAreaPtr = 0;
int   CfeData = 1234;

/*
** External global variables
*/
extern uint32 UT_RestartType;
extern int32  UT_StatusBSP;
extern int    UT_SizeofESResetArea;
extern int    UT_BSP_Fail;
extern int    UT_CDS_Size;
extern char   cMsg[];

extern CFE_ES_ResetData_t UT_CFE_ES_ResetData;
extern CFE_ES_ResetData_t *UT_CFE_ES_ResetDataPtr;

extern UT_SetRtn_t BSPWriteCDSRtn;
extern UT_SetRtn_t BSPReadCDSRtn;
extern UT_SetRtn_t BSPUnloadAppFileRtn;
extern UT_SetRtn_t PSPRestartRtn;
extern UT_SetRtn_t PSPMemValRangeRtn;
extern UT_SetRtn_t BSPGetCFETextRtn;
extern UT_SetRtn_t PSPPanicRtn;

extern boolean UT_CDS_GoodEnd;
extern boolean UT_BSPCheckValidity;
extern boolean UT_CDSReadBlock;
extern boolean UT_CDS_Rebuild;

extern OS_time_t BSP_Time;

/*
** Function prototypes
*/
int32 dummy_function(void);

#ifdef _ENHANCED_BUILD_

#include <target_config.h>

Target_PspConfigData GLOBAL_PSP_CONFIGDATA = { 0 };
Target_CfeConfigData GLOBAL_CFE_CONFIGDATA = { 0 };

/**
 * Instantiation of global system-wide configuration struct
 * This contains build info plus pointers to the PSP and CFE
 * configuration structures.  Everything will be linked together
 * in the final executable.
 */
Target_ConfigData GLOBAL_CONFIGDATA =
{
        .MissionVersion = "MissionUnitTest",
        .CfeVersion = "CfeUnitTest",
        .OsalVersion = "OsalUnitTest",
        .Config = "MissionConfig",
        .Date = "MissionBuildDate",
        .User = "MissionBuildUser",
        .Default_CpuName = "UnitTestCpu",
        .Default_CpuId = 1,
        .Default_SpacecraftId = 42,
        .CfeConfig = &GLOBAL_CFE_CONFIGDATA,
        .PspConfig = &GLOBAL_PSP_CONFIGDATA
};


#endif
/*
** Functions
*/

/*****************************************************************************/
/**
** \brief CFE_PSP_Panic stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        CFE_PSP_Panic.  The variable PSPPanicRtn.value is set equal to the
**        input variable ErrorCode and the variable PSPPanicRtn.count is
**        incremented each time this function is called.  The unit tests
**        compare these values to expected results to verify proper system
**        response.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_PSP_Panic(int32 ErrorCode)
{
#ifdef UT_VERBOSE
    snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
             "  CFE_PSP_Panic called: EC = 0x%lx", (uint32) ErrorCode);
    UT_Text(cMsg);
#endif
    PSPPanicRtn.value = ErrorCode;
    PSPPanicRtn.count++;
}

/*****************************************************************************/
/**
** \brief CFE_PSP_GetProcessorId stub function
**
** \par Description
**        This function is used as a placeholder for the PSP function
**        CFE_PSP_GetProcessorId.  It is set to return a fixed value for the
**        unit tests.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns 0.
**
******************************************************************************/
uint32 CFE_PSP_GetProcessorId(void)
{
    return 0;
}

/*****************************************************************************/
/**
** \brief CFE_PSP_GetSpacecraftId stub function
**
** \par Description
**        This function is used as a placeholder for the PSP function
**        CFE_PSP_GetSpacecraftId.  It is set to return a fixed value for the
**        unit tests.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns 0.
**
******************************************************************************/
uint32 CFE_PSP_GetSpacecraftId(void)
{
    return 0;
}

/*****************************************************************************/
/**
** \brief CFE_PSP_GetTime stub function
**
** \par Description
**        This function is used as a placeholder for the PSP function
**        CFE_PSP_GetTime.  The LocalTime structure is set to the user-defined
**        values in BSP_Time.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_PSP_GetTime(OS_time_t *LocalTime)
{
    LocalTime->seconds = BSP_Time.seconds;
    LocalTime-> microsecs = BSP_Time.microsecs;
}

/*****************************************************************************/
/**
** \brief CFE_PSP_WriteToCDS stub function
**
** \par Description
**        This function is used to mimic the response of the PSP function
**        CFE_PSP_WriteToCDS.  The user can adjust the response by setting
**        the values in the BSPWriteCDSRtn structure prior to this function
**        being called.  If the value BSPWriteCDSRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value BSPWriteCDSRtn.value.
**        Otherwise, the value of the user-defined variable UT_BSP_Fail
**        determines the status returned by the function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_SUCCESS, OS_ERROR, or a user-defined value.
**
******************************************************************************/
int32 CFE_PSP_WriteToCDS(void *PtrToDataToWrite,
                         uint32 CDSOffset,
                         uint32 NumBytes)
{
    int32   status = OS_SUCCESS;
    boolean flag = FALSE;

    if (BSPWriteCDSRtn.count > 0)
    {
        BSPWriteCDSRtn.count--;

        if (BSPWriteCDSRtn.count == 0)
        {
            status = BSPWriteCDSRtn.value;
            flag = TRUE;
        }
    }
      
    if (flag == FALSE)
    {
        if (UT_BSP_Fail & BSP_WRITECDS_FAIL)
        {
            status = OS_ERROR;
#ifdef UT_VERBOSE
            UT_Text("  CFE_PSP_WriteToCDS called: (FAILURE)");
#endif
        }
#ifdef UT_VERBOSE
        else
        {
            UT_Text("  CFE_PSP_WriteToCDS called: (SUCCESS)");
        }
#endif
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_PSP_ReadFromCDS stub function
**
** \par Description
**        This function is used to mimic the response of the PSP function
**        CFE_PSP_ReadFromCDS.  The user can adjust the response by setting
**        the values in the BSPReadCDSRtn structure prior to this function
**        being called.  If the value BSPReadCDSRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value BSPReadCDSRtn.value.
**        Otherwise, the value of the user-defined variable UT_BSP_Fail
**        determines the status returned by the function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_SUCCESS, OS_ERROR, or a user-defined value.
**
******************************************************************************/
int32 CFE_PSP_ReadFromCDS(void *PtrToDataToRead,
                          uint32 CDSOffset,
                          uint32 NumBytes)
{
    int32   status = OS_SUCCESS;
    boolean flag = FALSE;

    if (BSPReadCDSRtn.count > 0)
    {
        BSPReadCDSRtn.count--;

        if (BSPReadCDSRtn.count == 0)
        {
            if (UT_BSPCheckValidity == TRUE)
            {
                memcpy((char *) PtrToDataToRead, "_CDSBeg_", NumBytes);
            }

            status = BSPReadCDSRtn.value;
            flag = TRUE;
        }
    }    
  
    if (flag == FALSE)
    {
        if (UT_BSP_Fail & BSP_READCDS_FAIL)
        {
            status = OS_ERROR;
#ifdef UT_VERBOSE
            UT_Text("  CFE_PSP_ReadFromCDS called: (FAILURE)");
#endif
        }
        else if (UT_CDS_Rebuild)
        {
            if (UT_BSPCheckValidity == TRUE)
            {
                memcpy((char *) PtrToDataToRead, "_CDSBeg_", NumBytes);
                UT_BSPCheckValidity = FALSE;
                BSPReadCDSRtn.value = 1234;

            }
            else
            {
                memcpy(PtrToDataToRead, "_CDSEnd_", NumBytes);
                UT_SetCDSRebuild(FALSE);
            }

#ifdef UT_VERBOSE
            UT_Text("  CFE_PSP_ReadFromCDS called: (REBUILD)");
#endif
        }
        else
        {
            if (UT_CDS_GoodEnd == TRUE)
            {
                if (UT_BSPCheckValidity == TRUE)
                {
                    memcpy(PtrToDataToRead, "_CDSEnd_", NumBytes);
                }
            }
            else
            {
                if (UT_BSPCheckValidity == TRUE)
                {
                    memcpy(PtrToDataToRead, "gibberish", NumBytes);
                }
            }

#ifdef UT_VERBOSE
            UT_Text("  CFE_PSP_ReadFromCDS called: (SUCCESS)");
#endif
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_PSP_GetCDSSize stub function
**
** \par Description
**        This function is used to mimic the response of the PSP function
**        CFE_PSP_GetCDSSize.  The user can adjust the response by
**        setting the value of UT_BSP_Fail prior to this function being called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_SUCCESS or OS_ERROR.
**
******************************************************************************/
int32 CFE_PSP_GetCDSSize(uint32 *SizeOfCDS)
{
    int32 status = OS_SUCCESS;

    if (UT_BSP_Fail & BSP_GETCDSSIZE_FAIL)
    {
        status = OS_ERROR;
#ifdef UT_VERBOSE
        UT_Text("  CFE_PSP_GetCDSSize called: (FAILURE)");
#endif
    } 
#ifdef UT_VERBOSE
    else 
    {
        UT_Text("  CFE_PSP_GEtCDSSize called: (SUCCESS)");
    }
#endif

    *SizeOfCDS = UT_CDS_Size;
    return status;   
}

/*****************************************************************************/
/**
** \brief CFE_PSP_GetVolatileDiskMem stub function
**
** \par Description
**        This function is used to mimic the response of the PSP function
**        CFE_PSP_GetVolatileDiskMem.  The user can adjust the response by
**        setting the value of UT_BSP_Fail prior to this function being called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_SUCCESS or OS_ERROR.
**
******************************************************************************/
int32 CFE_PSP_GetVolatileDiskMem(cpuaddr *PtrToVolDisk, uint32 *SizeOfVolDisk)
{
    int32 status = OS_SUCCESS;

    if (UT_BSP_Fail & BSP_GETVOLDISKMEM_FAIL)
    {
        status = OS_ERROR;
#ifdef UT_VERBOSE
        UT_Text("  CFE_PSP_GetVolatileDiskMem called: (FAILURE)");
#endif
    } 
    else if (SizeOfVolDisk == NULL)
    {
        status = OS_ERROR;
    }
    else
    {
        *PtrToVolDisk = 0;
        *SizeOfVolDisk = 0;
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_PSP_Restart stub function
**
** \par Description
**        This function is used as a placeholder for the PSP function
**        CFE_PSP_Restart.  The variable PSPRestartRtn.value is set to the
**        value passed to the function, reset_type, and the variable
**        PSPRestartRtn.count is incremented each time this function is called.
**        The unit tests compare these values to expected results to verify
**        proper system response.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_PSP_Restart(uint32 reset_type)
{
#ifdef UT_VERBOSE
    snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
             "  CFE_PSP__Restart called: reset_type = %lu", reset_type);
    UT_Text(cMsg);
#endif
    PSPRestartRtn.value = reset_type;
    PSPRestartRtn.count++;
}

/*****************************************************************************/
/**
** \brief CFE_PSP_Get_Timebase stub function
**
** \par Description
**        This function is used as a placeholder for the PSP function
**        CFE_PSP_Get_Timebase.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_PSP_Get_Timebase(uint32 *Tbu, uint32* Tbl)
{
#ifdef UT_VERBOSE
    UT_Text("  CFE_PSP_Get_Timebase called");
#endif
}

/*****************************************************************************/
/**
** \brief CFE_PSP_GetResetArea stub function
**
** \par Description
**        This function is used as a placeholder for the PSP function
**        CFE_PSP_GetResetArea.  It returns the user-defined value,
**        UT_StatusBSP.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns a user-defined status value, UT_StatusBSP.
**
******************************************************************************/
int32 CFE_PSP_GetResetArea(cpuaddr *PtrToResetArea, uint32 *SizeOfResetArea)
{
    UT_CFE_ES_ResetDataPtr = &UT_CFE_ES_ResetData;
    *PtrToResetArea = (cpuaddr)UT_CFE_ES_ResetDataPtr;
    *SizeOfResetArea = UT_SizeofESResetArea;
    return UT_StatusBSP;
}

/*****************************************************************************/
/**
** \brief CFE_PSP_AttachExceptions stub function
**
** \par Description
**        This function is used as a placeholder for the PSP function
**        CFE_PSP_AttachExceptions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void  CFE_PSP_AttachExceptions(void)
{
}

/*****************************************************************************/
/**
** \brief CFE_PSP_SetDefaultExceptionEnvironment stub function
**
** \par Description
**        This function is used as a placeholder for the PSP function
**        CFE_PSP_SetDefaultExceptionEnvironment.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_PSP_SetDefaultExceptionEnvironment(void)
{
}

/*****************************************************************************/
/**
** \brief CFE_PSP_GetTimerTicksPerSecond stub function
**
** \par Description
**        This function is used as a placeholder for the PSP function
**        CFE_PSP_GetTimerTicksPerSecond.  It is set to return a fixed value
**        for the unit tests.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns 2000.
**
******************************************************************************/
uint32 CFE_PSP_GetTimerTicksPerSecond(void)
{
    return(2000);
}

/*****************************************************************************/
/**
** \brief CFE_PSP_GetTimerLow32Rollover stub function
**
** \par Description
**        This function is used as a placeholder for the PSP function
**        CFE_PSP_GetTimerLow32Rollover.  It is set to return a fixed value
**        for the unit tests.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns 1000000.
**
******************************************************************************/
uint32 CFE_PSP_GetTimerLow32Rollover(void)
{
    return(100000);
}

/*****************************************************************************/
/**
** \brief CFE_PSP_GetCFETextSegmentInfo stub function
**
** \par Description
**        This function is used to mimic the response of the PSP function
**        CFE_PSP_GetCFETextSegmentInfo.  The user can adjust the response by
**        setting the values in the BSPGetCFETextRtn structure prior to this
**        function being called.  If the value BSPGetCFETextRtn.count is
**        greater than zero then the counter is decremented; if it then equals
**        zero the return value is set to the user-defined value
**        BSPGetCFETextRtn.value.  OS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_SUCCESS.
**
******************************************************************************/
int32 CFE_PSP_GetCFETextSegmentInfo(cpuaddr *PtrToCFESegment,
                                    uint32 *SizeOfCFESegment)
{
    int32   status = OS_SUCCESS;
    boolean flag = FALSE;

    if (BSPGetCFETextRtn.count > 0)
    {
        BSPGetCFETextRtn.count--;

        if (BSPGetCFETextRtn.count == 0)
        {
            status = BSPGetCFETextRtn.value;
            flag = TRUE;
        }
    }   
    
    if (flag == FALSE)
    {
        /* Set the pointer and size to anything */
	    *PtrToCFESegment = (cpuaddr)&CfeData;
        *SizeOfCFESegment = sizeof(CfeData);
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_PSP_MemRead8 stub function
**
** \par Description
**        This function is used as a placeholder for the PSP function
**        CFE_PSP_MemRead8.  It sets the Data variable to a fixed value and
**        always returns OS_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_SUCCESS.
**
******************************************************************************/
int32 CFE_PSP_MemRead8(cpuaddr Address, uint8 *Data)
{
    *Data = 0x01;
    return UT_BSP_Fail;
}

/*****************************************************************************/
/**
** \brief CFE_PSP_MemValidateRange stub function
**
** \par Description
**        This function is used to mimic the response of the PSP function
**        CFE_PSP_MemValidateRange.  The user can adjust the response by
**        setting the values in the PSPMemValRangeRtn structure prior to this
**        function being called.  If the value PSPMemValRangeRtn.count is
**        greater than zero then the counter is decremented; if it then equals
**        zero the return value is set to the user-defined value
**        PSPMemValRangeRtn.value.  OS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_SUCCESS.
**
******************************************************************************/
int32 CFE_PSP_MemValidateRange(cpuaddr Address, uint32 Size, uint32 MemoryType)
{
    int32 status = OS_SUCCESS;

    if (PSPMemValRangeRtn.count > 0)
    {
        PSPMemValRangeRtn.count--;

        if (PSPMemValRangeRtn.count == 0)
        {
            status = PSPMemValRangeRtn.value;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_PSP_MemCpy stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        CFE_PSP_MemCpy.  It always returns OS_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_SUCCESS.
**
******************************************************************************/
int32 CFE_PSP_MemCpy(void *dst, void *src, uint32 size)
{
    memcpy(dst, src, size);
    return OS_SUCCESS;
}

/*****************************************************************************/
/**
** \brief CFE_PSP_MemSet stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        CFE_PSP_MemSet.  It always returns OS_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_SUCCESS.
**
******************************************************************************/
int32 CFE_PSP_MemSet(void *dst, uint8 value , uint32 size)
{
    memset(dst, (int)value, (size_t)size);
    return OS_SUCCESS;
}

