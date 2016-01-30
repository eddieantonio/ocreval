/**********************************************************************
 *
 *  dist.h
 *
 *  This module provides support for updating and writing a structure
 *  that describes a distribution of character or word accuracies.
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

#ifndef _DIST_
#define _DIST_

typedef
struct
{
    long count[101];	/* the (i)th element contains the total count for which
			   accuracies are greater than or equal to (i)% */
    long total_count;	/* total count for all accuracies */
} Dist;

void update_dist(/* Dist *dist, long count, long missed */);
			/* updates "dist" to reflect the accuracy described by
			   "count" and "missed" */

void write_dist(/* Dist *dist, char *filename */);
			/* writes the distribution represented by "dist" to the
			   named file (or stdout if "filename" is NULL);
			   reports an error and quits if unable to create the
			   file */

#endif
