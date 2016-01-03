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

static Boolean is_wordchar(value)
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

/*
 * Implements the Unicode TR29 Word Boundary Rules:
 * http://unicode.org/reports/tr29/#Word_Boundary_Rules
 */
static Boolean forms_boundary(left_char, right_char)
    Char *left_char, *right_char;
{
    /* Break at the start and end of text. */
    /* WB1, WB2 */
    if (left_char == START_OF_TEXT || right_char == END_OF_TEXT) {
        return True;
    }

    assert(right_char != START_OF_TEXT);
    assert(left_char != END_OF_TEXT);

    wb_property left = property(left_char->value),
                right = property(right_char->value);

    /* Do not break within CRLF. */
    /* WB3 */
    if (left == CR && right == LF) return False;
    /* Otherwise break before and after Newlines (including CR and LF) */
    /* WB3a */
    if (left == Newline || left == LF || left == CR) return True;
    /* WB3b */
    if (right == Newline || right == LF || right == CR) return True;

    /* Ignore Format and Extend characters, except when they appear at the
     * beginning of a region of text. */
    /* WB4: TODO: not implemented! */

    /* Do not break between most letters. */
    /* WB5 */
    if (AHLetter(left) && AHLetter(right)) return false;

    /* Do not break letters across certain punctuation. */
    /* WB6: TODO: need a character look-ahead... */
    if (AHLetter(left) && (right == MidLetter ||
                           MidNumLetQ(right))) return false;
    /* WB7 */
    if ((left == MidLetter ||
         MidNumLetQ(left)) && AHLetter(right)) return false;
    /* WB7 */
    if (left == Hebrew_Letter && right == Single_Quote) return false;
    /* WB7b: TODO: not implemented */
    if (left == Hebrew_Letter && right == Single_Quote) return false;
    /* WB7c: TODO: not implemented */
    if (left == Double_Quote && right == Hebrew_Letter) return false;

    /* Do not break within sequences of digits, or digits adjacent to letters
     * (“3a”, or “A3”). */
    /* WB8 */
    if (left == Numeric && right == Numeric) return false;
    /* WB9 */
    if (AHLetter(left) && right == Numeric) return false;
    /* WB10 */
    if (left == Numeric && AHLetter(right)) return false;

    /* Do not break within sequences, such as “3.2” or “3,456.789”. */
    /* WB11: TODO: need look-behind. */
    if ((left == MidNum ||
         MidNumLetQ(left)) && right == Numeric) return false;
    /* WB12: TODO: need look-ahead. */
    if (left == Numeric && (right == MidNum ||
                            MidNumLetQ(right))) return false;

    /* Do not break between Katakana. */
    /* WB13 */
    if (left == Katakana || right == Katakana) return false;

    /* Do not break from extenders. */
    /* WB13a */
    if ((AHLetter(left) ||
         left == Numeric ||
         left == Katakana ||
         left == ExtendNumLet) && right == ExtendNumLet) return false;
    /* WB13b */
    if (left == ExtendNumLet && (AHLetter(right) ||
                                 right == Numeric ||
                                 right == Katakana)) return false;

    /* Do not break between regional indicator symbols. */
    /* WB13c */
    if (left == Regional_Indicator && right == Regional_Indicator) return false;

    /* Otherwise, break everywhere (including around ideographs). */
    /* WB14 */
    return true;
}
/**********************************************************************/

static void append_word(buffer, len, wordlist)
    char* buffer;
    long len;
    Wordlist *wordlist;
{
    Word *word;

    word = NEW(Word);
    /* Null-terminate the buffer. */
    buffer[len] = '\0';
    word->string = strdup(buffer);
    list_insert_last(wordlist, word);
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
    char string[MAX_WORDLENGTH + 1];
    long len = 0;
    Char *last = START_OF_TEXT;
    Char *current;
    Boolean in_word = false;
    Boolean has_boundary = false;

    for (current = text->first; current != NULL; last = current, current = current->next)
    {
        has_boundary = forms_boundary(last, current);

        if (!in_word && has_boundary) {
            if (is_wordchar(current->value)) {
                in_word = true;
            }
        } else if (has_boundary /* && in word */) {
            in_word = false;
        }

        if (in_word)
        {
            if (len + 3 < MAX_WORDLENGTH) {
                len += encode_or_die(current->value, &string[len]);
            }
        }
        else if (len > 0)
        {
            append_word(string, len, wordlist);
            len = 0;
        }
    }

    if (len > 0) {
        append_word(string, len, wordlist);
    }

    fflush(stderr);
}
/**********************************************************************/

void free_word(word)
    Word *word;
{
    free(word->string);
    free(word);
}
