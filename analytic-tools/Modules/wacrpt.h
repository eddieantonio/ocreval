/**********************************************************************
 *
 *  wacrpt.h
 *
 *  This module provides support for reading and writing word accuracy
 *  reports.  The contents of one of these reports is represented by a
 *  "Wacdata" structure.
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

#ifndef _WACRPT_
#define _WACRPT_

#include "table.h"
#include "word.h"

#define MAX_OCCURRENCES  10
#define MAX_PHRASELENGTH  8

typedef
struct
{
    long count;		/* number of word occurrences */
    long missed;	/* number of these that were misrecognized */
} Wac;

BEGIN_ENTRY(Term)
    Wac wac;
END_ENTRY(Term);	/* a distinct word */

BEGIN_TABLE_OF(Term, Termlist)
END_TABLE(Termtable);	/* table of distinct words */

typedef
struct
{
    Wac total;		/* all words */
    Wac stopword[MAX_WORDLENGTH + 1];
			/* stopwords by word length (in characters); the total
			   for all stopwords is in the 0th element */
    Wac non_stopword[MAX_WORDLENGTH + 1];
			/* non-stopwords by word length (in characters); the
			   total for all non-stopwords is in the 0th element */
    Wac distinct_non_stopword[MAX_OCCURRENCES + 2];
			/* distinct non-stopwords by number of occurrences on
			   a page; the (MAX_OCCURRENCES + 1) element groups
			   all occurring more than MAX_OCCURRENCES times; the
			   total for all distinct non-stopwords is in the 0th
			   element */
    Wac phrase[MAX_PHRASELENGTH + 1];
			/* phrases by phrase length (in words); the 0th element
			   is not used */
    Termtable stopword_table;
			/* table of distinct stopwords */
    Termtable non_stopword_table;
			/* table of distinct non-stopwords */
} Wacdata;

void increment_wac(/* Wac *wac, long count, long missed */);
			/* adds "count" and "missed" to the respective fields
			   of "wac" */

void add_term(/* Termtable *termtable, char *key, long count, long missed */);
			/* adds the given word to "termtable"; "key" contains
			   the character string representation of the word;
			   a copy of this string is stored in the table */

void read_wacrpt(/* Wacdata *wacdata, char *filename */);
			/* reads the named file (or stdin if "filename" is NULL)
			   and adds its contents to "wacdata"; reports an error
			   and quits if unable to open the file, or if the file
			   does not contain a word accuracy report */

void write_wacrpt(/* Wacdata *wacdata, char *filename */);
			/* writes the contents of "wacdata" to the named file
			   (or stdout if "filename" is NULL); reports an error
			   and quits if unable to create the file */

#endif
