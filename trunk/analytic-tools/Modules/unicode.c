/**********************************************************************
 *
 *  unicode.c
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

#include "unicode.h"

/**********************************************************************/

static Boolean read_contents(f, text, find_markers, suspect_marker)
FILE *f;
Text *text;
Boolean find_markers;
int suspect_marker;
{
    Boolean MSB_first = True, suspect = False;
    int byte1, byte2;
    Charvalue value;
    byte1 = getc(f);
    byte2 = getc(f);
    if (byte1 == 0xFE && byte2 == 0xFF || byte1 == 0xFF && byte2 == 0xFE)
    {
	MSB_first = (byte1 == 0xFE ? True : False);
	byte1 = getc(f);
	byte2 = getc(f);
    }
    while (byte1 != EOF && byte2 != EOF)
    {
	value = (MSB_first ? (byte1 << 8) + byte2 : (byte2 << 8) + byte1);
	if (find_markers && value == suspect_marker)
	    suspect = True;
	else
	{
	    append_char(text, suspect, value);
	    suspect = False;
	}
	byte1 = getc(f);
	byte2 = getc(f);
    }
    return(byte1 == EOF ? True : False);
}
/**********************************************************************/

static void normalize(text, start)
Text *text;
Char *start;
{
}
/**********************************************************************/

void read_unicode(text, filename, unicodeopt)
Text *text;
char *filename;
Unicodeopt *unicodeopt;
{
    Char *last;
    FILE *f;
#ifndef unix
    if (!filename)
	error_string("invalid call to", "read_unicode", Exit);
#endif
    last = text->last;
    f = open_file(filename, "r+b");
    if (!read_contents(f, text, unicodeopt->find_markers, 
    (unicodeopt->suspect_marker ? unicodeopt->suspect_marker : SUSPECT_MARKER)))
	error_string("invalid Unicode file", (filename ? filename : "stdin"),
	Exit);
    close_file(f);
    if (unicodeopt->normalize)
	normalize(text, (last ? last->next : text->first));
}
/**********************************************************************/

void write_unicode(text, filename)
Text *text;
char *filename;
{
    unsigned short byte_order = 0xFEFF, suspect_marker = SUSPECT_MARKER;
    FILE *f;
    Char *c;
#ifndef unix
    if (!filename)
	error_string("invalid call to", "write_unicode", Exit);
#endif
    f = open_file(filename, "w+b");
    fwrite(&byte_order, sizeof(unsigned short), 1, f);
    for (c = text->first; c; c = c->next)
    {
	if (c->suspect)
	    fwrite(&suspect_marker, sizeof(unsigned short), 1, f);
	fwrite(&c->value, sizeof(unsigned short), 1, f);
    }
    close_file(f);
}
