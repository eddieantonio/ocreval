/**********************************************************************
 *
 *  text.c
 *
 *  Author: Stephen V. Rice
 *  Author: Eddie Antonio Santos
 *
 * Copyright 2016 Eddie Antonio Santos.
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
#include "ocreval_version.h"

#define TITLE    "ocreval Vendor-Independent Interface Version " OCREVAL_VERSION "\n"
#define DIVIDER  "------------------------------------------------\n"
/* Combining characters often use this character as a placeholder base
 * character: e.g., ◌̉ */
#define DOTTED_CIRCLE 0x25CC

/*
 * Assert that the internal character size is the same size as UTF-32.
 * (C89 static assert.)
 */
static char assert_sizes[sizeof(Charvalue) == sizeof(utf8proc_int32_t) ? 1 : -1];

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
	size_t code_unit_size;
	utf8proc_int32_t code_point;
	byte = getc(f);

	while (byte != EOF)
	{
		code_unit_size = utf8proc_utf8class[byte];
		if (code_unit_size < 1) {
			/* Encoding error! Stream is not UTF-8! */
			error("invalid UTF-8 character");
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
				error("unexpected end of input");
			}
		}

		/* Decode a single code unit -> code point. */
		decode_status = utf8proc_iterate(buffer, sizeof(buffer), &code_point);
		if (decode_status != code_unit_size) {
			error(utf8proc_errmsg(decode_status));
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

/**
 * Blank characters EXCEPT the newline!
 */
static Boolean is_blank(character)
	utf8proc_int32_t character;
{
	const utf8proc_property_t * props = utf8proc_get_property(character);

	/* Cases from ISRI < 6.0 */
	switch (character) {
		case NEWLINE:
			/* Special case: a newline is a NEWLINE; not a BLANK. */
			return False;

		case '\t':
		case LINE_TABULATION:
		case FORM_FEED:
		case CARRIAGE_RETURN:
		case NON_BREAKING_SPACE:
			return True;
	}

	/* See: http://www.unicode.org/versions/Unicode8.0.0/ch04.pdf */
	switch (props->category) {
		case UTF8PROC_CATEGORY_ZS:  /* Space characters. */
		case UTF8PROC_CATEGORY_ZL:  /* Line separators. */
		case UTF8PROC_CATEGORY_ZP:  /* Paragraph separators. */
			return True;
	}

	return False;
}


/**********************************************************************/

static void compress_spacing(text, start)
	Text *text;
	Char *start;
{
	Boolean found_non_blank = False;
	Char *c, *next;

	/* Normalize all blank-y charcaters to BLANK. */
	for (c = start; c; c = c->next)
		if (is_blank(c->value))
			c->value = BLANK;

	c = start;
	while (c)
	{
		next = c->next;
		if ((c->value == BLANK && (!found_non_blank || !next ||
						next->value == BLANK || next->value == NEWLINE)) ||
				(c->value == NEWLINE && !found_non_blank))
		{
			if (found_non_blank && next && next->value == BLANK)
				next->suspect |= c->suspect;
			list_remove(text, c);
			free(c);
		} else {
			found_non_blank = (c->value == NEWLINE ? False : True);
		}

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
		error_string("invalid call to", "read_text");
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
signed char encode_or_die(codepoint, buffer)
	utf8proc_int32_t codepoint;
	utf8proc_uint8_t *buffer;
{
	utf8proc_ssize_t status = utf8proc_encode_char(codepoint, buffer);
	if (status < 1) {
		error("could not encode UTF-8");
	}

	return status;
}
/**********************************************************************/

/**
 * Is this generally considered to be a graphic character?
 */
static Boolean is_graphic_character(character)
	utf8proc_int32_t character;
{
	const utf8proc_property_t * props = utf8proc_get_property(character);

	/* Special case the space character. */
	if (character == ' ') {
	    return true;
	}

	switch (props->category) {
		case UTF8PROC_CATEGORY_LU: /**< Letter, uppercase */
		case UTF8PROC_CATEGORY_LL: /**< Letter, lowercase */
		case UTF8PROC_CATEGORY_LT: /**< Letter, titlecase */
		case UTF8PROC_CATEGORY_LM: /**< Letter, modifier */
		case UTF8PROC_CATEGORY_LO: /**< Letter, other */
		case UTF8PROC_CATEGORY_MN: /**< Mark, nonspacing */
		case UTF8PROC_CATEGORY_MC: /**< Mark, spacing combining */
		case UTF8PROC_CATEGORY_ME: /**< Mark, enclosing */
		case UTF8PROC_CATEGORY_ND: /**< Number, decimal digit */
		case UTF8PROC_CATEGORY_NL: /**< Number, letter */
		case UTF8PROC_CATEGORY_NO: /**< Number, other */
		case UTF8PROC_CATEGORY_PC: /**< Punctuation, connector */
		case UTF8PROC_CATEGORY_PD: /**< Punctuation, dash */
		case UTF8PROC_CATEGORY_PS: /**< Punctuation, open */
		case UTF8PROC_CATEGORY_PE: /**< Punctuation, close */
		case UTF8PROC_CATEGORY_PI: /**< Punctuation, initial quote */
		case UTF8PROC_CATEGORY_PF: /**< Punctuation, final quote */
		case UTF8PROC_CATEGORY_PO: /**< Punctuation, other */
		case UTF8PROC_CATEGORY_SM: /**< Symbol, math */
		case UTF8PROC_CATEGORY_SC: /**< Symbol, currency */
		case UTF8PROC_CATEGORY_SK: /**< Symbol, modifier */
		case UTF8PROC_CATEGORY_SO: /**< Symbol, other */
			/* Allow for private use characters. */
		case UTF8PROC_CATEGORY_CO: /**< Other, private use */
			return True;
		default:
			return False;
	}
}
/**********************************************************************/

/**
 * Is this a combining character?
 *
 */
static Boolean is_combining_character(character)
	utf8proc_int32_t character;
{
    switch (utf8proc_category(character)) {
        case UTF8PROC_CATEGORY_MN:
        case UTF8PROC_CATEGORY_MC:
        case UTF8PROC_CATEGORY_ME:
            return True;
        default:
            return False;
    }
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

	if (value == NEWLINE) {
		sprintf(&string[i], (fake_newline ? "<\\n>" : "\n"));
	} else if (is_graphic_character(value)) {
		if (is_combining_character(value)) {
			i += encode_or_die(DOTTED_CIRCLE, &ustring[i]);
		}

		/* It's a printable character. */
		i += encode_or_die(value, &ustring[i]);
		string[i++] = '\0';
	} else if (value < 256) {
		sprintf(&string[i], "<%02X>", value);
	} else {
		sprintf(&string[i], "<%04X>", value);
	}
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

/**********************************************************************/

Boolean cstring_to_text(text, string)
	Text* text;
	const char *string;
{
	const char *c = string;
	int decode_status = 0;
	utf8proc_int32_t code_point = 0;

	for (c = string; *c != '\0'; c += decode_status) {
		/* Try to read a UTF-8 code unit... */
		decode_status = utf8proc_iterate((utf8proc_uint8_t *) c, -1, &code_point);

		if (decode_status < 1) {
			return false;
		}

		append_char(text, false, code_point);
	}

	return true;
}
