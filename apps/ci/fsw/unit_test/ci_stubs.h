/*
 * File: ci_stubs.h
 *
 * Purpose:
 *   Provide stubs for unit testing CI
 *
 * History:
 *   Jan 15, 2015  dasp
 *    *
 */

#ifndef Ut_CI_STUBS_H
#define Ut_CI_STUBS_H

#include "uttools.h"

typedef enum
{
    UT_CI_CUSTOMINIT_INDEX,
    UT_CI_CUSTOMAPPCMDS_INDEX,
    UT_CI_MAX_INDEX
} Ut_CI_INDEX_t;

typedef struct
{
    int32   Value;
    uint32  Count;
} Ut_CI_ReturnCodeTable_t;


void Ut_CI_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt);
boolean Ut_CI_UseReturnCode(uint32 Index);


#endif
