/* Copyright (C) 2009 - 2015 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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



/*************************************************************************
** Includes
*************************************************************************/
#include "hwlib.h"
#include "hwlib_version.h"

#include <ctype.h>
#include <string.h>

/*************************************************************************
** Macro Definitions
*************************************************************************/


/*************************************************************************
** Private Function Prototypes
*************************************************************************/
//#include "network_includes.h"
#include "common_types.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include "osapi.h"

CFS_MODULE_DECLARE_LIB(hwlib);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HW Library Initialization Routine                         */
/* cFE requires that a library have an initialization routine      */ 
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 hwlib_Init(void)
{
    int32 status = OS_SUCCESS;

    /*
     ** Register the events
     */
    CFE_EVS_Register(NULL, 0, CFE_EVS_NO_FILTER);

    /*
    ** Init all hardware subsystems and interfaces
    ** order may be important.
    */
    /* HWLIB initialization event */
    CFE_EVS_SendEvent(HWLIB_INIT_EID, CFE_EVS_INFORMATION,
        "HWLIB Initialized. Version %d.%d.%d.%d",
        HW_LIB_MAJOR_VERSION,
        HW_LIB_MINOR_VERSION,
        HW_LIB_REVISION,
        HW_LIB_MISSION_REV);
    
    /* Initialize the CAM Lib  */
    status = CAM_LibInit();
    if (status == OS_SUCCESS)
    {
        CFE_EVS_SendEvent(HWLIB_INIT_EID, CFE_EVS_INFORMATION, "CAM Lib HW Init Success");
    }
    else
    {
        CFE_EVS_SendEvent(HWLIB_INIT_EID, CFE_EVS_ERROR, "CAM Lib HW Init ERROR = 0x%lx\n", status);
    } 

    /* Initialize the NAV Lib */
    status = NAV_LibInit();
    if (status == OS_SUCCESS)
    {
        CFE_EVS_SendEvent(HWLIB_INIT_EID, CFE_EVS_INFORMATION,"NAV Lib HW Init Success");
    }
    else
    {
        CFE_EVS_SendEvent(HWLIB_INIT_EID, CFE_EVS_ERROR, "NAV Lib HW Init ERROR = 0x%lx", status);
    }

    return OS_SUCCESS;
}