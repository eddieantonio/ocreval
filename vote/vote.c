/**********************************************************************
 *
 *  vote.c
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
#include "sync.h"
#include "table.h"

#define usage  "[-O] [-o outputfile] [-s m/n] [-w m/n] textfile1 textfile2 ..."

Boolean debug, optimize;
char *outputfilename, *sfraction, *wfraction;

Option option[] =
{
    'D', NULL,            &debug,
    'O', NULL,            &optimize,
    'o', &outputfilename, NULL,
    's', &sfraction,      NULL,
    'w', &wfraction,      NULL,
    '\0'
};

Textopt textopt = { True, True, 0, True, True };

#define MIN_VOTERS   2
#define MAX_VOTERS  16

typedef
struct
{
    short argnum;
    char *filename;
    Text text;
    double distance;
} Voter;
Voter *voter[MAX_VOTERS];
short num_voters, actual_voters = 3;

#define N  2

short suspect_threshold;
short suspect_weight = 1, unmarked_weight = 1;

BEGIN_ENTRY(Sequence)
    long count[MAX_VOTERS];
    float median;
END_ENTRY(Sequence);

BEGIN_TABLE_OF(Sequence, Seqlist)
END_TABLE(Seqtable);
Seqtable seqtable;

Text input[MAX_VOTERS], output;

struct
{
    Char *c;
    short num_votes;
} candidate[MAX_VOTERS];
short num_candidates;

/**********************************************************************/

Boolean valid_fraction(fraction, m, n)
char *fraction;
short *m, *n;
{
    if (fraction[0] >= '1' && fraction[1] == '/' && fraction[2] <= '9' &&
    fraction[0] <= fraction[2] && !fraction[3])
    {
	*m = fraction[0] - '0';
	*n = fraction[2] - '0';
	return(True);
    }
    else
	return(False);
}
/**********************************************************************/

void validate_args(argc, argv)
int argc;
char *argv[];
{
    short i, m, n;
    if (argc < MIN_VOTERS || argc > MAX_VOTERS)
	error("invalid number of voters");
    for (i = 0; i < argc; i++)
    {
	voter[i] = NEW(Voter);
	voter[i]->argnum = i;
	voter[i]->filename = argv[i];
	read_text(&voter[i]->text, argv[i], &textopt);
    }
    num_voters = argc;
    if (!optimize || num_voters < actual_voters)
	actual_voters = num_voters;
    if (wfraction &&
    !valid_fraction(wfraction, &suspect_weight, &unmarked_weight))
	error_string("invalid weight", wfraction);
    if (sfraction) {
	if (valid_fraction(sfraction, &m, &n))
	    suspect_threshold = actual_voters * unmarked_weight * m / n;
	else
	    error_string("invalid threshold", sfraction);
    }
}
/**********************************************************************/

void add_sequence(key, reject, index)
char *key;
Boolean reject;
short index;
{
    Sequence *sequence;
    sequence = table_lookup(&seqtable, key);
    if (!sequence)
    {
	sequence = NEW(Sequence);
	sequence->key = strdup(key);
	if (!reject)
	    sequence->median = 1;
	table_insert(&seqtable, sequence);
    }
    sequence->count[index]++;
}
/**********************************************************************/

void count_sequences(index)
short index;
{
    Char *start, *c;
    char key[N * STRING_SIZE], string[STRING_SIZE];
    Boolean reject;
    short i;
    for (start = voter[index]->text.first; start; start = start->next)
    {
	key[0] = '\0';
	reject = False;
	for (i = 0, c = start; i < N; i++, c = c->next)
	{
	    if (!c)
		return;
	    char_to_string(False, c->value, string, False);
	    strcat(key, string);
	    if (c->value == REJECT_CHARACTER)
		reject = True;
	}
	add_sequence(key, reject, index);
    }
}
/**********************************************************************/

int compare_counts(count1, count2)
long *count1, *count2;
{
    return(*count1 - *count2);
}
/**********************************************************************/

void compute_median(sequence)
Sequence *sequence;
{
    static long *count[MAX_VOTERS];
    long i;
    if (!sequence->median)
	return;
    if (!count[0])
	for (i = 0; i < num_voters; i++)
	    count[i] = NEW(long);
    for (i = 0; i < num_voters; i++)
	*count[i] = sequence->count[i];
    sort(i, count, compare_counts);
    sequence->median =
    (i & 1 ? *count[i / 2] : (*count[i / 2 - 1] + *count[i / 2]) / 2.0);
}
/**********************************************************************/

void compute_distance(index)
short index;
{
    long i;
    double difference;
    for (i = 0; i < seqtable.count; i++)
    {
	difference =
        seqtable.array[i]->count[index] - seqtable.array[i]->median;
	if (difference < 0)
	    difference = -difference;
	voter[index]->distance += difference;
    }
}
/**********************************************************************/

int compare_distances(voter1, voter2)
Voter *voter1, *voter2;
{
    if (voter1->distance != voter2->distance)
	return(voter1->distance < voter2->distance ? -1 : 1);
    return(voter1->argnum - voter2->argnum);
}
/**********************************************************************/

void select_voters()
{
    long i;
    if (optimize)
    {
	for (i = 0; i < num_voters; i++)
	    count_sequences(i);
	table_in_array(&seqtable);
	for (i = 0; i < seqtable.count; i++)
	    compute_median(seqtable.array[i]);
	for (i = 0; i < num_voters; i++)
	    compute_distance(i);
	sort(i, voter, compare_distances);
	if (debug)
	    for (i = 0; i < num_voters; i++)
		printf("%11.1f %s\n", voter[i]->distance, voter[i]->filename);
    }
    for (i = 0; i < actual_voters; i++)
	input[i] = voter[i]->text;
}
/**********************************************************************/

void place_vote(c)
Char *c;
{
    short num_votes, i;
    num_votes = unmarked_weight;
    if (c)
    {
	if (c->value == REJECT_CHARACTER)
	    return;
	if (c->suspect)
	    num_votes = suspect_weight;
	for (i = 0; i < num_candidates && (!candidate[i].c ||
	c->value != candidate[i].c->value); i++);
    }
    else
	for (i = 0; i < num_candidates && candidate[i].c; i++);
    if (i < num_candidates)
	candidate[i].num_votes += num_votes;
    else
    {
	num_candidates++;
	candidate[i].c = c;
	candidate[i].num_votes = num_votes;
    }
}
/**********************************************************************/

Boolean winner()
{
    short i, leader = 0;
    if (num_candidates == 0)
    {
	append_char(&output, False, REJECT_CHARACTER);
	return(True);
    }
    for (i = 1; i < num_candidates; i++)
	if (candidate[i].num_votes > candidate[leader].num_votes)
	    leader = i;
    num_candidates = 0;
    if (!candidate[leader].c)
	return(False);
    append_char(&output,
    (candidate[leader].num_votes <= suspect_threshold ? True : False),
    candidate[leader].c->value);
    return(True);
}
/**********************************************************************/

void perform_vote(synclist)
Synclist *synclist;
{
    Sync *sync;
    short i;
    for (sync = synclist->first; sync; sync = sync->next)
	do
	    for (i = 0; i < actual_voters; i++)
		if (sync->substr[i].start <= sync->substr[i].stop)
		    place_vote(input[i].array[sync->substr[i].start++]);
		else
		    place_vote(NULL);
	while (winner());
}
/**********************************************************************/

main(argc, argv)
int argc;
char *argv[];
{
    Synclist synclist;
    initialize(&argc, argv, usage, option);
    validate_args(argc, argv);
    select_voters();
    synchronize(&synclist, actual_voters, input);
    perform_vote(&synclist);
    write_text(&output, outputfilename, NULL);
    terminate();
}
