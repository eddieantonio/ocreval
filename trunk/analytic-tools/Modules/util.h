/**********************************************************************
 *
 *  util.h
 *
 *  This module contains basic definitions and utility routines that
 *  are needed by almost every module/program in the OCR Experimental
 *  Environment.
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

#ifndef _UTIL_
#define _UTIL_

#include <malloc.h>
#include <stdio.h>
#include <string.h>

#ifndef True
typedef char Boolean;
#define True   1
#define False  0
#endif

#ifndef max
#define max(a, b)  ((a) > (b) ? (a) : (b))
#define min(a, b)  ((a) < (b) ? (a) : (b))
#endif

#define NEW(type)  ((type *) allocate((size_t) 1, sizeof(type)))
			/* allocate one instance of "type" */
#define NEW_ARRAY(number, type) \
                   ((type *) allocate((size_t) (number), sizeof(type)))
			/* allocate an array of "type" */
void *allocate(/* size_t number, size_t size */);

int ustrcmp(/* unsigned char *s1, unsigned char *s2 */);
			/* compares strings like "strcmp" but treats characters
			   as unsigned */

FILE *open_file(/* char *filename, char *mode */);
			/* opens the named file in the specified mode; reports
			   an error and quits if unable to open the file; if
			   "filename" is NULL, returns stdin or stdout,
			   depending on the mode */

void close_file(/* FILE *f */);
			/* closes the specified file */

Boolean file_exists(/* char *filename */);
			/* returns True if the named file exists */

char *tempfilename();	/* creates and returns a unique name for a temporary
			   file */

char *basefilename(/* char *pathname */);
			/* given a pathname, returns the base filename; e.g.,
			   basefilename("/local/isri/bin/ocr") returns "ocr" */

extern char *exec_name; /* base filename of the executable */

extern Boolean usage_when_no_args;
			/* indicates whether the usage should be displayed when
			   no arguments have been specified to the program; by
			   default it is True; to override this, set it to False
			   before calling "initialize" */

extern void (*usage_routine)();
			/* specifies a routine to be called to display the
			   usage, overriding the "usage" argument passed to
			   "initialize"; this must be set before calling
			   "initialize" */

extern void (*cleanup_routine)();
			/* specifies a routine to be called upon exit */

typedef
struct
{
    char name;		/* character identifying the option; e.g., 'D' for -D;
			   '\0' to terminate an array of these */
    char **string;	/* if a string-valued option, address of variable to
			   hold the string value; e.g., for -Dcaere or -D caere,
			   the variable gets "caere"; if a Boolean-valued
			   option, this should be NULL */
    Boolean *boolean;	/* if a Boolean-valued option, address of variable to
			   set to True to indicate the option was specified; if
			   a string-valued option, this should be NULL */
} Option;

void initialize(/* int *argc, char *argv[], char *usage, Option option[] */);
			/* parses the command line arguments looking for any
			   of the allowed options; reports any invalid option
			   and quits; updates "argc" and "argv" to contain only
			   the non-option arguments; displays "usage" when
			   appropriate; "usage" may be NULL if "usage_routine"
			   has been set; "option" may be NULL if there are no
			   options */

void terminate();	/* terminates the program with exit status 0 */

extern FILE *errfile;   /* file that errors are written to; stderr by default */

extern int errstatus;	/* status returned when exiting due to an error; this is
			   1 by default */

#define Exit      1
#define Continue  0

void error(/* char *message, Exit/Continue */);
			/* writes an error message, then quits or returns */

void error_status(/* char *message, int status, Exit/Continue */);
			/* writes an error message, including a status code,
			   then quits or returns */

void error_string(/* char *message, char *string, Exit/Continue */);
			/* writes an error message, including "string" in
			   quotes, then quits or returns */

#endif
