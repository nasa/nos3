/*************************************************************************
** File:
**   $Id: lc_utest_wdt.c 1.2 2015/03/04 16:09:50EST sstrege Exp  $
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
**   Limit Checker (LC) unit test watchpoint definition table (WDT)
**
** Notes:
**   This file provides a WDT for unit testing that simply sets all 
**   watchpoint entries to "not used". It is a modified version of
**   the lc_def_wdt.c file supplied with LC 
**
**   Compiler Note
**   -------------
**   This file may generate the following warning when compiling 
**   with gcc if you are using the flags "-ansi -pedantic":
** 
**   "warning: ISO C90 forbids specifying subobject to initialize"
**
**   Removing "-pedantic" should eliminate the warning. An alternate
**   solution is to replace "-ansi" with "-std=c99" and leave the
**   "-pedantic" flag in place
**
**   $Log: lc_utest_wdt.c  $
**   Revision 1.2 2015/03/04 16:09:50EST sstrege 
**   Added copyright information
**   Revision 1.1 2012/07/31 16:53:44EDT nschweis 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lcx/fsw/unit_test/project.pj
**   Revision 1.1 2009/01/15 15:25:15EST dahardis 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lc/fsw/unit_test/project.pj
** 
*************************************************************************/

/*************************************************************************
** Includes
*************************************************************************/
#include "lc_msg.h"
#include "lc_tbl.h"

/*************************************************************************
** Exported Data
*************************************************************************/
LC_WDTEntry_t LC_UnitTestWDT[LC_MAX_WATCHPOINTS] =
{
    /* #0 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #1 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #2 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #3 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #4 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #5 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #6 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #7 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #8 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #9 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #10 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #11 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #12 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #13 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #14 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #15 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #16 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #17 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #18 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #19 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #20 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #21 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #22 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #23 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #24 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #25 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #26 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #27 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #28 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #29 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #30 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #31 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #32 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #33 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #34 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #35 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #36 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #37 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #38 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #39 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #40 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #41 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #42 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #43 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #44 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #45 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #46 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #47 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #48 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #49 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #50 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #51 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #52 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #53 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #54 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #55 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #56 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #57 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #58 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #59 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #60 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #61 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #62 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #63 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #64 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #65 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #66 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #67 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #68 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #69 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #70 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #71 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #72 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #73 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #74 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #75 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #76 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #77 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #78 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #79 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #80 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #81 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #82 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #83 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #84 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #85 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #86 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #87 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #88 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #89 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #90 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #91 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #92 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #93 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #94 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #95 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #96 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #97 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #98 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #99 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #100 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #101 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #102 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #103 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #104 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #105 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #106 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #107 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #108 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #109 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #110 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #111 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #112 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #113 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #114 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #115 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #116 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #117 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #118 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #119 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #120 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #121 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #122 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #123 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #124 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #125 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #126 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #127 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #128 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #129 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #130 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #131 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #132 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #133 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #134 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #135 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #136 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #137 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #138 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #139 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #140 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #141 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #142 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #143 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #144 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #145 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #146 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #147 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #148 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #149 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #150 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #151 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #152 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #153 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #154 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #155 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #156 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #157 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #158 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #159 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #160 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #161 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #162 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #163 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #164 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #165 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #166 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #167 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #168 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #169 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #170 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #171 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #172 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #173 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #174 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #175 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = 0,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ComparisonValue.Unsigned32 = 0,
    }
}; /* end LC_UnitTestWDT */

/************************/
/*  End of File Comment */
/************************/
