/**********************************************************************
 *
 *  list.h
 *
 *  This module provides a general-purpose linked-list capability.  A
 *  "list" contains zero or more "items".
 *
 *  An item is a record structure having the following initial fields:
 *
 *	Item *prev;	- pointer to the previous item in the list
 *	Item *next;	- pointer to the next item in the list
 *
 *  This structure is declared by
 *
 *  BEGIN_ITEM(Item)
 * 	<declare other fields here>
 *  END_ITEM(Item);
 *
 *  A list is a record structure having the following initial fields:
 *
 *	Item *first;	- pointer to the first item in the list
 *	Item *last;	- pointer to the last item in the list
 *	Item *array[];  - array of pointers to the items in the list
 *	long count;	- number of items in the list
 *
 *  This structure is declared by
 *
 *  BEGIN_LIST_OF(Item)
 *	<declare other fields here, if any>
 *  END_LIST(List);
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

#ifndef _LIST_
#define _LIST_

#define BEGIN_ITEM(Item)		\
typedef					\
struct Item				\
{					\
    struct Item *prev, *next;

#define END_ITEM(Item)			\
} Item

#define BEGIN_LIST_OF(Item)		\
typedef					\
struct					\
{					\
    Item *first, *last, **array;	\
    long count;

#define END_LIST(List)			\
} List

void list_initialize(/* List *list */);
			/* initializes the list; this routine does not need to
			   be called if the list structure was initialized
			   statically or dynamically */

void list_insert_first(/* List *list, Item *item */);
			/* inserts the item at the beginning of the list */

void list_insert_last(/* List *list, Item *item */);
			/* inserts the item at the end of the list */

void list_insert_before(/* List *list, Item *item, Item *ref */);
			/* inserts the item before "ref" in the list */

void list_insert_after(/* List *list, Item *ref, Item *item */);
			/* inserts the item after "ref" in the list */

void list_remove(/* List *list, Item *item */);
			/* removes the item from the list */

void list_in_array(/* List *list */);
			/* if the list is non-empty, creates and initializes
			   the array of pointers to the items in the list */

void list_empty(/* List *list, void (*process_item)(Item *item) */);
			/* empties the list; as each item is removed, the given
			   routine is called to process it */

#endif
