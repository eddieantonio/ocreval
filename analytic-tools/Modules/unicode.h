/**********************************************************************
 *
 *  unicode.h
 *
 *  This module provides definitions and routines to support the reading
 *  and writing of Unicode files.
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

#ifndef _UNICODE_
#define _UNICODE_

#include "text.h"

typedef
struct
{
    Boolean find_markers;
			/* if True, any occurrence of the "suspect_marker"
			   character will be interpreted as marking the
			   following character as suspect */
    int suspect_marker; /* applicable when "find_markers" is True; if zero,
			   SUSPECT_MARKER will be used */
    Boolean normalize;	/* if True, normalization is performed */
} Unicodeopt;

void read_unicode(/* Text *text, char *filename, Unicodeopt *unicodeopt */);
			/* reads the named Unicode file (or stdin if "filename"
 			   is NULL and running under Unix) based on the options
			   specified in "unicodeopt", and appends each
			   character to "text"; reports an error and quits if
			   unable to open the file, or if the file is invalid */

void write_unicode(/* Text *text, char *filename */);
			/* writes each character of "text" to the named Unicode
			   file (or stdout if "filename" is NULL and running
			   under Unix); reports an error and quits if unable to
			   create the file */

#endif
