/**********************************************************************
 *
 *  asc2uni.c
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

#include "unicode.h"

#define usage  "<ASCII_file >Unicode_file"

Textopt textopt = { False, True, 0, True };
Text text;

/**********************************************************************/

main(argc, argv)
int argc;
char *argv[];
{
    usage_when_no_args = False;
    initialize(&argc, argv, usage, NULL);
    if (argc != 0)
	error("invalid arguments", Exit);
    read_text(&text, NULL, &textopt);
    write_unicode(&text, NULL);
    terminate();
}
