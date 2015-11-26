/**********************************************************************
 *
 *  accsum.c
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

#define usage  "accuracy_report1 accuracy_report2 ... >accuracy_report"

Accdata accdata;

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
	read_accrpt(&accdata, argv[i]);
    write_accrpt(&accdata, NULL);
    terminate();
}
