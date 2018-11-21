/**********************************************************************
 *
 *  wacrpt.c
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

#include "sort.h"
#include "wacrpt.h"
#include "ocreval_version.h"

#define TITLE    "ocreval Word Accuracy Report Version " OCREVAL_VERSION "\n"
#define DIVIDER  "----------------------------------------\n"

#define TOTAL    " Total\n"
#define OFFSET   29

static char line[100];

/**********************************************************************/

void increment_wac(wac, count, missed)
Wac *wac;
long count, missed;
{
    wac->count  += count;
    wac->missed += missed;
}
/**********************************************************************/

void add_term(termtable, key, count, missed)
Termtable *termtable;
char *key;
long count, missed;
{
    Term *term;
    term = table_lookup(termtable, key);
    if (!term)
    {
	term = NEW(Term);
	term->key = strdup(key);
	table_insert(termtable, term);
    }
    increment_wac(&term->wac, count, missed);
}
/**********************************************************************/

static Boolean read_line(f)
FILE *f;
{
    return(fgets(line, sizeof(line) - 1, f) ? True : False);
}
/**********************************************************************/

static Boolean read_one(f, value)
FILE *f;
long *value;
{
    return(read_line(f) && sscanf(line, "%ld", value) == 1 ? True : False);
}
/**********************************************************************/

static Boolean read_two(f, value1, value2)
FILE *f;
long *value1, *value2;
{
    return(read_line(f) && sscanf(line, "%ld %ld", value1, value2) == 2 ?
    True : False);
}
/**********************************************************************/

static long read_numbers(f, wac)
FILE *f;
Wac wac[];
{
    long count, missed, index, total_count = 0;
    if (read_line(f) && read_line(f))
	while (read_two(f, &count, &missed))
	{
	    index = atoi(&line[OFFSET]);
	    if (index == 0) {
		if (strcmp(&line[OFFSET], TOTAL) == 0)
		    total_count = count;
		else /* excess */
		    index = MAX_OCCURRENCES + 1;
	    }
	    increment_wac(&wac[index], count, missed);
	}
    return(total_count);
}
/**********************************************************************/

static void read_terms(f, termtable)
FILE *f;
Termtable *termtable;
{
    long count, missed;
    if (read_line(f) && read_line(f))
	while (read_two(f, &count, &missed))
	{
	    line[strlen(line) - 1] = '\0';
	    add_term(termtable, &line[OFFSET], count, missed);
	}
}
/**********************************************************************/

void read_wacrpt(wacdata, filename)
Wacdata *wacdata;
char *filename;
{
    FILE *f;
    long words, missed, stopwords, non_stopwords;
    f = open_file(filename, "r");
    if (read_line(f) && strncmp(line, TITLE, sizeof(TITLE) - 3) == 0 &&
    read_line(f) && strcmp(line, DIVIDER) == 0 &&
    read_one(f, &words) && read_one(f, &missed) &&
    read_line(f) && read_line(f))
    {
	increment_wac(&wacdata->total, words, missed);
	stopwords = read_numbers(f, wacdata->stopword);
	non_stopwords = read_numbers(f, wacdata->non_stopword);
	read_numbers(f, wacdata->distinct_non_stopword);
	if (words > 0)
	{
	    read_numbers(f, wacdata->phrase);
	    if (stopwords > 0)
		read_terms(f, &wacdata->stopword_table);
	    if (non_stopwords > 0)
		read_terms(f, &wacdata->non_stopword_table);
	}
    }
    else
	error_string("invalid format in", (filename ? filename : "stdin"));
    close_file(f);
}
/**********************************************************************/

static void write_pct(f, wac)
FILE *f;
Wac *wac;
{
    if (wac->count == 0)
	fputs("  ------", f);
    else
	fprintf(f, "%8.2f", 100.0 * (wac->count - wac->missed) / wac->count);
}
/**********************************************************************/

static void write_wac(f, wac)
FILE *f;
Wac *wac;
{
    if (wac)
    {
	fprintf(f, "%8ld %8ld ", wac->count, wac->missed);
	write_pct(f, wac);
    }
    else
	fputs("   Count   Missed   %Right", f);
    fputs("   ", f);
}
/**********************************************************************/

static void write_numbers(f, wac, limit, title, excess, total)
FILE *f;
Wac wac[];
short limit;
char *title;
Boolean excess, total;
{
    short i;
    fprintf(f, "\n%s\n", title);
    write_wac(f, NULL);
    fprintf(f, "%s\n", (excess ? "Occurs" : "Length"));
    for (i = 1; i <= limit; i++)
	if (wac[i].count > 0)
	{
	    write_wac(f, &wac[i]);
	    fprintf(f, "    %2d\n", i);
	}
    if (excess && wac[limit + 1].count > 0)
    {
	write_wac(f, &wac[limit + 1]);
	fprintf(f, "   >%2d\n", limit);
    }
    if (total)
    {
	write_wac(f, &wac[0]);
	fputs(TOTAL, f);
    }
}
/**********************************************************************/

static int compare_term(term1, term2)
Term *term1, *term2;
{
    return(ustrcmp(term1->key, term2->key));
}
/**********************************************************************/

static void write_terms(f, termtable, title)
FILE *f;
Termtable *termtable;
char *title;
{
    long i;
    table_in_array(termtable);
    sort(termtable->count, termtable->array, compare_term);
    fprintf(f, "\n%s\n", title);
    write_wac(f, NULL);
    fputc('\n', f);
    for (i = 0; i < termtable->count; i++)
    {
	write_wac(f, &termtable->array[i]->wac);
	fprintf(f, "%s\n", termtable->array[i]->key);
    }
}
/**********************************************************************/

void write_wacrpt(wacdata, filename)
Wacdata *wacdata;
char *filename;
{
    FILE *f;
    f = open_file(filename, "w");
    fprintf(f, "%s%s", TITLE, DIVIDER);
    fprintf(f, "%8ld   Words\n", wacdata->total.count);
    fprintf(f, "%8ld   Misrecognized\n", wacdata->total.missed);
    write_pct(f, &wacdata->total);
    fputs("%  Accuracy\n", f);
    write_numbers(f, wacdata->stopword, MAX_WORDLENGTH,
    "Stopwords", False, True);
    write_numbers(f, wacdata->non_stopword, MAX_WORDLENGTH,
    "Non-stopwords", False, True);
    write_numbers(f, wacdata->distinct_non_stopword, MAX_OCCURRENCES,
    "Distinct Non-stopwords", True, True);
    if (wacdata->total.count > 0)
    {
	write_numbers(f, wacdata->phrase, MAX_PHRASELENGTH,
	"Phrases", False, False);
	if (wacdata->stopword[0].count > 0)
	    write_terms(f, &wacdata->stopword_table, "Stopwords");
	if (wacdata->non_stopword[0].count > 0)
	    write_terms(f, &wacdata->non_stopword_table, "Non-stopwords");
    }
    close_file(f);
}
