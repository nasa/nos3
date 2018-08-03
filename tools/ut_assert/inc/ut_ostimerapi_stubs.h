/*
  Copyright (C) 2009 - 2016 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

  This software is provided "as is" without any warranty of any, kind either express, implied, or statutory, including, but not
  limited to, any warranty that the software will conform to, specifications any implied warranties of merchantability, fitness
  for a particular purpose, and freedom from infringement, and any warranty that the documentation will conform to the program, or
  any warranty that the software will be error free.

  In no event shall NASA be liable for any damages, including, but not limited to direct, indirect, special or consequential damages,
  arising out of, resulting from, or in any way connected with the software or its documentation.  Whether or not based upon warranty,
  contract, tort or otherwise, and whether or not loss was sustained from, or arose out of the results of, or use of, the software,
  documentation or services provided hereunder

  ITC Team
  NASA IV&V
  ivv-itc@lists.nasa.gov
*/

#ifndef UT_OSTIMERAPI_STUBS_H_
#define UT_OSTIMERAPI_STUBS_H_

typedef enum 
{
    UT_OSTIMERAPI_CREATE_INDEX,
    UT_OSTIMERAPI_SET_INDEX,
    UT_OSTIMERAPI_DELETE_INDEX,
    UT_OSTIMERAPI_GETINFO_INDEX,
    UT_OSTIMERAPI_MAX_INDEX
} Ut_OSTIMERAPI_INDEX_t;

typedef struct
{
    int32 (*OS_TimerCreate)(uint32*, const char*, uint32*, OS_TimerCallback_t);
    int32 (*OS_TimerSet)(uint32, uint32, uint32);
    int32 (*OS_TimerDelete)(uint32);
    int32 (*OS_TimerGetInfo)(uint32, OS_timer_prop_t*);
} Ut_OSTIMERAPI_HookTable_t;

typedef struct
{
    int32   Value;
    uint32  Count; 
    boolean ContinueReturnCodeAfterCountZero;
} Ut_OSTIMERAPI_ReturnCodeTable_t;

void Ut_OSTIMERAPI_Reset(void);
void Ut_OSTIMERAPI_SetFunctionHook(uint32 Index, void *FunPtr);
void Ut_OSTIMERAPI_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt);
void Ut_OSTIMERAPI_ContinueReturnCodeAfterCountZero(uint32 Index);

#endif 

