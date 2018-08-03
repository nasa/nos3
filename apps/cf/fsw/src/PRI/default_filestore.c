/* FILE: default_filestore.c    Default interface to the Virtual Filestore.
 *
 *  Copyright © 2007-2014 United States Government as represented by the 
 *  Administrator of the National Aeronautics and Space Administration. 
 *  All Other Rights Reserved.  
 *
 *  This software was created at NASA's Goddard Space Flight Center.
 *  This software is governed by the NASA Open Source Agreement and may be 
 *  used, distributed and modified only pursuant to the terms of that 
 *  agreement.
 * 
 * SPECS:  cfdp_requires.h
 * ORIGINAL PROGRAMMER:  Tim Ray 301-286-0581
 * NOTES:
 *   1) Most of the default interface to the Virtual Filestore is via 
 *      standard 'C' library routines (e.g. fopen, fclose, fread, ...).
 * CHANGES:
 *   2007_07_31 Tim Ray
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 */

#include <sys/types.h>
#include <sys/stat.h>
#include "cfdp.h"
#include "cfdp_private.h"



/*=r=************************************************************************/
static boolean m__does_file_exist (const char *filename)
{
  CFDP_FILE           *fp;
  /*------------------------------------------------------------*/
  if ((fp = fopen_callback (filename, "r")) != NULL)
    {
      fclose_callback (fp);
      return (YES);
    }
  else
    return (NO);
}



/*=r=************************************************************************/
u_int_4 file_size (const char *file_name)
   {
     struct stat        buf;
     int                file_size;
   /*------------------------------------------------------------*/
     stat (file_name, &buf);
     file_size = buf.st_size;
     return (u_int_4) (file_size);
   }



/*=r=************************************************************************/
boolean is_file_segmented (const char *file_name)
   {
   /*------------------------------------------------------------*/
     /* <NOT_SUPPORTED> Segmented Files */
     return (NO);
   }



/*=r=************************************************************************/
char *temp_file_name (char *name)
{
  static u_int_4       count = 0;
  char                 count_as_string [32];
  char                 safe [1024];
  /*------------------------------------------------------------*/

 LOOP:
  count ++;
  if (count >= 100000)
    count = 1;

  /* Be careful not to overflow the memory allocated to 'name' */
  COPY (safe, DEFAULT_TEMP_FILE_NAME_PREFIX);
  sprintf (count_as_string, "%5.5lu", count);
  APPEND (safe, count_as_string);

  /* Truncate if necessary */
  memset (name, 0, MAX_TEMP_FILE_NAME_LENGTH);
  strncpy (name, safe, MAX_TEMP_FILE_NAME_LENGTH);
  name[MAX_TEMP_FILE_NAME_LENGTH] = 0;

  /* The temp file name is not valid if it already exists */
  if (m__does_file_exist (name))
    goto LOOP;

  return (name);
}
