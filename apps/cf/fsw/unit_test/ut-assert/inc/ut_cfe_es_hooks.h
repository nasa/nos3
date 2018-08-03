/*
**
** File: ut_cfe_es_hooks.h
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
** $Id: ut_cfe_es_hooks.h 1.2 2015/03/06 14:34:28EST sstrege Exp  $
**
** Purpose: Unit test header file for cFE Executive Services hooks.
**
** $Log: ut_cfe_es_hooks.h  $
** Revision 1.2 2015/03/06 14:34:28EST sstrege 
** Added copyright information
** Revision 1.1 2011/05/04 11:20:17EDT rmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cf/fsw/unit_test/ut-assert/inc/project.pj
** Revision 1.1 2011/04/08 16:25:51EDT rmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cf/fsw/unit_test/ut-assert/inc/project.pj
** Revision 1.1 2011/03/07 17:54:46EST sslegel 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/FSW-TOOLS-REPOSITORY/ut-assert/inc/project.pj
**
*/

#ifndef UT_CFE_ES_HOOKS_H_
#define UT_CFE_ES_HOOKS_H_

#include "cfe.h"

int32 Ut_CFE_ES_RunLoopHook(uint32 *ExitStatus);

#endif
