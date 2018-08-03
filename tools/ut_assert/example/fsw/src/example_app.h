 /*************************************************************************
 ** File:
 **   $Id: example_app.h 1.1 2016/06/14 16:47:19EDT czogby Exp  $
 **
 ** Purpose: 
 **   The UT-Assert example application exists in order to be used by the UT-Assert example unit tests.
 **   The UT-Assert example unit tests exist in order to serve as an introduction for how to write a 
 **   unit test using UT-Assert
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: example_app.h  $
 **   Revision 1.1 2016/06/14 16:47:19EDT czogby 
 **   Initial revision
 **   Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/example/fsw/src/project.pj
 *************************************************************************/

#ifndef _example_app_h_
#define _example_app_h_

#include "cfe.h"

typedef struct
{
    uint32  Variable1;
    uint32  Variable2;
    uint32  Variable3;
    uint32  Variable4;

} Example_GlobalData_t;

extern Example_GlobalData_t   Example_GlobalData;

uint32 Example_Function1(void);

#endif /* _example_app_h_ */

/************************/
/*  End of File Comment */
/************************/
