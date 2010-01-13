/**********************************************************************
 *
 *  list.c
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

#include "list.h"
#include "util.h"

BEGIN_ITEM(Item)
END_ITEM(Item);

BEGIN_LIST_OF(Item)
END_LIST(List);

/**********************************************************************/

void list_initialize(list)
List *list;
{
    list->first = list->last = NULL;
    list->array = NULL;
    list->count = 0;
}
/**********************************************************************/

static void free_array(list)
List *list;
{
    if (list->array)
    {
	free(list->array);
	list->array = NULL;
    }
}
/**********************************************************************/

static void list_insert(list, ref1, item, ref2)
List *list;
Item *ref1, *item, *ref2;
{
    item->prev = ref1;
    item->next = ref2;
    if (ref1)
	ref1->next = item;
    else
	list->first = item;
    if (ref2)
	ref2->prev = item;
    else
	list->last = item;
    free_array(list);
    list->count++;
}
/**********************************************************************/

void list_insert_first(list, item)
List *list;
Item *item;
{
    list_insert(list, NULL, item, list->first);
}
/**********************************************************************/

void list_insert_last(list, item)
List *list;
Item *item;
{
    list_insert(list, list->last, item, NULL);
}
/**********************************************************************/

void list_insert_before(list, item, ref)
List *list;
Item *item, *ref;
{
    list_insert(list, ref->prev, item, ref);
}
/**********************************************************************/

void list_insert_after(list, ref, item)
List *list;
Item *ref, *item;
{
    list_insert(list, ref, item, ref->next);
}
/**********************************************************************/

void list_remove(list, item)
List *list;
Item *item;
{
    if (item->prev)
	item->prev->next = item->next;
    else
	list->first = item->next;
    if (item->next)
	item->next->prev = item->prev;
    else
	list->last = item->prev;
    item->prev = item->next = NULL;
    free_array(list);
    list->count--;
}
/**********************************************************************/

void list_in_array(list)
List *list;
{
    Item *item;
    long i = 0;
    if (list->array || list->count == 0)
	return;
    list->array = NEW_ARRAY(list->count, Item *);
    for (item = list->first; item; item = item->next)
	list->array[i++] = item;
}
/**********************************************************************/

void list_empty(list, process_item)
List *list;
void (*process_item)();
{
    Item *item;
    while (list->first)
    {
	item = list->first;
	list_remove(list, item);
	(*process_item)(item);
    }
}
