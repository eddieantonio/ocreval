/**********************************************************************
 *
 *  stopword.c
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

#include "stopword.h"
#include "table.h"
#include "word.h"

BEGIN_ENTRY(Stopword)
END_ENTRY(Stopword);

BEGIN_TABLE_OF(Stopword, Stopwordlist)
END_TABLE(Stopwordtable);
static Stopwordtable stopwordtable;
static Boolean initialized = False;

static Textopt textopt = { False, False, 0, True, True, True };
static Text text;
static Wordlist wordlist;

static char *default_stopword[] =
{
"a",		"about",	"after",	"all",		"also",
"an",		"and",		"any",		"are",		"as",
"at",		"back",		"be",		"because",	"been",
"but",		"by",		"can",		"could",	"did",
"do",		"does",		"down",		"each",		"first",
"for",		"from",		"get",		"good",		"had",
"has",		"have",		"he",		"her",		"him",
"his",		"how",		"i",		"if",		"in",
"into",		"is",		"it",		"its",		"just",
"know",		"like",		"little",	"long",		"made",
"make",		"man",		"many",		"may",		"me",
"more",		"most",		"my",		"new",		"no",
"not",		"now",		"of",		"on",		"one",
"only",		"or",		"other",	"our",		"out",
"over",		"said",		"same",		"see",		"she",
"so",		"some",		"than",		"that",		"the",
"their",	"them",		"then",		"there",	"these",
"they",		"this",		"to",		"too",		"two",
"up",		"us",		"used",		"very",		"was",
"way",		"we",		"were",		"what",		"when",
"where",	"which",	"who",		"why",		"will",
"with",		"woman",	"would",	"you",		"your"
};

/**********************************************************************/

static void save_stopword(key)
char *key;
{
    Stopword *stopword;
    stopword = table_lookup(&stopwordtable, key);
    if (stopword)
	warning_string("duplicate stopword", key);
    else
    {
	stopword = NEW(Stopword);
	stopword->key = key;
	table_insert(&stopwordtable, stopword);
    }
}
/**********************************************************************/

void init_stopwords(filename)
char *filename;
{
    Word *word;
    short i;
    if (initialized)
	error("stopwords already initialized");
    if (filename)
    {
	read_text(&text, filename, &textopt);
	find_words(&wordlist, &text);
	list_empty(&text, free);
	for (word = wordlist.first; word; word = word->next)
	    save_stopword(word->string);
    }
    else
	for (i = 0; i < sizeof(default_stopword) / sizeof(char *); i++)
	    save_stopword(default_stopword[i]);
    initialized = True;
}
/**********************************************************************/

Boolean is_stopword(string)
unsigned char *string;
{
    if (!initialized)
	error("stopwords not initialized");
    return(table_lookup(&stopwordtable, string) ? True : False);
}
