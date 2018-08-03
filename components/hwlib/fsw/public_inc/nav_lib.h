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

#ifndef _nav_lib_h_
#define _nav_lib_h_

/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"

/*************************************************************************
** Exported Functions
*************************************************************************/

/*
** Called when the Library Module is loaded - perform any initialization
** in this function that needs to occur for the gps or the lib.  This
** does NOT include opening/closing the UART/COM Port
*/
extern int32 NAV_LibInit(void);


/*
** Called by any cFS app that wants GPS data.  This function will
** return a pointer to a buffer of the last GPS data pulled off the UART
*/
extern void NAV_ReadAvailableData(uint8 DataBuffer[], int32 *DataLen);

#endif 


