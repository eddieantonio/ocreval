/**********************************************************************
 *
 *  table.c
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

#include "table.h"
#include "util.h"

BEGIN_ENTRY(Entry)
END_ENTRY(Entry);

BEGIN_TABLE_OF(Entry, Entrylist)
END_TABLE(Table);

/**********************************************************************/

void table_initialize(table)
Table *table;
{
    short i;
    for (i = 0; i < TABLE_SIZE; i++)
	list_initialize(&table->list[i]);
    table->array = NULL;
    table->count = 0;
}
/**********************************************************************/

static short table_index(key_string)
const char *key_string;
{
    const unsigned char *key = (const unsigned char*) key_string;
    long i, sum = 0;

    for (i = 0; key[i]; i++)
	sum += key[i];
    return(sum % TABLE_SIZE);
}
/**********************************************************************/

void *table_lookup(table, key)
Table *table;
char *key;
{
    Entry *entry;
    for (entry = table->list[table_index(key)].first; entry &&
    strcmp(key, entry->key) != 0; entry = entry->next);
    return(entry);
}
/**********************************************************************/

static void free_array(table)
Table *table;
{
    if (table->array)
    {
	free(table->array);
	table->array = NULL;
    }
}
/**********************************************************************/

void table_insert(table, entry)
Table *table;
Entry *entry;
{
    list_insert_first(&table->list[table_index(entry->key)], entry);
    free_array(table);
    table->count++;
}
/**********************************************************************/

void table_remove(table, entry)
Table *table;
Entry *entry;
{
    list_remove(&table->list[table_index(entry->key)], entry);
    free_array(table);
    table->count--;
}
/**********************************************************************/

void table_in_array(table)
Table *table;
{
    Entry *entry;
    long i, j = 0;
    if (table->array || table->count == 0)
	return;
    table->array = NEW_ARRAY(table->count, Entry *);
    for (i = 0; i < TABLE_SIZE; i++)
	for (entry = table->list[i].first; entry; entry = entry->next)
	    table->array[j++] = entry;
}
/**********************************************************************/

void table_empty(table, process_entry)
Table *table;
void (*process_entry)();
{
    short i;
    for (i = 0; i < TABLE_SIZE; i++)
	list_empty(&table->list[i], process_entry);
    free_array(table);
    table->count = 0;
}
