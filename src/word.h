/**********************************************************************
 *
 *  word.h
 *
 *  This module provides definitions and routines pertaining to words.
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

#ifndef _WORD_
#define _WORD_

#include "text.h"

#define MAX_WORDLENGTH  50

BEGIN_ITEM(Word)
    char *string;
                        /* character string representation of the word */
END_ITEM(Word);         /* an occurrence of a word */

BEGIN_LIST_OF(Word)
END_LIST(Wordlist);     /* a list of word occurrences */

void find_words(/* Wordlist *wordlist, Text *text */);
                        /* finds the word occurrences in "text" and appends
                           them to "wordlist" in sequence; all letters in
                           "text" are assumed to be in lowercase */

void free_word(/* Word *word */);
                        /* de-allocates a Word structure */

#endif
