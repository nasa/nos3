/* FILE: utils.h -- specs for (private) CFDP utility routines.  Note that
 *   many CFDP utility routines are public (see 'cfdp_provides.h').
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
 * ORIGINAL PROGRAMMER:  Tim Ray 301-286-0581
 * CHANGES:
 *   2007_07_31 Tim Ray
 *     - Official release.  (see pre_release_changes.log for earlier changes)
 */

#ifndef H_UTILS
#define H_UTILS 1

boolean utils__request_from_string (const char *request_string, 
                                 REQUEST *request);
/* WHAT IT DOES:  Given a char string, it attempts to convert the string
 *   to a REQUEST structure.  Returns 1 if successful; otherwise, 0.
 */

boolean utils__strncpy (char *output, const char *input, size_t max_storage);
/* WHAT IT DOES:  Performs a normal 'strncpy', plus ensures that the
 *   output string is null-terminated.  Issues a warning message if
 *   the input string exceeds max_storage chars.
 * EXAMPLE:  If the given 'input' is 20 chars long, and 'max_storage' is
 *   16, then the first 15 chars of 'input' will be copied, and the 16th
 *   char will be a null-char.
 * RETURN STATUS:  1 if entire string was copied; otherwise, 0.
 */

boolean utils__strncat (char *string1, const char *string2, 
                        size_t max_storage);
/* WARNING:  This routine does NOT match 'strncat'.
 * WHAT IT DOES:  If the combined length of string1 and string2 is less
 *   than max_storage chars, then 'strcat' occurs.
 *   Otherwise, a warning message is issued, and a partial concatenation
 *   is performed (whatever can be concatenated without violating storage
 *   constraints).  Regardless, the resulting 'string1' is always
 *   null-terminated.
 * EXAMPLE:  If the given 'string1' has a strlen of 10, the given 'string2'
 *   has a strlen of 10, and the 'max_storage' is 16, then the first 5
 *   chars of 'string2' will be appended to 'string1' and the 16th char 
 *   of 'string1' will be set to a null-char.
 * NOTE:  This routine uses the third argument differently than 'strncat'.
 * RETURN STATUS:  1 if entire string was concatenated; otherwise, 0.
 */

#endif
