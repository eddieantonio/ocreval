/**********************************************************************
 *
 *  dist.c
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

#include "dist.h"
#include "util.h"

/**********************************************************************/

void update_dist(dist, count, missed)
Dist *dist;
long count, missed;
{
    double accuracy;
    short i;
    if (count == 0)
	return;
    accuracy = 100.0 * (count - missed) / count;
    for (i = 0; accuracy >= i; i++)
	dist->count[i] += count;
    dist->total_count += count;
}
/**********************************************************************/

void write_dist(dist, filename)
Dist *dist;
char *filename;
{
    FILE *f;
    short i;
    if (dist->total_count == 0)
	return;
    f = open_file(filename, "w");
    for (i = 0; i <= 100; i++)
	fprintf(f, "%3d %6.2f\n", i,
	100.0 * dist->count[i] / dist->total_count);
    close_file(f);
}
