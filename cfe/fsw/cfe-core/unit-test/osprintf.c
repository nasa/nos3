/*****************************************************************************
 Copyright 2001, 2002 Georges Menie (<URL snipped>)
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *****************************************************************************/

/*****************************************************************************
  Updated by Daniel D Miller.  Changes to the original Menie code are
  Copyright 2009-2013 Daniel D Miller
  All such changes are distributed under the same license as the original,
  as described above.
  11/06/09 - adding floating-point support
  03/19/10 - pad fractional portion of floating-point number with 0s
  03/30/10 - document the \% bug
  07/20/10 - Fix a round-off bug in floating-point conversions
             ( 0.99 with %.1f did not round to 1.0 )
  10/25/11 - Add support for %+ format (always show + on positive numbers)
  05/10/13 - Add snprintf() function, which takes a maximum-output-buffer
             length as an argument.  Similar to snprintf()

  BUGS
  If '%' is included in a format string, in the form \% with the intent
  of including the percent sign in the output string, this function will
  mis-handle the data entirely!!
  Essentially, it will just discard the character following the percent sign.
  This bug is not easy to fix in the existing code;
  for now, I'll just try to remember to use %% instead of \% ...
 *****************************************************************************/

/*****************************************************************************
  Updated by Kevin McCluney
  5/22/2013 - Updated to mimic output of standard (v)(s)(n)printf() functions
              for the format codes supported.  Added ' ' format modifier
              (forces space if no sign is displayed)

  NOTE: putchar is the only external dependency for this file and is required
        for printf functionality (but not for sprintf, snprintf, or vsnprintf).
        Substitute a working putchar in order to use printf.

 *****************************************************************************/
/* Embedded (Replacement) printf Compiler Flags:
   - Define OS_USE_EMBEDDED_PRINTF to use the embedded (replacement) printf
     functions; undefine to use the standard printf library functions
   - Define OSP_NO_FLOAT to exclude the code to handle floating point values
     in the variable parameter printf functions; undefine to include this code
   - Define OSP_ARINC653 and OSP_ARINC653_CFE if using embedded (replacement)
     printf functions and compiling on the GHS target using cFE modified for
     ARINC653
*/

#ifdef OS_USE_EMBEDDED_PRINTF
/*
** Includes
*/
#include "osprintf.h"
#include "osprintf_priv.h"

/*
** External global variables
*/
extern unsigned OS_printf_enabled;

/*
** Global variables
*/
static int gMaxOutputLen = -1;
static int gCurrentOutputLen = 0;

/*
** Functions
*/
/*
** Determine the length of a character string
*/
int OS_GetStringLen(const char *strptr)
{
    int len = 0;

    /* Find string length */
    for (; *strptr; strptr++)
    {
        len++;
    }

    return len;
}

/*
** Output a single character to the string buffer or console
*/
void OS_OutputSingleChar(char **str, int c)
{
    if (!(gMaxOutputLen >= 0 && gCurrentOutputLen >= gMaxOutputLen))
    {
        if (str)
        {
            /* Output to the character string buffer */
            **str = (char) c;
            (*str)++;
        }
        else
        {
            /* Output to the console */
            /* ~~~ Insert putchar() replacement here ~~~ */
#ifdef OSP_ARINC653_CFE
            TUTF_putchar();
#else
            putchar(c);
#endif
            /* ~~~ Insert putchar() replacement here ~~~ */
        }

        gCurrentOutputLen++;
    }
}

/*
** Convert a floating point value to a character string representation
*/
#ifndef OSP_NO_FLOAT
void OS_Double2String(char *out_bfr, double dbl,
                      unsigned dec_digits, int *lead)
{
    unsigned mult = 1;
    unsigned idx;
    unsigned wholeNum;
    unsigned decimalNum;
    char temp_bfr[FLOAT_BUF_LEN + 1];

    static const double round_nums[8] =
    {
        0.5,
        0.05,
        0.005,
        0.0005,
        0.00005,
        0.000005,
        0.0000005,
        0.00000005
    };

    /* Extract negative info */
    if (dbl < 0.0)
    {
        dbl = -dbl;
        *lead |= LEAD_NEG;
    }

    /* Handling rounding by adding .5LSB to the floating-point data */
    if (dec_digits < 8)
    {
        dbl += round_nums[dec_digits];
    }

    /* Construct fractional multiplier for specified number of digits */
    for (idx = 0; idx < dec_digits; idx++)
    {
        mult *= 10;
    }

    wholeNum = (unsigned) dbl;
    decimalNum = (unsigned) ((dbl - wholeNum) * mult);

    /* Convert integer portion */
    idx = 0;

    while (wholeNum != 0 && idx < FLOAT_BUF_LEN)
    {
        temp_bfr[idx] = '0' + (wholeNum % 10);
        idx++;
        wholeNum /= 10;
    }

    if (idx == 0)
    {
        *out_bfr = '0';
        out_bfr++;
    }
    else
    {
        while (idx > 0)
        {
            *out_bfr = temp_bfr[idx - 1];
            out_bfr++;
            idx--;
        }
    }

    if (dec_digits > 0)
    {
        *out_bfr = '.';
        out_bfr++;

        /* Convert fractional portion */
        idx = 0;

        while (decimalNum != 0)
        {
            temp_bfr[idx] = '0' + (decimalNum % 10);
            idx++;
            decimalNum /= 10;
        }

        /* Pad the decimal portion with 0s as necessary */
        while (idx < dec_digits)
        {
            temp_bfr[idx] = '0';
            idx++;
        }

        while (idx > 0)
        {
            *out_bfr = temp_bfr[idx - 1];
            out_bfr++;
            idx--;
        }
    }

    *out_bfr = 0;
}
#endif

/*
** Format a character string for output
*/
int OS_ParseString(char **out, const char *string,
                   int min_width, int max_width, int pad,
                   int lead, unsigned s_flag)
{
    register int pc = 0, padchar = ' ';
    int len = 0;

    len = OS_GetStringLen(string);

    /* Set length to # if specified by %.#s format */
    if (s_flag && max_width && len > max_width)
    {
        len = max_width;
    }

    if (min_width > 0)
    {
        if (len >= min_width)
        {
            min_width = 0;
        }
        else
        {
            min_width -= len;
        }

        if ((pad & PAD_ZERO) && !s_flag)
        {
            padchar = '0';
        }
    }

    if (max_width > min_width)
    {
        max_width -= min_width;

        if (len >= max_width)
        {
            max_width = 0;
        }
        else
        {
            max_width -= len;
        }

        if (lead & LEAD_HEX)
        {
            if (max_width >= 2)
            {
                max_width -= 2;
            }
            else
            {
                max_width = 0;
            }
        }
        else if ((lead & LEAD_NEG) || (lead & LEAD_SIGN))
        {
            if (max_width >= 1)
            {
                max_width--;
            }
            else
            {
                max_width = 0;
            }
        }

        for (; max_width > 0; max_width--)
        {
            OS_OutputSingleChar(out, ' ');
            pc++;
        }
    }

    if ((lead & LEAD_HEX) && padchar == '0')
    {
        OS_OutputSingleChar(out, '0');
        OS_OutputSingleChar(out, 'x');
        pc += 2;
    }
    else if (lead & LEAD_NEG)
    {
        OS_OutputSingleChar(out, '-');
        pc++;
    }
    else if (lead & LEAD_SIGN)
    {
        OS_OutputSingleChar(out, '+');
        pc++;
    }
    else if (lead & LEAD_SPACE)
    {
        OS_OutputSingleChar(out, ' ');
        pc++;
    }

    if (!(pad & PAD_RIGHT))
    {
        for (; min_width > 0; min_width--)
        {
            OS_OutputSingleChar(out, padchar);
            pc++;
        }
    }

    if ((lead & LEAD_HEX) && padchar == ' ')
    {
        OS_OutputSingleChar(out, '0');
        OS_OutputSingleChar(out, 'x');
        pc += 2;
    }

    for (; len; string++)
    {
        OS_OutputSingleChar(out, *string);
        pc++;
        len--;
    }

    for (; min_width > 0; min_width--)
    {
        OS_OutputSingleChar(out, padchar);
        pc++;
    }

    return pc;
}

/*
** Convert and format an integer value as a character string
*/
int OS_ParseInteger(char **out, int i, unsigned base, int sign,
                    int space_pad_width, int zero_pad_width,
                    int pad, int lead, int letbase)
{
    char print_buf[PRINT_BUF_LEN + 1];
    char *s;
    int t, pc = 0;
    unsigned u = (unsigned) i;
    int rtn;

    if (i == 0)
    {
        print_buf[0] = '0';
        print_buf[1] = '\0';
        rtn = OS_ParseString(out, print_buf, zero_pad_width,
                             space_pad_width, pad, lead, 0);
    }
    else
    {
        if (sign && base == 10 && i < 0)
        {
            u = (unsigned) -i;
            lead |= LEAD_NEG;
        }

        /* Make sure print_buf is NULL-terminated */
        s = print_buf + PRINT_BUF_LEN - 1;
        *s = '\0';

        while (u)
        {
            t = u % base;

            if (t >= 10)
            {
                t += letbase - '0' - 10;
            }

            s--;
            *s = (char) t + '0';
            u /= base;
        }

        rtn = pc + OS_ParseString(out, s, zero_pad_width,
                                  space_pad_width, pad, lead, 0);
    }

    return rtn;
}

/*
** Write formatted data from a variable argument list to a sized buffer
*/
int OS_vsnprintf(char *out_buffer, int max_len,
                 const char *format, VA_LIST varg)
{
    unsigned post_decimal;
    int width, pad, lead;
    unsigned dec_width;
    int pc = 0;
    char scr[2];
    char *s;
    char **out;

#ifndef OSP_NO_FLOAT
    unsigned dec_width_set;
    double *dblptr;
    double dbl;
    char float_bfr[FLOAT_BUF_LEN + 1];
#endif

    if (out_buffer)
    {
        out = &out_buffer;
    }
    else
    {
        out = 0;
    }

    gMaxOutputLen = max_len;
    gCurrentOutputLen = 0;

    /* Step through and parse the format string */
    for (; *format != '\0'; format++)
    {
        /* Process the format code following the format specifier (%) */
        if (*format == '%')
        {
            format++;
            width = 0;
            dec_width = 0;
#ifndef OSP_NO_FLOAT
            dec_width_set = 0;
#endif
            pad = 0;
            lead = 0;
            post_decimal = 0;

            /* Look for format modifiers */
            while (*format == '-' ||
                   *format == '+' ||
                   *format == ' ' ||
                   *format == '0')
            {
                if (*format == '-')
                {
                    /* Left-justify output */
                    format++;
                    pad |= PAD_RIGHT;
                }
                else if (*format == '+')
                {
                    /* Prepend the plus sign (+) to the output if the value is
                     * positive
                     */
                    format++;
                    lead |= LEAD_SIGN;
                }
                else if (*format == ' ')
                {
                    /* Prepend a space in place of the plus sign (+) if the
                     * value is positive
                     */
                    format++;
                    lead |= LEAD_SPACE;
                }
                else if (*format == '0')
                {
                    /* Pad numerical output with leading zeroes */
                    format++;
                    pad |= PAD_ZERO;
                }
            }

            /* Look for field width and precision specifier (e.g., ###.###) */
            while (1)
            {
                if (*format == '.')
                {
                    /* Decimal point indicates any following numbers are the
                     * precision specifier
                     */
                    if (post_decimal)
                    {
                        /* Already found one decimal point, any others indicate
                         * a format string error; back up pointer so the output
                         * will show all of the erroneous modifier
                         */
                        while (*(format -1) != '%')
                        {
                            format--;
                        }

                        break;
                    }

                    post_decimal = 1;
                    format++;
                }
                else if (*format >= '0' &&  *format <= '9')
                {
                    if (post_decimal)
                    {
                        /* After the decimal is the precision specifier; add
                         * number to the total, accounting for the number of
                         * digits
                         */
                        dec_width *= 10;
                        dec_width += (unsigned) (unsigned char)
                                         (*format - '0');
#ifndef OSP_NO_FLOAT
                        dec_width_set = 1;
#endif
                    }
                    else
                    {
                        /* Prior to the decimal is the field width; add number
                         * to the total, accounting for the number of digits
                         */
                        width *= 10;
                        width += *format - '0';
                    }

                    format++;
                }
                else
                {
                    /* End of field width and precision specifier reached;
                     * exit 'while' loop
                     */
                    break;
                }
            }

            /* Check for long format modifier; if found then skip since all
             * are treated as long regardless
             */
            if (*format == 'l')
            {
                format++;
            }

            switch (*format)
            {
                case 's': /* Character string */
                    s = (char *) *varg;
                    varg++;
                    pc += OS_ParseString(out, s, width, dec_width, pad, 0, 1);
                    break;

                case 'd': /* (Long) integer */
                case 'i':
                    if (dec_width)
                    {
                        pad = PAD_ZERO;
                    }
                    else if (width && (pad & PAD_ZERO))
                    {
                        dec_width = width;
                        width = 0;

                        if ((int) *varg < 0 || lead)
                        {
                            dec_width--;
                        }
                    }

                    pc += OS_ParseInteger(out, (int) *varg, 10, 1, width,
                                          dec_width, pad, lead, 'a');
                    varg++;
                    break;

                case 'u': /* (Long) unsigned integer */
                    if (dec_width)
                    {
                        pad = PAD_ZERO;
                    }
                    else if (width && (pad & PAD_ZERO))
                    {
                        dec_width = width;
                        width = 0;
                    }

                    pc += OS_ParseInteger(out, (int) *varg, 10, 0, width,
                                          dec_width, pad, 0, 'a');
                    varg++;
                    break;

                case 'x': /* Hexadecimal (lower case) */
                    if (dec_width)
                    {
                        pad = PAD_ZERO;
                    }
                    else if ((pad & PAD_ZERO))
                    {
                        dec_width = width;
                    }

                    pc += OS_ParseInteger(out, (int) *varg, 16, 0, width,
                                          dec_width, pad, lead, 'a');
                    varg++;
                    break;

                case 'X': /* Hexadecimal (upper case) */
                    if (dec_width)
                    {
                        pad = PAD_ZERO;
                    }
                    else if ((pad & PAD_ZERO))
                    {
                        dec_width = width;
                    }

                    pc += OS_ParseInteger(out, (int) *varg, 16, 0, width,
                                          dec_width, pad, lead, 'A');
                    varg++;
                    break;

                case 'p': /* Address (hexadecimal with '0x' prepended) */
                    if (dec_width)
                    {
                        pad = PAD_ZERO;
                    }
                    else if ((pad & PAD_ZERO) && width >= 2)
                    {
                        dec_width = width - 2;
                    }

                    lead = LEAD_HEX;
                    pc += OS_ParseInteger(out, (int) *varg, 16, 0, width,
                                          dec_width, pad, lead, 'a');
                    varg++;
                    break;

                case 'c': /* Single character */
                    scr[0] = (char) *varg;
                    varg++;
                    scr[1] = '\0';
                    pc += OS_ParseString(out, scr, width, 0, pad, 0, 0);
                    break;

#ifndef OSP_NO_FLOAT
                case 'f': /* (Long) float */
                    dblptr = (double *) varg;
                    dbl = *dblptr;
                    dblptr++;
                    varg = (VA_LIST) dblptr;

                    if (!dec_width_set)
                    {
                        dec_width = 6;
                    }

                    OS_Double2String(float_bfr, dbl, dec_width, &lead);

                    if (width && !(pad & PAD_RIGHT) && !(pad & PAD_ZERO))
                    {
                        dec_width = 0;
                    }
                    else
                    {
                        dec_width = width - (lead != 0);
                        width = 0;
                    }

                    pc += OS_ParseString(out, float_bfr, dec_width,
                                         width, pad, lead, 0);
                    break;
#endif

                case '%': /* Output percent character (%) */
                    OS_OutputSingleChar(out, *format);
                    pc++;
                    break;

                case '\0':
                    /* Premature end of format string; back up one so the
                     * 'for' loop will terminate parsing gracefully
                     */
                    format--;
                    break;

                default:
                    /* Invalid format code; output the format specifier
                     * and code
                     */
                    OS_OutputSingleChar(out, '%');
                    OS_OutputSingleChar(out, *format);
                    pc += 2;
                    break;
            }
        }
        /* Normal output character (not a format specifier); send to output */
        else
        {
            OS_OutputSingleChar(out, *format);
            pc++;
        }
    }

    if (out)
    {
        **out = '\0';
    }

    return pc;
}

/*
** Print formatted output to stdout
*/
void OS_printf(const char *format, ...)
{
    VA_LIST varg;

    if (OS_printf_enabled)
    {
        /* Create a pointer into the stack */
        VA_START(varg, format, OFFSET_OS_PRINTF, BREAK_OS_PRINTF,
                 SKIP_OS_PRINTF);
        OS_vsnprintf(0, -1, format, varg);
        VA_END(varg);
    }
}

/*
** Write formatted output to a character string
*/
int OS_sprintf(char *out, const char *format, ...)
{
    VA_LIST varg;
    int length;

    /* Create a pointer into the stack */
    VA_START(varg, format, OFFSET_OS_SPRINTF, BREAK_OS_SPRINTF,
             SKIP_OS_SPRINTF);
    length = OS_vsnprintf(out, -1, format, varg);
    VA_END(varg);
    return(length);
}

/*
** Write formatted output to a sized buffer
*/
int OS_snprintf(char *out, unsigned max_len, const char *format, ...)
{
    VA_LIST varg;
    int length;

    /* Create a pointer into the stack */
    VA_START(varg, format, OFFSET_OS_SNPRINTF, BREAK_OS_SNPRINTF,
             SKIP_OS_SNPRINTF);
    length = OS_vsnprintf(out, (int) max_len - 1, format, varg);
    VA_END(varg);
    return(length);
}

#endif
