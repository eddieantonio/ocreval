/**********************************************************************
 *
 *  editop.c
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

#include "edorpt.h"
#include "sync.h"

#define usage  "correctfile generatedfile [editop_report]"

Boolean debug;

Option option[] =
{
    'D', NULL, &debug,
    '\0'
};

Textopt textopt = { True, True, 0, True, True };

Text text1, text2;

Edodata edodata;

typedef
struct
{
    Boolean for_insertion, for_deletion;
    Sync *sync;
} Candidate;

/**********************************************************************/

void display(synclist, label)
Synclist *synclist;
char *label;
{
    Sync *sync;
    printf("%-9s:", label);
    for (sync = synclist->first; sync; sync = sync->next)
	printf(" {%ld:%ld}", *sync->match, sync->substr->length);
    printf("\n\n");
}
/**********************************************************************/

void discard_sync(synclist, sync)
Synclist *synclist;
Sync *sync;
{
    list_remove(synclist, sync);
    free(sync->substr);
    if (sync->match)
	free(sync->match);
    free(sync);
}
/**********************************************************************/

void count_insertions(synclist, text)
Synclist *synclist;
Text *text;
{
    Sync *sync;
    long i;
    for (sync = synclist->first; sync; sync = sync->next)
	if (!sync->match)
	    for (i = sync->substr->start; i <= sync->substr->stop; i++)
		if (text->array[i]->value != REJECT_CHARACTER)
		    edodata.total_insertions++;
}
/**********************************************************************/

void count_deletions(synclist)
Synclist *synclist;
{
    Sync *sync, *next;
    sync = synclist->first;
    while (sync)
    {
	next = sync->next;
	if (!sync->match)
	{
	    edodata.total_deletions += sync->substr->length;
	    discard_sync(synclist, sync);
	}
	sync = next;
    }
}
/**********************************************************************/

void decrement_match(synclist, limit)
Synclist *synclist;
long limit;
{
    Sync *sync;
    for (sync = synclist->first; sync; sync = sync->next)
	if (*sync->match > limit)
	    *sync->match -= 1;
}
/**********************************************************************/

void combine_adjacent(synclist)
Synclist *synclist;
{
    Sync *sync, *next;
    sync = synclist->first;
    while (sync)
    {
	next = sync->next;
	if (next && *next->match == *sync->match + 1)
	{
	    sync->substr->length += next->substr->length;
	    discard_sync(synclist, next);
	    decrement_match(synclist, *sync->match);
	}
	else
	    sync = next;
    }
    if (debug)
	display(synclist, "combined");
}
/**********************************************************************/

Candidate *find_candidates(synclist)
Synclist *synclist;
{
    Candidate *candidate;
    Sync *sync;
    candidate = NEW_ARRAY(synclist->count + 1, Candidate);
    for (sync = synclist->first; sync; sync = sync->next)
    {
	if (sync->next && *sync->next->match == *sync->match + 2)
	    candidate[*sync->match + 1].for_insertion = True;
	if (sync->next && sync->next->next &&
	*sync->next->next->match == *sync->match + 1)
	    candidate[*sync->next->match].for_deletion = True;
	candidate[*sync->match].sync = sync;
    }
    return(candidate);
}
/**********************************************************************/

Sync *find_move(synclist, candidate)
Synclist *synclist;
Candidate candidate[];
{
    long i, reduction, move_i, move_length, move_reduction = 0;
    for (i = 1; i <= synclist->count; i++)
    {
	reduction = 1;
	if (candidate[i].for_insertion)
	    reduction++;
	if (candidate[i].for_deletion)
	    reduction++;
	if (reduction > move_reduction || reduction == move_reduction &&
	candidate[i].sync->substr->length < move_length)
	{
	    move_i = i;
	    move_length = candidate[i].sync->substr->length;
	    move_reduction = reduction;
	}
    }
    return(candidate[move_i].sync);
}
/**********************************************************************/

void perform_move(synclist, candidate, sync)
Synclist *synclist;
Candidate candidate[];
Sync *sync;
{
    short length;
    list_remove(synclist, sync);
    if (*sync->match == 1)
	list_insert_before(synclist, sync, candidate[2].sync);
    else
	list_insert_after(synclist, candidate[*sync->match - 1].sync, sync);
    edodata.total_moves++;
    length = min(sync->substr->length, MAX_MOVE_LENGTH);
    edodata.moves[length]++;
    if (debug)
    {
	char label[20];
	sprintf(label, "moved %ld", *sync->match);
	display(synclist, label);
    }
}
/**********************************************************************/

void count_moves(synclist)
Synclist *synclist;
{
    Candidate *candidate;
    Sync *sync;
    if (debug)
	display(synclist, "original");
    combine_adjacent(synclist);
    while (synclist->count > 1)
    {
	candidate = find_candidates(synclist);
	sync = find_move(synclist, candidate);
	perform_move(synclist, candidate, sync);
	free(candidate);
	combine_adjacent(synclist);
    }
}
/**********************************************************************/

main(argc, argv)
int argc;
char *argv[];
{
    Synclist synclist1, synclist2;
    initialize(&argc, argv, usage, option);
    if (argc < 2 || argc > 3)
	error("invalid number of files", Exit);
    read_text(&text1, argv[0], &textopt);
    if (textopt.found_header)
	error("no correct file specified", Exit);
    read_text(&text2, argv[1], &textopt);
    transpose_sync(&synclist1, &synclist2, &text1, &text2);
    count_insertions(&synclist1, &text1);
    count_deletions(&synclist2);
    count_moves(&synclist2);
    write_edorpt(&edodata, (argc == 3 ? argv[2] : NULL));
    terminate();
}
