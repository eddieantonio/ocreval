/**********************************************************************
 *
 *  ngram.c
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
#include "table.h"
#include "text.h"

#define usage  "[-n 1|2|3] textfile1 textfile2 ... >resultfile"

#define MAX_N  3
short n;
char *nstring;

Option option[] =
{
    'n', &nstring, NULL,
    '\0'
};

Textopt textopt = { True, True, 0, True, True };

Text text;

BEGIN_ENTRY(Sequence)
    Charvalue value[MAX_N];
    long count, suspect;
END_ENTRY(Sequence);

BEGIN_TABLE_OF(Sequence, Seqlist)
    long total_count, total_suspect;
END_TABLE(Seqtable);
Seqtable seqtable;

/**********************************************************************/

short get_n()
{
    if (!nstring)
	return(1);
    if (nstring[0] >= '1' && nstring[0] <= '0' + MAX_N && !nstring[1])
	return(nstring[0] - '0');
    error_string("invalid value", nstring);
}
/**********************************************************************/

void add_sequence(key, value, suspect)
char *key;
Charvalue value[];
Boolean suspect;
{
    Sequence *sequence;
    short i;
    sequence = table_lookup(&seqtable, key);
    if (!sequence)
    {
	sequence = NEW(Sequence);
	sequence->key = strdup(key);
	for (i = 0; i < n; i++)
	    sequence->value[i] = value[i];
	table_insert(&seqtable, sequence);
    }
    sequence->count++;
    seqtable.total_count++;
    if (suspect)
    {
	sequence->suspect++;
	seqtable.total_suspect++;
    }
}
/**********************************************************************/

void process_file(filename)
char *filename;
{
    Char *start, *c;
    char key[MAX_N * STRING_SIZE], string[STRING_SIZE];
    Boolean suspect;
    short i;
    Charvalue value[MAX_N];
    list_empty(&text, free);
    read_text(&text, filename, &textopt);
    for (start = text.first; start; start = start->next)
    {
	key[0] = '\0';
	suspect = False;
	for (i = 0, c = start; i < n; i++, c = c->next)
	{
	    if (!c)
		return;
	    char_to_string(False, c->value, string, True);
	    strcat(key, string);
	    value[i] = c->value;
	    if (c->suspect)
		suspect = True;
	}
	add_sequence(key, value, suspect);
    }
}
/**********************************************************************/

int order_by_value(sequence1, sequence2)
Sequence *sequence1, *sequence2;
{
    short i;
    for (i = 0; i < n && sequence1->value[i] == sequence2->value[i]; i++);
    if (i < n)
	return(sequence1->value[i] < sequence2->value[i] ? -1 : 1);
    return(0);
}
/**********************************************************************/

int order_by_count(sequence1, sequence2)
Sequence *sequence1, *sequence2;
{
    if (sequence1->count != sequence2->count)
	return(sequence2->count - sequence1->count);
    if (sequence1->suspect != sequence2->suspect)
	return(sequence2->suspect - sequence1->suspect);
    return(order_by_value(sequence1, sequence2));
}
/**********************************************************************/

void write_array()
{
    long i;
    printf("   Count  Suspect\n");
    for (i = 0; i < seqtable.count; i++)
	printf("%8ld %8ld   {%s}\n", seqtable.array[i]->count,
	seqtable.array[i]->suspect, seqtable.array[i]->key);
    printf("%8ld %8ld   Total\n", seqtable.total_count,
    seqtable.total_suspect);
}
/**********************************************************************/

void write_report()
{
    table_in_array(&seqtable);
    sort(seqtable.count, seqtable.array, order_by_value);
    write_array();
    printf("\n\n");
    sort(seqtable.count, seqtable.array, order_by_count);
    write_array();
}
/**********************************************************************/

main(argc, argv)
int argc;
char *argv[];
{
    int i;
    initialize(&argc, argv, usage, option);
    if (argc == 0)
	error("no text files specified");
    n = get_n();
    for (i = 0; i < argc; i++)
	process_file(argv[i]);
    write_report();
    terminate();
}
