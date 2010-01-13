/**********************************************************************
 *
 *  ci.h
 *
 *  This module provides definitions and utility routines pertaining to
 *  confidence intervals.
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

#ifndef _CI_
#define _CI_

#include "list.h"

BEGIN_ITEM(Obs)
    long count;		/* total number */
    long missed;	/* number that were misrecognized */
    double theta;	/* estimator */
    double j;		/* pseudovalue */
END_ITEM(Obs);		/* an observation */

BEGIN_LIST_OF(Obs)
    Obs total;		/* total for all observations */
END_LIST(Obslist);	/* a list of observations */

void append_obs(/* Obslist *obslist, long count, long missed */);
			/* appends the given observation to "obslist" */

void compute_ci(/* Obslist *obslist, double *lower, double *upper */);
			/* computes and returns an approximate 95% confidence
			   interval for accuracy for the given set of
			   observations */

#endif
