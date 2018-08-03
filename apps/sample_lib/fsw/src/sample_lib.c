/*************************************************************************
** File:
**   $Id: sample_lib.c $
**
** Purpose: 
**   Sample CFS library
**
*************************************************************************/

/*************************************************************************
** Includes
*************************************************************************/
#include "sample_lib.h"
#include "sample_lib_version.h"

/*************************************************************************
** Macro Definitions
*************************************************************************/


/*************************************************************************
** Private Function Prototypes
*************************************************************************/
int32 SAMPLE_LibInit(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Library Initialization Routine                                  */
/* cFE requires that a library have an initialization routine      */ 
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 SAMPLE_LibInit(void)
{
    
    OS_printf ("SAMPLE Lib Initialized.  Version %d.%d.%d.%d",
                SAMPLE_LIB_MAJOR_VERSION,
                SAMPLE_LIB_MINOR_VERSION, 
                SAMPLE_LIB_REVISION, 
                SAMPLE_LIB_MISSION_REV);
                
    return CFE_SUCCESS;
 
}/* End SAMPLE_LibInit */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Sample Lib function                                             */ 
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 SAMPLE_Function( void ) 
{
   OS_printf ("SAMPLE_Function called\n");

   return(CFE_SUCCESS);
   
} /* End SAMPLE_Function */

/************************/
/*  End of File Comment */
/************************/
