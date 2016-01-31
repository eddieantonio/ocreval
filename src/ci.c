/**********************************************************************
 *
 *  ci.c
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

#include <math.h>

#include "ci.h"
#include "util.h"

/**********************************************************************/

void append_obs(obslist, count, missed)
Obslist *obslist;
long count, missed;
{
    Obs *obs;
    obs = NEW(Obs);
    obs->count  = count;
    obs->missed = missed;
    obslist->total.count  += count;
    obslist->total.missed += missed;
    list_insert_last(obslist, obs);
}
/**********************************************************************/

static Boolean valid_obslist(obslist)
Obslist *obslist;
{
    long n = 0;
    Obs *obs;
    for (obs = obslist->first; obs; obs = obs->next)
	if (obs->count > 0)
	    n++;
    return(n > 1 ? True : False);
}
/**********************************************************************/

static double accuracy(count, missed)
long count, missed;
{
    return(100.0 * (count - missed) / count);
}
/**********************************************************************/

void compute_ci(obslist, lower, upper)
Obslist *obslist;
double *lower, *upper;
{
    long n;
    double ntheta, sum = 0.0, w;
    Obs *obs;
    if (!valid_obslist(obslist))
	error("not enough observations");
    n = obslist->count;
    obslist->total.theta = accuracy(obslist->total.count,
    obslist->total.missed);
    ntheta = n * obslist->total.theta;
    for (obs = obslist->first; obs; obs = obs->next)
    {
	obs->theta = accuracy(obslist->total.count - obs->count,
	obslist->total.missed - obs->missed);
	obs->j = ntheta - (n - 1) * obs->theta;
	sum += obs->theta;
    }
    obslist->total.j = ntheta - (n - 1) * sum / n;
    sum = 0.0;
    for (obs = obslist->first; obs; obs = obs->next)
	sum += (obs->j - obslist->total.j) * (obs->j - obslist->total.j);
    w = 1.96 * sqrt(sum / (n - 1) / n);
    *lower = max(0.0, min(100.0, obslist->total.j - w));
    *upper = max(0.0, min(100.0, obslist->total.j + w));
}
