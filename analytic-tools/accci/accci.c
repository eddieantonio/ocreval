/**********************************************************************
 *
 *  accci.c
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
#include "ci.h"

#define usage  "accuracy_report1 accuracy_report2 ... >resultfile"

Accdata accdata;
Obslist obslist;

/**********************************************************************/

void process_file(filename)
char *filename;
{
    long chars, errors;
    chars  = accdata.characters;
    errors = accdata.errors;
    read_accrpt(&accdata, filename);
    append_obs(&obslist, accdata.characters - chars, accdata.errors - errors);
}
/**********************************************************************/

void write_results()
{
    double lower, upper;
    compute_ci(&obslist, &lower, &upper);
    printf("%14ld   Observations\n", obslist.count);
    printf("%14ld   Characters\n", accdata.characters);
    printf("%14ld   Errors\n", accdata.errors);
    printf("%14.2f%%  Accuracy\n",
    100.0 * (accdata.characters - accdata.errors) / accdata.characters);
    printf("%6.2f%%,%6.2f%%  %s\n", lower, upper,
    "Approximate 95% Confidence Interval for Accuracy");
}
/**********************************************************************/

main(argc, argv)
int argc;
char *argv[];
{
    int i;
    initialize(&argc, argv, usage, NULL);
    if (argc < 2)
	error("not enough input files", Exit);
    for (i = 0; i < argc; i++)
	process_file(argv[i]);
    write_results();
    terminate();
}
