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

#include "text.h"

#define TITLE    "UNLV-ISRI OCR Vendor-Independent Interface Version 5.1\n"
#define DIVIDER  "------------------------------------------------------\n"

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
    int byte;
    byte = getc(f);
    while (byte != EOF)
    {
	if (find_markers && byte == suspect_marker)
	    suspect = True;
	else
	{
	    append_char(text, suspect, byte);
	    suspect = False;
	}
	byte = getc(f);
    }
}
/**********************************************************************/

static Boolean is_hex(c, value)
Char *c;
Charvalue *value;
{
    if (c->value >= '0' && c->value <= '9')
	*value = (*value << 4) + c->value - '0';
    else if (c->value >= 'A' && c->value <= 'F')
	*value = (*value << 4) + c->value - 'A' + 10;
    else
	return(False);
    return(True);
}
/**********************************************************************/

static void convert_hex(text, start)
Text *text;
Char *start;
{
    Char *c, *prev;
    Charvalue value;
    while (start)
    {
	if (start->value == '<' && !(value = 0) &&
	(c = start->next) && is_hex(c, &value) &&
	(c = c->next) && is_hex(c, &value) &&
	(c = c->next) && (c->value == '>' || is_hex(c, &value) &&
	(c = c->next) && is_hex(c, &value) &&
	(c = c->next) && c->value == '>'))
	{
	    start->value = value;
	    while (c != start)
	    {
		prev = c->prev;
		list_remove(text, c);
		free(c);
		c = prev;
	    }
	}
	start = start->next;
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
	if (c->value >= 'A' && c->value <= 'Z' ||
	c->value >= 0xC0 && c->value <= 0xDE && c->value != 0xD7)
	    c->value += 32;
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
    if (textopt->find_hex_values)
	convert_hex(text, (last ? last->next : text->first));
    if (textopt->normalize)
	compress_spacing(text, (last ? last->next : text->first));
    if (textopt->case_insensitive)
	lowercase(last ? last->next : text->first);
}
/**********************************************************************/

void char_to_string(suspect, value, string, fake_newline)
Boolean suspect;
Charvalue value;
char *string;
Boolean fake_newline;
{
    short i = 0;
    if (suspect)
	string[i++] = SUSPECT_MARKER;
    if (value == NEWLINE)
	sprintf(&string[i], (fake_newline ? "<\\n>" : "\n"));
    else if (value >= BLANK && value <= REJECT_CHARACTER ||
    value > 0xA0 && value <= 0xFF)
	sprintf(&string[i], "%c", value);
    else if (value < 256)
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
