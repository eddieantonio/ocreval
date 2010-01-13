/**********************************************************************
 *
 *  accuracy.c
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

#include "accrpt.h"
#include "sync.h"

#define usage  "correctfile generatedfile [accuracy_report]"

#define MAX_DISPLAY  24

Textopt textopt = { True, True, 0, True, True };

Text text[2];

Accdata accdata;

/**********************************************************************/

void make_key(key, sync)
char *key;
Sync *sync;
{
    long i, j;
    char buffer[2][MAX_DISPLAY + 4], string[STRING_SIZE];
    for (i = 0; i < 2; i++)
    {
	buffer[i][0] = '\0';
	for (j = sync->substr[i].start; j <= sync->substr[i].stop; j++)
	{
	    char_to_string(False, text[i].array[j]->value, string, True);
	    if (strlen(buffer[i]) + strlen(string) > MAX_DISPLAY)
	    {
		strcat(buffer[i], "...");
		break;
	    }
	    strcat(buffer[i], string);
	}
    }
    sprintf(key, "{%s}-{%s}\n", buffer[0], buffer[1]);
}
/**********************************************************************/

void add_ops(sum_ops, ops)
Accops *sum_ops, *ops;
{
    sum_ops->ins    += ops->ins;
    sum_ops->subst  += ops->subst;
    sum_ops->del    += ops->del;
    sum_ops->errors += ops->errors;
}
/**********************************************************************/

void process_synclist(synclist)
Synclist *synclist;
{
    Sync *sync;
    long i, characters, wildcards, reject_characters, suspect_markers, genchars;
    Accops ops;
    char key[100];
    for (sync = synclist->first; sync; sync = sync->next)
    {
	characters = wildcards = 0;
	for (i = sync->substr[0].start; i <= sync->substr[0].stop; i++)
	    if (text[0].array[i]->value == REJECT_CHARACTER)
		wildcards++;
	    else
	    {
		characters++;
		add_class(&accdata, text[0].array[i]->value, 1,
		(sync->match ? 0 : 1));
	    }
	accdata.characters += characters;
	reject_characters = suspect_markers = 0;
	for (i = sync->substr[1].start; i <= sync->substr[1].stop; i++)
	    if (text[1].array[i]->value == REJECT_CHARACTER)
		reject_characters++;
	    else if (text[1].array[i]->suspect)
		suspect_markers++;
	accdata.reject_characters += reject_characters;
	accdata.suspect_markers += suspect_markers;
	if (sync->match)
	    accdata.false_marks += suspect_markers;
	else
	{
	    genchars = max(0, sync->substr[1].length - wildcards);
	    ops.errors = max(characters, genchars);
	    if (ops.errors > 0)
	    {
		accdata.errors += ops.errors;
		ops.ins   = max(0, characters - genchars);
		ops.subst = min(characters, genchars);
		ops.del   = max(0, genchars - characters);
		make_key(key, sync);
		if (reject_characters + suspect_markers > 0)
		{
		    add_ops(&accdata.marked_ops, &ops);
		    add_conf(&accdata, key, ops.errors, ops.errors);
		}
		else
		{
		    add_ops(&accdata.unmarked_ops, &ops);
		    add_conf(&accdata, key, ops.errors, 0);
		}
		add_ops(&accdata.total_ops, &ops);
	    }
	}
    }
}
/**********************************************************************/

main(argc, argv)
int argc;
char *argv[];
{
    Synclist synclist;
    initialize(&argc, argv, usage, NULL);
    if (argc < 2 || argc > 3)
	error("invalid number of files", Exit);
    read_text(&text[0], argv[0], &textopt);
    if (textopt.found_header)
	error("no correct file specified", Exit);
    read_text(&text[1], argv[1], &textopt);
    fastukk_sync(&synclist, text);
    process_synclist(&synclist);
    write_accrpt(&accdata, (argc == 3 ? argv[2] : NULL));
    terminate();
}
