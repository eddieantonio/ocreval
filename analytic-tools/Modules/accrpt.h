/**********************************************************************
 *
 *  accrpt.h
 *
 *  This module provides support for reading and writing character
 *  accuracy reports.  The contents of one of these reports is
 *  represented by an "Accdata" structure.
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

#ifndef _ACCRPT_
#define _ACCRPT_

#include "charclass.h"
#include "table.h"

typedef
struct
{
    long ins;		/* number of insertions */
    long subst;		/* number of substitutions */
    long del;		/* number of deletions */
    long errors;	/* number of errors = ins + subst + del */
} Accops;

typedef
struct
{
    long count;		/* number of ground-truth characters in this class */
    long missed;	/* number of these that were misrecognized */
} Accclass;

BEGIN_ENTRY(Conf)
    long errors;	/* number of errors caused by this confusion */
    long marked;	/* number of these that were marked */
END_ENTRY(Conf);

BEGIN_TABLE_OF(Conf, Conflist)
END_TABLE(Conftable);	/* table of confusions */

typedef
struct
{
    long characters;	/* number of ground-truth characters */
    long errors;	/* number of errors made */
    long reject_characters;
			/* number of reject characters generated */
    long suspect_markers;
			/* number of characters marked as suspect */
    long false_marks;	/* number of false marks */
    Accops marked_ops;	/* edit operations for marked errors */
    Accops unmarked_ops;/* edit operations for unmarked errors */
    Accops total_ops;	/* edit operations for all errors */
    Accclass large_class[MAX_CHARCLASSES];
			/* enumeration for each character class */
    Accclass total_class;
			/* enumeration for all classes combined */
    Conftable conftable;/* table of confusions */
    Accclass small_class[NUM_CHARVALUES];
			/* enumeration for each character code */
} Accdata;

void add_class(/* Accdata *accdata, Charvalue value, long count,
                  long missed */);
			/* adds the given character value to "accdata",
                           updating all relevant class enumerations */

void add_conf(/* Accdata *accdata, char *key, long errors, long marked */);
			/* adds the given confusion to "accdata"; "key" contains
			   the character string representation of the confusion
			   that will appear in the accuracy report (including
			   the trailing newline character); a copy of this
			   string is stored in the table */

void read_accrpt(/* Accdata *accdata, char *filename */);
			/* reads the named file (or stdin if "filename" is NULL)
			   and adds its contents to "accdata"; reports an error
			   and quits if unable to open the file, or if the file
			   does not contain an accuracy report */

void write_accrpt(/* Accdata *accdata, char *filename */);
			/* writes the contents of "accdata" to the named file
			   (or stdout if "filename" is NULL); reports an error
			   and quits if unable to create the file */

#endif
