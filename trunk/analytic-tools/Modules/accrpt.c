/**********************************************************************
 *
 *  accrpt.c
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
#include "sort.h"

#define TITLE    "UNLV-ISRI OCR Accuracy Report Version 5.1\n"
#define DIVIDER  "-----------------------------------------\n"

#define CLASS_OFFSET  29
#define CONF_OFFSET   20

static unsigned char line[100];

/**********************************************************************/

static void update_class(class, count, missed)
Accclass *class;
long count, missed;
{
    class->count  += count;
    class->missed += missed;
}
/**********************************************************************/

void add_class(accdata, value, count, missed)
Accdata *accdata;
Charvalue value;
long count, missed;
{
    update_class(&accdata->large_class[charclass(value)], count, missed);
    update_class(&accdata->total_class, count, missed);
    update_class(&accdata->small_class[value], count, missed);
}
/**********************************************************************/

void add_conf(accdata, key, errors, marked)
Accdata *accdata;
char *key;
long errors, marked;
{
    Conf *conf;
    conf = table_lookup(&accdata->conftable, key);
    if (conf)
    {
	conf->errors += errors;
	conf->marked += marked;
    }
    else
    {
	conf = NEW(Conf);
	conf->key = strdup(key);
	conf->errors = errors;
	conf->marked = marked;
	table_insert(&accdata->conftable, conf);
    }
}
/**********************************************************************/

static Boolean read_line(f)
FILE *f;
{
    return(fgets(line, sizeof(line) - 1, f) ? True : False);
}
/**********************************************************************/

static Boolean read_value(f, value, sum_value)
FILE *f;
long *value, *sum_value;
{
    if (read_line(f) && sscanf(line, "%ld", value) == 1)
    {
	*sum_value += *value;
	return(True);
    }
    else
	return(False);
}
/**********************************************************************/

static Boolean read_ops(f, sum_ops)
FILE *f;
Accops *sum_ops;
{
    Accops ops;
    if (read_line(f) && sscanf(line, "%ld %ld %ld %ld", &ops.ins, &ops.subst,
    &ops.del, &ops.errors) == 4)
    {
	sum_ops->ins    += ops.ins;
	sum_ops->subst  += ops.subst;
	sum_ops->del    += ops.del;
	sum_ops->errors += ops.errors;
	return(True);
    }
    else
	return(False);
}
/**********************************************************************/

static Boolean read_two(f, value1, value2)
FILE *f;
long *value1, *value2;
{
    return(read_line(f) && sscanf(line, "%ld %ld", value1, value2) == 2 ?
    True : False);
}
/**********************************************************************/

void read_accrpt(accdata, filename)
Accdata *accdata;
char *filename;
{
    FILE *f;
    long characters, errors, value1, value2, value3;
    f = open_file(filename, "r");
    if (read_line(f) && strncmp(line, TITLE, sizeof(TITLE) - 3) == 0 &&
    read_line(f) && strcmp(line, DIVIDER) == 0 &&
    read_value(f, &characters, &accdata->characters) &&
    read_value(f, &errors, &accdata->errors) &&
    read_line(f) && read_line(f) &&
    read_value(f, &value1, &accdata->reject_characters) &&
    read_value(f, &value1, &accdata->suspect_markers) &&
    read_value(f, &value1, &accdata->false_marks) &&
    read_line(f) && read_line(f) && read_line(f) && read_line(f) &&
    read_ops(f, &accdata->marked_ops) &&
    read_ops(f, &accdata->unmarked_ops) &&
    read_ops(f, &accdata->total_ops) && read_line(f))
    {
	while (read_line(f) && line[0] != NEWLINE);
	if (errors > 0 && read_line(f))
	    while (read_two(f, &value1, &value2))
		add_conf(accdata, &line[CONF_OFFSET], value1, value2);
	if (characters > 0 && read_line(f))
	    while (read_two(f, &value1, &value2))
	    {
		if (line[CLASS_OFFSET + 2] == '}')
		    value3 = line[CLASS_OFFSET + 1];
		else if (line[CLASS_OFFSET + 2] == '\\')
		    value3 = NEWLINE;
		else
		    sscanf(&line[CLASS_OFFSET + 2], "%x", &value3);
		add_class(accdata, value3, value1, value2);
	    }
    }
    else
	error_string("invalid format in", (filename ? filename : "stdin"),
	Exit);
    close_file(f);
}
/**********************************************************************/

static void write_value(f, value, string)
FILE *f;
long value;
char *string;
{
    fprintf(f, "%8ld   %s\n", value, string);
}
/**********************************************************************/

static void write_pct(f, numerator, denominator)
FILE *f;
long numerator, denominator;
{
    if (denominator == 0)
	fprintf(f, "  ------");
    else
	fprintf(f, "%8.2f", 100.0 * numerator / denominator);
}
/**********************************************************************/

static void write_ops(f, ops, string)
FILE *f;
Accops *ops;
char *string;
{
    fprintf(f, "%8ld %8ld %8ld %8ld   %s\n", ops->ins, ops->subst, ops->del,
    ops->errors, string);
}
/**********************************************************************/

static void write_class(f, class, string, value)
FILE *f;
Accclass *class;
char *string;
Charvalue value;
{
    char buffer[STRING_SIZE];
    fprintf(f, "%8ld %8ld ", class->count, class->missed);
    write_pct(f, class->count - class->missed, class->count);
    if (string)
	fprintf(f, "   %s\n", string);
    else
    {
	char_to_string(False, value, buffer, True);
	fprintf(f, "   {%s}\n", buffer);
    }
}
/**********************************************************************/

static void write_conf(f, conf)
FILE *f;
Conf *conf;
{
    fprintf(f, "%8ld %8ld   %s", conf->errors, conf->marked, conf->key);
}
/**********************************************************************/

static int compare_conf(conf1, conf2)
Conf *conf1, *conf2;
{
    if (conf1->errors != conf2->errors)
	return(conf2->errors - conf1->errors);
    if (conf1->marked != conf2->marked)
	return(conf2->marked - conf1->marked);
    return(ustrcmp(conf1->key, conf2->key));
}
/**********************************************************************/

void write_accrpt(accdata, filename)
Accdata *accdata;
char *filename;
{
    FILE *f;
    long i;
    f = open_file(filename, "w");
    fprintf(f, "%s%s", TITLE, DIVIDER);
    write_value(f, accdata->characters, "Characters");
    write_value(f, accdata->errors, "Errors");
    write_pct(f, accdata->characters - accdata->errors, accdata->characters);
    fprintf(f, "%%  Accuracy\n\n");
    write_value(f, accdata->reject_characters, "Reject Characters");
    write_value(f, accdata->suspect_markers, "Suspect Markers");
    write_value(f, accdata->false_marks, "False Marks");
    write_pct(f, accdata->reject_characters + accdata->suspect_markers,
    accdata->characters);
    fprintf(f, "%%  Characters Marked\n");
    write_pct(f, accdata->characters - accdata->unmarked_ops.errors,
    accdata->characters);
    fprintf(f, "%%  Accuracy After Correction\n");
    fprintf(f, "\n     Ins    Subst      Del   Errors\n");
    write_ops(f, &accdata->marked_ops, "Marked");
    write_ops(f, &accdata->unmarked_ops, "Unmarked");
    write_ops(f, &accdata->total_ops, "Total");
    fprintf(f, "\n   Count   Missed   %%Right\n");
    for (i = 0; i < MAX_CHARCLASSES; i++)
	if (accdata->large_class[i].count > 0)
	    write_class(f, &accdata->large_class[i], charclass_name(i), 0);
    write_class(f, &accdata->total_class, "Total", 0);
    if (accdata->errors > 0)
    {
	table_in_array(&accdata->conftable);
	sort(accdata->conftable.count, accdata->conftable.array, compare_conf);
	fprintf(f, "\n  Errors   Marked   Correct-Generated\n");
	for (i = 0; i < accdata->conftable.count; i++)
	    write_conf(f, accdata->conftable.array[i]);
    }
    if (accdata->characters > 0)
    {
	fprintf(f, "\n   Count   Missed   %%Right\n");
	for (i = 0; i < NUM_CHARVALUES; i++)
	    if (accdata->small_class[i].count > 0)
		write_class(f, &accdata->small_class[i], NULL, i);
    }
    close_file(f);
}
