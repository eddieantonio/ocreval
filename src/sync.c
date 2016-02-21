/**********************************************************************
 *
 *  sync.c
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

#include <stdint.h>

#include "sync.h"

#define MIN_STREAMS   2
#define MAX_STREAMS  16

/* #define SHOW_FASTUKK_REDUCTION */

/* Support up to 2**32 characters. */
typedef uint_fast32_t F;
#define MAX_F     INT_FAST32_MAX

BEGIN_ITEM(Diag)
    F f;
END_ITEM(Diag);

BEGIN_LIST_OF(Diag)
END_LIST(Diaglist);
static Diaglist diaglist;

BEGIN_ITEM(Path)
    long p, start_k, stop_k;
    F *f;
END_ITEM(Path);

BEGIN_LIST_OF(Path)
END_LIST(Pathlist);
static Pathlist pathlist;

static Char **a, **b;

static long m, n, n_m, min_k, max_k;

/**********************************************************************/

static Sync *create_sync(num_text)
short num_text;
{
    Sync *sync;
    sync = NEW(Sync);
    sync->substr = NEW_ARRAY(num_text, Substr);
    return(sync);
}
/**********************************************************************/

static void destroy_sync(sync)
Sync *sync;
{
    free(sync->substr);
    free(sync);
}
/**********************************************************************/

static Boolean valid_sync(num_text, sync)
short num_text;
Sync *sync;
{
    short i;
    for (i = 0; i < num_text; i++)
	if (sync->substr[i].length > 0)
	    return(True);
    return(False);
}
/**********************************************************************/

static void initialize_synclist(synclist, num_text, text)
Synclist *synclist;
short num_text;
Text *text;
{
    Sync *sync;
    short i;
    list_initialize(synclist);
    sync = create_sync(num_text);
    for (i = 0; i < num_text; i++)
    {
	list_in_array(&text[i]);
	sync->substr[i].start  = 0;
	sync->substr[i].stop   = text[i].count - 1;
	sync->substr[i].length = text[i].count;
    }
    if (valid_sync(num_text, sync))
	list_insert_first(synclist, sync);
    else
	destroy_sync(sync);
}
/**********************************************************************/

static long longest_match(maxlen, c1, start1, stop1, c2, start2, stop2, mstart1,
mstart2)
long maxlen, start1, stop1, start2, stop2, *mstart1, *mstart2;
Char *c1[], *c2[];
{
    long s1limit, s2limit, s1, s2, i1, i2, increase;
    s1limit = stop1 - maxlen;
    s2limit = stop2 - maxlen;
    for (s1 = start1; s1 <= s1limit; s1++)
	for (s2 = start2; s2 <= s2limit; s2++)
	    if (c1[s1]->value == c2[s2]->value &&
	    c1[s1]->value != REJECT_CHARACTER)
	    {
		for (i1 = s1 + 1, i2 = s2 + 1; i1 <= stop1 && i2 <= stop2 &&
		c1[i1]->value == c2[i2]->value &&
		c1[i1]->value != REJECT_CHARACTER; i1++, i2++);
		increase = i1 - s1 - maxlen;
		if (increase > 0)
		{
		    s1limit -= increase;
		    s2limit -= increase;
		    maxlen  += increase;
		    *mstart1 = s1;
		    *mstart2 = s2;
		}
	    }
    return(maxlen);
}
/**********************************************************************/

static void save_match(synclist, num_text, sync, start, length, match)
Synclist *synclist;
short num_text;
Sync *sync;
long *start, length, *match;
{
    Sync *left, *right;
    short i;
    long stop;
    left  = create_sync(num_text);
    right = create_sync(num_text);
    for (i = 0; i < num_text; i++)
    {
	stop = start[i] + length - 1;
	left->substr[i].start   = sync->substr[i].start;
	left->substr[i].stop    = start[i] - 1;
	left->substr[i].length  = start[i] - sync->substr[i].start;
	right->substr[i].start  = stop + 1;
	right->substr[i].stop   = sync->substr[i].stop;
	right->substr[i].length = sync->substr[i].stop - stop;
	sync->substr[i].start	= start[i];
	sync->substr[i].stop	= stop;
	sync->substr[i].length	= length;
    }
    sync->match = match;
    if (valid_sync(num_text, left))
	list_insert_before(synclist, left, sync);
    else
	destroy_sync(left);
    if (valid_sync(num_text, right))
	list_insert_after(synclist, sync, right);
    else
	destroy_sync(right);
}
/**********************************************************************/

static void number_matches(synclist)
Synclist *synclist;
{
    long match = 0;
    Sync *sync;
    for (sync = synclist->first; sync; sync = sync->next)
	if (sync->match)
	    *sync->match = ++match;
}
/**********************************************************************/

static void recursive_sync(synclist, num_text, text, sync)
Synclist *synclist;
short num_text;
Text *text;
Sync *sync;
{
    short i, j;
    long start[MAX_STREAMS], length, new_start, change;
    for (i = 0; i < num_text; i++)
	if (sync->substr[i].length == 0)
	    return;
    start[0] = sync->substr[0].start;
    length   = sync->substr[0].length;
    for (i = 1; i < num_text; i++)
    {
	length = longest_match(0,
	text[0].array, start[0], start[0] + length - 1,
	text[i].array, sync->substr[i].start, sync->substr[i].stop,
	&new_start, &start[i]);
	if (length == 0)
	    return;
	change = new_start - start[0];
	if (change > 0)
	    for (j = 0; j < i; j++)
		start[j] += change;
    }
    save_match(synclist, num_text, sync, start, length, NEW(long));
    if (sync->prev)
	recursive_sync(synclist, num_text, text, sync->prev);
    if (sync->next)
	recursive_sync(synclist, num_text, text, sync->next);
}
/**********************************************************************/

void synchronize(synclist, num_text, text)
Synclist *synclist;
short num_text;
Text *text;
{
    if (num_text < MIN_STREAMS || num_text > MAX_STREAMS)
	error("invalid number of text streams");
    initialize_synclist(synclist, num_text, text);
    if (synclist->first)
	recursive_sync(synclist, num_text, text, synclist->first);
    number_matches(synclist);
}
/**********************************************************************/

void transpose_sync(synclist1, synclist2, text1, text2)
Synclist *synclist1, *synclist2;
Text *text1, *text2;
{
    long length, newlength, start1, start2, *match;
    Sync *s1, *s2, *sync1, *sync2;
    initialize_synclist(synclist1, 1, text1);
    initialize_synclist(synclist2, 1, text2);
    while (1)
    {
	length = 0;
	for (s1 = synclist1->first; s1; s1 = s1->next)
	    if (!s1->match && s1->substr->length > length)
		for (s2 = synclist2->first; s2; s2 = s2->next)
		    if (!s2->match && s2->substr->length > length)
		    {
			newlength = longest_match(length,
			text1->array, s1->substr->start, s1->substr->stop,
			text2->array, s2->substr->start, s2->substr->stop,
			&start1, &start2);
			if (newlength > length)
			{
			    length = newlength;
			    sync1  = s1;
			    sync2  = s2;
			}
		    }
	if (length == 0)
	    break;
	match = NEW(long);
	save_match(synclist1, 1, sync1, &start1, length, match);
	save_match(synclist2, 1, sync2, &start2, length, match);
    }
    number_matches(synclist1);
}
/**********************************************************************/

static long initial_f(k, prev_path, prev_k)
long k, *prev_k;
Path *prev_path;
{
    long start, stop, i, value, result = 0;
    if (prev_path)
    {
	start = max(k - 1, prev_path->start_k);
	stop  = min(k + 1, prev_path->stop_k);
	if (start > stop)
	    error("internal synchronization error");
	for (i = start; i <= stop; i++)
	{
	    value = prev_path->f[i - prev_path->start_k];
	    if (i >= k)
		value++;
	    if (value >= result)
	    {
		result = value;
		*prev_k = i;
	    }
	}
    }
    return(result);
}
/**********************************************************************/

static void compute_f(k, path)
long k;
Path *path;
{
    long i, j, value, bound;
    Diag *diag;
    i = initial_f(k, path->prev, &value);
    j = i + k;
    while (i < m && j < n && a[i]->value == b[j]->value &&
    a[i]->value != REJECT_CHARACTER)
    {
	i++;
	j++;
    }
    if (i == m)
	min_k = k + 1;
    if (j == n)
	max_k = k - 1;
    bound = path->p + max(m, n - k) - i;
    value = (1 - bound + n_m) >> 1;
    if (value > min_k)
	min_k = value;
    value = (bound + n_m) >> 1;
    if (value < max_k)
	max_k = value;
    diag = NEW(Diag);
    diag->f = i;
    if (diaglist.count == 0)
    {
	path->start_k = path->stop_k = k;
	list_insert_last(&diaglist, diag);
    }
    else
	if (k < path->start_k)
	{
	    path->start_k = k;
	    list_insert_first(&diaglist, diag);
	}
	else
	{
	    path->stop_k = k;
	    list_insert_last(&diaglist, diag);
	}
}
/**********************************************************************/

static void compute_pathlist(text)
Text *text;
{
#ifdef SHOW_FASTUKK_REDUCTION
    long ukkonen_count = 0, fastukk_count = 0, r, count;
#endif
    long p = -1, k, i;
    Path *path;
    Diag *diag;
    a = text[0].array;
    b = text[1].array;
    m = text[0].count;
    n = text[1].count;
    n_m = n - m;
    min_k = -m;
    max_k =  n;
    while (min_k <= n_m)
    {
	path = NEW(Path);
	path->p = ++p;
	list_insert_last(&pathlist, path);
	if (m <= n)
	{
	    for (k = min(n_m, p); k >= max(min_k, -p); k--)
		compute_f(k, path);
	    for (k = n_m + 1; k <= min(max_k, p); k++)
		compute_f(k, path);
	}
	else
	{
	    for (k = max(n_m, -p); k <= min(max_k, p); k++)
		compute_f(k, path);
	    for (k = n_m - 1; k >= max(min_k, -p); k--)
		compute_f(k, path);
	}
#ifdef SHOW_FASTUKK_REDUCTION
	r = p - min(m, n);
	count = min(-1, -r) - max(-m, -p) + 1;
	if (count > 0)
	    ukkonen_count += count;
	count = min(n, p) - max(0, r) + 1;
	if (count > 0)
	    ukkonen_count += count;
	fastukk_count += diaglist.count;
#endif
	path->f = NEW_ARRAY(diaglist.count, F);
	i = 0;
	while (diaglist.first)
	{
	    diag = diaglist.first;
	    list_remove(&diaglist, diag);
	    path->f[i++] = diag->f;
	    free(diag);
	}
    }
#ifdef SHOW_FASTUKK_REDUCTION
    fprintf(stderr, "%8d - %8d = %8d; %5.1f%% reduction\n",
    ukkonen_count, fastukk_count, ukkonen_count - fastukk_count,
    100.0 * (ukkonen_count - fastukk_count) / ukkonen_count);
#endif
}
/**********************************************************************/

static void obtain_matches(synclist)
Synclist *synclist;
{
    long k, f, start[2], prev_k;
    Path *path;
    k = n_m;
    while (pathlist.last)
    {
	path = pathlist.last;
	f = path->f[k - path->start_k];
	start[0] = initial_f(k, path->prev, &prev_k);
	if (f > start[0])
	{
	    start[1] = start[0] + k;
	    save_match(synclist, 2, synclist->first, start, f - start[0],
	    NEW(long));
	}
	list_remove(&pathlist, path);
	free(path->f);
	free(path);
	k = prev_k;
    }
}
/**********************************************************************/

void fastukk_sync(synclist, text)
Synclist *synclist;
Text *text;
{
    if (text[0].count > MAX_F)
	error("text stream is too long");
    initialize_synclist(synclist, 2, text);
    compute_pathlist(text);
    obtain_matches(synclist);
    number_matches(synclist);
}
