/**********************************************************************
 *
 *  text.c
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

#include <utf8proc.h>

#include "text.h"

#define TITLE    "UNLV-ISRI OCR Vendor-Independent Interface Version 5.1\n"
#define DIVIDER  "------------------------------------------------------\n"

/*
 * Assert that the internal character size is the same size as UTF-32.
 * (C89 style!)
 */
static int assert_sizes[sizeof(Charvalue) == sizeof(utf8proc_int32_t) ? 0 : -1];

/**********************************************************************/

void append_char(text, suspect, value)
Text *text;
Boolean suspect;
Charvalue value;
{
    Char *c;
    c = NEW(Char);
    c->suspect = suspect;
    c->value = value;
    list_insert_last(text, c);
}
/**********************************************************************/

static Boolean read_header(f)
FILE *f;
{
    char line[100];
    short divider_count = 0;
    if (fgets(line, sizeof(line) - 1, f) &&
    strncmp(line, TITLE, sizeof(TITLE) - 5) == 0)
    {
	while (divider_count < 2 && fgets(line, sizeof(line) - 1, f))
	    if (strcmp(line, DIVIDER) == 0)
		divider_count++;
	return(True);
    }
    rewind(f);
    return(False);
}
/**********************************************************************/

static void read_contents(f, text, find_markers, suspect_marker)
FILE *f;
Text *text;
Boolean find_markers;
int suspect_marker;
{
    Boolean suspect = False;

    /* Buffer for one full UTF-8 code unit. */
    utf8proc_uint8_t buffer[4];

    int byte, read_status, decode_status;
    ssize_t code_unit_size;
    utf8proc_int32_t code_point;
    byte = getc(f);

    while (byte != EOF)
    {
	code_unit_size = utf8proc_utf8class[byte];
	if (code_unit_size < 1) {
	    /* Encoding error! Stream is not UTF-8! */
	    error("invalid UTF-8 character", Exit);
	}

	buffer[0] = byte;
	/* Read the remaining bytes into the buffer. */
	if (code_unit_size > 1) {
	    read_status = fread(
		    buffer + 1,
		    sizeof(utf8proc_uint8_t),
		    /* The first byte is already in the buffer. */
		    code_unit_size - 1,
		    f
	    );

	    if (read_status != (code_unit_size - 1)) {
		/* Could not read enough bytes. */
		error("unexpected end of input", Exit);
	    }
	}

	/* Decode a single code unit -> code point. */
	decode_status = utf8proc_iterate(buffer, sizeof(buffer), &code_point);
	if (decode_status != code_unit_size) {
	    error(utf8proc_errmsg(decode_status), Exit);
	}

	if (find_markers && code_point == suspect_marker) {
	    suspect = True;
	} else {
	    append_char(text, suspect, code_point);
	    suspect = False;
	}
	byte = getc(f);
    }
}
/**********************************************************************/

static void compress_spacing(text, start)
Text *text;
Char *start;
{
    Boolean found_non_blank = False;
    Char *c, *next;
    for (c = start; c; c = c->next)
	/* TODO: Use Unicode character values. */
	if (c->value <= '\r' && (c->value >= '\v' || c->value == '\t') ||
	c->value == 0xA0)
	    c->value = BLANK;
    c = start;
    while (c)
    {
	next = c->next;
	if (c->value == BLANK && (!found_non_blank || !next ||
	next->value == BLANK || next->value == NEWLINE) ||
	c->value == NEWLINE && !found_non_blank)
	{
	    if (found_non_blank && next && next->value == BLANK)
		next->suspect |= c->suspect;
	    list_remove(text, c);
	    free(c);
	}
	else
	    found_non_blank = (c->value == NEWLINE ? False : True);
	c = next;
    }
}
/**********************************************************************/

static void lowercase(start)
Char *start;
{
    Char *c;
    for (c = start; c; c = c->next)
	/* Delegate to utf8proc. */
	c->value = utf8proc_tolower(c->value);
}

/**********************************************************************/

void read_text(text, filename, textopt)
Text *text;
char *filename;
Textopt *textopt;
{
    Char *last;
    FILE *f;
    if (textopt->find_header && !filename)
	error_string("invalid call to", "read_text", Exit);
    last = text->last;
    f = open_file(filename, "r");
    if (textopt->find_header)
	textopt->found_header = read_header(f);
    read_contents(f, text, textopt->find_markers, 
    (textopt->suspect_marker ? textopt->suspect_marker : SUSPECT_MARKER));
    close_file(f);
    if (textopt->normalize)
	compress_spacing(text, (last ? last->next : text->first));
    if (textopt->case_insensitive)
	lowercase(last ? last->next : text->first);
}

/**********************************************************************/
utf8proc_ssize_t encode_or_die(codepoint, buffer)
utf8proc_int32_t codepoint;
utf8proc_uint8_t *buffer;
{
    utf8proc_ssize_t status = utf8proc_encode_char(codepoint, buffer);
    if (status < 1) {
	error("could not encode UTF-8", Exit);
    }

    return status;
}
/**********************************************************************/

void char_to_string(suspect, value, string, fake_newline)
Boolean suspect;
Charvalue value;
char *string;
Boolean fake_newline;
{
    short i = 0;
    utf8proc_uint8_t *ustring = (utf8proc_uint8_t *) string;

    /* Prepend the suspect marker. */
    if (suspect) {
	i += encode_or_die(SUSPECT_MARKER, ustring);
    }

    if (value == NEWLINE)
	sprintf(&string[i], (fake_newline ? "<\\n>" : "\n"));
    /* TODO: Base this on character classes. */
    else if ((value >= BLANK && value <= REJECT_CHARACTER) || value > 0xA0) {
	/* It's a printable character. */
	i += encode_or_die(value, &ustring[i]);
	string[i++] = '\0';
    } else if (value < 256)
	sprintf(&string[i], "<%02X>", value);
    else
	sprintf(&string[i], "<%04X>", value);
}
/**********************************************************************/

void write_text(text, filename, write_header)
Text *text;
char *filename;
void (*write_header)();
{
    FILE *f;
    Char *c;
    char string[STRING_SIZE];
    f = open_file(filename, "w");
    if (write_header)
    {
	fputs(TITLE, f);
	fputs(DIVIDER, f);
	(*write_header)(f);
	fputs(DIVIDER, f);
    }
    for (c = text->first; c; c = c->next)
    {
	char_to_string(c->suspect, c->value, string, False);
	fputs(string, f);
    }
    close_file(f);
}
