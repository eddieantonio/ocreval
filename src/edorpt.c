/**********************************************************************
 *
 *  edorpt.c
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
#include "util.h"
#include "ocreval_version.h"

#define TITLE    "ocreval Edit Operation Report Version " OCREVAL_VERSION "\n"
#define DIVIDER  "-----------------------------------------\n"

static char line[100];

/**********************************************************************/

static Boolean read_line(f)
FILE *f;
{
    return(fgets(line, sizeof(line) - 1, f) ? True : False);
}
/**********************************************************************/

static Boolean read_value(f, value, sum_value)
FILE *f;
long *value, *sum_value;
{
    if (read_line(f) && sscanf(line, "%ld", value) == 1)
    {
	*sum_value += *value;
	return(True);
    }
    else
	return(False);
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

void read_edorpt(edodata, filename)
Edodata *edodata;
char *filename;
{
    FILE *f;
    long moves, value1, value2;
    f = open_file(filename, "r");
    if (read_line(f) && strncmp(line, TITLE, sizeof(TITLE) - 3) == 0 &&
    read_line(f) && strcmp(line, DIVIDER) == 0 &&
    read_value(f, &value1, &edodata->total_insertions) &&
    read_value(f, &value1, &edodata->total_deletions) &&
    read_value(f, &moves, &edodata->total_moves))
    {
	if (moves > 0 && read_line(f) && read_line(f) && read_line(f))
	    while (read_two(f, &value1, &value2))
		edodata->moves[value2] += value1;
    }
    else
	error_string("invalid format in", (filename ? filename : "stdin"));
    close_file(f);
}
/**********************************************************************/

static void write_value(f, value, string)
FILE *f;
long value;
char *string;
{
    fprintf(f, "%8ld   %s\n", value, string);
}
/**********************************************************************/

static void write_move(f, count, length)
FILE *f;
long count, length;
{
    fprintf(f, "%8ld %8ld\n", count, length);
}
/**********************************************************************/

void write_edorpt(edodata, filename)
Edodata *edodata;
char *filename;
{
    FILE *f;
    long i;
    f = open_file(filename, "w");
    fprintf(f, "%s%s", TITLE, DIVIDER);
    write_value(f, edodata->total_insertions, "Insertions");
    write_value(f, edodata->total_deletions, "Deletions");
    write_value(f, edodata->total_moves, "Moves");
    if (edodata->total_moves > 0)
    {
	fprintf(f, "\nMoves\n   Count   Length\n");
	for (i = 1; i <= MAX_MOVE_LENGTH; i++)
	    if (edodata->moves[i] > 0)
		write_move(f, edodata->moves[i], i);
    }
    close_file(f);
}
