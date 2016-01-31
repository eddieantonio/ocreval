/**********************************************************************
 *
 *  table.h
 *
 *  This module provides a general-purpose hash table capability.  A
 *  "table" contains zero or more "entries".
 *
 *  An entry is a record structure having the following initial fields:
 *
 *	Entry *prev;	- pointer to the previous entry in the list
 *	Entry *next;	- pointer to the next entry in the list
 *	char *key;	- null-terminated string containing the hash key
 *
 *  This structure is declared by
 *
 *  BEGIN_ENTRY(Entry)
 *	<declare other fields here>
 *  END_ENTRY(Entry);
 *
 *  A table is a record structure having the following initial fields:
 *
 *	Entrylist list[TABLE_SIZE];
 *			- array of lists of entries
 *	Entry *array[]; - array of pointers to the entries in the table
 *	long count;	- number of entries in the table
 *
 *  The following declares this structure and the Entrylist structure:
 *
 *  BEGIN_TABLE_OF(Entry, Entrylist)
 *	<declare other fields here, if any>
 *  END_TABLE(Table);
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

#ifndef _TABLE_
#define _TABLE_

#include "list.h"

#define TABLE_SIZE  503

#define BEGIN_ENTRY(Entry)			\
BEGIN_ITEM(Entry)				\
    char *key;

#define END_ENTRY(Entry)			\
END_ITEM(Entry)

#define BEGIN_TABLE_OF(Entry, Entrylist)	\
BEGIN_LIST_OF(Entry)				\
END_LIST(Entrylist);				\
typedef						\
struct						\
{						\
    Entrylist list[TABLE_SIZE];			\
    Entry **array;				\
    long count;

#define END_TABLE(Table)			\
} Table

void table_initialize(/* Table *table */);
			/* initializes the table; this routine does not need to
			   be called if the table structure was initialized
			   statically or dynamically */

void *table_lookup(/* Table *table, char *key */);
			/* searches the table for an entry having the specified
			   key value; returns a pointer to it if found; returns
			   NULL if not found */

void table_insert(/* Table *table, Entry *entry */);
			/* inserts the entry into the table */

void table_remove(/* Table *table, Entry *entry */);
			/* removes the entry from the table */

void table_in_array(/* Table *table */);
			/* if the table is non-empty, creates and initializes
			   the array of pointers to the entries in the table */

void table_empty(/* Table *table, void (*process_entry)(Entry *entry) */);
			/* empties the table; as each entry is removed, the
			   given routine is called to process it */

#endif
