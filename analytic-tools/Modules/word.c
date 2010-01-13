/**********************************************************************
 *
 *  word.c
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

#include "word.h"

/**********************************************************************/

static Boolean is_wordchar(value)
Charvalue value;
{
    return(value >= 'a' && value <= 'z' ||
    value >= 0xDF && value <= 0xFF && value != 0xF7 ? True : False);
}
/**********************************************************************/

void find_words(wordlist, text)
Wordlist *wordlist;
Text *text;
{
    unsigned char string[MAX_WORDLENGTH + 1];
    long len = 0, i;
    Charvalue value;
    Word *word;
    list_in_array(text);
    for (i = 0; i < text->count || len > 0; i++)
    {
	value = (i < text->count ? text->array[i]->value : 0);
	if (is_wordchar(value))
	{
	    if (len < MAX_WORDLENGTH)
		string[len++] = value;
	}
	else if (len > 0)
	{
	    word = NEW(Word);
	    string[len] = '\0';
	    word->string = (unsigned char *) strdup(string);
	    list_insert_last(wordlist, word);
	    len = 0;
	}
    }
}
/**********************************************************************/

void free_word(word)
Word *word;
{
    free(word->string);
    free(word);
}
