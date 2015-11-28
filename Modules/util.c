/**********************************************************************
 *
 *  util.c
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

#include <signal.h>

#if defined(__unix__) || defined(__MACH__)
#include <sys/types.h>
#include <sys/stat.h>
#else
#include <sys\types.h>
#include <sys\stat.h>
#endif

#include "util.h"

char *exec_name;

Boolean usage_when_no_args = True;
void (*usage_routine)();

void (*cleanup_routine)();

int errstatus = 1;

static short tempfile_id;
static void quit(/* int status */) __attribute__ ((noreturn));

/**********************************************************************/

void *allocate(number, size)
size_t number, size;
{
    void *p;
    p = calloc(number, size);
    if (!p)
	error("unable to allocate memory");
    return(p);
}
/**********************************************************************/

int ustrcmp(s1, s2)
unsigned char *s1, *s2;
{
    long i;
    for (i = 0; s1[i] && s1[i] == s2[i]; i++);
    if (s1[i] || s2[i])
	return(s1[i] < s2[i] ? -1 : 1);
    else
	return(0);
}
/**********************************************************************/

FILE *open_file(filename, mode)
char *filename, *mode;
{
    FILE *f;
    if (!filename)
	return(mode[0] == 'r' ? stdin : stdout);
    f = fopen(filename, mode);
    if (f)
	return(f);
    if (mode[0] == 'w')
	error_string("unable to create file", filename);
    else
	error_string("unable to open file", filename);
}
/**********************************************************************/

void close_file(f)
FILE *f;
{
    if (f != stdin && f != stdout)
	fclose(f);
}
/**********************************************************************/

Boolean file_exists(filename)
char *filename;
{
    struct stat buffer;
    return(stat(filename, &buffer) == 0 ? True : False);
}
/**********************************************************************/

static char *create_tempfilename(id)
short id;
{
    char name[100];
#ifdef unix
    sprintf(name, "/tmp/.%s%d-%d", exec_name, getpid(), id);
#else
    sprintf(name, "c:\\temp\\tempfile.%d", id);
#endif
    return(strdup(name));
}
/**********************************************************************/

char *tempfilename()
{
    char *name;
    name = create_tempfilename(++tempfile_id);
    unlink(name);
    return(name);
}
/**********************************************************************/

static void delete_tempfiles()
{
    short i;
    char *name;
    for (i = 1; i <= tempfile_id; i++)
    {
	name = create_tempfilename(i);
	unlink(name);
	free(name);
    }
}
/**********************************************************************/

char *basefilename(pathname)
char *pathname;
{
#ifdef unix
    char delimiter = '/';
#else
    char delimiter = '\\';
#endif
    short i;
    for (i = strlen(pathname) - 1; i >= 0 && pathname[i] != delimiter; i--);
    return(&pathname[i + 1]);
}
/**********************************************************************/

static void handle_interrupt(signal)
int signal;
{
    static Boolean handling_interrupt = False;
    if (handling_interrupt)
	return;
    handling_interrupt = True;
    error("process killed");
}
/**********************************************************************/

static void trap_interrupts()
{
    signal(SIGINT, handle_interrupt);
    signal(SIGTERM, handle_interrupt);
}
/**********************************************************************/

static void show_usage(usage)
char *usage;
{
    if (usage_routine)
	(*usage_routine)();
    else
	fprintf(stderr, "Usage:  %s %s\n", exec_name, usage);
    terminate();
}
/**********************************************************************/

static Boolean split_option(arg, next_arg, option)
char *arg, *next_arg;
Option option[];
{
    short i;
    if (!option)
	goto invalid_option;
    for (i = 0; option[i].name && arg[1] != option[i].name; i++);
    if (!option[i].name)
	goto invalid_option;
    if (option[i].string && (arg[2] || next_arg))
    {
	if (*option[i].string)
	    goto duplicate_option;
	*option[i].string = (arg[2] ? &arg[2] : next_arg);
	return(arg[2] ? False : True);
    }
    if (option[i].boolean && !arg[2])
    {
	if (*option[i].boolean)
	    goto duplicate_option;
	*option[i].boolean = True;
	return(False);
    }
invalid_option:
    error_string("invalid option", arg);
duplicate_option:
    error_string("duplicate option", arg);
}
/**********************************************************************/

static void parse_args(argc, argv, usage, option)
int *argc;
char *argv[], *usage;
Option option[];
{
    short i, j = 0;
    if (*argc == 1 && usage_when_no_args)
	show_usage(usage);
    for (i = 1; i < *argc; i++)
	if (argv[i][0] == '-' && argv[i][1])
	{
	    if (strncmp("-help", argv[i], strlen(argv[i])) == 0)
		show_usage(usage);
	    if (split_option(argv[i], (i + 1 < *argc ? argv[i + 1] : NULL),
	    option))
		i++;
	}
	else
	    argv[j++] = argv[i];
    *argc = j;
}
/**********************************************************************/

void initialize(argc, argv, usage, option)
int *argc;
char *argv[], *usage;
Option option[];
{
    exec_name = basefilename(argv[0]);
    trap_interrupts();
    parse_args(argc, argv, usage, option);
}
/**********************************************************************/

static void quit(status)
int status;
{
    if (cleanup_routine)
	(*cleanup_routine)();
    delete_tempfiles();
    exit(status);
}
/**********************************************************************/

void terminate()
{
    quit(0);
}
/**********************************************************************/

void error(message)
char *message;
{
    fprintf(stderr, "%s: %s\n", exec_name, message);
    quit(errstatus);
}
/**********************************************************************/

void error_string(message, string)
char *message, *string;
{
    fprintf(stderr, "%s: %s \"%s\"\n", exec_name, message, string);
    quit(errstatus);
}

void warning_string(message, string)
char *message, *string;
{
    fprintf(stderr, "%s: %s \"%s\"\n", exec_name, message, string);
}
