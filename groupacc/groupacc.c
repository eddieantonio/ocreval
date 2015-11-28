/**********************************************************************
 *
 *  groupacc.c
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

#define usage  "groupfile accuracy_report [groupacc_report]"

Textopt textopt = { False, True, 0, True, True };

Text text;

Accdata accdata;

/**********************************************************************/

void write_line(f, count, missed)
FILE *f;
long count, missed;
{
    fprintf(f, "%8ld %8ld ", count, missed);
    if (count == 0)
	fprintf(f, "  ------   ");
    else
	fprintf(f, "%8.2f   ", 100.0 * (count - missed) / count);
}
/**********************************************************************/

void write_report(filename)
char *filename;
{
    FILE *f;
    long total_count = 0, total_missed = 0, count, missed;
    Char *c;
    char buffer[STRING_SIZE];
    f = open_file(filename, "w");
    fprintf(f, "   Count   Missed   %%Right\n");
    for (c = text.first; c; c = c->next)
	if (accdata.small_class[c->value].count > 0 && c->value != BLANK &&
	c->value != NEWLINE)
	{
	    count  = accdata.small_class[c->value].count;
	    missed = accdata.small_class[c->value].missed;
	    write_line(f, count, missed);
	    char_to_string(False, c->value, buffer, True);
	    fprintf(f, "{%s}\n", buffer);
	    total_count  += count;
	    total_missed += missed;
	}
    write_line(f, total_count, total_missed);
    fprintf(f, "Total\n");
    close_file(f);
}
/**********************************************************************/

main(argc, argv)
int argc;
char *argv[];
{
    initialize(&argc, argv, usage, NULL);
    if (argc < 2 || argc > 3)
	error("invalid number of files");
    read_text(&text, argv[0], &textopt);
    read_accrpt(&accdata, argv[1]);
    write_report(argc == 3 ? argv[2] : NULL);
    terminate();
}
