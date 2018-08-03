/*
**
** File: uttest.c
**
** $Id: uttest.c 1.1 2016/02/08 20:53:32EST rperera Exp  $
**
** Copyright 2012-2013 United States Government as represented by the 
** Administrator of the National Aeronautics and Space Administration. 
** All Other Rights Reserved.  
**
** This software was created at NASA's Goddard Space Flight Center.
** This software is governed by the NASA Open Source Agreement and may be 
** used, distributed and modified only pursuant to the terms of that 
** agreement.
**
** Purpose: This file contains functions to implement a standard way to execute unit tests.
**
** $Log: uttest.c  $
** Revision 1.1 2016/02/08 20:53:32EST rperera 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/src/project.pj
** Revision 1.4 2015/06/16 16:14:05EDT sstrege 
** Added copyright information
** Revision 1.3 2015/05/01 14:53:07EDT czogby 
** Changed "#if defined UT_VERBOSE" to "#if defined UT_VERBOSE || defined UT_VERBOSE_TEST_NAME"
** Revision 1.2 2015/03/10 15:21:44EDT czogby 
** Add Missing Functionality to UT Assert Library
** --- Added comments ---  czogby [2015/03/31 18:37:28Z]
** No updates were made to this source code in this revision
**
*/

/*
 * Includes
 */

#include "common_types.h"
#include "utassert.h"
#include "utlist.h"

/*
 * Type Definitions
 */

typedef struct {
    void    (*Test)(void);
    void    (*Setup)(void);
    void    (*Teardown)(void);
    const char     *TestName;
} UtTestDataBaseEntry_t;

/*
 * Local Data
 */

UtListHead_t    UtTestDataBase;
uint32          UtTestsExecutedCount = 0;

/*
 * Function Definitions
 */

void UtTest_Add(void (*Test)(void), void (*Setup)(void), void (*Teardown)(void), const char *TestName)
{
    UtTestDataBaseEntry_t   UtTestDataBaseEntry;

    UtTestDataBaseEntry.Test = Test;
    UtTestDataBaseEntry.Setup = Setup;
    UtTestDataBaseEntry.Teardown = Teardown;
    UtTestDataBaseEntry.TestName = TestName;
    UtList_Add(&UtTestDataBase, &UtTestDataBaseEntry, sizeof(UtTestDataBaseEntry_t), 0);
}

int UtTest_Run(void)
{
    uint32                   i;
    UtListNode_t            *UtListNode;
    UtTestDataBaseEntry_t   *UtTestDataBaseEntry;
    
    if (UtTestDataBase.NumberOfEntries > 0) {
        
        UtListNode = UtTestDataBase.First;
        for (i=0; i < UtTestDataBase.NumberOfEntries; i++) {
            
            UtTestDataBaseEntry = UtListNode->Data;

            #if defined UT_VERBOSE || defined UT_VERBOSE_TEST_NAME
            if (strlen(UtTestDataBaseEntry->TestName) > 0) { printf("\nRunning Test: %s\n", UtTestDataBaseEntry->TestName); }
            #endif

            if (UtTestDataBaseEntry->Setup)    { UtTestDataBaseEntry->Setup(); }
            if (UtTestDataBaseEntry->Test)     { UtTestDataBaseEntry->Test(); UtTestsExecutedCount++; }
            if (UtTestDataBaseEntry->Teardown) { UtTestDataBaseEntry->Teardown(); }

            UtListNode = UtListNode->Next;
        }
    }

    printf("\n");
    printf("Tests Executed:    %lu\n", UtTestsExecutedCount);
    printf("Assert Pass Count: %lu\n", UtAssert_GetPassCount());
    printf("Assert Fail Count: %lu\n", UtAssert_GetFailCount());
    
    UtList_Reset(&UtTestDataBase);

    return (UtAssert_GetFailCount() > 0);
}
