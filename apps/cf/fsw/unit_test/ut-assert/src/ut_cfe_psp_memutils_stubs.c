/*
**
** File: ut_cfe_psp_memutils_stubs.c
**
** $Id: ut_cfe_psp_memutils_stubs.c 1.2 2015/03/06 14:37:06EST sstrege Exp  $
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
** Purpose: Unit test stubs for cFE PSP Memory Utilities routines
**
** $Log: ut_cfe_psp_memutils_stubs.c  $
** Revision 1.2 2015/03/06 14:37:06EST sstrege 
** Added copyright information
** Revision 1.1 2011/05/04 11:20:54EDT rmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cf/fsw/unit_test/ut-assert/src/project.pj
** Revision 1.1 2011/04/08 16:26:39EDT rmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cf/fsw/unit_test/ut-assert/src/project.pj
** Revision 1.1 2011/02/15 11:13:02EST sslegel 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/FSW-TOOLS-REPOSITORY/ut-assert/src/project.pj
**
*/

/*
**  Include Files
*/
#include "cfe.h"
#include <string.h>

int32 CFE_PSP_MemCpy(void *Dest, void *Src, uint32 Size)
{
    memcpy(Dest, Src, Size);
    return(CFE_PSP_SUCCESS);
}

int32 CFE_PSP_MemSet(void *Dest, uint8 Value, uint32 Size)
{
    memset(Dest, Value, Size);
    return(CFE_PSP_SUCCESS);
}
