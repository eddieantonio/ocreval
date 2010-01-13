/**********************************************************************
 *
 *  text.h
 *
 *  This module provides definitions and routines to support the reading
 *  and writing of OCR-generated text.  A "Text" structure is defined to
 *  be a linked list of "Char" structures, where each "Char" structure
 *  gives the value of one 16-bit character and indicates whether it
 *  is suspect.
 *
 *  Author: Stephen V. Rice
 *  
 * Copyright 1996 The Board of Regents of the Nevada System of Higher
 * Education, on behalf, of the University of Nevada, Las Vegas,
 * Information Science Research Institute
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you
 * may not use this file except in compliance with the License.  You
 * may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * permissions and limitations under the License.
 *
 **********************************************************************/

#ifndef _TEXT_
#define _TEXT_

#include "list.h"
#include "util.h"

#define BLANK             ' '
#define NEWLINE           '\n'
#define REJECT_CHARACTER  '~'
#define SUSPECT_MARKER    '^'

typedef unsigned short Charvalue;
#define NUM_CHARVALUES  65536

BEGIN_ITEM(Char)
    Boolean suspect;
    Charvalue value;
END_ITEM(Char);

BEGIN_LIST_OF(Char)
END_LIST(Text);

void append_char(/* Text *text, Boolean suspect, Charvalue value */);
			/* appends the given character to "text" */

typedef
struct
{
    Boolean find_header;/* if True, a header will be looked for and skipped if
			   present; if False and a header is present, it will be
			   stored as text */
    Boolean find_markers;
			/* if True, any occurrence of the "suspect_marker"
			   character will be interpreted as marking the
			   following character as suspect */
    int suspect_marker; /* applicable when "find_markers" is True; if zero,
			   SUSPECT_MARKER will be used */
    Boolean find_hex_values;
			/* if True, any occurrence of a hex value of the form
			   <FF> or <FFFF> will be converted to the corresponding
			   character value; if False, any such string will be
			   stored as text */
    Boolean normalize;	/* if True, spacing is compressed */
    Boolean case_insensitive;
			/* if True, letters are converted to lower-case */
    Boolean found_header;
			/* set to True if a header was found; applicable when
			   "find_header" is True */
} Textopt;

void read_text(/* Text *text, char *filename, Textopt *textopt */);
			/* reads the named file (or stdin if "filename" is NULL
			   and "textopt->find_header" is False) based on the
			   options specified in "textopt", and appends each
			   character to "text"; reports an error and quits if
			   unable to open the file */

#define STRING_SIZE  8

void char_to_string(/* Boolean suspect, Charvalue value, char *string,
		       Boolean fake_newline */);
			/* stores a representation of the given character in
			   "string", which must be at least STRING_SIZE bytes; a
			   non-printable character is represented by a hex value
			   of the form <FF> or <FFFF>; if "fake_newline" is
			   True, the newline character is represented by <\n>,
			   which is desirable for some reports */

void write_text(/* Text *text, char *filename,
		   void (*write_header)(FILE *f) */);
			/* writes each character of "text" to the named file
			   (or stdout if "filename" is NULL) using
			   "char_to_string" to represent the characters;
			   if "write_header" is non-NULL, this routine is
			   called to write a header to the file; reports an
			   error and quits if unable to create the file */

#endif
