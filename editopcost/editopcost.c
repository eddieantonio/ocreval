/**********************************************************************
 *
 *  editopcost.c
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

#define usage  "editop_report [editop_report2] >xyfile"

Edodata edodata, edodata2;

/**********************************************************************/

void decrement_edodata()
{
    short i;
    edodata.total_insertions -= edodata2.total_insertions;
    edodata.total_deletions  -= edodata2.total_deletions;
    edodata.total_moves      -= edodata2.total_moves;
    for (i = 1; i <= MAX_MOVE_LENGTH; i++)
	edodata.moves[i] -= edodata2.moves[i];
}
/**********************************************************************/

void write_results()
{
    long insertions, moves, i;
    insertions = edodata.total_insertions;
    moves = edodata.total_moves;
    for (i = 0; i <= MAX_MOVE_LENGTH; i++)
    {
	printf("%3ld %10ld\n", i, insertions + i * moves);
	insertions += i * edodata.moves[i];
	moves -= edodata.moves[i];
    }
}
/**********************************************************************/

main(argc, argv)
int argc;
char *argv[];
{
    initialize(&argc, argv, usage, NULL);
    if (argc < 1 || argc > 2)
	error("invalid number of files");
    read_edorpt(&edodata, argv[0]);
    if (argc == 2)
    {
	read_edorpt(&edodata2, argv[1]);
	decrement_edodata();
    }
    write_results();
    terminate();
}
