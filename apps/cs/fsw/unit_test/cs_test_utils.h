 /*************************************************************************
 ** File:
 **   $Id: cs_test_utils.h 1.2 2017/02/16 15:33:20EST mdeschu Exp  $
 **
 **   Copyright (c) 2007-2014 United States Government as represented by the 
 **   Administrator of the National Aeronautics and Space Administration. 
 **   All Other Rights Reserved.  
 **
 **   This software was created at NASA's Goddard Space Flight Center.
 **   This software is governed by the NASA Open Source Agreement and may be 
 **   used, distributed and modified only pursuant to the terms of that 
 **   agreement.
 **
 ** Purpose: 
 **   This file contains the function prototypes and global variables for the unit test utilities for the CS application.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **
 *************************************************************************/

/*
 * Includes
 */

#include "cs_app.h"
#include "ut_cfe_evs_hooks.h"
#include "ut_cfe_time_stubs.h"
#include "ut_cfe_psp_memutils_stubs.h"
#include "ut_cfe_tbl_stubs.h"
#include "ut_cfe_tbl_hooks.h"
#include "ut_cfe_fs_stubs.h"
#include "ut_cfe_time_stubs.h"
#include "ut_osapi_stubs.h"
#include "ut_osfileapi_stubs.h"
#include "ut_cfe_sb_stubs.h"
#include "ut_cfe_es_stubs.h"
#include "ut_cfe_evs_stubs.h"
#include <time.h>

/*
 * Function Definitions
 */

void CS_Test_Setup(void);
void CS_Test_TearDown(void);

/*
 * Additional UT-Assert Stub Functions and Required Data Structures
 *
 * Note: This code needs to be moved into the UT-Assert library.  We are including it here for now because the 
 * next release of the UT-Assert library is not expected to happen in the near future.
 */

typedef enum 
{
    UT_CFE_PSP_MEMORY_GETKERNELTEXTSEGMENTINFO_INDEX,
    UT_CFE_PSP_MEMORY_GETCFETEXTSEGMENTINFO_INDEX,
    UT_CFE_PSP_MEMORY_MAX_INDEX
} Ut_CFE_PSP_MEMORY_INDEX_t;

typedef struct
{
    int32 (*CFE_PSP_GetKernelTextSegmentInfo)(cpuaddr *PtrToKernelSegment, uint32 *SizeOfKernelSegment);
    int32 (*CFE_PSP_GetCFETextSegmentInfo)(cpuaddr *PtrToCFESegment, uint32 *SizeOfCFESegment);
} Ut_CFE_PSP_MEMORY_HookTable_t;

typedef struct
{
    int32   Value;
    uint32  Count;
    boolean ContinueReturnCodeAfterCountZero;
} Ut_CFE_PSP_MEMORY_ReturnCodeTable_t;

void Ut_CFE_PSP_MEMORY_SetFunctionHook(uint32 Index, void *FunPtr);
void Ut_CFE_PSP_MEMORY_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt);
void Ut_CFE_PSP_MEMORY_ContinueReturnCodeAfterCountZero(uint32 Index);

/************************/
/*  End of File Comment */
/************************/
