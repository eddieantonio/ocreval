/**********************************************************************
 *
 *  sync.h
 *
 *  This module provides support for string synchronization.  There are
 *  two algorithms based on the detection of long common substrings:
 *  "synchronize" can match two or more text streams but does not find
 *  transposed matches, while "transpose_sync" locates transposed
 *  matches but can be applied to only two text streams.  A third
 *  algorithm, "fastukk_sync", is based on an algorithm by Ukkonen, and
 *  finds an optimal match of two text streams using cost function
 *  (1,1,1).
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

#ifndef _SYNC_
#define _SYNC_

#include "text.h"

typedef
struct
{
    long start;		/* index of the first character of the substring */
    long stop;		/* index of the last character of the substring */
    long length;	/* length of the substring in characters */
} Substr;		/* describes a substring of a text value by specifying
			   its indices within the "array" representation of the
			   text value */

BEGIN_ITEM(Sync)
    Substr *substr;	/* array describing one substring for each text value */
    long *match;	/* if non-NULL, then the substrings have been matched
			   and this points to the match number; otherwise, the
			   substrings are unmatched */
END_ITEM(Sync);

BEGIN_LIST_OF(Sync)
END_LIST(Synclist);	/* list of matched and unmatched substrings */

void synchronize(/* Synclist *synclist, short num_text, Text *text */);
			/* given "num_text" streams of text, synchronizes the
			   streams and stores the results in "synclist"; each
			   item in the list points to an array of "num_text"
			   substrings */

void transpose_sync(/* Synclist *synclist1, Synclist *synclist2, 
		       Text *text1, Text *text2 */);
			/* synchronizes two streams of text while allowing for
			   transposed matches; each stream has its results
			   stored in its own list, and each item of its list
			   points to only a single substring */

void fastukk_sync(/* Synclist *synclist, Text *text */);
			/* given two streams in the array "text", synchronizes
			   them optimally and stores the results in "synclist";
			   each item in the list points to an array of two
			   substrings */

#endif
