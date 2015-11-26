/**********************************************************************
 *
 *  nonstopacc.c
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

#include "wacrpt.h"

#define usage  "stopwordfile wordacc_report >xyfile"

Textopt textopt = { False, False, 0, True, True, True };
Text text;
Wordlist wordlist;

Wacdata wacdata;

long count, missed;

/**********************************************************************/

void find_stopword(termtable, string)
Termtable *termtable;
char *string;
{
    Term *term;
    term = table_lookup(termtable, string);
    if (term)
    {
	count  -= term->wac.count;
	missed -= term->wac.missed;
    }
}
/**********************************************************************/

void write_line()
{
    static long linenum = 0;
    printf("%3ld %6.2f\n", linenum++, 100.0 * (count - missed) / count);
}
/**********************************************************************/

void write_results()
{
    Word *word;
    count  = wacdata.total.count;
    missed = wacdata.total.missed;
    if (count == 0)
	return;
    write_line();
    for (word = wordlist.first; word; word = word->next)
    {
	find_stopword(&wacdata.stopword_table, word->string);
	find_stopword(&wacdata.non_stopword_table, word->string);
	if (count == 0)
	    return;
	write_line();
    }
}
/**********************************************************************/

main(argc, argv)
int argc;
char *argv[];
{
    initialize(&argc, argv, usage, NULL);
    if (argc != 2)
	error("invalid number of files", Exit);
    read_text(&text, argv[0], &textopt);
    find_words(&wordlist, &text);
    read_wacrpt(&wacdata, argv[1]);
    write_results();
    terminate();
}
