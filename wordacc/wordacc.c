/**********************************************************************
 *
 *  wordacc.c
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
#include "wacrpt.h"

#define usage  "[-S stopwordfile] correctfile generatedfile [wordacc_report]"

char *stopwordfilename;

Option option[] =
{
    'S', &stopwordfilename, NULL,
    '\0'
};

Textopt textopt = { True, True, 0, True, True, True };
Text text[2];

Wordlist wordlist[2];

BEGIN_ENTRY(Id)
    Boolean found[2];
END_ENTRY(Id);

BEGIN_TABLE_OF(Id, Idlist)
END_TABLE(Idtable);
Idtable idtable;

typedef
struct
{
    Id *id;
    Boolean recognized;
} Symbol;

Symbol *symbol[2], **a, **b;

long m, n, min_k, max_k;

typedef unsigned short F;
#define MAX_F  65535

BEGIN_ITEM(Path)
    long p;
    F *f;
END_ITEM(Path);

BEGIN_LIST_OF(Path)
END_LIST(Pathlist);
Pathlist pathlist;

Wacdata wacdata;

/**********************************************************************/

Symbol **setup_array(index, length)
long index, *length;
{
    Symbol **array;
    long i, j = 0;
    array = NEW_ARRAY(wordlist[index].count + 1, Symbol *);
    for (i = 0; i < wordlist[index].count; i++)
	if (symbol[index][i].id->found[1 - index])
	    array[j++] = &symbol[index][i];
    *length = j;
    return(array);
}
/**********************************************************************/

void setup(filename)
char *filename[];
{
    long i, j;
    Word *word;
    Id *id;
    for (i = 0; i < 2; i++)
    {
	read_text(&text[i], filename[i], &textopt);
	if (i == 0 && textopt.found_header)
	    error("no correct file specified");
	find_words(&wordlist[i], &text[i]);
	symbol[i] = NEW_ARRAY(wordlist[i].count + 1, Symbol);
	j = 0;
	for (word = wordlist[i].first; word; word = word->next)
	{
	    id = table_lookup(&idtable, word->string);
	    if (!id)
	    {
		id = NEW(Id);
		id->key = (char *) word->string;
		table_insert(&idtable, id);
	    }
	    id->found[i] = True;
	    symbol[i][j++].id = id;
	}
    }
    a = setup_array(0, &m);
    if (m > MAX_F)
	error("text stream is too long");
    b = setup_array(1, &n);
}
/**********************************************************************/

long initial_f(k, prev_path, prev_k)
long k, *prev_k;
Path *prev_path;
{
    long value, result = 0;
    if (prev_path)
    {
	if (k > -prev_path->p)
	{
	    result = prev_path->f[(k - 1 + prev_path->p) >> 1];
	    *prev_k = k - 1;
	}
	if (k < prev_path->p)
	{
	    value = prev_path->f[(k + 1 + prev_path->p) >> 1] + 1;
	    if (value >= result)
	    {
		result = value;
		*prev_k = k + 1;
	    }
	}
    }
    return(result);
}
/**********************************************************************/

void compute_f(k, path)
long k;
Path *path;
{
    long i, j, value;
    i = initial_f(k, path->prev, &value);
    j = i + k;
    while (i < m && j < n && a[i]->id == b[j]->id)
    {
	i++;
	j++;
    }
    if (i == m)
	min_k = k + 1;
    if (j == n)
	max_k = k - 1;
    path->f[(k + path->p) >> 1] = i;
}
/**********************************************************************/

void compute_pathlist()
{
    long p = -1, k;
    Path *path;
    min_k = -m;
    max_k = n;
    while (min_k <= n - m)
    {
	path = NEW(Path);
	path->p = ++p;
	path->f = NEW_ARRAY(p + 1, F);
	list_insert_last(&pathlist, path);
	k = -p;
	while (k <= p)
	{
	    if (k >= min_k && k <= max_k)
		compute_f(k, path);
	    k += 2;
	}
    }
}
/**********************************************************************/

void obtain_matches()
{
    long k, f, start, prev_k;
    Path *path;
    k = n - m;
    for (path = pathlist.last; path; path = path->prev)
    {
	f = path->f[(k + path->p) >> 1];
	start = initial_f(k, path->prev, &prev_k);
	while (f > start)
	    a[--f]->recognized = True;
	k = prev_k;
    }
}
/**********************************************************************/

void process_terms(termtable, length, occurs)
Termtable *termtable;
Wac length[], occurs[];
{
    long i, count, missed;
    table_in_array(termtable);
    for (i = 0; i < termtable->count; i++)
    {
	count  = termtable->array[i]->wac.count;
	missed = termtable->array[i]->wac.missed;
	increment_wac(&wacdata.total, count, missed);
	increment_wac(&length[0], count, missed);
	increment_wac(&length[strlen(termtable->array[i]->key)], count, missed);
	if (occurs)
	{
	    increment_wac(&occurs[0], 1, (count == missed ? 1 : 0));
	    increment_wac(&occurs[min(count, MAX_OCCURRENCES + 1)], 1,
	    (count == missed ? 1 : 0));
	}
    }
}
/**********************************************************************/

void process_phrases()
{
    long i, j;
    Boolean recognized;
    for (i = 0; i < wordlist[0].count; i++)
    {
	recognized = True;
	for (j = 0; j < MAX_PHRASELENGTH && i + j < wordlist[0].count; j++)
	{
	    recognized &= symbol[0][i + j].recognized;
	    increment_wac(&wacdata.phrase[j + 1], 1, (recognized ? 0 : 1));
	}
    }
}
/**********************************************************************/

void determine_wacdata()
{
    long i;
    for (i = 0; i < wordlist[0].count; i++)
	add_term((is_stopword(symbol[0][i].id->key) ?
	&wacdata.stopword_table : &wacdata.non_stopword_table),
	symbol[0][i].id->key, 1, (symbol[0][i].recognized ? 0 : 1));
    process_terms(&wacdata.stopword_table, wacdata.stopword, NULL);
    process_terms(&wacdata.non_stopword_table, wacdata.non_stopword,
    wacdata.distinct_non_stopword);
    process_phrases();
}
/**********************************************************************/

main(argc, argv)
int argc;
char *argv[];
{
    initialize(&argc, argv, usage, option);
    if (argc < 2 || argc > 3)
	error("invalid number of files");
    init_stopwords(stopwordfilename);
    setup(argv);
    compute_pathlist();
    obtain_matches();
    determine_wacdata();
    write_wacrpt(&wacdata, (argc == 3 ? argv[2] : NULL));
    terminate();
}
