/**********************************************************************
 *
 *  sort.h
 *
 *  This module provides a general-purpose sorting routine.
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

#ifndef _SORT_
#define _SORT_

void sort(/* long num_elements, void *array[],
             int (*compare)(void *element1, void *element2) */);
			/* given an array of pointers having the specified
			   number of elements, sorts the array using the
			   comparison routine provided; this routine returns
			   a negative value if "element1" precedes "element2",
			   a positive value if "element1" follows "element2",
			   and a zero value if they are equal */

#endif
