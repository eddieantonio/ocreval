/**********************************************************************
 *
 *  sort.c
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

static void **a;
static int (*cmp)();

/**********************************************************************/

static void quicksort(left, right)
long left, right;
{
    long i, j;
    void *ref;
    i = left;
    j = right;
    ref = a[i];
    while (i < j)
    {
	while (i < j && (*cmp)(ref, a[j]) < 0)
	    j--;
	if (i != j)
	    a[i++] = a[j];
	while (i < j && (*cmp)(ref, a[i]) > 0)
	    i++;
	if (i != j)
	    a[j--] = a[i];
    }
    a[j] = ref;
    if (left < --j)
	quicksort(left, j);
    if (++i < right)
	quicksort(i, right);
}
/**********************************************************************/

void sort(num_elements, array, compare)
long num_elements;
void *array[];
int (*compare)();
{
    if (num_elements < 2)
	return;
    a = array;
    cmp = compare;
    quicksort(0, num_elements - 1);
}
