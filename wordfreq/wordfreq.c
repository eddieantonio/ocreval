/**********************************************************************
 *
 *  wordfreq.c
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

#define usage  "textfile1 textfile2 ... >resultfile"

Textopt textopt = { True, True, 0, True, True, True };
Text text;

Wordlist wordlist;

Termtable termtable;

/**********************************************************************/

void process_file(filename)
char *filename;
{
    Word *word;
    read_text(&text, filename, &textopt);
    find_words(&wordlist, &text);
    for (word = wordlist.first; word; word = word->next)
	add_term(&termtable, word->string, 1, 0);
    list_empty(&text, free);
    list_empty(&wordlist, free_word);
}
/**********************************************************************/

int order_by_key(term1, term2)
Term *term1, *term2;
{
    return(ustrcmp(term1->key, term2->key));
}
/**********************************************************************/

int order_by_count(term1, term2)
Term *term1, *term2;
{
    if (term1->wac.count != term2->wac.count)
	return(term2->wac.count - term1->wac.count);
    return(order_by_key(term1, term2));
}
/**********************************************************************/

void write_array()
{
    long i, total = 0;
    printf("   Count\n");
    for (i = 0; i < termtable.count; i++)
    {
	printf("%8ld   %s\n", termtable.array[i]->wac.count,
	termtable.array[i]->key);
	total += termtable.array[i]->wac.count;
    }
    printf("%8ld   Total\n", total);
}
/**********************************************************************/

void write_report()
{
    table_in_array(&termtable);
    sort(termtable.count, termtable.array, order_by_key);
    write_array();
    printf("\n\n");
    sort(termtable.count, termtable.array, order_by_count);
    write_array();
}
/**********************************************************************/

main(argc, argv)
int argc;
char *argv[];
{
    int i;
    initialize(&argc, argv, usage, NULL);
    if (argc == 0)
	error("no text files specified");
    for (i = 0; i < argc; i++)
	process_file(argv[i]);
    write_report();
    terminate();
}
