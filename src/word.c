/**********************************************************************
 *
 *  word.c
 *
 *  Author: Stephen V. Rice
 *  Author: Eddie Antonio Santos
 *
 * Copyright 2016 Eddie Antonio Santos
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

#include <assert.h>
#include <string.h>

#include <utf8proc.h>
#include "word.h"

#include "word_break_property.h"

/* Macros defined in http://unicode.org/reports/tr29/#Word_Boundary_Rules */
#define AHLetter(prop)       ((prop) == ALetter || (prop) == Hebrew_Letter)
#define MidNumLetQ(prop)     ((prop) == MidNumLet || (prop) == Single_Quote)

/* Macro used in WB4 */
#define ExtendOrFormat(prop) ((prop) == Extend || (prop) == Format)

/**********************************************************************/

/* Recursive binary search for the property. */
static wb_property search_for_property(code_point, left, right)
    Charvalue code_point;
    int left, right;
{

    /* Not found, therefore not specified. */
    if (right < left) {
        return Other;
    }

    const size_t midpoint = left + (right - left) / 2;
    const wb_range *middle = &WORD_BREAK_PROPERTY[midpoint];

    if (code_point < middle->start) {
        return search_for_property(code_point, left, midpoint - 1);
    } else if (code_point > middle->end) {
        return search_for_property(code_point, midpoint + 1, right);
    } else {
        assert(code_point >= middle->start && code_point <= middle->end);
        return middle->value;
    }
}
/**********************************************************************/

/* Returns the word break property for a Unicode code point. */
__attribute__ ((const))
static wb_property property(code_point)
    Charvalue code_point;
{
    const size_t len = sizeof(WORD_BREAK_PROPERTY) / sizeof(wb_range);
    return search_for_property(code_point, 0, len - 1);
}
/**********************************************************************/

__attribute__ ((pure))
static wb_property char_property(node)
    const Char *node;
{
    if (node == NULL) {
        return eot;
    }
    return property(node->value);
}

/* Returns the next character, skipping Extend and Format characters.
 * WB4: Skip over Extend and Format characters.  */
__attribute__ ((pure))
static Char* skip_to_next(from)
    Char *from;
{
    if (from == NULL) {
        return NULL;
    }

    do {
        from = from->next;
    } while (from != NULL && ExtendOrFormat(char_property(from)));
    return from;
}

__attribute__ ((pure))
static Char* skip_twice(from)
    Char *from;
{
    return skip_to_next(skip_to_next(from));
}

/* Returns the last code point of a grapheme, including extend and format
 * characters.  */
static Char* skip_to_end_of_extend(from)
    Char *from;
{
    if (from == NULL) {
        return NULL;
    }

    /* Skip until the LAST extend or format character. */
    while (from->next != NULL && ExtendOrFormat(char_property(from->next))) {
        from = from->next;
    }

    return from;
}

/*
 * Returns the next Char* BEFORE the next boundary. Note that this may just
 * be the same character as was given.
 *
 * Implements the Unicode TR29 Word Boundary Rules:
 * http://unicode.org/reports/tr29/#Word_Boundary_Rules
 */
static Char* find_next_boundary(start)
    Char *start;
{
    /* The first thing the loop does is advance, so we need a dummy "start of
     * loop" character that does not participate in the search. */
    Char dummy;
    dummy.next = start;
    Char *current = &dummy;
    wb_property left = sot, right, lookahead, lookbehind;

    /* WB1: Break at the start and end of text. */
    if (start == NULL) {
        return NULL;
    }

    /* Loop to find the next word break. */

    /* WB2: Break at the start and end of text. */
    while (current->next != NULL) {
        /* Advance all the pointers. */
        current = skip_to_next(current);
        lookbehind = left;
        left = char_property(current);
        right = char_property(skip_to_next(current));
        lookahead = char_property(skip_twice(current));

        /* WB3: Do not break within CRLF. */
        if (left == CR && right == LF) continue;

        /* WB3a: Otherwise break before and after newlines */
        if (left == Newline || left == CR || left == LF) break;
        /* WB3b */
        if (right == Newline || right == CR || right == LF) break;

        /* Ignore Format and Extend characters, except when they appear at the
         * beginning of a region of text. */
        /* WB4: handled by skip_to_next() and skip_twice(). */

        /* WB5: Do not break between most letters. */
        if (AHLetter(left) && AHLetter(right)) continue;

        /* WB6: Do not break letters across certain punctuation. */
        if (AHLetter(left) &&
            (right == MidLetter || MidNumLetQ(right)) &&
            AHLetter(lookahead)) continue;
        /* WB7 */
        if (AHLetter(lookbehind) &&
            (left == MidLetter || MidNumLetQ(left)) &&
            AHLetter(right )) continue;
        /* WB7a */
        if (left == Hebrew_Letter && right == Single_Quote) continue;
        /* WB7b */
        if (left == Hebrew_Letter &&
            right == Double_Quote &&
            lookahead == Hebrew_Letter) continue;
        /* WB7b */
        if (lookbehind == Hebrew_Letter &&
            left == Double_Quote &&
            right == Hebrew_Letter) continue;

        /* WB8: Do not break within sequences of digits, or digits adjacent to
         * letters (“3a”, or “A3”). */
        if (left == Numeric && right == Numeric) continue;
        /* WB9 */
        if (AHLetter(left) && right == Numeric) continue;
        /* WB10 */
        if (left == Numeric && AHLetter(right)) continue;

        /* WB11: Do not break within sequences such as “3.2” or “3,456.789” */
        if (lookbehind == Numeric &&
            (left == MidNum || MidNumLetQ(left)) &&
            right == Numeric) continue;
        /* WB12 */
        if (left == Numeric &&
            (right == MidNum || MidNumLetQ(right)) &&
            lookahead == Numeric) continue;

        /* WB13: Do not break between Katakana. */
        if (left == Katakana && right == Katakana) continue;

        /* WB13a: Do not break from extenders. */
        if ((AHLetter(left) ||
             left == Numeric ||
             left == Katakana ||
             left == ExtendNumLet) &&
            right == ExtendNumLet) continue;
        /* WB13b */
        if (left == ExtendNumLet &&
            (AHLetter(right) ||
             right == Numeric ||
             right == Katakana ||
             right == ExtendNumLet)) continue;

        /* WB13c: Do not break between regional indicator symbols. */
        if (left == Regional_Indicator &&
            right == Regional_Indicator) continue;

        /* WB14: Otherwise, break everywhere (including around ideographs). */
        break;
    }

    return skip_to_end_of_extend(current);
}
/**********************************************************************/

static size_t find_utf8_length(start, after)
    const Char *start, *after;
{
    const Char *current;
    size_t len = 0, encoded_length = 0;
    utf8proc_uint8_t dummy_buffer[4];

    for (current = start; current != after; current = current->next) {
        assert(current != NULL);
        /* Do a dummy encoding of the character for the side-effect of
         * returning its length. */
        encoded_length = utf8proc_encode_char(current->value, dummy_buffer);
        assert(encoded_length > 0);

        len += encoded_length;
    }

    return len;
}

static void copy_into_buffer(buffer, start, after)
    char *buffer;
    Char *start, *after;
{
    Char *current;
    size_t i = 0;

    for (current = start; current != after; current = current->next) {
        assert(current != NULL);
        i += utf8proc_encode_char(current->value, (utf8proc_uint8_t*) &buffer[i]);
    }

    /* Null-terminate the buffer. */
    buffer[i] = '\0';
}
/**********************************************************************/


static void append_word(wordlist, start, end)
    Char *start, *end;
    Wordlist *wordlist;
{
    Word *word;
    Char *after = end->next;
    size_t len = find_utf8_length(start, after);
    char *buffer = malloc(len + 1);
    copy_into_buffer(buffer, start, after);

    word = NEW(Word);
    /* Convert the null-terminated string to NFC. */
    word->string = (char *) utf8proc_NFC((const utf8proc_uint8_t *) buffer);
    list_insert_last(wordlist, word);
    free(buffer);
}
/**********************************************************************/

static Boolean is_word_start(value)
    Charvalue value;
{
    const utf8proc_property_t * props = utf8proc_get_property(value);

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
            /* Allow for private use characters. */
        case UTF8PROC_CATEGORY_CO: /**< Other, private use */
            return True;
        default:
            return False;
    }
}
/**********************************************************************/

void find_words(wordlist, text)
    Wordlist *wordlist;
    Text *text;
{
    /* First and last characters of a segment. */
    Char *first = text->first, *last;

    while ((last = find_next_boundary(first)) != NULL) {
        /* Only append this segment if it starts with a "word-y" character. */
        if (is_word_start(first->value)) {
            append_word(wordlist, first, last);
        }

        /* Advance to the next segment. */
        first = last->next;
    }
}
/**********************************************************************/

void free_word(word)
    Word *word;
{
    free(word->string);
    free(word);
}
