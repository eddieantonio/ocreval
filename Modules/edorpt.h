/**********************************************************************
 *
 *  edorpt.h
 *
 *  This module provides support for reading and writing edit operation
 *  reports.  The contents of one of these reports is represented by an
 *  "Edodata" structure.
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

#ifndef _EDORPT_
#define _EDORPT_

#define MAX_MOVE_LENGTH  100
			/* maximum length of a move operation, given in number
			   of characters moved; longer moves are counted as
			   moves of this length */

typedef
struct
{
    long total_insertions;
			/* number of "character insert" operations */
    long total_deletions;
			/* number of "character delete" operations */
    long total_moves;	/* number of "block move" operations */
    long moves[MAX_MOVE_LENGTH + 1];
			/* number of move operations for each length */
} Edodata;

void read_edorpt(/* Edodata *edodata, char *filename */);
			/* reads the named file (or stdin if "filename" is NULL)
			   and adds its contents to "edodata"; reports an error
			   and quits if unable to open the file, or if the file
			   does not contain an edit operation report */

void write_edorpt(/* Edodata *edodata, char *filename */);
			/* writes the contents of "edodata" to the named file
			   (or stdout if "filename" is NULL); reports an error
			   and quits if unable to create the file */

#endif
