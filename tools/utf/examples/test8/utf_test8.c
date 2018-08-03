/*
**
**  Filename: utf_test8.c
**  $Id: utf_test8.c 1.1 2008/04/17 07:09:26GMT-05:00 ruperera Exp  $
**
**  Purpose:
**	 Example of using time hook utility.
**
**  References:
**     User's Guide for the core Flight Executive (cFE)
**     	Unit Test Framework (UTF)-- Release 3.2.0
**
**	Notes:
**
**  $Date: 2008/04/17 07:09:26GMT-05:00 $
**  $Revision: 1.1 $
**  $Log: utf_test8.c  $
**  Revision 1.1 2008/04/17 07:09:26GMT-05:00 ruperera 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/examples/test8/project.pj
**  Revision 1.5 2006/08/01 08:53:02EDT wfmoleski 
**  Changes to upgrade this example to cFE 3.3.0
**  Revision 1.4 2006/06/14 16:13:26EDT nsschweiss 
**  Included tests for UTF_double_to_hwtime microsecond fix.
**  Revision 1.3 2006/06/08 16:14:24EDT nsschweiss 
**  Replaced previous file with Time Hook scenario.
*/

#include "utf_cfe.h"

void time_hook(OS_time_t *time_struct);

int main(void)
{
	OS_time_t OS_time;
	double double_time[10]={0.57, 0.58, 
						256.51, 256.52, 256.53, 256.57, 256.58, 
						0.009999,0.009996, 0.009994};
	int i;

	/********************************/
	/* Set up output file           */
	/********************************/
	UTF_set_output_filename("utf_test8.ActualOutput");
	
	UTF_put_text("** Test time hook capability **\n");
	UTF_put_text("** Set sim time to 500. **\n");
	
    UTF_set_sim_time(500.0);
	OS_GetLocalTime(&OS_time);
   UTF_put_text("OS_GetLocalTime returned %lu.  Expected 500.\n", OS_time.seconds);

    UTF_OSAPI_set_function_hook(OS_GETLOCALTIME_HOOK, time_hook);

	UTF_put_text("** Call OS_GetLocalTime 3 times.  Expect return values of 501,502,503. **\n");
	OS_GetLocalTime(&OS_time);
   UTF_put_text("OS_GetLocalTime returned %lu.\n", OS_time.seconds);
	OS_GetLocalTime(&OS_time);
   UTF_put_text("OS_GetLocalTime returned %lu.\n", OS_time.seconds);
	OS_GetLocalTime(&OS_time);
   UTF_put_text("OS_GetLocalTime returned %lu.\n", OS_time.seconds);
   
   	UTF_put_text("** Test UTF_double_to_hwtime **\n");
   	for (i=0;i<=9;i++)
   	{
   		OS_time = UTF_double_to_hwtime(double_time[i]);
   		UTF_put_text("UTF_double_to_hwtime returned %lu microsecs for %lf input.\n", 
   						OS_time.microsecs, double_time[i]);
   	}

	exit(0);

}

void time_hook(OS_time_t *time_struct)
{
   /* this also works: time_struct->seconds = time_struct->seconds + 1.0; */
   /* Increment time */
   UTF_set_sim_time (UTF_get_sim_time() + 1.0);
   /* Associate new time with passed in argument */
   
   *time_struct = UTF_double_to_hwtime (UTF_get_sim_time());
}
