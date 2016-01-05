/**********************************************************************
 *
 *  synctext.c
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

#include "sync.h"

#define usage  "[-H] [-i] [-s] [-T] textfile1 textfile2 ... >resultfile"

Textopt textopt = { True, True, 0, True, True };

Boolean heuristic, show_suspect, transpose;

Option option[] =
{
    'H', NULL, &heuristic,
    'i', NULL, &textopt.case_insensitive,
    's', NULL, &show_suspect,
    'T', NULL, &transpose,
    '\0'
};

/**********************************************************************/

void write_separator()
{
    short i;
    for (i = 1; i < 80; i++)
	putchar('=');
    putchar(NEWLINE);
}
/**********************************************************************/

void write_transposed(synclist, text)
Synclist *synclist;
Text *text;
{
    Sync *sync;
    long i;
    char string[STRING_SIZE];
    write_separator();
    putchar(NEWLINE);
    for (sync = synclist->first; sync; sync = sync->next)
    {
	if (sync->match)
	    printf("{%ld:", *sync->match);
	for (i = sync->substr->start; i <= sync->substr->stop; i++)
	{
	    char_to_string(show_suspect & text->array[i]->suspect,
	    text->array[i]->value, string, False);
	    printf("%s", string);
	}
	if (sync->match)
	    putchar('}');
    }
    putchar(NEWLINE);
}
/**********************************************************************/

void write_matches(synclist, num_text, text)
Synclist *synclist;
short num_text;
Text *text;
{
    Sync *sync;
    long i, j, footnote = 0;
    Boolean suspect;
    char string[STRING_SIZE];
    write_separator();
    putchar(NEWLINE);
    for (sync = synclist->first; sync; sync = sync->next)
	if (sync->match)
	    for (i = 0; i < sync->substr[0].length; i++)
	    {
		suspect = False;
		if (show_suspect)
		    for (j = 0; j < num_text && !suspect; j++)
			if (text[j].array[sync->substr[j].start + i]->suspect)
			    suspect = True;
		char_to_string(suspect,
		text[0].array[sync->substr[0].start + i]->value, string, False);
		printf("%s", string);
	    }
	else
	    printf("{%ld}", ++footnote);
    putchar(NEWLINE);
}
/**********************************************************************/

void write_differences(synclist, num_text, text, filename)
Synclist *synclist;
short num_text;
Text *text;
char *filename[];
{
    long i, j, maxlen = 0, footnote = 0;
    char format[20], string[STRING_SIZE];
    Sync *sync;
    for (i = 0; i < num_text; i++)
	maxlen = max(maxlen, strlen(filename[i]));
    sprintf(format, "%%-%lds", maxlen);
    for (sync = synclist->first; sync; sync = sync->next)
	if (!sync->match)
	{
	    write_separator();
	    printf("{%ld}\n", ++footnote);
	    for (i = 0; i < num_text; i++)
	    {
		printf(format, filename[i]);
		printf(" {");
		for (j = sync->substr[i].start; j <= sync->substr[i].stop; j++)
		{
		    char_to_string(show_suspect & text[i].array[j]->suspect,
		    text[i].array[j]->value, string, False);
		    printf("%s", string);
		    if (text[i].array[j]->value == NEWLINE)
		    {
			printf(format, "");
			printf("  ");
		    }
		}
		printf("}\n");
	    }
	}
}
/**********************************************************************/

main(argc, argv)
int argc;
char *argv[];
{
    Text *text;
    int i;
    Synclist synclist1, synclist2;
    initialize(&argc, argv, usage, option);
    if (argc < 2 || (transpose && argc > 2))
	error("invalid number of text files");
    text = NEW_ARRAY(argc, Text);
    for (i = 0; i < argc; i++)
	read_text(&text[i], argv[i], &textopt);
    if (transpose)
    {
	transpose_sync(&synclist1, &synclist2, &text[0], &text[1]);
	write_transposed(&synclist1, &text[0]);
	write_transposed(&synclist2, &text[1]);
    }
    else
    {
	if (heuristic || argc > 2)
	    synchronize(&synclist1, argc, text);
	else
	    fastukk_sync(&synclist1, text);
	write_matches(&synclist1, argc, text);
	write_differences(&synclist1, argc, text, argv);
    }
    write_separator();
    terminate();
}
