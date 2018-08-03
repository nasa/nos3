/** \file
 * \brief Report "bad characters" in portable text files
 *
 * This program scans a collection of text files looking for
 * characters that are not portable across various development
 * platforms. Allowed characters are the printable characters from the
 * 7-bit ASCII character set plus TAB and NEWLINE.
 *
 * The intended purpose of this is to assure that a body of source
 * code is presented identically to readers on all supported
 * development platforms.
 *
 * If any unportable characters are found, a message is produced
 * in a form similar to that used for compiler errors and warnings,
 * allowing output to be similarly parsed.
 *
 * As a special case, if a Carriage Return is found at the end of a
 * line of text (immediately before a Newline), then only a single
 * warning is produced for that file indicating that it has CRLF line
 * termination.
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

/** maximum messages to report for each file */
static int              err_count_limit = 10;

static void
dofile (
    char const *name,
    FILE *file)
{
    int                     curr_line = 0;
    int                     curr_col = 0;
    int                     curr_char;
    int                     prev_char;
    int                     err_count = 0;
    int                     err_line = -1;

    int                     saw_crlf = 0;

    curr_char = EOF;
    while (err_count < err_count_limit) {
        prev_char = curr_char;
        curr_char = fgetc (file);

        if (EOF == curr_char)
            break;

        if (curr_char == '\n') {
            if (prev_char == '\r') {
                saw_crlf++;
                if (saw_crlf == 1)
                    printf ("%s:%d: warning: file has DOS style line termination\n", name, curr_line + 1);
            }
            curr_line++;
            curr_col = 0;
            continue;
        }

        if (curr_char == '\r') {
            continue;
        }

        if (prev_char == '\r') {
            printf ("%s:%d:%d: error: Carriage Return present (not followed by Newline)\n", name, curr_line + 1, curr_col + 1);
            ++err_count;
        }

        if (curr_char == '\t') {
            curr_col += 8 - (curr_col & 7);
            continue;
        }

        if ((curr_char >= 32) && (curr_char <= 126)) {
            curr_col++;
            continue;
        }

        /*
         * Only report one bad character per text line,
         * because some bad characters are really just
         * the first of a sequence (for example, if the
         * document has a three-byte UTF-8 encoding
         * of a fancier Unicode character).
         */
        if (err_line == curr_line)
            continue;
        err_line = curr_line;

        switch (curr_char) {
              /*
               * also observed:
               * - 0xE2 0x80 0x94 for hyphen,
               * - 0xEF 0xBB 0xBF inside a unit test result message,
               * will not special case them.
               */

          case 0xD0:
              /*
               * HEURISTIC: probable Microsoft Office Word Document File V2 
               */
              if ((curr_line == 0) && (curr_col == 0)) {
                  printf ("%s:1: warning: probable Microsoft Word document\n", name);
                  return;               /* no further messages for this file. */
              } else
                  printf ("%s:%d:%d: error: character code %d (0x%02X) is not portable.\n", name, curr_line + 1, curr_col + 1, curr_char, (unsigned) (curr_char & 255));
              break;

          case 0x03:
              /*
               * HEURISTIC: probable Microsoft Word 2007+ 
               */
              if ((curr_line == 0) && (curr_col == 2)) {
                  printf ("%s:1: warning: probable Microsoft Word document\n", name);
                  return;               /* no further messages for this file. */
              } else
                  printf ("%s:%d:%d: error: character code %d (0x%02X) is not portable.\n", name, curr_line + 1, curr_col + 1, curr_char, (unsigned) (curr_char & 255));
              break;

          case 0x80:
              /*
               * Unused in Microsoft Code Page 1252 (Latin 1), but the
               * signle instance in our project appers to match a much
               * older sense of NON-BREAKING SPACE.
               */
              printf ("%s:%d:%d: error: character code %d (0x%02X) is a nonportable WHITESPACE character\n", name, curr_line + 1, curr_col + 1, curr_char, (unsigned) (curr_char & 255));
              break;

          case 0x95:            /* CP1252 bullet */
          case 0x98:            /* CP1252 small tilde */
              printf ("%s:%d:%d: error: character code %d (0x%02X) is a nonportable BULLET or similar glyph.\n", name, curr_line + 1, curr_col + 1, curr_char, (unsigned) (curr_char & 255));
              break;

          case 0x91:            /* CP1252 left single quotation mark */
          case 0x92:            /* CP1252 right single quotation mark */
          case 0x93:            /* CP1252 left double quotation mark */
          case 0x94:            /* CP1252 right double quotation mark */

              /* 
               * also observed:
               *   0xE2 0x80 0x98 for left single quote
               *   0xE2 0x80 0x99 for right single quote
               * GCC can generate these as part of its warning messages, if allowed to play
               * freely in the Unicode space. will not set up to recognize them, as they
               * appear only where we blindly copied warning messages from UTF-8 locale
               * into source code. These also occur when typing single-quote inside comments
               * in some Microsoft IDEs, apparently ...
               */

          case 0xBF:            /* context indicates APOSTROPHE intended */
          case 0xC2:            /* context indicates APOSTROPHE intended */
              printf ("%s:%d:%d: error: character code %d (0x%02X) is a nonportable QUOTE glyph.\n", name, curr_line + 1, curr_col + 1, curr_char, (unsigned) (curr_char & 255));
              break;

          case 0x99:            /* CP1252 trademark sign */
          case 0xA9:            /* CP1252 copyright sign */

          case 0xBD:            /* context indicates COPYRIGHT intended */
          case 0xE3:            /* context indicates COPYRIGHT intended */
          case 0xEF:            /* context indicates COPYRIGHT intended */
              printf ("%s:%d:%d: error: character code %d (0x%02X) is a nonportable COPYRIGHT or TRADEMARK glyph.\n", name, curr_line + 1, curr_col + 1, curr_char, (unsigned) (curr_char & 255));
              break;

          default:
              printf ("%s:%d:%d: error: character code %d (0x%02X) is not portable.\n", name, curr_line + 1, curr_col + 1, curr_char, (unsigned) (curr_char & 255));
              break;
        }

        ++err_count;
    }
}

static void
doname (
    char const *name)
{
    FILE                   *file = fopen (name, "r");
    if (!file) {
        fprintf (stderr, "%s:1: error: %s\n", name, strerror (errno));
    } else {
        dofile (name, file);
        fclose (file);
    }
}

int
main (
    int argc,
    char const **argv)
{
    for (int argi = 1; argi < argc; ++argi)
        doname (argv[argi]);
    return 0;
}
