 /*************************************************************************
 ** File:
 **   $Id: example_app.c 1.1 2016/06/14 16:47:19EDT czogby Exp  $
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
 **   $Log: example_app.c  $
 **   Revision 1.1 2016/06/14 16:47:19EDT czogby 
 **   Initial revision
 **   Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/example/fsw/src/project.pj
 *************************************************************************/

#include "cfe.h"
#include "example_app.h"

Example_GlobalData_t   Example_GlobalData;

uint32 Example_Function1(void)
{
    if (Example_GlobalData.Variable1 == 1)
    {
        Example_GlobalData.Variable2 = 2;

        if (CFE_SB_Subscribe(0, 0) == 5)  /* Input values don't matter in this example */
        {
            return 25;
        }
    }
    else
    {
        Example_GlobalData.Variable2 = 99;
    }

    return 10;

} /* End Example_Function1 */

/************************/
/*  End of File Comment */
/************************/
