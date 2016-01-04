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

/* Special constants for start and end of text. */
#define START_OF_TEXT ((Char *) 0x01)
#define END_OF_TEXT   (NULL)

#define AHLetter(prop) (prop == ALetter || prop == Hebrew_Letter)
#define MidNumLetQ(prop)  (prop == MidNumLet || prop == Single_Quote)

/**********************************************************************/

/* Recursive binary search for the propery. */
static wb_property search_for_property(code_point, left, right)
    Charvalue code_point;
    size_t left, right;
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

static wb_property property(code_point)
    Charvalue code_point;
{
    const size_t len = sizeof(WORD_BREAK_PROPERTY) / sizeof(wb_range);
    return search_for_property(code_point, 0, len - 1);
}
/**********************************************************************/

/*
 * Implements the Unicode TR29 Word Boundary Rules:
 * http://unicode.org/reports/tr29/#Word_Boundary_Rules
 */
static Char* find_next_boundary(start)
    Char *start;
{
    return NULL;
}
/**********************************************************************/

static size_t find_utf8_length(start, after)
    Char *start, *after;
{
    Char *current;
    size_t len = 0, cplen = 0;
    utf8proc_uint8_t dummy_buffer[4];


    for (current = start; current != after; current = current->next) {
        assert(current != NULL);
        /* Do a dummy encoding of the character for the side-effect of
         * returning its length. */
        cplen = utf8proc_encode_char(current->value, dummy_buffer);
        assert(cplen > 0);

        len += cplen;
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
    size_t result;
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
        case UTF8PROC_CATEGORY_ND: /**< Number, decimal digit */
        case UTF8PROC_CATEGORY_NL: /**< Number, letter */
        case UTF8PROC_CATEGORY_NO: /**< Number, other */
        case UTF8PROC_CATEGORY_PC: /**< Punctuation, connector */
        case UTF8PROC_CATEGORY_SC: /**< Symbol, currency */
        case UTF8PROC_CATEGORY_SK: /**< Symbol, modifier */
        case UTF8PROC_CATEGORY_SO: /**< Symbol, other */
        case UTF8PROC_CATEGORY_MN : /**< Mark, nonspacing */
        case UTF8PROC_CATEGORY_MC : /**< Mark, spacing combining */
        case UTF8PROC_CATEGORY_ME : /**< Mark, enclosing */
            /* Allow for private use characters. */
        case UTF8PROC_CATEGORY_CO: /**< Other, private use */
            return True;
        default:
            return False;
    }
}
/**********************************************************************/

/* TODO:
 * Use find_next_boundary() instead.
 *  - [ ] implement FSM
 *  - [ ] traverse the linked list
 *  - [ ] convert the linked list from one node to another into a UTF-8 string
 *      - [ ] Write tests for this
 *      - [ ] Convert to NFC for comparison purposes.
 */
void find_words(wordlist, text)
    Wordlist *wordlist;
    Text *text;
{
    Char *current, *first = text->first;
    current = first;

    while (current->next != NULL) {
        current = current->next;
    }

    append_word(wordlist, first, current);
}
/**********************************************************************/

void free_word(word)
    Word *word;
{
    free(word->string);
    free(word);
}
